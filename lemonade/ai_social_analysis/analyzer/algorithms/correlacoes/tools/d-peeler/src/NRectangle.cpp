// Copyright 2007-2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "NRectangle.h"

NRectangle::NRectangle(): present(), coveredArea(0), nbOfLogicOperators(0)
{
}

NRectangle::NRectangle(const NRectangle& otherNRectangle): present(), coveredArea(0), nbOfLogicOperators(0)
{
  copy(otherNRectangle);
}

NRectangle::NRectangle(const vector<vector<unsigned int> >& presentParam): present(presentParam), coveredArea(1), nbOfLogicOperators(0)
{
  vector<vector<unsigned int> >::const_iterator presentElementsIt = present.begin();
  for (unsigned int dimensionId = 0; dimensionId != Tree::n; ++dimensionId)
    {
      unsigned int nbOfPresentElementsInDimension = presentElementsIt->size();
      nbOfLogicOperators += nbOfPresentElementsInDimension;
      coveredArea *= nbOfPresentElementsInDimension;
      ++presentElementsIt;
    }
}

NRectangle::~NRectangle()
{
}

NRectangle& NRectangle::operator=(const NRectangle& otherNRectangle)
{
  copy(otherNRectangle);
  return *this;
}

bool NRectangle::operator<(const NRectangle& otherNRectangle) const
{
  return coverRate() > otherNRectangle.coverRate();
}

ostream& operator<<(ostream& out, const NRectangle& nRectangle)
{
  Tree::printNVector(nRectangle.present, out);
  if (Tree::isSizePrinted)
    {
      out << Tree::patternSizeSeparator;
      bool isFirstSize = true;
      vector<unsigned int>::const_iterator dimensionOrderIt = Tree::dimensionOrder.begin();
      for (unsigned int dimensionId = 0; dimensionId != Tree::n; ++dimensionId)
	{
	  if (isFirstSize)
	    {
	      isFirstSize = false;
	    }
	  else
	    {
	      out << Tree::sizeSeparator;
	    }
	  out << nRectangle.present[*dimensionOrderIt++].size();
	}
    }
  if (Tree::isAreaPrinted)
    {
      float area = 1;
      vector<float>::const_iterator areaCoefficientIt = Tree::areaCoefficients.begin();
      vector<vector<unsigned int> >::const_iterator presentIt = nRectangle.present.begin();
      for (unsigned int dimensionId = 0; dimensionId != Tree::n; ++dimensionId)
	{
	  area *= pow(presentIt->size(), *areaCoefficientIt++);
	  ++presentIt;
	}	  
      out << Tree::sizeAreaSeparator << area;
    }
  return out;
}

void NRectangle::copy(const NRectangle& otherNRectangle)
{
  present = otherNRectangle.present;
  coveredArea = otherNRectangle.coveredArea;
  nbOfLogicOperators = otherNRectangle.nbOfLogicOperators;
}

bool NRectangle::empty() const
{
  return coveredArea == 0;
}

bool NRectangle::multidimensional() const
{
  if (empty())
    {
      return false;
    }
  vector<vector<unsigned int> >::const_iterator presentIt = present.begin();
  unsigned int dimensionId = 1;
  for (; dimensionId != Tree::n && presentIt->size() == 1; ++dimensionId)
    {
      ++presentIt;
    }
  return dimensionId != Tree::n || presentIt->size() == Tree::nbOfElementsInDi.back();
}

float NRectangle::coverRate() const
{
  return static_cast<float>(coveredArea) / static_cast<float>(nbOfLogicOperators);
}

void NRectangle::setValues(const bool value) const
{
  Tree::data->setValues(present, value);
}

void NRectangle::resetValues() const
{
  return setValues(false);
}

bool NRectangle::update()
{
  pair<unsigned int, vector<vector<unsigned int> > > update = Tree::data->countAndCropOn(present, Tree::nbOfElementsInDi);
  unsigned int& newCoveredArea = update.first;
  if (newCoveredArea == coveredArea)
    {
      return false;
    }
  coveredArea = newCoveredArea;
  nbOfLogicOperators = 0;
  present = update.second;
  vector<unsigned int>::const_iterator nbOfElementsInDiIt = Tree::nbOfElementsInDi.begin();
  vector<vector<unsigned int> >::const_iterator presentElementsIt = present.begin();
  for (unsigned int dimensionId = 0; dimensionId != Tree::n; ++dimensionId)
    {
      unsigned int nbOfPresentElementsInDimension = presentElementsIt->size();
      if (nbOfPresentElementsInDimension != *nbOfElementsInDiIt++)
	{
	  nbOfLogicOperators += nbOfPresentElementsInDimension;
	}
      ++presentElementsIt;
    }
  return true;
}
