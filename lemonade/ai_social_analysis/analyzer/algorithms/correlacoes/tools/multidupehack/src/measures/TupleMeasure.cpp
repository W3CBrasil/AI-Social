// Copyright 2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "TupleMeasure.h"

vector<TupleMeasure*> TupleMeasure::firstMeasures;
vector<pair<vector<vector<unsigned int>>, vector<vector<unsigned int>>>> TupleMeasure::presentAndPotential;
vector<bool> TupleMeasure::relevantDimensions;
vector<bool> TupleMeasure::relevantDimensionsForMonotoneMeasures;
vector<unsigned int> TupleMeasure::sortedElements;

TupleMeasure::TupleMeasure()
{
  if (firstMeasures.empty())
    {
      firstMeasures.push_back(this);
    }
}

TupleMeasure::TupleMeasure(const TupleMeasure& otherTupleMeasure)
{
  if (&otherTupleMeasure == firstMeasures.back())
    {
      firstMeasures.push_back(this);
      presentAndPotential.push_back(pair<vector<vector<unsigned int>>, vector<vector<unsigned int>>>(presentAndPotential.back()));
    }
}

TupleMeasure::TupleMeasure(TupleMeasure&& otherTupleMeasure)
{
}

TupleMeasure::~TupleMeasure()
{
  if (!firstMeasures.empty() && firstMeasures.back() == this)
    {
      firstMeasures.pop_back();
      if (!presentAndPotential.empty())	// test required in case of exception raised in the constructor of a child class
	{
	  presentAndPotential.pop_back();
	}
    }
}

TupleMeasure& TupleMeasure::operator=(const TupleMeasure& otherTupleMeasure)
{
  if (&otherTupleMeasure == firstMeasures.back())
    {
      presentAndPotential.push_back(pair<vector<vector<unsigned int>>, vector<vector<unsigned int>>>(presentAndPotential.back()));
    }
  return *this;  
}

TupleMeasure& TupleMeasure::operator=(TupleMeasure&& otherTupleMeasure)
{
  return *this;  
}

void TupleMeasure::allMeasuresSet(const vector<unsigned int>& cardinalities)
{
  vector<vector<unsigned int>> potential;
  potential.reserve(cardinalities.size());
  bool isNoMeasureMonotone = true;
  for (unsigned int dimensionId = 0; dimensionId != relevantDimensions.size(); ++dimensionId)
    {
      vector<unsigned int> potentialDimension;
      if (relevantDimensionsForMonotoneMeasures[dimensionId])
	{
	  isNoMeasureMonotone = false;
	  potentialDimension.reserve(cardinalities[dimensionId]);
	  for (unsigned int elementId = 0; elementId != cardinalities[dimensionId]; ++elementId)
	    {
	      potentialDimension.push_back(elementId);
	    }
	}
      potential.push_back(potentialDimension);
    }
  // If unnecessary, clear potential vector (in this way, do not copy them at every copy of the tuple measures)
  if (isNoMeasureMonotone)
    {
      potential.clear();
    }
  presentAndPotential.push_back(pair<vector<vector<unsigned int>>, vector<vector<unsigned int>>>(vector<vector<unsigned int>>(cardinalities.size()), potential));
}

const vector<vector<unsigned int>>& TupleMeasure::present()
{
  return presentAndPotential.back().first;
}

const vector<vector<unsigned int>>& TupleMeasure::potential()
{
  return presentAndPotential.back().second;
}

const bool TupleMeasure::violationAfterAdding(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent)
{
  if (relevantDimensions[dimensionIdOfElementsSetPresent])
    {
      if (this == firstMeasures.back())
	{
	  sortedElements = elementsSetPresent;
	  sort(sortedElements.begin(), sortedElements.end());
	  vector<unsigned int>& potentialDimension = presentAndPotential.back().second[dimensionIdOfElementsSetPresent];
	  vector<unsigned int> newPotential(potentialDimension.size() - sortedElements.size());
	  set_difference(potentialDimension.begin(), potentialDimension.end(), sortedElements.begin(), sortedElements.end(), newPotential.begin());
	  potentialDimension.swap(newPotential);
	  vector<unsigned int>& presentDimension = presentAndPotential.back().first[dimensionIdOfElementsSetPresent];
	  vector<unsigned int> newPresent(presentDimension.size() + elementsSetPresent.size());
	  newPresent.resize(set_union(presentDimension.begin(), presentDimension.end(), sortedElements.begin(), sortedElements.end(), newPresent.begin()) - newPresent.begin());
	  presentDimension.swap(newPresent);
	}
      return violationAfterPresentIncreased(dimensionIdOfElementsSetPresent, sortedElements);
    }
  return false;
}

const bool TupleMeasure::violationAfterRemoving(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent)
{
  if (relevantDimensionsForMonotoneMeasures[dimensionIdOfElementsSetAbsent])
    {
      if (this == firstMeasures.back())
	{
	  sortedElements = elementsSetAbsent;
	  sort(sortedElements.begin(), sortedElements.end());
	  vector<unsigned int>& potentialDimension = presentAndPotential.back().second[dimensionIdOfElementsSetAbsent];
	  vector<unsigned int> newPotential(potentialDimension.size() - sortedElements.size());
	  set_difference(potentialDimension.begin(), potentialDimension.end(), sortedElements.begin(), sortedElements.end(), newPotential.begin());
	  potentialDimension.swap(newPotential);
        }
      return violationAfterPresentAndPotentialDecreased(dimensionIdOfElementsSetAbsent, sortedElements);
    }
  return false;
}

const bool TupleMeasure::violationAfterPresentIncreased(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent)
{
  return false;
}

const bool TupleMeasure::violationAfterPresentAndPotentialDecreased(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent)
{
  return false;
}
