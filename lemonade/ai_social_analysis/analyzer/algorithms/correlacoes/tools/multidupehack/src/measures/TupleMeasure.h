// Copyright 2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef TUPLE_MEASURE_H
#define TUPLE_MEASURE_H

#include <algorithm>

#include "Measure.h"

class TupleMeasure : public Measure
{
 public:
  TupleMeasure();
  TupleMeasure(const TupleMeasure& otherTupleMeasure);
  TupleMeasure(TupleMeasure&& otherTupleMeasure);
  virtual ~TupleMeasure();

  TupleMeasure& operator=(const TupleMeasure& otherTupleMeasure);
  TupleMeasure& operator=(TupleMeasure&& otherTupleMeasure);

  static void allMeasuresSet(const vector<unsigned int>& cardinalities); /* must be called after the construction of all tuple measures in (SkyPattern)Tree::initMeasures */
  static const vector<vector<unsigned int>>& present(); /* elements are sorted */
  static const vector<vector<unsigned int>>& potential(); /* elements are sorted */
  
  const bool violationAfterAdding(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent);
  const bool violationAfterRemoving(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent);

 protected:
  static vector<TupleMeasure*> firstMeasures;
  static vector<pair<vector<vector<unsigned int>>, vector<vector<unsigned int>>>> presentAndPotential; /* elements are sorted */
  static vector<bool> relevantDimensions;
  static vector<bool> relevantDimensionsForMonotoneMeasures;
  static vector<unsigned int> sortedElements;
  
  virtual const bool violationAfterPresentIncreased(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent); /* elementsSetPresent is sorted */
  virtual const bool violationAfterPresentAndPotentialDecreased(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent); /* elementsSetAbsent is sorted */
};

#endif /*TUPLE_MEASURE_H*/
