// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015,2016 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "SymmetricAttribute.h"

SymmetricAttribute::SymmetricAttribute(): Attribute(), symmetricAttribute(nullptr)
{
}

SymmetricAttribute::SymmetricAttribute(const vector<unsigned int>& nbOfValuesPerAttribute, const double epsilon, const vector<string>& labels): Attribute(nbOfValuesPerAttribute, epsilon, labels), symmetricAttribute(nullptr)
{
}

SymmetricAttribute::SymmetricAttribute(const SymmetricAttribute& parentAttribute): Attribute(parentAttribute), symmetricAttribute(nullptr)
{
}

SymmetricAttribute* SymmetricAttribute::clone() const
{
  return new SymmetricAttribute(*this);
}

const bool SymmetricAttribute::symmetric() const
{
  return true;
}

SymmetricAttribute* SymmetricAttribute::thisOrFirstSymmetricAttribute()
{
  if (id < symmetricAttribute->id)
    {
      return this;
    }
  return symmetricAttribute;
}

const bool SymmetricAttribute::isEnumeratedAttribute(const unsigned int enumeratedAttributeId) const
{
  return enumeratedAttributeId == id || enumeratedAttributeId == symmetricAttribute->id;
}

void SymmetricAttribute::setSymmetricAttribute(SymmetricAttribute* symmetricAttributeParam)
{
  symmetricAttribute = symmetricAttributeParam;
}

void SymmetricAttribute::subtractSelfLoopsFromPotentialNoise(const unsigned int totalMembershipDueToSelfLoopsOnASymmetricValue, const unsigned int nbOfSymmetricElements, const vector<Attribute*>::const_iterator attributeIt, const vector<Attribute*>::const_iterator attributeEnd)
{
  for (Value* value : values)
    {
      value->subtractPotentialNoise(totalMembershipDueToSelfLoopsOnASymmetricValue);
    }
  unsigned int intersectionIndex = 0;
  for (vector<Attribute*>::const_iterator subsequentAttributeIt = attributeIt; ++subsequentAttributeIt != attributeEnd; ++intersectionIndex)
    {
      if ((*subsequentAttributeIt)->symmetric())
	{
	  for (Value* value : values)
	    {
	      value->subtractPotentialNoiseAtIntersectionsWithSymmetricValue(totalMembershipDueToSelfLoopsOnASymmetricValue, intersectionIndex);
	    }
	}
      else
	{
	  const unsigned int totalMembershipDueToSelfLoopsOnIntersectionOfValueWithNonSymmetricValue = totalMembershipDueToSelfLoopsOnASymmetricValue / (*subsequentAttributeIt)->sizeOfPresentAndPotential();
	  for (Value* value : values)
	    {
	      value->subtractPotentialNoiseAtIntersections(totalMembershipDueToSelfLoopsOnIntersectionOfValueWithNonSymmetricValue, intersectionIndex);
	    }
	}
    }
}

void SymmetricAttribute::repositionChosenPresentValue()
{
  // The chosen symmetric Attribute cannot be the last one (given chooseValue)
  if (symmetricAttribute->id == orderedAttributeId)
    {
      repositionChosenPresentValueInOrderedAttribute();
      symmetricAttribute->repositionChosenPresentValueInOrderedAttribute();
    }
}

pair<const bool, vector<unsigned int>> SymmetricAttribute::setChosenValueAbsent()
{
  --irrelevantIndex;
  swap(values[potentialIndex], values[--absentIndex]);
  return symmetricAttribute->Attribute::setChosenValueAbsent();
}

void SymmetricAttribute::keepChosenAbsentValue(const bool isValuePotentiallyPreventingClosedness)
{
  // The chosen symmetric Attribute cannot be the last one (given how chosen in Tree::peel)
  if (symmetricAttribute->id == orderedAttributeId)
    {
      keepChosenAbsentValueInOrderedAttribute(isValuePotentiallyPreventingClosedness);
      symmetricAttribute->keepChosenAbsentValueInOrderedAttribute(isValuePotentiallyPreventingClosedness);
      return;
    }
  Attribute::keepChosenAbsentValue(isValuePotentiallyPreventingClosedness);
  symmetricAttribute->Attribute::keepChosenAbsentValue(isValuePotentiallyPreventingClosedness);
}

void SymmetricAttribute::keepChosenAbsentValueInOrderedAttribute(const bool isValuePotentiallyPreventingClosedness)
{
  const vector<Value*>::iterator valuesBegin = values.begin();
  vector<Value*>::iterator valueIt = valuesBegin + irrelevantIndex;
#ifdef DETECT_NON_EXTENSION_ELEMENTS
  if (isValuePotentiallyPreventingClosedness && isClosedVector[id])
#else
  if (isClosedVector[id])
#endif
    {
      Value* chosenValue = *valueIt;
      const unsigned int chosenValueDataId = chosenValue->getDataId();
      if (values.back()->getDataId() > chosenValueDataId)
	{
	  for (vector<Value*>::iterator nextValueIt = valueIt; (*++nextValueIt)->getDataId() < chosenValueDataId; )
	    {
	      *valueIt++ = *nextValueIt;
	    }
	  *valueIt = chosenValue;
	  return;
	}
      rotate(valueIt, valueIt + 1, values.end());
      return;
    }
  delete *valueIt;
  values.erase(valueIt);
}

pair<double, unsigned int> SymmetricAttribute::getAppealAndIndexOfValueToChoose(const double presentCoeff, const double presentAndPotentialCoeff) const
{
  if (id > symmetricAttribute->id)
    {
      return pair<double, unsigned int>(-1, 0);
    }
  vector<Value*>::const_iterator valueToChooseIt;
  double appealOfValueToChoose = -1;
  vector<Value*>::const_iterator symmetricPotentialValueIt = symmetricAttribute->potentialBegin();
  const vector<Value*>::const_iterator end = values.begin() + irrelevantIndex;
  for (vector<Value*>::const_iterator potentialValueIt = values.begin() + potentialIndex; potentialValueIt != end; ++potentialValueIt)
    {
      const double appealOfValue = presentCoeff * ((*potentialValueIt)->getPresentNoise() + (*symmetricPotentialValueIt)->getPresentNoise()) + presentAndPotentialCoeff * ((*potentialValueIt)->getPresentAndPotentialNoise() + (*symmetricPotentialValueIt)->getPresentAndPotentialNoise());
      if (appealOfValue > appealOfValueToChoose)
	{
	  appealOfValueToChoose = appealOfValue;
	  valueToChooseIt = potentialValueIt;
	}
      ++symmetricPotentialValueIt;
    }
  return pair<double, unsigned int>(appealOfValueToChoose / 2, valueToChooseIt - values.begin());
}

void SymmetricAttribute::chooseValue(const unsigned int indexOfValue)
{
  vector<Value*>::iterator valueIt = symmetricAttribute->presentBegin() + indexOfValue;
  if (symmetricAttribute->id == orderedAttributeId)
    {
      rotate(symmetricAttribute->potentialBegin(), valueIt, valueIt + 1);      
      valueIt = values.begin() + indexOfValue;
      rotate(values.begin() + potentialIndex, valueIt, valueIt + 1);
      return;
    }
  swap(*symmetricAttribute->potentialBegin(), *valueIt);
  swap(values[potentialIndex], values[indexOfValue]);
}

const bool SymmetricAttribute::findIrrelevantValuesAndCheckTauContiguity(const vector<Attribute*>::iterator attributeBegin, const vector<Attribute*>::iterator attributeEnd)
{
  if (id < symmetricAttribute->id)
    {
      vector<Value*>::iterator symmetricPotentialValueIt = symmetricAttribute->potentialBegin();
      vector<Value*>::iterator potentialEnd = values.begin() + irrelevantIndex;
      for (vector<Value*>::iterator potentialValueIt = values.begin() + potentialIndex; potentialValueIt != potentialEnd; )
	{
	  if (symmetricValuesDoNotExtendPresent(**potentialValueIt, **symmetricPotentialValueIt, attributeBegin, attributeEnd))
	    {
#ifdef DEBUG
	      cout << labelsVector[id][(*potentialValueIt)->getDataId()] << " in symmetric attributes will never be present nor extend any future pattern" << endl;
#endif
	      swap(*potentialValueIt, *--potentialEnd);
	      --irrelevantIndex;
	      symmetricAttribute->setPotentialValueIrrelevant(symmetricPotentialValueIt);
	    }
	  else
	    {
	      ++potentialValueIt;
	      ++symmetricPotentialValueIt;
	    }
	}
    }
  return false;
}

#ifdef MIN_SIZE_ELEMENT_PRUNING
pair<bool, vector<unsigned int>> SymmetricAttribute::findPresentAndPotentialIrrelevantValuesAndCheckTauContiguity(const unsigned int presentAndPotentialIrrelevancyThreshold)
{
  vector<unsigned int> newIrrelevantValueDataIds;
  const vector<Value*>::iterator symmetricPotentialBegin = symmetricAttribute->potentialBegin();
  vector<Value*>::iterator symmetricPotentialValueIt = symmetricPotentialBegin;
  const vector<Value*>::iterator potentialBegin = values.begin() + potentialIndex;
  vector<Value*>::iterator potentialEnd = values.begin() + irrelevantIndex;
  for (vector<Value*>::iterator potentialValueIt = potentialBegin; potentialValueIt != potentialEnd; )
    {
      // **potentialValueIt is irrelevant if it contains too much noise in any extension satisfying the minimal size constraints
      if (presentAndPotentialIrrelevantValue(**potentialValueIt, presentAndPotentialIrrelevancyThreshold))
	{
	  newIrrelevantValueDataIds.push_back((*potentialValueIt)->getDataId());
	  swap(*potentialValueIt, *--potentialEnd);
	  --irrelevantIndex;
	  symmetricAttribute->setPotentialValueIrrelevant(symmetricPotentialValueIt);
	}
      else
	{
	  ++potentialValueIt;
	  ++symmetricPotentialValueIt;
	}
    }
  return pair<bool, vector<unsigned int>>(false, newIrrelevantValueDataIds);
}

void SymmetricAttribute::presentAndPotentialCleanAbsent(const unsigned int presentAndPotentialIrrelevancyThreshold)
{
  vector<Value*>::iterator symmetricAbsentValueIt = symmetricAttribute->absentBegin();
  for (vector<Value*>::iterator absentValueIt = values.begin() + absentIndex; absentValueIt != values.end(); )
    {
      if ((*absentValueIt)->getPresentAndPotentialNoise() > presentAndPotentialIrrelevancyThreshold)
	{
	  removeAbsentValue(absentValueIt);
	  symmetricAttribute->removeAbsentValue(symmetricAbsentValueIt);
	}
      else
	{
	  ++absentValueIt;
	  ++symmetricAbsentValueIt;
	}
    }
}

void SymmetricAttribute::sortPotentialIrrelevantAndAbsent()
{
  if (id == orderedAttributeId)
    {
      vector<Value*>::iterator irrelevantBegin = values.begin() + irrelevantIndex;
      sort(values.begin() + potentialIndex, irrelevantBegin, Value::smallerDataId);
      vector<Value*>::iterator absentBegin = values.begin() + absentIndex;
      sort(irrelevantBegin, absentBegin, Value::smallerDataId);
      sort(absentBegin, values.end(), Value::smallerDataId);
      irrelevantBegin = symmetricAttribute->irrelevantBegin();
      sort(symmetricAttribute->potentialBegin(), irrelevantBegin, Value::smallerDataId);
      absentBegin = symmetricAttribute->absentBegin();
      sort(irrelevantBegin, absentBegin, Value::smallerDataId);
      sort(absentBegin, symmetricAttribute->absentEnd(), Value::smallerDataId);
    }
}

void SymmetricAttribute::computeMinAreaWithSymmetricAttributes(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>& orthogonalAttributes, unsigned int& minArea) const
{
  orthogonalAttributes.front()->computeMinAreaOnSymmetricValue(number, minSizeIt, productOfSubsequentMinSizesIt, orthogonalAttributes.begin(), orthogonalAttributes.end(), 0, 1, minArea);
}

void SymmetricAttribute::computeMinAreaOnNonSymmetricValue(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>::const_iterator thisIt, const vector<Attribute*>::const_iterator attributeEnd, const unsigned int nbOfSymmetricElements, const unsigned int currentArea, unsigned int& minArea) const
{
  const vector<Attribute*>::const_iterator nextIt = thisIt + 2;
  if (nextIt == attributeEnd)
    {
      const unsigned int factor = ceil(sqrt(number));
      if (factor <= irrelevantIndex)
	{
	  unsigned int area = factor * (factor - 1) * currentArea;
	  if (area < minArea)
	    {
	      minArea = area;
	    }
	}
      return;
    }
  const vector<unsigned int>::const_reverse_iterator nextMinSizesIt = minSizeIt + 2;
  const vector<unsigned int>::const_reverse_iterator nextProductOfSubsequentMinSizesIt = productOfSubsequentMinSizesIt + 2;
  const unsigned int end = min(static_cast<unsigned int>(ceil(sqrt(number / *(productOfSubsequentMinSizesIt + 1)))), irrelevantIndex);
  for (unsigned int factor = *minSizeIt; factor <= end; ++factor)
    {
      (*nextIt)->computeMinAreaOnNonSymmetricValue(number / factor / factor, nextMinSizesIt, nextProductOfSubsequentMinSizesIt, nextIt, attributeEnd, factor, factor * factor * currentArea, minArea);
    }
}

void SymmetricAttribute::computeMinAreaOnSymmetricValue(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>::const_iterator thisIt, const vector<Attribute*>::const_iterator attributeEnd, const unsigned int nbOfSymmetricElements, const unsigned int currentArea, unsigned int& minArea) const
{
  const vector<Attribute*>::const_iterator nextIt = thisIt + 1;
  if (nextIt == attributeEnd)
    {
      const unsigned int factor = ceil(number);
      if (factor <= irrelevantIndex)
	{
	  unsigned int area = (factor - 1) * currentArea;
	  if (area < minArea)
	    {
	      minArea = area;
	    }
	}
      return;
    }
  const vector<unsigned int>::const_reverse_iterator nextMinSizesIt = minSizeIt + 1;
  const vector<unsigned int>::const_reverse_iterator nextProductOfSubsequentMinSizesIt = productOfSubsequentMinSizesIt + 1;
  const unsigned int end = min(static_cast<unsigned int>(ceil(number / *productOfSubsequentMinSizesIt)), irrelevantIndex);
  for (unsigned int factor = *minSizeIt; factor <= end; ++factor)
    {
      (*nextIt)->computeMinAreaOnSymmetricValue(number / factor, nextMinSizesIt, nextProductOfSubsequentMinSizesIt, nextIt, attributeEnd, factor, factor * currentArea, minArea);
    }
}
#endif

const bool SymmetricAttribute::symmetricValuesDoNotExtendPresent(const Value& value, const Value& symmetricValue, const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const
{
  const vector<unsigned int>::const_iterator thisEpsilonIt = epsilonVector.begin() + id;
  if (value.getPresentNoise() > *thisEpsilonIt || symmetricValue.getPresentNoise() > *(thisEpsilonIt + 1))
    {
      return true;
    }
  vector<Attribute*>::const_iterator attributeIt = attributeBegin;
  vector<unsigned int>::const_iterator epsilonIt = epsilonVector.begin();
  for (unsigned int intersectionIndex = id; epsilonIt != thisEpsilonIt && value.symmetricValuesExtendPastPresent((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd(), *epsilonIt, --intersectionIndex); ++epsilonIt)
    {
      ++attributeIt;
    }
  unsigned int reverseAttributeIndex = maxId - id - 1;
  if (!(epsilonIt == thisEpsilonIt && symmetricValue.extendsPastPresent(values.begin(), values.begin() + potentialIndex, *epsilonIt, 0) && value.extendsFuturePresent(symmetricAttribute->presentBegin(), symmetricAttribute->presentEnd(), *++epsilonIt, reverseAttributeIndex)))
    {
      return true;
    }
  for (attributeIt += 2; attributeIt != attributeEnd && value.symmetricValuesExtendFuturePresent(symmetricValue, (*attributeIt)->presentBegin(), (*attributeIt)->presentEnd(), *++epsilonIt, reverseAttributeIndex--); ++attributeIt)
    {
    }
  return attributeIt != attributeEnd;
}

const bool SymmetricAttribute::unclosed(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const
{
  if (id > symmetricAttribute->id)
    {
      return false;
    }
  vector<Value*>::const_iterator symmetricAbsentValueIt = symmetricAttribute->absentBegin();
  const vector<Value*>::const_iterator absentEnd = values.end();
  vector<Value*>::const_iterator absentValueIt = values.begin() + irrelevantIndex;
  for (; absentValueIt != absentEnd && symmetricValuesDoNotExtendPresentAndPotential(**absentValueIt, **symmetricAbsentValueIt, attributeBegin, attributeEnd); ++absentValueIt)
    {
      ++symmetricAbsentValueIt;
    }
#ifdef DEBUG
  if (absentValueIt != absentEnd)
    {
      cout << labelsVector[id][(*absentValueIt)->getDataId()] << " in symmetric attributes extends the pattern -> Prune!" << endl;
    }
#endif
  return absentValueIt != absentEnd;
}

const bool SymmetricAttribute::symmetricValuesDoNotExtendPresentAndPotential(const Value& value, const Value& symmetricValue, const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const
{
  const vector<unsigned int>::const_iterator thisEpsilonIt = epsilonVector.begin() + id;
  if (value.getPresentAndPotentialNoise() > *thisEpsilonIt || symmetricValue.getPresentAndPotentialNoise() > *(thisEpsilonIt + 1))
    {
      return true;
    }
  vector<Attribute*>::const_iterator attributeIt = attributeBegin;
  vector<unsigned int>::const_iterator epsilonIt = epsilonVector.begin();
  for (unsigned int intersectionIndex = id; epsilonIt != thisEpsilonIt && value.symmetricValuesExtendPastPresentAndPotential((*attributeIt)->presentBegin(), (*attributeIt)->potentialEnd(), *epsilonIt, --intersectionIndex); ++epsilonIt)
    {
      ++attributeIt;
    }
  unsigned int reverseAttributeIndex = maxId - id - 1;
  if (!(epsilonIt == thisEpsilonIt && symmetricValue.extendsPastPresentAndPotential(values.begin(), values.begin() + irrelevantIndex, *epsilonIt, 0) && value.extendsFuturePresentAndPotential(symmetricAttribute->presentBegin(), symmetricAttribute->potentialEnd(), *++epsilonIt, reverseAttributeIndex)))
    {
      return true;
    }
  for (attributeIt += 2; attributeIt != attributeEnd && value.symmetricValuesExtendFuturePresentAndPotential(symmetricValue, (*attributeIt)->presentBegin(), (*attributeIt)->potentialEnd(), *++epsilonIt, reverseAttributeIndex--); ++attributeIt)
    {
    }
  return attributeIt != attributeEnd;
}

void SymmetricAttribute::cleanAndSortAbsent(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd)
{
  if (id < symmetricAttribute->id)
    {
      vector<Value*>::iterator symmetricAbsentValueIt = symmetricAttribute->absentBegin();
      const vector<Value*>::iterator absentBegin = values.begin() + absentIndex;
      for (vector<Value*>::iterator absentValueIt = absentBegin; absentValueIt != values.end(); )
	{
	  if (symmetricValuesDoNotExtendPresent(**absentValueIt, **symmetricAbsentValueIt, attributeBegin, attributeEnd))
	    {
	      removeAbsentValue(absentValueIt);
	      symmetricAttribute->removeAbsentValue(symmetricAbsentValueIt);
	    }
	  else
	    {
	      ++absentValueIt;
	      ++symmetricAbsentValueIt;
	    }
	}
      sort(absentBegin, values.end(), Value::smallerDataId);
      sort(symmetricAttribute->absentBegin(), symmetricAttribute->absentEnd(), Value::smallerDataId);
    }
}

void SymmetricAttribute::shiftPotential()
{
  if (symmetricAttribute->id == orderedAttributeId && potentialIndex != irrelevantIndex)
    {
      vector<Value*>::iterator potentialBegin = values.begin() + potentialIndex;
      rotate(potentialBegin, potentialBegin + 1, values.begin() + irrelevantIndex);
      potentialBegin = symmetricAttribute->potentialBegin();
      rotate(potentialBegin, potentialBegin + 1, symmetricAttribute->potentialEnd());
    }
}
