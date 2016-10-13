// Copyright 2012,2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef NOISY_TUPLES_H_
#define NOISY_TUPLES_H_

#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <boost/lexical_cast.hpp>

#include "../utilities/vector_hash.h"
#include "Dimension.h"

using namespace std;
using namespace boost;

class NoisyTuples
{
 public:
  NoisyTuples();

  const bool empty() const;
  unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator begin() const;  
  unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator end() const;  

  void insert(const vector<unsigned int>& tuple, const double membership);

  static vector<Dimension*> preProcess(const vector<unsigned int>& nbOfTuples, const vector<unsigned int>& minimalNbOfTuples, const vector<double>& epsilonVector, const vector<unsigned int>& symDimensionIds, vector<vector<NoisyTuples*>>& hyperplanes);
  static vector<unsigned int> createNewIds(const vector<NoisyTuples*>& hyperplanesInDimension, const unsigned int nbOfValidHyperplanes); /* for regular attributes */
  static vector<unsigned int> createNewIds(const vector<unsigned int>& symDimensionIdsParam, const vector<vector<NoisyTuples*>>& hyperplanesParam, const unsigned int nbOfValidHyperplanes); /* for symmetric attributes */
  static vector<unsigned int> createNewIds(const vector<NoisyTuples*>& hyperplanesInDimension, const unsigned int nbOfValidHyperplanes, const vector<string>& numericLabels); /* for metric attributes */
  
 protected:
  unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>> tuples;
  double lowestMembershipInMinimalNSet; /* 2 when the hyperplane is unprocessed; 3 when currently/already cleared */

  static vector<unsigned int> minimalNbOfTuples;
  static vector<double> epsilonVector;
  static vector<unsigned int> symDimensionIds;
  static vector<vector<NoisyTuples*>> hyperplanes;
  static vector<Dimension*> dimensions;
  static vector<unordered_set<unsigned int>> hyperplanesToClear;

  const bool erase(const vector<unsigned int>& tuple); /* returns whether the (already processed) hyperplane should be checked again */
  void clear(const unsigned int dimensionId, const unsigned int hyperplaneId);
  void clearIfTooNoisy(const unsigned int dimensionId, const unsigned int hyperplaneId);

  static vector<pair<unsigned int, double>> getIds2TotalMemberships(const vector<NoisyTuples*>& hyperplanesInDimension, const unsigned int nbOfValidHyperplanes);
};

#endif /*NOISY_TUPLES_H_*/
