// Copyright 2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef MIN_SUM_H_
#define MIN_SUM_H_

#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <boost/tokenizer.hpp>

#include "../utilities/NoFileException.h"
#include "../utilities/DataFormatException.h"
#include "TupleMeasure.h"
#include "TupleValueTrie.h"

using namespace boost;

class MinSum final : public TupleMeasure
{
 public:
  MinSum(const string& tupleValueFileName, const char* dimensionSeparator, const char* elementSeparator, const vector<unordered_map<string, unsigned int>>& labels2Ids, const vector<unsigned int>& dimensionOrder, const vector<unsigned int>& cardinalities, const float densityThreshold, const float threshold);
  MinSum* clone() const;

  const bool monotone() const;
  const bool violationAfterPresentAndPotentialDecreased(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent);
  const float optimisticValue() const;

  static void deleteTupleValues();

 protected:
  double minSum;

  static vector<unsigned int> dimensionIds;
  static AbstractTupleValueData* tupleValues;
  static float threshold;
};

#endif /*MIN_SUM_H_*/
