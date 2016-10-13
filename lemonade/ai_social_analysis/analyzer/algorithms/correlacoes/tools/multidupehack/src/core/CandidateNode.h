// Copyright 2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef CANDIDATE_NODE_H_
#define CANDIDATE_NODE_H_

#include <unordered_set>

#include "Trie.h"

class DendrogramNode;

class CandidateNode
{
 public:
  CandidateNode(const vector<vector<unsigned int>>& nSet);

  const vector<vector<unsigned int>>& getNSet() const;
  const unsigned int getArea() const;
  const double getMembershipSum() const;
  const double getG() const;
  const double getQuadraticErrorVariation() const;
  const float getQuadraticErrorVariationEstimation(const vector<vector<unsigned int>>::const_iterator maxDensityNSetBegin, const double maxDensityMembershipSum, const double maxDensity, const double maxDensityG, const vector<vector<unsigned int>>::const_iterator minDensityNSetBegin, const unsigned int minDensityArea, const double minDensityMembershipSum) const;
  void insertGeneratingPair(const list<DendrogramNode*>::iterator child1It, const list<DendrogramNode*>::iterator child2It, const Trie* data);
  void setQuadraticErrorVariation(const double twoPatternModelG);
  pair<bool, unordered_set<DendrogramNode*>> unlinkGeneratingPairsInvolving(const DendrogramNode* child);

  static void setSimilarityShift(const double similarityShift);
  static const double getMaxMembershipMinusSimilarityShift();
  static const bool smallerQuadraticErrorVariation(const CandidateNode* node1, const CandidateNode* node2);

 protected:
  vector<vector<unsigned int>> nSet;
  unsigned int area;
  double membershipSum;
  double g;
  double quadraticErrorVariation;
  vector<list<DendrogramNode*>::iterator> generatingPairs;
  
  static double maxMembershipMinusSimilarityShift;
};

#endif	/* CANDIDATE_NODE_H_ */
