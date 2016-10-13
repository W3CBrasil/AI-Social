// Copyright 2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "TupleValueTrie.h"

TupleValueTrie::TupleValueTrie(const vector<unsigned int>::const_iterator dimensionIdIt, const vector<unsigned int>::const_iterator dimensionIdsEnd, const vector<unsigned int>& cardinalities) : hyperplanes()
{
  const unsigned int cardinality = cardinalities[*dimensionIdIt];
  hyperplanes.reserve(cardinality);
  if (dimensionIdIt + 2 == dimensionIdsEnd)
    {
      for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
	{
	  hyperplanes.push_back(new TupleValueSparseTube());
	}
    }
  else
    {
      const vector<unsigned int>::const_iterator nextDimensionIdIt = dimensionIdIt + 1;
      for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
	{
	  hyperplanes.push_back(new TupleValueTrie(nextDimensionIdIt, dimensionIdsEnd, cardinalities));
	}
    }
}

TupleValueTrie::TupleValueTrie(const TupleValueTrie& otherTupleValueTrie): hyperplanes()
{
  copy(otherTupleValueTrie);
}

TupleValueTrie::TupleValueTrie(TupleValueTrie&& otherTupleValueTrie): hyperplanes(std::move(otherTupleValueTrie.hyperplanes))
{
}

TupleValueTrie::~TupleValueTrie()
{
  for (AbstractTupleValueData* hyperplane : hyperplanes)
    {
      delete hyperplane;
    }
}

TupleValueTrie* TupleValueTrie::clone() const
{
  return new TupleValueTrie(*this);
}

TupleValueTrie& TupleValueTrie::operator=(const TupleValueTrie& otherTupleValueTrie)
{
  copy(otherTupleValueTrie);
  return *this;
}

TupleValueTrie& TupleValueTrie::operator=(TupleValueTrie&& otherTupleValueTrie)
{
  hyperplanes = std::move(otherTupleValueTrie.hyperplanes);
  return *this;
}

void TupleValueTrie::copy(const TupleValueTrie& otherTupleValueTrie)
{
  hyperplanes.reserve(otherTupleValueTrie.hyperplanes.size());
  for (const AbstractTupleValueData* hyperplane : otherTupleValueTrie.hyperplanes)
    {
      hyperplanes.push_back(hyperplane->clone());
    }
}

const bool TupleValueTrie::setTupleValues(const vector<vector<unsigned int>>::const_iterator dimensionIt, const double value, const unsigned int sizeThreshold, const unsigned int lastDimensionCardinality)
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int element : *dimensionIt)
    {
      AbstractTupleValueData* hyperplane = hyperplanes[element];
      if (hyperplane->setTupleValues(nextDimensionIt, value, sizeThreshold, lastDimensionCardinality))
	{
	  TupleValueDenseTube* newHyperplane = new TupleValueDenseTube(static_cast<TupleValueSparseTube*>(hyperplane)->getDenseRepresentation(lastDimensionCardinality));
	  delete hyperplane;
	  hyperplanes[element] = newHyperplane;
	}
    }
  return false;
}

void TupleValueTrie::sortTubesAndSetSum(double& sum)
{
  for (AbstractTupleValueData* hyperplane : hyperplanes)
    {
      hyperplane->sortTubesAndSetSum(sum);
    }
}

void TupleValueTrie::decreaseSum(const vector<vector<unsigned int>>& present, const vector<vector<unsigned int>>& potential, const vector<unsigned int>& elementsSetAbsent, const vector<unsigned int>::const_iterator dimensionIdIt, const vector<unsigned int>::const_iterator absentDimensionIdIt, double& sum) const
{
  const vector<unsigned int>::const_iterator nextDimensionIdIt = dimensionIdIt + 1;
  if (dimensionIdIt == absentDimensionIdIt)
    {
      for (const unsigned int elementSetAbsent : elementsSetAbsent)
	{
	  hyperplanes[elementSetAbsent]->decreaseSum(present, potential, nextDimensionIdIt, sum);
	}
      return;
    }
  for (const unsigned int presentElement : present[*dimensionIdIt])
    {
      hyperplanes[presentElement]->decreaseSum(present, potential, elementsSetAbsent, nextDimensionIdIt, absentDimensionIdIt, sum);
    }
  for (const unsigned int potentialElement : potential[*dimensionIdIt])
    {
      hyperplanes[potentialElement]->decreaseSum(present, potential, elementsSetAbsent, nextDimensionIdIt, absentDimensionIdIt, sum);
    }
}

void TupleValueTrie::decreaseSum(const vector<vector<unsigned int>>& present, const vector<vector<unsigned int>>& potential, const vector<unsigned int>::const_iterator dimensionIdIt, double& sum) const
{
  const vector<unsigned int>::const_iterator nextDimensionIdIt = dimensionIdIt + 1;
  for (const unsigned int presentElement : present[*dimensionIdIt])
    {
      hyperplanes[presentElement]->decreaseSum(present, potential, nextDimensionIdIt, sum);
    }
  for (const unsigned int potentialElement : potential[*dimensionIdIt])
    {
      hyperplanes[potentialElement]->decreaseSum(present, potential, nextDimensionIdIt, sum);
    }
}
