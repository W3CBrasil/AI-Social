// Copyright 2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "CandidateNode.h"

double CandidateNode::maxMembershipMinusSimilarityShift;

CandidateNode::CandidateNode(const vector<vector<unsigned int>>& nSetParam): nSet(nSetParam), area(1), membershipSum(), g(), quadraticErrorVariation(), generatingPairs()
{
  for (vector<unsigned int>& nSetDimension : nSet)
    {
      area *= nSetDimension.size();
    }
}

const vector<vector<unsigned int>>& CandidateNode::getNSet() const
{
  return nSet;
}

const unsigned int CandidateNode::getArea() const
{
  return area;
}

const double CandidateNode::getMembershipSum() const
{
  return membershipSum;
}

const double CandidateNode::getG() const
{
  return g;
}

const double CandidateNode::getQuadraticErrorVariation() const
{
  return quadraticErrorVariation;
}

// TODO: estimate the quadraticErrorVariation instead
const float CandidateNode::getQuadraticErrorVariationEstimation(const vector<vector<unsigned int>>::const_iterator maxDensityNSetBegin, const double maxDensityMembershipSum, const double maxDensity, const double maxDensityG, const vector<vector<unsigned int>>::const_iterator minDensityNSetBegin, const unsigned int minDensityArea, const double minDensityMembershipSum) const
{
  unsigned int nbOfTuplesInIntersection = 1;
  vector<vector<unsigned int>>::const_iterator maxDensityDimensionIt = maxDensityNSetBegin;
  vector<vector<unsigned int>>::const_iterator minDensityDimensionIt = minDensityNSetBegin;
  for (const vector<unsigned int>& nSetDimension : nSet)
    {
      nbOfTuplesInIntersection *= maxDensityDimensionIt->size() + minDensityDimensionIt->size() - nSetDimension.size();
      ++maxDensityDimensionIt;
      ++minDensityDimensionIt;
    }
  float membershipSumEstimation = maxDensityMembershipSum;
  const float estimatedSumAtIntersection = maxDensity * nbOfTuplesInIntersection;
  if (estimatedSumAtIntersection < minDensityMembershipSum)
    {
      const double minMinusMaxMembershipSum = minDensityMembershipSum - estimatedSumAtIntersection;
      membershipSumEstimation += minMinusMaxMembershipSum;
      const double gEstimation = membershipSumEstimation * membershipSumEstimation / area;
      if (membershipSumEstimation < 0)
	{
	  return gEstimation + maxDensityG + minMinusMaxMembershipSum * minMinusMaxMembershipSum / minDensityArea;
	}
      return -gEstimation + maxDensityG + minMinusMaxMembershipSum * minMinusMaxMembershipSum / minDensityArea;
    }
  const double gEstimation = membershipSumEstimation * membershipSumEstimation / area;
  if (membershipSumEstimation < 0)
    {
      return gEstimation + maxDensityG;
    }
  return -gEstimation + maxDensityG;
}

void CandidateNode::insertGeneratingPair(const list<DendrogramNode*>::iterator child1It, const list<DendrogramNode*>::iterator child2It, const Trie* data)
{
  if (generatingPairs.empty())
    {
      quadraticErrorVariation = numeric_limits<double>::infinity();
      membershipSum = maxMembershipMinusSimilarityShift * area - data->noiseSum(nSet);
      g = membershipSum * membershipSum / area;      
      if (membershipSum < 0)
	{
	  g = -g;
	}
    }
  generatingPairs.push_back(child1It);
  generatingPairs.push_back(child2It);
}

void CandidateNode::setQuadraticErrorVariation(const double twoPatternModelG)
{
  const double newQuadraticErrorVariation = twoPatternModelG - g;
  if (newQuadraticErrorVariation < quadraticErrorVariation)
    {
      quadraticErrorVariation = newQuadraticErrorVariation;
    }
}

pair<bool, unordered_set<DendrogramNode*>> CandidateNode::unlinkGeneratingPairsInvolving(const DendrogramNode* child)
{
  unordered_set<DendrogramNode*> otherComponentsOfErasedPairs;
  for (vector<list<DendrogramNode*>::iterator>::iterator otherChildItIt = generatingPairs.begin(); otherChildItIt != generatingPairs.end(); )
    {
      if (**otherChildItIt == child)
	{
	  // child is the first component of the generating pair
	  if (otherChildItIt + 2 == generatingPairs.end())
	    {
	      otherComponentsOfErasedPairs.insert(*generatingPairs.back());
	      generatingPairs.pop_back();
	      generatingPairs.pop_back();
	      break;
	    }
	  vector<list<DendrogramNode*>::iterator>::iterator otherComponentOfErasedPairsIt = otherChildItIt + 1;
	  otherComponentsOfErasedPairs.insert(**otherComponentOfErasedPairsIt);
	  *otherChildItIt = generatingPairs.back();
	  generatingPairs.pop_back();
	  *otherComponentOfErasedPairsIt = generatingPairs.back();
	  generatingPairs.pop_back();
	}
      else
	{
	  if (**++otherChildItIt == child)
	    {
	      // child is the second component of the generating pair
	      if (otherChildItIt + 1 == generatingPairs.end())
		{
		  generatingPairs.pop_back();
		  otherComponentsOfErasedPairs.insert(*generatingPairs.back());
		  generatingPairs.pop_back();
		  break;
		}
	      *otherChildItIt = generatingPairs.back();
	      generatingPairs.pop_back();
	      otherComponentsOfErasedPairs.insert(**--otherChildItIt);
	      *otherChildItIt = generatingPairs.back();
	      generatingPairs.pop_back();
	    }
	  else
	    {
	      // child is not a component of the generating pair
	      ++otherChildItIt;
	    }
	}
    }
  if (generatingPairs.empty())
    {
      return pair<bool, unordered_set<DendrogramNode*>>(true, otherComponentsOfErasedPairs);
    }
  // Do not unlink a child if still in a valid generating pair
  for (const list<DendrogramNode*>::iterator childIt : generatingPairs)
    {
      otherComponentsOfErasedPairs.erase(*childIt);
    }
  return pair<bool, unordered_set<DendrogramNode*>>(false, otherComponentsOfErasedPairs);
}

void CandidateNode::setSimilarityShift(const double similarityShift)
{
  maxMembershipMinusSimilarityShift = similarityShift + Attribute::noisePerUnit;
}

const double CandidateNode::getMaxMembershipMinusSimilarityShift()
{
  return maxMembershipMinusSimilarityShift;
}

const bool CandidateNode::smallerQuadraticErrorVariation(const CandidateNode* node1, const CandidateNode* node2)
{
  return node1->quadraticErrorVariation < node2->quadraticErrorVariation;
}
