// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015,2016 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MetricAttribute.h"

vector<double> MetricAttribute::tauVector;
vector<vector<double>> MetricAttribute::timestampsVector;

MetricAttribute::MetricAttribute(const vector<unsigned int>& nbOfValuesPerAttribute, const double epsilon, const vector<string>& labels, const double tau): Attribute(nbOfValuesPerAttribute, epsilon, labels)
{
  tauVector.resize(id + 1);
  tauVector.back() = tau;
  timestampsVector.resize(id + 1);
  vector<double>& timestampsOfThisAttribute = timestampsVector.back();
  timestampsOfThisAttribute.reserve(labels.size());
  for (const string& label : labels)
    {
      timestampsOfThisAttribute.push_back(lexical_cast<double>(label));
    }
}

MetricAttribute::MetricAttribute(const MetricAttribute& parentAttribute): Attribute(parentAttribute)
{
}

MetricAttribute* MetricAttribute::clone() const
{
  return new MetricAttribute(*this);
}

void MetricAttribute::chooseValue(const unsigned int indexOfValue)
{
  const vector<Value*>::iterator valueIt = values.begin() + indexOfValue;
  rotate(values.begin() + potentialIndex, valueIt, valueIt + 1);
}

void MetricAttribute::repositionChosenPresentValue()
{
  repositionChosenPresentValueInOrderedAttribute();
}

vector<Value*>::iterator MetricAttribute::repositionChosenValueAndGetIt()
{
  const vector<Value*>::iterator valuesBegin = values.begin();
  vector<Value*>::iterator valueIt = valuesBegin + potentialIndex - 1;
  Value* chosenValue = *valueIt;
  const unsigned int chosenValueDataId = chosenValue->getDataId();
  if ((*valuesBegin)->getDataId() < chosenValueDataId)
    {
      for (vector<Value*>::iterator nextValueIt = valueIt - 1; (*nextValueIt)->getDataId() > chosenValueDataId; --nextValueIt)
	{
	  *valueIt = *nextValueIt;
	  --valueIt;
	}
      *valueIt = chosenValue;
      return valueIt;
    }
  rotate(valuesBegin, valueIt, valueIt + 1);
  return valuesBegin;
}

pair<const bool, vector<unsigned int>> MetricAttribute::setChosenValueAbsent()
{
  pair<const bool, vector<unsigned int>> irrelevantValueDataIds = tauFarValueDataIdsAndCheckTauContiguity();
  if (!irrelevantValueDataIds.first)
    {
      if (id == orderedAttributeId)
	{
	  sort(irrelevantValueDataIds.second.begin(), irrelevantValueDataIds.second.end());
	}
      --irrelevantIndex;
      swap(values[potentialIndex], values[--absentIndex]);
    }
  return irrelevantValueDataIds;
}

void MetricAttribute::keepChosenAbsentValue(const bool isValuePotentiallyPreventingClosedness)
{
  keepChosenAbsentValueInOrderedAttribute(isValuePotentiallyPreventingClosedness);
}

// PERF: decrease of the absent intervals on which binary searches are performed (use of the last iterator returned as a new bound)
const bool MetricAttribute::findIrrelevantValuesAndCheckTauContiguity(const vector<Attribute*>::iterator attributeBegin, const vector<Attribute*>::iterator attributeEnd)
{
  if (potentialIndex == irrelevantIndex)
    {
      return false;
    }
  if (potentialIndex == 0)
    {
      Attribute::findIrrelevantValuesAndCheckTauContiguity(attributeBegin, attributeEnd);
      sort(values.begin(), values.begin() + irrelevantIndex, Value::smallerDataId);
      return false;
    }
  vector<Value*>::iterator chosenValueIt = repositionChosenValueAndGetIt(); // If *this was chosen, the chosen value currently is at the end of present values
  const double tau = tauVector[id];
  const vector<double>& timestamps = timestampsVector[id];
  const vector<Value*>::iterator potentialBegin = values.begin() + potentialIndex;
  vector<Value*>::iterator potentialEnd = values.begin() + irrelevantIndex;
  vector<Value*>::iterator lowerPotentialBorderIt = lower_bound(potentialBegin, potentialEnd, values.front(), Value::smallerDataId);
  vector<Value*>::iterator upperPotentialBorderIt = lower_bound(potentialBegin, potentialEnd, *(potentialBegin - 1), Value::smallerDataId);
  if (lowerPotentialBorderIt != potentialEnd)
    {
      // There are some potential values that are greater than the smallest present value
      // Values between the lowest and the greatest present timestamps
      vector<Value*>::const_iterator presentIt = values.begin();
      double presentTimestamp = timestamps[(*presentIt)->getDataId()];
      double scopeTimestamp = presentTimestamp;
      for (vector<Value*>::iterator potentialValueIt = lowerPotentialBorderIt; potentialValueIt != upperPotentialBorderIt; )
	{
	  // Increase scopeTimestamp w.r.t. present timestamps within scope
	  while (presentTimestamp <= scopeTimestamp && presentIt != potentialBegin)
	    {
	      scopeTimestamp = presentTimestamp + tau;
	      if (++presentIt != potentialBegin)
		{
		  presentTimestamp = timestamps[(*presentIt)->getDataId()];
		}
	    }
	  if (valueDoesNotExtendPresent(**potentialValueIt, attributeBegin, attributeEnd))
	    {
	      if (potentialValueIt == --upperPotentialBorderIt && presentIt != potentialBegin)
		{
#ifdef DEBUG
		  cout << tau << "-contiguity constraint on attribute " << internal2ExternalAttributeOrder[id] << " not verified -> Prune!" << endl;
#endif
		  return true;
		}
#ifdef DEBUG
	      cout << labelsVector[id][(*potentialValueIt)->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id] << " will never be present nor extend any future pattern" << endl;
#endif
	      rotate(potentialValueIt, potentialValueIt + 1, potentialEnd--);
	      --irrelevantIndex;
	    }
	  else
	    {
	      // Check tau-contiguity
	      const double potentialTimestamp = timestamps[(*potentialValueIt)->getDataId()];
	      if (potentialTimestamp > scopeTimestamp)
		{
#ifdef DEBUG
		  cout << tau << "-contiguity constraint on attribute " << internal2ExternalAttributeOrder[id] << " not verified -> Prune!" << endl;
#endif
		  return true;
		}
	      // Increase (if possible) scopeTimestamp w.r.t. potentialTimestamp
	      const double scopePotentialTimestamp = potentialTimestamp + tau;
	      if (scopePotentialTimestamp > scopeTimestamp)
		{
		  scopeTimestamp = scopePotentialTimestamp;
		}
	      ++potentialValueIt;
	    }
	}
    }
  // Values beyond the greatest present timestamp
  if (upperPotentialBorderIt != potentialEnd)
    {
      // There are some potential values that are greater than the largest present value
      double scopeTimestamp = timestamps[(*(potentialBegin - 1))->getDataId()] + tau;
      while (upperPotentialBorderIt != potentialEnd)
	{
	  // Check tau-contiguity
	  const double nextTimestamp = timestamps[(*upperPotentialBorderIt)->getDataId()];
	  if (nextTimestamp > scopeTimestamp)
	    {
#ifdef DEBUG
	      cout << "In the " << tau << "-contiguous attribute " << internal2ExternalAttributeOrder[id] << ", every potential value beyond " << scopeTimestamp << " is irrelevant" << endl;
#endif
	      irrelevantIndex -= potentialEnd - upperPotentialBorderIt;
	      potentialEnd = upperPotentialBorderIt;
	      break;
	    }
	  if (valueDoesNotExtendPresent(**upperPotentialBorderIt, attributeBegin, attributeEnd))
	    {
#ifdef DEBUG
	      cout << labelsVector[id][(*upperPotentialBorderIt)->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id] << " will never be present nor extend any future pattern" << endl;
#endif
	      rotate(upperPotentialBorderIt, upperPotentialBorderIt + 1, potentialEnd--);
	      --irrelevantIndex;
	    }
	  else
	    {
	      scopeTimestamp = nextTimestamp + tau;
	      ++upperPotentialBorderIt;
	    }
	}
      eraseAbsentValuesBeyondTimestamp(scopeTimestamp);
    }
  // Values beneath the lowest present timestamp
  if (lowerPotentialBorderIt != potentialBegin)
    {
      // There are some potential values that are lower than the smallest present value
      double scopeTimestamp = timestamps[values.front()->getDataId()] - tau;
      do
	{
	  // Check tau-contiguity
	  const double nextTimestamp = timestamps[(*(--lowerPotentialBorderIt))->getDataId()];
	  if (nextTimestamp < scopeTimestamp)
	    {
#ifdef DEBUG
	      cout << "In the " << tau << "-contiguous attribute " << internal2ExternalAttributeOrder[id] << ", every potential value beneath " << scopeTimestamp << " is irrelevant" << endl;
#endif
	      rotate(potentialBegin, ++lowerPotentialBorderIt, potentialEnd);
	      irrelevantIndex -= lowerPotentialBorderIt - potentialBegin;
	      break;
	    }
	  if (valueDoesNotExtendPresent(**lowerPotentialBorderIt, attributeBegin, attributeEnd))
	    {
#ifdef DEBUG
	      cout << labelsVector[id][(*lowerPotentialBorderIt)->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id] << " will never be present nor extend of any future pattern" << endl;
#endif
	      rotate(lowerPotentialBorderIt, lowerPotentialBorderIt + 1, potentialEnd--);
	      --irrelevantIndex;
	    }
	  else
	    {
	      scopeTimestamp = nextTimestamp - tau;
	    }
	} while (lowerPotentialBorderIt != potentialBegin);
      eraseAbsentValuesBeneathTimestamp(scopeTimestamp);
    }
  if (chosenValueIt != potentialBegin)
    {
      // Put the chosen value back to where it was for Attribute::setPresentAndPotentialIntersections
      rotate(chosenValueIt, chosenValueIt + 1, potentialBegin);
    }
  return false;
}

#ifdef MIN_SIZE_ELEMENT_PRUNING
pair<bool, vector<unsigned int>> MetricAttribute::findPresentAndPotentialIrrelevantValuesAndCheckTauContiguity(const unsigned int presentAndPotentialIrrelevancyThreshold)
{
  if (potentialIndex == irrelevantIndex)
    {
      return pair<bool, vector<unsigned int>>(false, vector<unsigned int>());
    }
  if (potentialIndex == 0)
    {
      const pair<bool, vector<unsigned int>> newIrrelevantValueDataIds = Attribute::findPresentAndPotentialIrrelevantValuesAndCheckTauContiguity(presentAndPotentialIrrelevancyThreshold);
      sort(values.begin() + potentialIndex, values.begin() + irrelevantIndex, Value::smallerDataId);
      return newIrrelevantValueDataIds;
    }
  vector<unsigned int> newIrrelevantValueDataIds;
  const double tau = tauVector[id];
  const vector<double>& timestamps = timestampsVector[id];
  const vector<Value*>::iterator potentialBegin = values.begin() + potentialIndex;
  vector<Value*>::iterator potentialEnd = values.begin() + irrelevantIndex;
  vector<Value*>::iterator lowerPotentialBorderIt = lower_bound(potentialBegin, potentialEnd, values.front(), Value::smallerDataId);
  vector<Value*>::iterator upperPotentialBorderIt = lower_bound(potentialBegin, potentialEnd, *(potentialBegin - 1), Value::smallerDataId);
  if (lowerPotentialBorderIt != potentialEnd)
    {
      // There are some potential values that are greater than the smallest present value
      // Values between the lowest and the greatest present timestamps
      vector<Value*>::const_iterator presentIt = values.begin();
      double presentTimestamp = timestamps[(*presentIt)->getDataId()];
      double scopeTimestamp = presentTimestamp;
      for (vector<Value*>::iterator potentialValueIt = lowerPotentialBorderIt; potentialValueIt != upperPotentialBorderIt; )
	{
	  // Increase scopeTimestamp w.r.t. present timestamps within scope
	  while (presentTimestamp <= scopeTimestamp && presentIt != potentialBegin)
	    {
	      scopeTimestamp = presentTimestamp + tau;
	      if (++presentIt != potentialBegin)
		{
		  presentTimestamp = timestamps[(*presentIt)->getDataId()];
		}
	    }
	  if (presentAndPotentialIrrelevantValue(**potentialValueIt, presentAndPotentialIrrelevancyThreshold))
	    {
	      newIrrelevantValueDataIds.push_back((*potentialValueIt)->getDataId());
	      if (potentialValueIt == --upperPotentialBorderIt && presentIt != potentialBegin)
		{
#ifdef DEBUG
		  cout << tau << "-contiguity constraint on attribute " << internal2ExternalAttributeOrder[id] << " not verified -> Prune!" << endl;
#endif
		  return pair<bool, vector<unsigned int>>(true, newIrrelevantValueDataIds);
		}
	      rotate(potentialValueIt, potentialValueIt + 1, potentialEnd--);
	      --irrelevantIndex;
	    }
	  else
	    {
	      // Check tau-contiguity
	      const double potentialTimestamp = timestamps[(*potentialValueIt)->getDataId()];
	      if (potentialTimestamp > scopeTimestamp)
		{
#ifdef DEBUG
		  cout << tau << "-contiguity constraint on attribute " << internal2ExternalAttributeOrder[id] << " not verified -> Prune!" << endl;
#endif
		  return pair<bool, vector<unsigned int>>(true, newIrrelevantValueDataIds);
		}
	      // Increase (if possible) scopeTimestamp w.r.t. potentialTimestamp
	      const double scopePotentialTimestamp = potentialTimestamp + tau;
	      if (scopePotentialTimestamp > scopeTimestamp)
		{
		  scopeTimestamp = scopePotentialTimestamp;
		}
	      ++potentialValueIt;
	    }
	}
    }
  // Values beyond the greatest present timestamp
  if (upperPotentialBorderIt != potentialEnd)
    {
      // There are some potential values that are greater than the largest present value
      double scopeTimestamp = timestamps[(*(potentialBegin - 1))->getDataId()] + tau;
      while (upperPotentialBorderIt != potentialEnd)
	{
	  // Check tau-contiguity
	  const double nextTimestamp = timestamps[(*upperPotentialBorderIt)->getDataId()];
	  if (nextTimestamp > scopeTimestamp)
	    {
#ifdef DEBUG
	      cout << "In the " << tau << "-contiguous attribute " << internal2ExternalAttributeOrder[id] << ", every potential value beyond " << scopeTimestamp << " is irrelevant" << endl;
#endif
	      irrelevantIndex -= potentialEnd - upperPotentialBorderIt;
	      while (upperPotentialBorderIt != potentialEnd)
	      	{
	      	  newIrrelevantValueDataIds.push_back((*--potentialEnd)->getDataId());
	      	}
	      break;
	    }
	  if (presentAndPotentialIrrelevantValue(**upperPotentialBorderIt, presentAndPotentialIrrelevancyThreshold))
	    {
	      newIrrelevantValueDataIds.push_back((*upperPotentialBorderIt)->getDataId());
	      rotate(upperPotentialBorderIt, upperPotentialBorderIt + 1, potentialEnd--);
	      --irrelevantIndex;
	    }
	  else
	    {
	      scopeTimestamp = nextTimestamp + tau;
	      ++upperPotentialBorderIt;
	    }
	}
      eraseAbsentValuesBeyondTimestamp(scopeTimestamp);
    }
  // Values beneath the lowest present timestamp
  if (lowerPotentialBorderIt != potentialBegin)
    {
      // There are some potential values that are lower than the smallest present value
      double scopeTimestamp = timestamps[values.front()->getDataId()] - tau;
      do
	{
	  // Check tau-contiguity
	  const double nextTimestamp = timestamps[(*(--lowerPotentialBorderIt))->getDataId()];
	  if (nextTimestamp < scopeTimestamp)
	    {
#ifdef DEBUG
	      cout << "In the " << tau << "-contiguous attribute " << internal2ExternalAttributeOrder[id] << ", every potential value beneath " << scopeTimestamp << " is irrelevant" << endl;
#endif
	      irrelevantIndex -= ++lowerPotentialBorderIt - potentialBegin;
	      for (vector<Value*>::iterator potentialValueIt = potentialBegin; potentialValueIt != lowerPotentialBorderIt; ++potentialValueIt)
	      	{
	      	  newIrrelevantValueDataIds.push_back((*potentialValueIt)->getDataId());
	      	}
	      rotate(potentialBegin, lowerPotentialBorderIt, potentialEnd);
	      break;
	    }
	  if (presentAndPotentialIrrelevantValue(**lowerPotentialBorderIt, presentAndPotentialIrrelevancyThreshold))
	    {
	      newIrrelevantValueDataIds.push_back((*lowerPotentialBorderIt)->getDataId());
	      rotate(lowerPotentialBorderIt, lowerPotentialBorderIt + 1, potentialEnd--);
	      --irrelevantIndex;
	    }
	  else
	    {
	      scopeTimestamp = nextTimestamp - tau;
	    }
	} while (lowerPotentialBorderIt != potentialBegin);
      eraseAbsentValuesBeneathTimestamp(scopeTimestamp);
    }
  return pair<bool, vector<unsigned int>>(false, newIrrelevantValueDataIds);
}

void MetricAttribute::sortAbsent()
{
  sort(values.begin() + absentIndex, values.end(), Value::smallerDataId);
}

void MetricAttribute::sortPotentialIrrelevantAndAbsent()
{
  // potential and absent values are already sorted
  if (id == orderedAttributeId)
    {
      sort(values.begin() + irrelevantIndex, values.begin() + absentIndex, Value::smallerDataId);
    }
}
#endif

void MetricAttribute::eraseAbsentValuesBeneathTimestamp(const double timestamp)
{
#ifdef DEBUG
  cout << "In the " << tauVector[id] << "-contiguous attribute " << internal2ExternalAttributeOrder[id] << ", every absent value beneath " << timestamp << " cannot extend any future pattern" << endl;
#endif
  const vector<double>& timestamps = timestampsVector[id];
  vector<Value*>::iterator valueIt = values.begin() + absentIndex;
  const vector<Value*>::iterator absentBegin = valueIt;
  for (; valueIt != values.end() && timestamps[(*valueIt)->getDataId()] < timestamp; ++valueIt)
    {
      delete *valueIt;
    }
  values.erase(absentBegin, valueIt);
}

void MetricAttribute::eraseAbsentValuesBeyondTimestamp(const double timestamp)
{
#ifdef DEBUG
  cout << "In the " << tauVector[id] << "-contiguous attribute " << internal2ExternalAttributeOrder[id] << ", every absent value beyond " << timestamp << " cannot extend any future pattern" << endl;
#endif
  if (absentIndex != values.size())
    {
      const vector<double>& timestamps = timestampsVector[id];
      vector<Value*>::iterator absentBegin = values.begin() + absentIndex;
      if (timestamps[(*absentBegin)->getDataId()] > timestamp)
	{
	  for (vector<Value*>::iterator valueIt = absentBegin; valueIt != values.end(); ++valueIt)
	    {
	      delete *valueIt;
	    }
	  values.resize(absentIndex);
	  return;
	}
      vector<Value*>::iterator valueIt = --(values.end());
      for (; timestamps[(*valueIt)->getDataId()] > timestamp; --valueIt)
	{
	  delete *valueIt;
	}
      values.erase(valueIt + 1, values.end());
    }
}

const bool MetricAttribute::unclosed(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const
{
  if (potentialIndex == 0 || irrelevantIndex == values.size())
    {
      return false;
    }
  const double tau = tauVector[id];
  const Value* presentBack = values[potentialIndex - 1];
  const vector<double>& timestamps = timestampsVector[id];
  double scopeTimestamp = timestamps[presentBack->getDataId()] + tau;
  const vector<Value*>::const_iterator absentBegin = values.begin() + irrelevantIndex;
  const vector<Value*>::const_iterator end = values.end();
  vector<Value*>::const_iterator tauCloseAbsentEnd = lower_bound(absentBegin, end, presentBack, Value::smallerDataId);
  for (; tauCloseAbsentEnd != end && timestamps[(*tauCloseAbsentEnd)->getDataId()] <= scopeTimestamp; ++tauCloseAbsentEnd)
    {
    }
  scopeTimestamp = timestamps[values.front()->getDataId()] - tau;
  vector<Value*>::const_iterator tauCloseAbsentBegin;
  if (timestamps[(*absentBegin)->getDataId()] < scopeTimestamp)
    {
      tauCloseAbsentBegin = lower_bound(absentBegin, end, values.front(), Value::smallerDataId);
      while (timestamps[(*(--tauCloseAbsentBegin))->getDataId()] >= scopeTimestamp)
	{
	}
      ++tauCloseAbsentBegin;
    }
  else
    {
      tauCloseAbsentBegin = absentBegin;
    }
  for (; tauCloseAbsentBegin != tauCloseAbsentEnd && valueDoesNotExtendPresentAndPotential(**tauCloseAbsentBegin, attributeBegin, attributeEnd); ++tauCloseAbsentBegin)
    {
    }
#ifdef DEBUG
  if (tauCloseAbsentBegin != tauCloseAbsentEnd)
    {
      cout << labelsVector[id][(*tauCloseAbsentBegin)->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id] << " extends any future pattern -> Prune!" << endl;
    }
#endif
  return tauCloseAbsentBegin != tauCloseAbsentEnd;
}

const bool MetricAttribute::globallyUnclosed(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const
{
  return Attribute::unclosed(attributeBegin, attributeEnd);
}

void MetricAttribute::sortPotential()
{
  // potential already sorted
  if (id == orderedAttributeId)
    {
      sort(values.begin() + irrelevantIndex, values.begin() + absentIndex, Value::smallerDataId);
    }
}

void MetricAttribute::sortPotentialAndAbsentButChosenValue(const unsigned int presentAttributeId)
{
  // Already sorted
}

pair<const bool, vector<unsigned int>> MetricAttribute::tauFarValueDataIdsAndCheckTauContiguity()
{
  const unsigned int minPresentDataId = values.front()->getDataId();
  if (potentialIndex == 0)
    {
      return pair<const bool, vector<unsigned int>>(false, vector<unsigned int> {minPresentDataId});
    }
  vector<Value*>::iterator potentialBegin = values.begin() + potentialIndex;
  const unsigned int absentValueDataId = (*potentialBegin)->getDataId();
  vector<unsigned int> irrelevantValueDataIds {absentValueDataId};
  const double tau = tauVector[id];
  const vector<double>& timestamps = timestampsVector[id];
  vector<Value*>::iterator potentialEnd = values.begin() + irrelevantIndex;
  // PERF: binary search
  vector<Value*>::iterator lowerPotentialBorderIt = ++potentialBegin;
  for (; lowerPotentialBorderIt != potentialEnd && (*lowerPotentialBorderIt)->getDataId() < absentValueDataId; ++lowerPotentialBorderIt)
    {
    }
  if (minPresentDataId > absentValueDataId)
    {
      // absentValue is beneath the present range
      double scopeTimestamp;
      if (lowerPotentialBorderIt != potentialEnd && (*lowerPotentialBorderIt)->getDataId() < minPresentDataId)
	{
	  scopeTimestamp = timestamps[(*lowerPotentialBorderIt)->getDataId()] - tau;
	}
      else
	{
	  scopeTimestamp = timestamps[values.front()->getDataId()] - tau;
	}
      if (lowerPotentialBorderIt == potentialBegin)
	{
	  eraseAbsentValuesBeneathTimestamp(scopeTimestamp);
	  return pair<const bool, vector<unsigned int>>(false, irrelevantValueDataIds);
	}
      if (scopeTimestamp > timestamps[(*(lowerPotentialBorderIt - 1))->getDataId()])
	{
#ifdef DEBUG
	  cout << "In the " << tau << "-contiguous attribute " << internal2ExternalAttributeOrder[id] << ", every potential value beneath " << scopeTimestamp << " is irrelevant" << endl;
#endif
	  // Potential values with too small timestamps are irrelevant
	  irrelevantValueDataIds.reserve(irrelevantIndex - potentialIndex);
	  for (vector<Value*>::iterator tauFarPotentialValueIt = potentialBegin; tauFarPotentialValueIt != lowerPotentialBorderIt; ++tauFarPotentialValueIt)
	    {
	      irrelevantValueDataIds.push_back((*tauFarPotentialValueIt)->getDataId());
	      delete *tauFarPotentialValueIt;
	    }
	  const unsigned int nbOfValueBeforeErasure = values.size();
	  values.erase(potentialBegin, lowerPotentialBorderIt);
	  irrelevantIndex -= nbOfValueBeforeErasure - values.size();
	  absentIndex = irrelevantIndex;
	  eraseAbsentValuesBeneathTimestamp(scopeTimestamp);
	}
      return pair<const bool, vector<unsigned int>>(false, irrelevantValueDataIds);
    }
  const unsigned int maxPresentDataId = (*(potentialBegin - 2))->getDataId();
  if (maxPresentDataId < absentValueDataId)
    {
      // absentValue is beyond the present range
      if (lowerPotentialBorderIt == potentialEnd)
	{
	  const unsigned int maxPotentialDataId = (*(potentialEnd - 1))->getDataId();
	  if (potentialIndex == irrelevantIndex || maxPotentialDataId < maxPresentDataId)
	    {
	      eraseAbsentValuesBeyondTimestamp(timestamps[maxPresentDataId] + tau);
	      return pair<const bool, vector<unsigned int>>(false, irrelevantValueDataIds);
	    }
	  eraseAbsentValuesBeyondTimestamp(timestamps[maxPotentialDataId] + tau);
	  return pair<const bool, vector<unsigned int>>(false, irrelevantValueDataIds);
	}
      double scopeTimestamp;
      if (lowerPotentialBorderIt == potentialBegin)
	{
	  scopeTimestamp = timestamps[maxPresentDataId] + tau;
	}
      else
	{
	  if ((*(lowerPotentialBorderIt - 1))->getDataId() < maxPresentDataId)
	    {
	      scopeTimestamp = timestamps[maxPresentDataId] + tau;
	    }
	  else
	    {
	      scopeTimestamp = timestamps[(*(lowerPotentialBorderIt - 1))->getDataId()] + tau;
	    }
	}
      if (scopeTimestamp < timestamps[(*lowerPotentialBorderIt)->getDataId()])
	{
#ifdef DEBUG
	  cout << "In the " << tau << "-contiguous attribute " << internal2ExternalAttributeOrder[id] << ", every potential value beyond " << scopeTimestamp << " is irrelevant" << endl;
#endif
	  // Potential values with too great timestamps are irrelevant
	  irrelevantValueDataIds.reserve(irrelevantIndex - potentialIndex);
	  for (vector<Value*>::iterator tauFarPotentialValueIt = lowerPotentialBorderIt; tauFarPotentialValueIt != potentialEnd; ++tauFarPotentialValueIt)
	    {
	      irrelevantValueDataIds.push_back((*tauFarPotentialValueIt)->getDataId());
	      delete *tauFarPotentialValueIt;
	    }
	  const unsigned int nbOfValueBeforeErasure = values.size();
	  values.erase(lowerPotentialBorderIt, potentialEnd);
	  irrelevantIndex -= nbOfValueBeforeErasure - values.size();
	  absentIndex = irrelevantIndex;
	  eraseAbsentValuesBeyondTimestamp(scopeTimestamp);
	}
      return pair<const bool, vector<unsigned int>>(false, irrelevantValueDataIds);
    }
  // absentValue is inside the present range
  // PERF: binary search
  vector<Value*>::iterator lowerPresentBorderIt = values.begin();
  const vector<Value*>::const_iterator presentEnd = lowerPresentBorderIt + potentialIndex;
  for (; lowerPresentBorderIt != presentEnd && (*lowerPresentBorderIt)->getDataId() < absentValueDataId; ++lowerPresentBorderIt)
    {
    }
  double previousTimestamp;
  if (lowerPotentialBorderIt == potentialBegin)
    {
      previousTimestamp = timestamps[(*(lowerPresentBorderIt - 1))->getDataId()];
    }
  else
    {
      previousTimestamp = timestamps[max((*(lowerPresentBorderIt - 1))->getDataId(), (*(lowerPotentialBorderIt - 1))->getDataId())];
    }
  double nextTimestamp;
  if (lowerPotentialBorderIt == potentialEnd)
    {
      nextTimestamp = timestamps[(*lowerPresentBorderIt)->getDataId()];
    }
  else
    {
      nextTimestamp = timestamps[min((*lowerPresentBorderIt)->getDataId(), (*lowerPotentialBorderIt)->getDataId())];
    }
  if (previousTimestamp + tau < nextTimestamp)
    {
#ifdef DEBUG
      cout << tau << "-contiguity constraint on attribute " << internal2ExternalAttributeOrder[id] << " not verified -> Prune!" << endl;
#endif
      return pair<const bool, vector<unsigned int>>(true, irrelevantValueDataIds);
    }
  return pair<const bool, vector<unsigned int>>(false, irrelevantValueDataIds);
}

const bool MetricAttribute::finalizable() const
{
  return (potentialIndex != 0 || potentialIndex == irrelevantIndex) && Attribute::finalizable();
}

vector<unsigned int> MetricAttribute::finalize()
{
  vector<unsigned int> dataIdsOfValuesSetPresent;
  dataIdsOfValuesSetPresent.reserve(irrelevantIndex - potentialIndex);
  // WARNING: present and potential must be ordered
  const vector<Value*>::iterator begin = values.begin() + potentialIndex;
  const vector<Value*>::iterator end = values.begin() + irrelevantIndex;
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      dataIdsOfValuesSetPresent.push_back((*valueIt)->getDataId());
    }
  inplace_merge(values.begin(), begin, end, Value::smallerDataId);
  return dataIdsOfValuesSetPresent;
}
