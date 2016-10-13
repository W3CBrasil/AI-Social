// Copyright 2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MaxArea.h"

unsigned int MaxArea::threshold;

MaxArea::MaxArea(const unsigned int nbOfDimensions, const unsigned int thresholdParam): nbOfElementsInDimensions(nbOfDimensions), maxArea(0)
{
  threshold = thresholdParam;
}

MaxArea* MaxArea::clone() const
{
  return new MaxArea(*this);
}

const bool MaxArea::violationAfterAdding(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent)
{
  maxArea = 1;
  unsigned int dimensionId = 0;
  for (unsigned int& nbOfElementsInDimension : nbOfElementsInDimensions)
    {
      if (dimensionId++ == dimensionIdOfElementsSetPresent)
	{
	  nbOfElementsInDimension += elementsSetPresent.size();
	}
      maxArea *= nbOfElementsInDimension;
    }
#ifdef DEBUG
  if (maxArea > threshold)
    {
      cout << threshold << "-maximal area constraint cannot be satisfied -> Prune!" << endl;
    }
#endif
  return maxArea > threshold;
}

const float MaxArea::optimisticValue() const
{
  return -maxArea;
}
