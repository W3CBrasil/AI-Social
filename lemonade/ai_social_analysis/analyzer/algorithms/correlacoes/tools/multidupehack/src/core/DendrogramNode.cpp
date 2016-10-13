// Copyright 2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "DendrogramNode.h"

unsigned int DendrogramNode::nbOfParents;
list<DendrogramNode*> DendrogramNode::dendrogram;
list<DendrogramNode*> DendrogramNode::dendrogramFrontier;
unordered_set<CandidateNode*> DendrogramNode::candidates;
unordered_map<vector<vector<unsigned int>>, CandidateNode*, vector_hash<vector<unsigned int>>> DendrogramNode::candidateNSets;

DendrogramNode::DendrogramNode(const vector<Attribute*>& attributes): nSet(), area(1), membershipSum(), g(), children(), parents()
{
  dendrogramFrontier.push_back(this);
  nSet.reserve(attributes.size());
  for (const Attribute* attribute : attributes)
    {
      nSet.push_back(attribute->getPresentAndPotentialDataIds());
      sort(nSet.back().begin(), nSet.back().end());
      area *= nSet.back().size();
    }
  membershipSum = CandidateNode::getMaxMembershipMinusSimilarityShift() * area - attributes.front()->totalPresentAndPotentialNoise();
  g = membershipSum * membershipSum / area;
  if (membershipSum < 0)
    {
      g = -g;
    }
}

DendrogramNode::DendrogramNode(const vector<vector<unsigned int>>& nSetParam, const Trie* data): nSet(nSetParam), area(1), membershipSum(), g(), children(), parents()
{
  dendrogramFrontier.push_back(this);
  for (vector<unsigned int>& nSetDimension : nSet)
    {
      sort(nSetDimension.begin(), nSetDimension.end());
      area *= nSetDimension.size();
    }
  membershipSum = CandidateNode::getMaxMembershipMinusSimilarityShift() * area - data->noiseSum(nSet);
  g = membershipSum * membershipSum / area;
  if (membershipSum < 0)
    {
      g = -g;
    }
}

DendrogramNode::DendrogramNode(const CandidateNode* candidate, const Trie* data): nSet(candidate->getNSet()), area(candidate->getArea()), membershipSum(candidate->getMembershipSum()), g(candidate->getG()), children(), parents()
{
#ifdef DEBUG_HA
  cout << "Quadratic error variation: " << candidate->getQuadraticErrorVariation() / Attribute::noisePerUnit / Attribute::noisePerUnit << endl;
#endif
  dendrogramFrontier.push_back(this);
  // Find the nodes in dendrogramFrontier that are subsets of this and construct the new candidates
  vector<list<DendrogramNode*>::iterator> subsetsInDendrogramFrontier;
  const list<DendrogramNode*>::iterator insertedCandidateIt = --dendrogramFrontier.end();
  for (list<DendrogramNode*>::iterator child2It = dendrogramFrontier.begin(); child2It != insertedCandidateIt; ++child2It)
    {
      if (constructNewCandidate(insertedCandidateIt, child2It, data, candidate))
	{
	  subsetsInDendrogramFrontier.push_back(child2It);
	}
    }
  for (const list<DendrogramNode*>::iterator subsetInDendrogramFrontierIt : subsetsInDendrogramFrontier)
    {
      // Store the children, which are not the future children yet because, when this isIrrelevant will be computed in getParentChildren, there must be one child per partition of the covered leaves
      const vector<list<DendrogramNode*>::iterator> childrenOfOneChildWithAtLeastItsG = (*subsetInDendrogramFrontierIt)->getParentChildren();
      if (childrenOfOneChildWithAtLeastItsG.empty())
	{
	  dendrogram.push_back(*subsetInDendrogramFrontierIt);
	  children.push_back(--dendrogram.end());
	}
      else
	{
	  delete *subsetInDendrogramFrontierIt;
	  children.insert(children.end(), childrenOfOneChildWithAtLeastItsG.begin(), childrenOfOneChildWithAtLeastItsG.end());
	}
      dendrogramFrontier.erase(subsetInDendrogramFrontierIt);
    }
}

#ifdef DEBUG_HA
ostream& operator<<(ostream& out, const DendrogramNode& dendrogramNode)
{
  out << &dendrogramNode << ':';
  unsigned int dimensionId = 0;
  for (const vector<unsigned int>& dimension : dendrogramNode.nSet)
    {
      out << ' ';
      Attribute::printValuesFromDataIds(dimension, dimensionId++, out);
    }
  out << endl << "  area: " << dendrogramNode.area << endl << "  membershipSum: " << dendrogramNode.membershipSum / Attribute::noisePerUnit << endl << "  density: " << dendrogramNode.membershipSum / dendrogramNode.area / Attribute::noisePerUnit << endl << "  g: " << dendrogramNode.g / Attribute::noisePerUnit / Attribute::noisePerUnit;
  return out;
}
#endif

const vector<unsigned int>& DendrogramNode::dimension(const unsigned int dimensionId) const
{
  return nSet[dimensionId];
}

const unsigned int DendrogramNode::getArea() const
{
  return area;
}

vector<vector<unsigned int>> DendrogramNode::unionWith(const DendrogramNode& otherDendrogramNode) const
{
  vector<vector<unsigned int>> unionNSet;
  unionNSet.reserve(nSet.size());
  vector<vector<unsigned int>>::const_iterator otherNSetIt = otherDendrogramNode.nSet.begin();
  for (const vector<unsigned int>& nSetDimension : nSet)
    {
      vector<unsigned int> unionNSetDimension(nSetDimension.size() + otherNSetIt->size());
      unionNSetDimension.resize(set_union(nSetDimension.begin(), nSetDimension.end(), otherNSetIt->begin(), otherNSetIt->end(), unionNSetDimension.begin()) - unionNSetDimension.begin());
      unionNSet.push_back(unionNSetDimension);
      ++otherNSetIt;
    }
  return unionNSet;
}

const unsigned int DendrogramNode::countFutureChildren(const double ancestorG) const
{
  if (children.empty())
    {
      return 0;
    }
  unsigned int nbOfCoveredLeaves = 0;
  for (const list<DendrogramNode*>::iterator childIt : children)
    {
      if ((*childIt)->g > ancestorG)
	{
	  ++nbOfCoveredLeaves;
	}
      else
	{
	  nbOfCoveredLeaves += (*childIt)->countFutureChildren(ancestorG);
	}
    }
  return nbOfCoveredLeaves;
}

void DendrogramNode::deleteOffspringWithSmallerG(const double ancestorG, vector<list<DendrogramNode*>::iterator>& ancestorChildren)
{
  for (list<DendrogramNode*>::iterator childIt : children)
    {
      if ((*childIt)->g > ancestorG)
	{
	  ancestorChildren.push_back(childIt);
	}
      else
	{
	  (*childIt)->deleteOffspringWithSmallerG(ancestorG, ancestorChildren);
	  delete *childIt;
	  dendrogram.erase(childIt);
	}
    }
}

vector<list<DendrogramNode*>::iterator> DendrogramNode::getParentChildren()
{
  // Unlink *this from its parents
  for (unordered_set<CandidateNode*>::iterator parentIt = parents.begin(); parentIt != parents.end(); parentIt = parents.erase(parentIt))
    {
      const pair<bool, unordered_set<DendrogramNode*>> isCandidateToBeDeletedAndAffectedNodesAtFrontier = (*parentIt)->unlinkGeneratingPairsInvolving(this);
      // Remove parent from the parents of nodes that can no longer generate it
      for (DendrogramNode* otherChild : isCandidateToBeDeletedAndAffectedNodesAtFrontier.second)
	{
	  otherChild->parents.erase(*parentIt);
	}
      if (isCandidateToBeDeletedAndAffectedNodesAtFrontier.first)
	{
	  candidateNSets.erase((*parentIt)->getNSet());
	  candidates.erase(*parentIt);
	  delete *parentIt;
	}
    }
  bool isIrrelevant = true;
  // Compute the number of future children to reserve enough space (essential to guarantee no reallocation when deleteOffspringWithSmallerG inserts new children)
  unsigned int nbOfFutureChildren = 1; // + 1 because, in deleteOffspringWithSmallerG, new children are inserted before removing those with smaller g measures
  for (const list<DendrogramNode*>::iterator childIt : children)
    {
      const unsigned int nbOfFutureChildrenBelowChild = (*childIt)->countFutureChildren(g);
      if (nbOfFutureChildrenBelowChild == 0)
	{
	  ++nbOfFutureChildren;
	}
      else
	{
	  nbOfFutureChildren += nbOfFutureChildrenBelowChild;
	}
    }
  children.reserve(nbOfFutureChildren);
  const vector<list<DendrogramNode*>::iterator>::reverse_iterator rend = children.rend();
  for (vector<list<DendrogramNode*>::iterator>::reverse_iterator childItIt = children.rbegin(); childItIt != rend; )
    {
      if ((**childItIt)->g > g)
	{
	  ++childItIt;
	}
      else
	{
	  isIrrelevant = false;
	  (**childItIt)->deleteOffspringWithSmallerG(g, children);
	  delete **childItIt;
	  dendrogram.erase(*childItIt);
	  *childItIt++ = children.back();
	  children.pop_back();
	}
    }
  if (isIrrelevant)
    {
      return std::move(children);
    }
  return vector<list<DendrogramNode*>::iterator>();
}

const bool DendrogramNode::greaterG(const DendrogramNode* node1, const DendrogramNode* node2)
{
  return node2->g < node1->g;
}

void DendrogramNode::retainCandidate(CandidateNode* candidate, const list<DendrogramNode*>::iterator child1It, const list<DendrogramNode*>::iterator child2It, const Trie* data)
{
  (*child1It)->parents.insert(candidate);
  (*child2It)->parents.insert(candidate);  
  candidate->insertGeneratingPair(child1It, child2It, data);
  // Compute the intersection of the two children and the area of this intersection
  unsigned int intersectionArea = 1;
  vector<vector<unsigned int>> intersection;
  intersection.reserve((*child1It)->nSet.size());
  vector<vector<unsigned int>>::const_iterator child1DimensionIt = (*child1It)->nSet.begin();
  for (const vector<unsigned int>& child2Dimension : (*child2It)->nSet)
    {
      vector<unsigned int> intersectionDimension(child2Dimension.size());
      const unsigned int intersectionDimensionSize = set_intersection(child2Dimension.begin(), child2Dimension.end(), child1DimensionIt->begin(), child1DimensionIt->end(), intersectionDimension.begin()) - intersectionDimension.begin();
      intersectionDimension.resize(intersectionDimensionSize);
      intersection.push_back(intersectionDimension);      
      intersectionArea *= intersectionDimensionSize;
      ++child1DimensionIt;
    }
  // Compute the quality of the model composed of the two children
  const double membershipSumAtIntersection = CandidateNode::getMaxMembershipMinusSimilarityShift() * intersectionArea + data->noiseSum(intersection);
  if ((*child1It)->membershipSum / (*child1It)->area < (*child2It)->membershipSum / (*child2It)->area)
    {
      const unsigned int child1MinusIntersectionArea = (*child1It)->area - intersectionArea;
      const double child1MinusIntersectionMembershipSum = (*child1It)->membershipSum - membershipSumAtIntersection;
      const double marginMembershipSum = candidate->getMembershipSum() - (*child2It)->membershipSum - child1MinusIntersectionMembershipSum;
      candidate->setQuadraticErrorVariation((*child2It)->g + child1MinusIntersectionMembershipSum * child1MinusIntersectionMembershipSum / child1MinusIntersectionArea + marginMembershipSum * marginMembershipSum / (candidate->getArea() - (*child2It)->area - child1MinusIntersectionArea));
      return;
    }
  const unsigned int child2MinusIntersectionArea = (*child2It)->area - intersectionArea;
  const double child2MinusIntersectionMembershipSum = (*child2It)->membershipSum - membershipSumAtIntersection;
  const double marginMembershipSum = candidate->getMembershipSum() - (*child1It)->membershipSum - child2MinusIntersectionMembershipSum;
  candidate->setQuadraticErrorVariation((*child1It)->g + child2MinusIntersectionMembershipSum * child2MinusIntersectionMembershipSum / child2MinusIntersectionArea + marginMembershipSum * marginMembershipSum / (candidate->getArea() - (*child1It)->area - child2MinusIntersectionArea));
}

void DendrogramNode::constructOriginalCandidate(const list<DendrogramNode*>::iterator child1It, const list<DendrogramNode*>::iterator child2It, const Trie* data)
{
  DendrogramNode& child1 = **child1It;
  DendrogramNode& child2 = **child2It;
  const vector<vector<unsigned int>> unionNSet = child1.unionWith(child2);
  const unordered_map<vector<vector<unsigned int>>, CandidateNode*, vector_hash<vector<unsigned int>>>::iterator candidateNSetIt = candidateNSets.find(unionNSet);
  if (candidateNSetIt == candidateNSets.end())
    {
      CandidateNode* candidate = new CandidateNode(unionNSet);
      retainCandidate(candidate, child1It, child2It, data);
      candidateNSets[unionNSet] = candidate;
      candidates.insert(candidate);
      return;
    }
  CandidateNode* candidate = candidateNSetIt->second;
  unordered_set<CandidateNode*>::iterator previouslyInsertedIt = child1.parents.find(candidate);
  if (previouslyInsertedIt == child1.parents.end())
    {
      retainCandidate(candidate, child1It, child2It, data);
    }
}

const bool DendrogramNode::constructNewCandidate(const list<DendrogramNode*>::iterator child1It, const list<DendrogramNode*>::iterator child2It, const Trie* data, const CandidateNode* insertedCandidate)
{
  DendrogramNode& child1 = **child1It;
  DendrogramNode& child2 = **child2It;
  const vector<vector<unsigned int>> unionNSet = child1.unionWith(child2);
  const unordered_map<vector<vector<unsigned int>>, CandidateNode*, vector_hash<vector<unsigned int>>>::iterator candidateNSetIt = candidateNSets.find(unionNSet);
  if (candidateNSetIt == candidateNSets.end())
    {
      CandidateNode* candidate = new CandidateNode(unionNSet);
      retainCandidate(candidate, child1It, child2It, data);
      candidateNSets[unionNSet] = candidate;
      candidates.insert(candidate);
      return false;
    }
  CandidateNode* candidate = candidateNSetIt->second;
  if (candidate == insertedCandidate)
    {
      return true;
    }
  unordered_set<CandidateNode*>::iterator previouslyInsertedIt = child1.parents.find(candidate);
  if (previouslyInsertedIt == child1.parents.end())
    {
      retainCandidate(candidate, child1It, child2It, data);
    }
  return false;
}

pair<list<DendrogramNode*>::const_iterator, list<DendrogramNode*>::const_iterator> DendrogramNode::agglomerateAndSelect(const Trie* data, const double maximalNbOfCandidateAgglomerates)
{
  nbOfParents = dendrogramFrontier.size();
  if (nbOfParents == 0 || nbOfParents > maximalNbOfCandidateAgglomerates)
    {
      return pair<list<DendrogramNode*>::const_iterator, list<DendrogramNode*>::const_iterator>(dendrogramFrontier.begin(), dendrogramFrontier.end());
    }
#ifdef DEBUG_HA
  cout << endl << "Dendrogram:" << endl << endl << "* " << nbOfParents << " leaves generating at most " << static_cast<unsigned int>(maximalNbOfCandidateAgglomerates / nbOfParents) << " candidates each:" << endl;
#endif
  nbOfParents = maximalNbOfCandidateAgglomerates / nbOfParents;
  // Candidate construction
  const list<DendrogramNode*>::iterator end = dendrogramFrontier.end();
  for (list<DendrogramNode*>::iterator child1It = dendrogramFrontier.begin(); child1It != end; ++child1It)
    {
#ifdef DEBUG_HA
      cout << **child1It << endl;
#endif
      list<DendrogramNode*>::iterator child2It = dendrogramFrontier.begin();
      for (; child2It != child1It; ++child2It)
	{
	  constructOriginalCandidate(child1It, child2It, data);
	}
      while (++child2It != end)
      	{
      	  constructOriginalCandidate(child1It, child2It, data);
      	}
    }
    // Hierarchical agglomeration
#ifdef DEBUG_HA
  cout << endl << "* Agglomerates:" << endl;
#endif
  while (!candidates.empty())
    {
      new DendrogramNode(*min_element(candidates.begin(), candidates.end(), CandidateNode::smallerQuadraticErrorVariation), data);
#ifdef DEBUG_HA
      cout << **--dendrogramFrontier.end() << endl << "  children:";
      for (const list<DendrogramNode*>::iterator childIt : (*--dendrogramFrontier.end())->children)
	{
	  cout << ' ' << *childIt;
	}
      cout << endl;
#endif
    }
  DendrogramNode* root = dendrogramFrontier.front();
  if (root->getParentChildren().empty())
    {
      dendrogram.push_back(root);
    }
  else
    {
      delete root;
    }
  // Order the nodes, more relevant first
  dendrogram.sort(greaterG);
  return pair<list<DendrogramNode*>::const_iterator, list<DendrogramNode*>::const_iterator>(dendrogram.begin(), dendrogram.end());
}
