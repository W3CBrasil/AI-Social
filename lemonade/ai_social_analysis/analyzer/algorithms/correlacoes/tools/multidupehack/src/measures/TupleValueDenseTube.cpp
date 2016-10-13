// Copyright 2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "TupleValueDenseTube.h"

TupleValueDenseTube::TupleValueDenseTube(const vector<double>& tubeParam) : tube(tubeParam)
{
}

TupleValueDenseTube* TupleValueDenseTube::clone() const
{
  return new TupleValueDenseTube(*this);
}

const bool TupleValueDenseTube::setTupleValues(const vector<vector<unsigned int>>::const_iterator dimensionIt, const double value, const unsigned int sizeThreshold, const unsigned int lastDimensionCardinality)
{
  for (const unsigned int hyperplaneId : *dimensionIt)
    {
      tube[hyperplaneId] = value;
    }
  return false;
}

void TupleValueDenseTube::sortTubesAndSetSum(double& sum)
{
  for (const double keyValue : tube)
    {
      sum += keyValue;
    }
}

void TupleValueDenseTube::decreaseSum(const vector<unsigned int>& dimension, double& sum) const
{
  for (const unsigned int element : dimension)
    {
      sum -= tube[element];
    }
}
