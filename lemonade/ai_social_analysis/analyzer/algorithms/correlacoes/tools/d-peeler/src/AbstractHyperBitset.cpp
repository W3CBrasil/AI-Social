// Copyright 2007-2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "AbstractHyperBitset.h"

AbstractHyperBitset::AbstractHyperBitset()
{
}

AbstractHyperBitset::~AbstractHyperBitset()
{
}

AbstractHyperBitset& AbstractHyperBitset::operator&=(AbstractHyperBitset& otherAbstractHyperBitset)
{
  return bitwiseAndEqual(&otherAbstractHyperBitset);
}

bool AbstractHyperBitset::operator<(AbstractHyperBitset& otherAbstractHyperBitset) const
{
  return count() < otherAbstractHyperBitset.count();
}

float AbstractHyperBitset::density() const
{
  return static_cast<float>(count()) / static_cast<float>(size());
}

void AbstractHyperBitset::resetValue()
{
  setValue(false);
}

void AbstractHyperBitset::resetValues(const vector<vector<unsigned int> >& bits)
{
  setValues(bits, false);
}

vector<vector<vector<unsigned int> > > AbstractHyperBitset::unidimensionalNRectangles() const
{
  vector<vector<unsigned int> > nRectangle(nbOfDimensions());
  return unidimensionalNRectangles(nRectangle.begin(), nRectangle.begin(), nRectangle.end());
}

bool AbstractHyperBitset::enough0On(const unsigned int threshold, const vector<vector<unsigned int> >& subset) const
{
  int thresholdDistance = threshold;
  return enough0On(thresholdDistance, subset.begin());  
}

bool AbstractHyperBitset::trueOn(const vector<vector<unsigned int> >& subset) const
{
  return trueOn(subset.begin());
}

bool AbstractHyperBitset::falseOn(const vector<vector<unsigned int> >& subset) const
{
  return falseOn(subset.begin());
}

bool AbstractHyperBitset::trueOnPotentialBits(const vector<vector<unsigned int> >& present, const vector<vector<unsigned int> >& potential) const
{
  return trueOnPotentialBits(present.begin(), potential.begin());
}

unsigned int AbstractHyperBitset::countOn(const vector<vector<unsigned int> >& subset) const
{
  return countOn(subset.begin());  
}

pair<unsigned int, vector<vector<unsigned int> > > AbstractHyperBitset::countAndCropOn(const vector<vector<unsigned int> >& subset, const vector<unsigned int>& nbOfElementsInDi) const
{
  unsigned int n = subset.size();
  vector<vector<bool> > coveringElements;
  coveringElements.reserve(n);
  vector<vector<unsigned int> >::const_iterator subsetIt = subset.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      coveringElements.push_back(vector<bool>(subsetIt->size(), false));
      ++subsetIt;
    }
  subsetIt = subset.begin();
  vector<vector<bool> >::iterator coveringElementsIt = coveringElements.begin();
  const unsigned int count = countAndCropOn(subsetIt, coveringElementsIt);
  vector<vector<unsigned int> > croppedSubset(n);
  vector<vector<unsigned int> >::iterator croppedSubsetIt = croppedSubset.begin();
  vector<unsigned int>::const_iterator nbOfElementsInDiIt = nbOfElementsInDi.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      if (subsetIt->size() == *nbOfElementsInDiIt++)
	{
	  *croppedSubsetIt = *subsetIt;
	}
      else
	{
	  vector<bool>::const_iterator coveringElementIt = coveringElementsIt->begin();
	  for (vector<unsigned int>::const_iterator subsetElementIt = subsetIt->begin(); subsetElementIt != subsetIt->end(); ++subsetElementIt)
	    {
	      if (*coveringElementIt++)
		{
		  croppedSubsetIt->push_back(*subsetElementIt);
		}
	    }
	}
      ++croppedSubsetIt;
      ++subsetIt;
      ++coveringElementsIt;
    }
  return pair<unsigned int, vector<vector<unsigned int> > >(count, croppedSubset);
}

vector<vector<unsigned int> > AbstractHyperBitset::setPresent(const vector<vector<unsigned int> >& present, vector<vector<unsigned int> >& potential) const
{
  vector<vector<unsigned int> > newAbsentElements(potential.size());
  setPresent(newAbsentElements.begin(), present.begin(), potential.begin());
  return newAbsentElements;
}
