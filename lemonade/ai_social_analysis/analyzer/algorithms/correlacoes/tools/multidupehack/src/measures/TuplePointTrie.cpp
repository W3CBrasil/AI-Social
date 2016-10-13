// Copyright 2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "TuplePointTrie.h"

TuplePointTrie::TuplePointTrie(const vector<unsigned int>::const_iterator dimensionIdIt, const vector<unsigned int>::const_iterator dimensionIdsEnd, const vector<unsigned int>& cardinalities) : hyperplanes()
{
  const unsigned int cardinality = cardinalities[*dimensionIdIt];
  hyperplanes.reserve(cardinality);
  if (dimensionIdIt + 2 == dimensionIdsEnd)
    {
      for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
	{
	  hyperplanes.push_back(new TuplePointSparseTube());
	}
    }
  else
    {
      const vector<unsigned int>::const_iterator nextDimensionIdIt = dimensionIdIt + 1;
      for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
	{
	  hyperplanes.push_back(new TuplePointTrie(nextDimensionIdIt, dimensionIdsEnd, cardinalities));
	}
    }
}

TuplePointTrie::TuplePointTrie(const TuplePointTrie& otherTuplePointTrie): hyperplanes()
{
  copy(otherTuplePointTrie);
}

TuplePointTrie::TuplePointTrie(TuplePointTrie&& otherTuplePointTrie): hyperplanes(std::move(otherTuplePointTrie.hyperplanes))
{
}

TuplePointTrie::~TuplePointTrie()
{
  for (AbstractTuplePointData* hyperplane : hyperplanes)
    {
      delete hyperplane;
    }
}

TuplePointTrie* TuplePointTrie::clone() const
{
  return new TuplePointTrie(*this);
}

TuplePointTrie& TuplePointTrie::operator=(const TuplePointTrie& otherTuplePointTrie)
{
  copy(otherTuplePointTrie);
  return *this;
}

TuplePointTrie& TuplePointTrie::operator=(TuplePointTrie&& otherTuplePointTrie)
{
  hyperplanes = std::move(otherTuplePointTrie.hyperplanes);
  return *this;
}

void TuplePointTrie::copy(const TuplePointTrie& otherTuplePointTrie)
{
  hyperplanes.reserve(otherTuplePointTrie.hyperplanes.size());
  for (const AbstractTuplePointData* hyperplane : otherTuplePointTrie.hyperplanes)
    {
      hyperplanes.push_back(hyperplane->clone());
    }
}

const bool TuplePointTrie::setTuplePoints(const vector<vector<unsigned int>>::const_iterator dimensionIt, const pair<double, double>& point, const unsigned int sizeThreshold, const unsigned int lastDimensionCardinality)
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int element : *dimensionIt)
    {
      AbstractTuplePointData* hyperplane = hyperplanes[element];
      if (hyperplane->setTuplePoints(nextDimensionIt, point, sizeThreshold, lastDimensionCardinality))
	{
	  TuplePointDenseTube* newHyperplane = new TuplePointDenseTube(static_cast<TuplePointSparseTube*>(hyperplane)->getDenseRepresentation(lastDimensionCardinality));
	  delete hyperplane;
	  hyperplanes[element] = newHyperplane;
	}
    }
  return false;
}

void TuplePointTrie::sortTubesAndGetMinCoordinates(double& minX, double& minY)
{
  for (AbstractTuplePointData* hyperplane : hyperplanes)
    {
      hyperplane->sortTubesAndGetMinCoordinates(minX, minY);
    }
}

void TuplePointTrie::translate(const double deltaX, const double deltaY)
{
  for (AbstractTuplePointData* hyperplane : hyperplanes)
    {
      hyperplane->translate(deltaX, deltaY);
    }
}

void TuplePointTrie::setSlopeSums(SlopeSums& slopeSums) const
{
  for (AbstractTuplePointData* hyperplane : hyperplanes)
    {
      hyperplane->setSlopeSums(slopeSums);
    }
}

void TuplePointTrie::increaseSlopeSums(const vector<vector<unsigned int>>& present, const vector<unsigned int>::const_iterator dimensionIdIt, SlopeSums& slopeSums) const
{
  const vector<unsigned int>::const_iterator nextDimensionIdIt = dimensionIdIt + 1;
  for (const unsigned int presentElement : present[*dimensionIdIt])
    {
      hyperplanes[presentElement]->increaseSlopeSums(present, nextDimensionIdIt, slopeSums);
    }
}

void TuplePointTrie::decreaseSlopeSums(const vector<vector<unsigned int>>& present, const vector<vector<unsigned int>>& potential, const vector<unsigned int>::const_iterator dimensionIdIt, SlopeSums& slopeSums) const
{
  const vector<unsigned int>::const_iterator nextDimensionIdIt = dimensionIdIt + 1;
  for (const unsigned int presentElement : present[*dimensionIdIt])
    {
      hyperplanes[presentElement]->decreaseSlopeSums(present, potential, nextDimensionIdIt, slopeSums);
    }
  for (const unsigned int potentialElement : potential[*dimensionIdIt])
    {
      hyperplanes[potentialElement]->decreaseSlopeSums(present, potential, nextDimensionIdIt, slopeSums);
    }
}

void TuplePointTrie::increaseSlopeSums(const vector<vector<unsigned int>>& present, const vector<unsigned int>& elementsSetPresent, const vector<unsigned int>::const_iterator dimensionIdIt, const vector<unsigned int>::const_iterator presentDimensionIdIt, SlopeSums& slopeSums) const
{
  const vector<unsigned int>::const_iterator nextDimensionIdIt = dimensionIdIt + 1;
  if (dimensionIdIt == presentDimensionIdIt)
    {
      for (const unsigned int elementSetPresent : elementsSetPresent)
	{
	  hyperplanes[elementSetPresent]->increaseSlopeSums(present, nextDimensionIdIt, slopeSums);
	}
      return;
    }
  for (const unsigned int presentElement : present[*dimensionIdIt])
    {
      hyperplanes[presentElement]->increaseSlopeSums(present, elementsSetPresent, nextDimensionIdIt, presentDimensionIdIt, slopeSums);
    }
}

void TuplePointTrie::decreaseSlopeSums(const vector<vector<unsigned int>>& present, const vector<vector<unsigned int>>& potential, const vector<unsigned int>& elementsSetAbsent, const vector<unsigned int>::const_iterator dimensionIdIt, const vector<unsigned int>::const_iterator absentDimensionIdIt, SlopeSums& slopeSums) const
{
  const vector<unsigned int>::const_iterator nextDimensionIdIt = dimensionIdIt + 1;
  if (dimensionIdIt == absentDimensionIdIt)
    {
      for (const unsigned int elementSetAbsent : elementsSetAbsent)
	{
	  hyperplanes[elementSetAbsent]->decreaseSlopeSums(present, potential, nextDimensionIdIt, slopeSums);
	}
      return;
    }
  for (const unsigned int presentElement : present[*dimensionIdIt])
    {
      hyperplanes[presentElement]->decreaseSlopeSums(present, potential, elementsSetAbsent, nextDimensionIdIt, absentDimensionIdIt, slopeSums);
    }
  for (const unsigned int potentialElement : potential[*dimensionIdIt])
    {
      hyperplanes[potentialElement]->decreaseSlopeSums(present, potential, elementsSetAbsent, nextDimensionIdIt, absentDimensionIdIt, slopeSums);
    }
}
