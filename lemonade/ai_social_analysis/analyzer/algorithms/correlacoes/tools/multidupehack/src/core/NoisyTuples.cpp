// Copyright 2012,2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "NoisyTuples.h"

const bool smallerSecond(const pair<unsigned int, double>& pair1, const pair<unsigned int, double>& pair2)
{
  return pair1.second < pair2.second;
}

vector<unsigned int> getFirst2Index(const vector<pair<unsigned int, double>>& pairs, const unsigned int nbOfFirst)
{
  vector<unsigned int> oldIds2NewIds(nbOfFirst, numeric_limits<unsigned int>::max());
  unsigned int hyperplaneId = 0;
  for (const pair<unsigned int, double>& pair : pairs)
    {
      oldIds2NewIds[pair.first] = hyperplaneId++;
    }
  return oldIds2NewIds;
}

vector<unsigned int> NoisyTuples::minimalNbOfTuples;
vector<double> NoisyTuples::epsilonVector;
vector<unsigned int> NoisyTuples::symDimensionIds;
vector<vector<NoisyTuples*>> NoisyTuples::hyperplanes;
vector<Dimension*> NoisyTuples::dimensions;
vector<unordered_set<unsigned int>> NoisyTuples::hyperplanesToClear;

NoisyTuples::NoisyTuples() : tuples(), lowestMembershipInMinimalNSet(2)
{
}

const bool NoisyTuples::empty() const
{
  return lowestMembershipInMinimalNSet == 3;
}

unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator NoisyTuples::begin() const
{
  return tuples.begin();
}

unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator NoisyTuples::end() const
{
  return tuples.end();
}

void NoisyTuples::insert(const vector<unsigned int>& tuple, const double membership)
{
  tuples[tuple] = membership;
}

const bool NoisyTuples::erase(const vector<unsigned int>& tuple)
{
  const unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::iterator tupleToEraseIt = tuples.find(tuple);
  if (tupleToEraseIt != tuples.end())
    {
      // Because lowestMembershipInMinimalNSet initially is 2 and is set to 3 when an hyperplane is already/currently cleared, the following test can only pass for an already processed hyperplane that is yet to be found too noisy
      if (tupleToEraseIt->second >= lowestMembershipInMinimalNSet)
	{
	  tuples.erase(tupleToEraseIt);
	  return true;
	}
      tuples.erase(tupleToEraseIt);
    }
  return false;
}

void NoisyTuples::clear(const unsigned int dimensionId, const unsigned int hyperplaneId)
{
  dimensions[dimensionId]->decrementCardinality();
  for (unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator tupleIt = tuples.begin(); tupleIt != tuples.end(); tupleIt = tuples.erase(tupleIt))
    {
      // Erasing the same tuple from the orthogonal hyperplanes
      vector<vector<NoisyTuples*>>::iterator hyperplanesInDimensionIt = hyperplanes.begin();
      vector<unsigned int>::const_iterator elementIt = tupleIt->first.begin();
      vector<unsigned int> orthogonalTuple;
      orthogonalTuple.reserve(tupleIt->first.size());
      // Constructing the first orthogonal tuple
      if (dimensionId < 2)
	{
	  orthogonalTuple.push_back(hyperplaneId);
	  orthogonalTuple.insert(orthogonalTuple.end(), elementIt + 1, tupleIt->first.end());
	}
      else
	{
	  const vector<unsigned int>::const_iterator suffixBegin = elementIt + dimensionId;
	  orthogonalTuple.insert(orthogonalTuple.end(), elementIt + 1, suffixBegin);
	  orthogonalTuple.push_back(hyperplaneId);
	  orthogonalTuple.insert(orthogonalTuple.end(), suffixBegin, tupleIt->first.end());
	}
      // Erasing the tuple from the orthogonal hyperplanes whose dimension ids are lesser than that of this hyperplane
      vector<unsigned int>::iterator orthogonalTupleIt = orthogonalTuple.begin();
      unsigned int orthogonalDimensionId = 0;
      for (; orthogonalDimensionId != dimensionId; ++orthogonalDimensionId)
	{
	  NoisyTuples& orthogonalHyperplane = *(*hyperplanesInDimensionIt)[*elementIt];
	  if (orthogonalHyperplane.erase(orthogonalTuple))
	    {
	      hyperplanesToClear[orthogonalDimensionId].insert(*elementIt);
	    }
	  *orthogonalTupleIt++ = *elementIt++;
	  ++hyperplanesInDimensionIt;
	}
      if (++orthogonalDimensionId != hyperplanes.size())
	{
	  // Erasing the tuple from the orthogonal hyperplanes whose dimension ids are greater than that of this hyperplane
	  for (*orthogonalTupleIt = hyperplaneId; ; *++orthogonalTupleIt = *elementIt++)
	    {
	      NoisyTuples& orthogonalHyperplane = *(*++hyperplanesInDimensionIt)[*elementIt];
	      if (orthogonalHyperplane.erase(orthogonalTuple))
		{
		  hyperplanesToClear[orthogonalDimensionId].insert(*elementIt);
		}
	      if (++orthogonalDimensionId == hyperplanes.size())
		{
		  break;
		}
	    }
	}
    }
}

void NoisyTuples::clearIfTooNoisy(const unsigned int dimensionId, const unsigned int hyperplaneId)
{
  // Given the minimal size constraints, computing the minimal possible noise in a pattern involving this hyperplane
  double minNoise = minimalNbOfTuples[dimensionId];
  if (!tuples.empty())
    {
      multiset<double> highestMembershipValues;
      unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator tupleIt = tuples.begin();
      for (unsigned int minNbOfTuples = minimalNbOfTuples[dimensionId]; tupleIt != tuples.end() && minNbOfTuples != 0; --minNbOfTuples)
	{
	  highestMembershipValues.insert(tupleIt->second);
	  ++tupleIt;
	}
      lowestMembershipInMinimalNSet = *(highestMembershipValues.begin());
      for (; tupleIt != tuples.end(); ++tupleIt)
	{
	  const double membership = tupleIt->second;
	  if (membership > lowestMembershipInMinimalNSet)
	    {
	      highestMembershipValues.erase(highestMembershipValues.begin());
	      highestMembershipValues.insert(membership);
	      lowestMembershipInMinimalNSet = *(highestMembershipValues.begin());
	    }
	}
      for (const double highestMembershipValue : highestMembershipValues)
	{
	  minNoise -= highestMembershipValue;
	}
    }
  if (minNoise > epsilonVector[dimensionId])
    {
      // This hyperplane cannot possibly be in a pattern respecting the minimal size constraints: clear it
      if (dimensions[dimensionId]->symmetric())
	{
	  for (const unsigned int symDimensionId : symDimensionIds)
	    {
	      hyperplanes[symDimensionId][hyperplaneId]->lowestMembershipInMinimalNSet = 3; // To never test again if the any of the symmetric hyperplane is too noisy
	      hyperplanes[symDimensionId][hyperplaneId]->clear(symDimensionId, hyperplaneId);
	    }
	}
      else
	{
	  lowestMembershipInMinimalNSet = 3; // To never test again if this hyperplane is too noisy
	  clear(dimensionId, hyperplaneId);
	}
    }
}

vector<Dimension*> NoisyTuples::preProcess(const vector<unsigned int>& nbOfTuples, const vector<unsigned int>& minimalNbOfTuplesParam, const vector<double>& epsilonVectorParam, const vector<unsigned int>& symDimensionIdsParam, vector<vector<NoisyTuples*>>& hyperplanesParam)
{
  minimalNbOfTuples = minimalNbOfTuplesParam;
  epsilonVector = epsilonVectorParam;
  symDimensionIds = symDimensionIdsParam;
  hyperplanes = hyperplanesParam;
  const unsigned int n = hyperplanes.size();
  dimensions.reserve(n);
  hyperplanesToClear.resize(n);
  // Consider the dimensions in decreasing order of (minimalNbOfTuples - epsilon) / nbOfTuples
  vector<pair<double, unsigned int>> order;
  order.reserve(n);
  const vector<unsigned int>::const_iterator symDimensionIdEnd = symDimensionIds.end();
  vector<unsigned int>::const_iterator symDimensionIdIt = symDimensionIds.begin();
  vector<vector<NoisyTuples*>>::iterator hyperplanesInDimensionIt = hyperplanes.begin();
  vector<double>::const_iterator epsilonIt = epsilonVector.begin();
  vector<unsigned int>::const_iterator minimalNbOfTuplesIt = minimalNbOfTuples.begin();
  vector<unsigned int>::const_iterator nbOfTuplesIt = nbOfTuples.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      order.push_back(pair<double, unsigned int>((*epsilonIt++ - *minimalNbOfTuplesIt++) / *nbOfTuplesIt++, dimensionId));
      if (symDimensionIdIt != symDimensionIdEnd && *symDimensionIdIt == dimensionId)
	{
	  ++symDimensionIdIt;
	  dimensions.push_back(new Dimension(dimensionId, hyperplanesInDimensionIt->size(), true));
	}
      else
	{
	  dimensions.push_back(new Dimension(dimensionId, hyperplanesInDimensionIt->size(), false));
	}
      ++hyperplanesInDimensionIt;
    }
  sort(order.begin(), order.end());
  for (const pair<double, unsigned int>& orderPair : order)
    {
      if (minimalNbOfTuples[orderPair.second] != 0)
	{
	  vector<NoisyTuples*>& hyperplanesInDimension = hyperplanes[orderPair.second];
	  vector<NoisyTuples*>::iterator hyperplaneIt = hyperplanesInDimension.begin();
	  for (unsigned int hyperplaneId = 0; hyperplaneId != hyperplanesInDimension.size(); ++hyperplaneId)
	    {
	      if ((*hyperplaneIt)->lowestMembershipInMinimalNSet != 3)
		{
		  // **hyperplaneIt is not a symmetric hyperplane that has already been erased
		  (*hyperplaneIt)->clearIfTooNoisy(orderPair.second, hyperplaneId);
		}
	      ++hyperplaneIt;
	    }
	}
    }
  vector<pair<double, unsigned int>>::const_iterator orderIt = order.begin();
  while (true)
    {
      for (; orderIt != order.end() && hyperplanesToClear[orderIt->second].empty(); ++orderIt)
	{
	}
      if (orderIt == order.end())
	{
	  break;
	}
      unordered_set<unsigned int>& hyperplaneIdsToClearInDimension = hyperplanesToClear[orderIt->second];
      vector<NoisyTuples*>& hyperplanesToClearInDimension = hyperplanes[orderIt->second];
      unordered_set<unsigned int>::iterator hyperplaneIdIt = hyperplaneIdsToClearInDimension.begin();
      for (; hyperplaneIdIt != hyperplaneIdsToClearInDimension.end() && hyperplanesToClearInDimension[*hyperplaneIdIt]->lowestMembershipInMinimalNSet == 3; hyperplaneIdIt = hyperplaneIdsToClearInDimension.erase(hyperplaneIdIt))
	{
	}
      if (hyperplaneIdIt == hyperplaneIdsToClearInDimension.end())
	{
	  ++orderIt;
	}
      else
	{
	  hyperplanesToClearInDimension[*hyperplaneIdIt]->clearIfTooNoisy(orderIt->second, *hyperplaneIdIt);
	  hyperplaneIdsToClearInDimension.erase(hyperplaneIdIt);
	  orderIt = order.begin();
	}
    }
  // PERF: Clear in a tau-contiguous dimension too small sets of elements that are more than tau-far distant from any other element in the dimension
  return dimensions;
}

// Order the new ids by increasing total membership to take advantage of the prefetch buffer
vector<unsigned int> NoisyTuples::createNewIds(const vector<NoisyTuples*>& hyperplanesInDimension, const unsigned int nbOfValidHyperplanes)
{
  vector<pair<unsigned int, double>> ids2TotalMemberships = getIds2TotalMemberships(hyperplanesInDimension, nbOfValidHyperplanes);
  sort(ids2TotalMemberships.begin(), ids2TotalMemberships.end(), smallerSecond);
  return getFirst2Index(ids2TotalMemberships, hyperplanesInDimension.size());
}

// Order the new ids of the symmetric elements by increasing total membership in all symmetric dimensions
vector<unsigned int> NoisyTuples::createNewIds(const vector<unsigned int>& symDimensionIdsParam, const vector<vector<NoisyTuples*>>& hyperplanesParam, const unsigned int nbOfValidHyperplanes)
{
  const vector<unsigned int>::const_iterator lastSymDimensionIdIt = --symDimensionIdsParam.end();
  vector<pair<unsigned int, double>> ids2TotalMemberships = getIds2TotalMemberships(hyperplanesParam[*lastSymDimensionIdIt], nbOfValidHyperplanes);
  for (vector<unsigned int>::const_iterator symDimensionIdIt = symDimensionIdsParam.begin(); symDimensionIdIt != lastSymDimensionIdIt; ++symDimensionIdIt)
    {
      vector<pair<unsigned int, double>>::iterator id2TotalMembershipIt = ids2TotalMemberships.begin();
      for (const NoisyTuples* hyperplane : hyperplanesParam[*symDimensionIdIt])
	{
	  if (hyperplane->lowestMembershipInMinimalNSet != 3)
	    {
	      for (const pair<vector<unsigned int>, double>& tuple : hyperplane->tuples)
		{
		  id2TotalMembershipIt->second += tuple.second;
		}
	      ++id2TotalMembershipIt;
	    }
	}
    }
  sort(ids2TotalMemberships.begin(), ids2TotalMemberships.end(), smallerSecond);
  return getFirst2Index(ids2TotalMemberships, hyperplanesParam[*lastSymDimensionIdIt].size());
}

// Order the new ids by increasing numeric label
vector<unsigned int> NoisyTuples::createNewIds(const vector<NoisyTuples*>& hyperplanesInDimension, const unsigned int nbOfValidHyperplanes, const vector<string>& numericLabels)
{
  vector<pair<unsigned int, double>> ids2NumericLabels;
  ids2NumericLabels.reserve(nbOfValidHyperplanes);
  unsigned int hyperplaneId = 0;
  for (const NoisyTuples* hyperplane : hyperplanesInDimension)
    {
      if (hyperplane->lowestMembershipInMinimalNSet != 3)
	{
	  ids2NumericLabels.push_back(pair<unsigned int, double>(hyperplaneId, lexical_cast<double>(numericLabels[hyperplaneId])));
	}
      ++hyperplaneId;
    }
  sort(ids2NumericLabels.begin(), ids2NumericLabels.end(), smallerSecond);
  return getFirst2Index(ids2NumericLabels, hyperplanesInDimension.size());
}

vector<pair<unsigned int, double>> NoisyTuples::getIds2TotalMemberships(const vector<NoisyTuples*>& hyperplanesInDimension, const unsigned int nbOfValidHyperplanes)
{
  vector<pair<unsigned int, double>> ids2TotalMemberships;
  ids2TotalMemberships.reserve(nbOfValidHyperplanes);
  unsigned int hyperplaneId = 0;
  for (const NoisyTuples* hyperplane : hyperplanesInDimension)
    {
      if (hyperplane->lowestMembershipInMinimalNSet != 3)
	{
	  double totalMembership = 0;
	  for (const pair<vector<unsigned int>, double>& tuple : hyperplane->tuples)
	    {
	      totalMembership += tuple.second;
	    }
	  ids2TotalMemberships.push_back(pair<unsigned int, double>(hyperplaneId, totalMembership));
	}
      ++hyperplaneId;
    }
  return ids2TotalMemberships;
}
