// Copyright 2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MinArea.h"

unsigned int MinArea::threshold;

MinArea::MinArea(const vector<unsigned int>& nbOfElementsInDimensionsParam, const unsigned int thresholdParam): nbOfElementsInDimensions(nbOfElementsInDimensionsParam), minArea(0)
{
  threshold = thresholdParam;
}

MinArea* MinArea::clone() const
{
  return new MinArea(*this);
}

const bool MinArea::monotone() const
{
  return true;
}

const bool MinArea::violationAfterRemoving(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent)
{
  minArea = 1;
  unsigned int dimensionId = 0;
  for (unsigned int& nbOfElementsInDimension : nbOfElementsInDimensions)
    {
      if (dimensionId++ == dimensionIdOfElementsSetAbsent)
	{
	  nbOfElementsInDimension -= elementsSetAbsent.size();
	}
      minArea *= nbOfElementsInDimension;
    }
#ifdef DEBUG
  if (minArea < threshold)
    {
      cout << threshold << "-minimal area constraint cannot be satisfied -> Prune!" << endl;
    }
#endif
  return minArea < threshold;
}

const float MinArea::optimisticValue() const
{
  return minArea;
}
