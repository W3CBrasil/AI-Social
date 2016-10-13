// Copyright 2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "TupleValueSparseTube.h"

TupleValueSparseTube::TupleValueSparseTube() : tube()
{
}

TupleValueSparseTube* TupleValueSparseTube::clone() const
{
  return new TupleValueSparseTube(*this);
}

vector<double> TupleValueSparseTube::getDenseRepresentation(const unsigned int nbOfHyperplanes) const
{
  vector<double> denseRepresentation(nbOfHyperplanes);
  for (const pair<unsigned int, double>& entry : tube)
    {
      denseRepresentation[entry.first] = entry.second;
    }
  return denseRepresentation;
}

const bool TupleValueSparseTube::setTupleValues(const vector<vector<unsigned int>>::const_iterator dimensionIt, const double value, const unsigned int sizeThreshold, const unsigned int lastDimensionCardinality)
{
  for (const unsigned int hyperplaneId : *dimensionIt)
    {
      tube.push_back(pair<unsigned int, double>(hyperplaneId, value));
    }
  return tube.size() > sizeThreshold;
}

vector<pair<unsigned int, double>>::const_iterator TupleValueSparseTube::begin() const
{
  return tube.begin();
}

vector<pair<unsigned int, double>>::const_iterator TupleValueSparseTube::end() const
{
  return tube.end();
}

void TupleValueSparseTube::sortTubesAndSetSum(double& sum)
{
  tube.resize(tube.size());
  sort(tube.begin(), tube.end());  
  for (const pair<unsigned int, double>& keyValue : tube)
    {
      sum += keyValue.second;
    }
}

void TupleValueSparseTube::decreaseSum(const vector<unsigned int>& dimension, double& sum) const
{
  const vector<pair<unsigned int, double>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, double>>::const_iterator tubeBegin = tube.begin();
  for (const unsigned int element : dimension)
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, double>(element, 0));
      if (tubeBegin != tubeEnd && tubeBegin->first == element)
	{
	  sum -= tubeBegin->second;
	}
    }
}
