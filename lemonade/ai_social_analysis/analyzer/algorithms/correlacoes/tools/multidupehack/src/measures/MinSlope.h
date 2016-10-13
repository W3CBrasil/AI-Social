// Copyright 2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef MIN_SLOPE_H_
#define MIN_SLOPE_H_

#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <boost/tokenizer.hpp>

#include "../utilities/NoFileException.h"
#include "../utilities/DataFormatException.h"
#include "TupleMeasure.h"
#include "TuplePointTrie.h"

using namespace boost;

class MinSlope final : public TupleMeasure
{
 public:
  MinSlope(const string& tuplePointFileName, const char* dimensionSeparator, const char* elementSeparator, const vector<unordered_map<string, unsigned int>>& labels2Ids, const vector<unsigned int>& dimensionOrder, const vector<unsigned int>& cardinalities, const float densityThreshold, const float threshold);
  MinSlope* clone() const;

  const bool violationAfterPresentIncreased(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent);
  const bool violationAfterPresentAndPotentialDecreased(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent);
  const float optimisticValue() const;

  static void deleteTuplePoints();

 protected:
  SlopeSums minSums;
  SlopeSums maxSums;
  float minSlope;

  static vector<unsigned int> dimensionIds;
  static AbstractTuplePointData* tuplePoints;
  static float threshold;

  void computeOptimisticValue();
};

#endif /*MIN_SLOPE_H_*/
