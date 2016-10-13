// Copyright 2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MaxSize.h"

vector<unsigned int> MaxSize::thresholds;

MaxSize::MaxSize(const unsigned int dimensionIdParam, const unsigned int threshold): dimensionId(dimensionIdParam), maxSize(0)
{
  if (dimensionId >= thresholds.size())
    {
      thresholds.resize(dimensionId + 1);
    }
  thresholds[dimensionId] = threshold;
}

MaxSize* MaxSize::clone() const
{
  return new MaxSize(*this);
}

const bool MaxSize::violationAfterAdding(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent)
{
  if (dimensionIdOfElementsSetPresent == dimensionId)
    {
      maxSize += elementsSetPresent.size();
#ifdef DEBUG
      if (maxSize > thresholds[dimensionId])
	{
	  cout << thresholds[dimensionId] << "-maximal size constraint on attribute " << internal2ExternalAttributeOrder[dimensionId] << " cannot be satisfied -> Prune!" << endl;
	}
#endif
      return maxSize > thresholds[dimensionId];
    }
  return false;
}

const float MaxSize::optimisticValue() const
{
  return -maxSize;
}
