// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015,2016 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "Value.h"

Value::Value(const unsigned int dataIdParam): dataId(dataIdParam), presentIntersectionId(0), presentAndPotentialIntersectionId(dataIdParam), presentNoise(0), presentAndPotentialNoise(0), intersectionsWithPresentValues(), intersectionsWithPresentAndPotentialValues()
{
}

Value::Value(const unsigned int dataIdParam, const unsigned int presentAndPotentialNoiseParam, const vector<unsigned int>::const_iterator nbOfValuesPerAttributeBegin, const vector<unsigned int>::const_iterator nbOfValuesPerAttributeEnd, const vector<unsigned int>& noisesInIntersections): dataId(dataIdParam), presentIntersectionId(0), presentAndPotentialIntersectionId(dataIdParam), presentNoise(0), presentAndPotentialNoise(presentAndPotentialNoiseParam), intersectionsWithPresentValues(noisesInIntersections.size()), intersectionsWithPresentAndPotentialValues()
{
  intersectionsWithPresentAndPotentialValues.reserve(noisesInIntersections.size());
  vector<unsigned int>::const_iterator sizeOfIntersectionIt = noisesInIntersections.begin();
  for (vector<unsigned int>::const_iterator nbOfValuesPerAttributeIt = nbOfValuesPerAttributeBegin; nbOfValuesPerAttributeIt != nbOfValuesPerAttributeEnd; ++nbOfValuesPerAttributeIt)
    {
      vector<unsigned int> intersectionsWithPresentAndPotential(*nbOfValuesPerAttributeIt, *sizeOfIntersectionIt++);
      intersectionsWithPresentAndPotentialValues.push_back(intersectionsWithPresentAndPotential);
    }
}

Value::Value(const Value& parent, const unsigned int presentIntersectionIdParam): dataId(parent.dataId), presentIntersectionId(presentIntersectionIdParam), presentAndPotentialIntersectionId(0), presentNoise(parent.presentNoise), presentAndPotentialNoise(parent.presentAndPotentialNoise), intersectionsWithPresentValues(parent.intersectionsWithPresentValues.size()), intersectionsWithPresentAndPotentialValues()
{
}

const unsigned int Value::getPresentIntersectionId() const
{
  return presentIntersectionId;
}

const unsigned int Value::getPresentAndPotentialIntersectionId() const
{
  return presentAndPotentialIntersectionId;
}

const unsigned int Value::getDataId() const
{
  return dataId;
}

const unsigned int Value::getPresentNoise() const
{
  return presentNoise;
}

const unsigned int Value::getPresentAndPotentialNoise() const
{
  return presentAndPotentialNoise;
}

vector<vector<unsigned int>>::iterator Value::setPresentAndPotentialIntersectionId(const unsigned int presentAndPotentialIntersectionIdParam)
{
  presentAndPotentialIntersectionId = presentAndPotentialIntersectionIdParam;
  intersectionsWithPresentAndPotentialValues.resize(intersectionsWithPresentValues.size());
  return intersectionsWithPresentAndPotentialValues.begin();
}

void Value::incrementPresentNoise()
{
  ++presentNoise;
}

void Value::addPresentNoise(const unsigned int noise)
{
  presentNoise += noise;
}

void Value::decrementPotentialNoise()
{
  --presentAndPotentialNoise;
}

void Value::subtractPotentialNoise(const unsigned int noise)
{
  presentAndPotentialNoise -= noise;
}

vector<vector<unsigned int>>::iterator Value::getIntersectionsBeginWithPresentValues()
{
  return intersectionsWithPresentValues.begin();
}

vector<vector<unsigned int>>::const_iterator Value::getIntersectionsBeginWithPresentValues() const
{
  return intersectionsWithPresentValues.begin();
}

vector<vector<unsigned int>>::iterator Value::getIntersectionsBeginWithPresentAndPotentialValues()
{
  return intersectionsWithPresentAndPotentialValues.begin();
}

vector<vector<unsigned int>>::const_iterator Value::getIntersectionsBeginWithPresentAndPotentialValues() const
{
  return intersectionsWithPresentAndPotentialValues.begin();
}

void Value::subtractPotentialNoiseAtIntersections(const unsigned int noise, const unsigned int index)
{
  for (unsigned int& potentialNoise : intersectionsWithPresentAndPotentialValues[index])
    {
      potentialNoise -= noise;
    }
}

void Value::subtractPotentialNoiseAtIntersectionsWithSymmetricValue(const unsigned int noise, const unsigned int index)
{
  intersectionsWithPresentAndPotentialValues[index][presentAndPotentialIntersectionId] -= noise;
}

const bool Value::extendsPastPresent(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int intersectionIndex) const
{
  vector<Value*>::const_iterator valueIt = valueBegin;
  for (; valueIt != valueEnd && (*valueIt)->presentNoise + (*valueIt)->intersectionsWithPresentValues[intersectionIndex][presentIntersectionId] <= threshold; ++valueIt)
    {
    }
  return valueIt == valueEnd;
}

const bool Value::extendsFuturePresent(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int reverseAttributeIndex) const
{
  const vector<unsigned int>& presentIntersectionsWithFutureValues = intersectionsWithPresentValues[intersectionsWithPresentValues.size() - reverseAttributeIndex];
  vector<Value*>::const_iterator valueIt = valueBegin;
  for (; valueIt != valueEnd && (*valueIt)->presentNoise + presentIntersectionsWithFutureValues[(*valueIt)->presentIntersectionId] <= threshold; ++valueIt)
    {
    }
  return valueIt == valueEnd;
}

const bool Value::extendsPastPresentAndPotential(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int intersectionIndex) const
{
  vector<Value*>::const_iterator valueIt = valueBegin;
  for (; valueIt != valueEnd && (*valueIt)->presentAndPotentialNoise + (*valueIt)->intersectionsWithPresentAndPotentialValues[intersectionIndex][presentAndPotentialIntersectionId] <= threshold; ++valueIt)
    {
    }
  return valueIt == valueEnd;
}

const bool Value::extendsFuturePresentAndPotential(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int reverseAttributeIndex) const
{
  const vector<unsigned int>& presentAndPotentialIntersectionsWithFutureValues = intersectionsWithPresentAndPotentialValues[intersectionsWithPresentAndPotentialValues.size() - reverseAttributeIndex];
  vector<Value*>::const_iterator valueIt = valueBegin;
  for (; valueIt != valueEnd && (*valueIt)->presentAndPotentialNoise + presentAndPotentialIntersectionsWithFutureValues[(*valueIt)->presentAndPotentialIntersectionId] <= threshold; ++valueIt)
    {
    }
  return valueIt == valueEnd;
}

const bool Value::symmetricValuesExtendPastPresent(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int intersectionIndex) const
{
  vector<Value*>::const_iterator valueIt = valueBegin;
  for (; valueIt != valueEnd && (*valueIt)->presentNoise + (*valueIt)->intersectionsWithPresentValues[intersectionIndex][presentIntersectionId] + (*valueIt)->intersectionsWithPresentValues[intersectionIndex + 1][presentIntersectionId] <= threshold; ++valueIt)
    {
    }
  return valueIt == valueEnd;
}

const bool Value::symmetricValuesExtendFuturePresent(const Value& symmetricValue, const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int reverseAttributeIndex) const
{
  const unsigned int intersectionIndex = intersectionsWithPresentValues.size() - reverseAttributeIndex;
  const vector<unsigned int>& presentIntersectionsWithFutureValues1 = intersectionsWithPresentValues[intersectionIndex + 1];
  const vector<unsigned int>& presentIntersectionsWithFutureValues2 = symmetricValue.intersectionsWithPresentValues[intersectionIndex];
  vector<Value*>::const_iterator valueIt = valueBegin;
  for (; valueIt != valueEnd && (*valueIt)->presentNoise + presentIntersectionsWithFutureValues1[(*valueIt)->presentIntersectionId] + presentIntersectionsWithFutureValues2[(*valueIt)->presentIntersectionId] <= threshold; ++valueIt)
    {
    }
  return valueIt == valueEnd;
}

const bool Value::symmetricValuesExtendPastPresentAndPotential(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int intersectionIndex) const
{
  vector<Value*>::const_iterator valueIt = valueBegin;
  for (; valueIt != valueEnd && (*valueIt)->presentAndPotentialNoise + (*valueIt)->intersectionsWithPresentAndPotentialValues[intersectionIndex][presentAndPotentialIntersectionId] + (*valueIt)->intersectionsWithPresentAndPotentialValues[intersectionIndex + 1][presentAndPotentialIntersectionId] <= threshold; ++valueIt)
    {
    }
  return valueIt == valueEnd;
}

const bool Value::symmetricValuesExtendFuturePresentAndPotential(const Value& symmetricValue, const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int reverseAttributeIndex) const
{
  const unsigned int intersectionIndex = intersectionsWithPresentAndPotentialValues.size() - reverseAttributeIndex;
  const vector<unsigned int>& presentAndPotentialIntersectionsWithFutureValues1 = intersectionsWithPresentAndPotentialValues[intersectionIndex + 1];
  const vector<unsigned int>& presentAndPotentialIntersectionsWithFutureValues2 = symmetricValue.intersectionsWithPresentAndPotentialValues[intersectionIndex];
  vector<Value*>::const_iterator valueIt = valueBegin;
  for (; valueIt != valueEnd && (*valueIt)->presentAndPotentialNoise + presentAndPotentialIntersectionsWithFutureValues1[(*valueIt)->presentAndPotentialIntersectionId] + presentAndPotentialIntersectionsWithFutureValues2[(*valueIt)->presentAndPotentialIntersectionId] <= threshold; ++valueIt)
    {
    }
  return valueIt == valueEnd;
}

#ifdef ASSERT
const unsigned int Value::presentNoiseAtIntersectionWith(const Value& otherValue, const unsigned int intersectionIndex) const
{
  return intersectionsWithPresentValues[intersectionIndex][otherValue.presentIntersectionId];
}

const unsigned int Value::presentAndPotentialNoiseAtIntersectionWith(const Value& otherValue, const unsigned int intersectionIndex) const
{
  return intersectionsWithPresentAndPotentialValues[intersectionIndex][otherValue.presentAndPotentialIntersectionId];
}
#endif

const bool Value::smallerDataId(const Value* value, const Value* otherValue)
{
  return value->getDataId() < otherValue->getDataId();
}
