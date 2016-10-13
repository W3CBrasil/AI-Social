// Copyright 2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef DENDROGRAM_NODE_H_
#define DENDROGRAM_NODE_H_

#include "CandidateNode.h"

class DendrogramNode
{
 public:
  DendrogramNode(const DendrogramNode& otherDendrogramNode) = delete;
  DendrogramNode(const vector<Attribute*>& attributes);
  DendrogramNode(const vector<vector<unsigned int>>& nSet, const Trie* data);
  
  DendrogramNode& operator=(const DendrogramNode& otherDendrogramNode) = delete;
#ifdef DEBUG_HA
  friend ostream& operator<<(ostream& out, const DendrogramNode& patternNode);
#endif

  const vector<unsigned int>& dimension(const unsigned int dimensionId) const;
  const unsigned int getArea() const;

  static pair<list<DendrogramNode*>::const_iterator, list<DendrogramNode*>::const_iterator> agglomerateAndSelect(const Trie* data, const double maximalNbOfCandidateAgglomerates);

 protected:
  vector<vector<unsigned int>> nSet;
  unsigned int area;
  double membershipSum;
  double g;
  vector<list<DendrogramNode*>::iterator> children;
  unordered_set<CandidateNode*> parents;
  
  DendrogramNode(const CandidateNode* thisCandidateNode, const Trie* data);

  vector<vector<unsigned int>> unionWith(const DendrogramNode& otherPatternNode) const;
  void computeGoodParentsG(const Trie* data);
  const unsigned int countFutureChildren(const double ancestorG) const;
  void deleteOffspringWithSmallerG(const double ancestorG, vector<list<DendrogramNode*>::iterator>& ancestorChildren);
  vector<list<DendrogramNode*>::iterator> getParentChildren();

  static unsigned int nbOfParents;
  static list<DendrogramNode*> dendrogram;
  static list<DendrogramNode*> dendrogramFrontier;
  static unordered_set<CandidateNode*> candidates;
  static unordered_map<vector<vector<unsigned int>>, CandidateNode*, vector_hash<vector<unsigned int>>> candidateNSets;

  static const bool greaterG(const DendrogramNode* node1, const DendrogramNode* node2);
  static void constructOriginalCandidate(const list<DendrogramNode*>::iterator child1It, const list<DendrogramNode*>::iterator child2It, const Trie* data);
  static const bool constructNewCandidate(const list<DendrogramNode*>::iterator child1It, const list<DendrogramNode*>::iterator child2It, const Trie* data, const CandidateNode* insertedCandidate);
  static void retainCandidate(CandidateNode* candidate, const list<DendrogramNode*>::iterator child1It, const list<DendrogramNode*>::iterator child2It, const Trie* data);
};

#endif	/* DENDROGRAM_NODE_H_ */
