// Copyright 2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef ABSTRACT_TUPLE_VALUE_DATA_H
#define ABSTRACT_TUPLE_VALUE_DATA_H

#include <vector>

using namespace std;

class AbstractTupleValueData
{
 public:
  virtual ~AbstractTupleValueData();
  virtual AbstractTupleValueData* clone() const = 0;
	
  virtual const bool setTupleValues(const vector<vector<unsigned int>>::const_iterator dimensionIt, const double value, const unsigned int sizeThreshold, const unsigned int lastDimensionCardinality) = 0;
  virtual void sortTubesAndSetSum(double& sum) = 0;
  virtual void decreaseSum(const vector<vector<unsigned int>>& present, const vector<vector<unsigned int>>& potential, const vector<unsigned int>::const_iterator dimensionIdIt, double& sum) const = 0;
  virtual void decreaseSum(const vector<vector<unsigned int>>& present, const vector<vector<unsigned int>>& potential, const vector<unsigned int>& elementsSetAbsent, const vector<unsigned int>::const_iterator dimensionIdIt, const vector<unsigned int>::const_iterator absentDimensionIdIt, double& sum) const = 0;
};

#endif /*ABSTRACT_TUPLE_VALUE_DATA_H*/
