// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015,2016 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "Attribute.h"

// // debug
// void Attribute::printPresentAndPotentialIntersectionsWithEnumeratedValueInNextAttribute(const Attribute& nextAttribute) const
// {
//   if (nextAttribute.absentIndex < nextAttribute.values.size())
//     {
//   cout << "intersections with ";
//   nextAttribute.printValueFromDataId(nextAttribute.values[nextAttribute.absentIndex]->getDataId(), cout);
//   cout << " aka " << nextAttribute.values[nextAttribute.absentIndex]->getDataId() << ':';
//   const unsigned int valueId = nextAttribute.values[nextAttribute.absentIndex]->getPresentAndPotentialIntersectionId();
//   for (const Value* value : values)
//     {
//       cout << ' ' << value->getIntersectionsBeginWithPresentAndPotentialValues()->at(valueId);
//     }
//   cout << endl;
//     }
// }

unsigned int Attribute::noisePerUnit;

#if defined DEBUG || defined VERBOSE_ELEMENT_CHOICE || defined ASSERT
vector<unsigned int> Attribute::internal2ExternalAttributeOrder;
#endif

unsigned int Attribute::maxId = 0;
unsigned int Attribute::orderedAttributeId;
vector<unsigned int> Attribute::epsilonVector;
vector<vector<vector<unsigned int>>> Attribute::nbOfValuesToReachEpsilonVector;
vector<bool> Attribute::isClosedVector;
vector<vector<string>> Attribute::labelsVector;
bool Attribute::isDensestValuePreferred;

string Attribute::outputElementSeparator;
string Attribute::emptySetString;
string Attribute::elementNoiseSeparator;
bool Attribute::isNoisePrinted;

Attribute::Attribute(): id(0), values(), potentialIndex(0), irrelevantIndex(0), absentIndex(0)
{
}

Attribute::Attribute(const vector<unsigned int>& nbOfValuesPerAttribute, const double epsilon, const vector<string>& labels): id(maxId++), values(), potentialIndex(0), irrelevantIndex(0), absentIndex(0)
{
  const vector<unsigned int>::const_iterator nbOfValuesInThisAttributeIt = nbOfValuesPerAttribute.begin() + id;
  irrelevantIndex = *nbOfValuesInThisAttributeIt;
  absentIndex = irrelevantIndex;
  unsigned int sizeOfAValue = noisePerUnit;
  for (vector<unsigned int>::const_iterator nbOfValuesInAttributeIt = nbOfValuesPerAttribute.begin(); nbOfValuesInAttributeIt != nbOfValuesPerAttribute.end(); ++nbOfValuesInAttributeIt)
    {
      if (nbOfValuesInAttributeIt != nbOfValuesInThisAttributeIt)
	{
	  sizeOfAValue *= *nbOfValuesInAttributeIt;
	}
    }
  vector<unsigned int> noisesInIntersections(nbOfValuesPerAttribute.size() - id - 1, noisePerUnit);
  vector<unsigned int>::const_iterator attributeIt = nbOfValuesInThisAttributeIt + 1;
  for (unsigned int& noiseInIntersection : noisesInIntersections)
    {
      for (vector<unsigned int>::const_iterator nbOfValuesInAttributeIt = nbOfValuesPerAttribute.begin(); nbOfValuesInAttributeIt != nbOfValuesPerAttribute.end(); ++nbOfValuesInAttributeIt)
	{
	  if (nbOfValuesInAttributeIt != attributeIt && nbOfValuesInAttributeIt != nbOfValuesInThisAttributeIt)
	    {
	      noiseInIntersection *= *nbOfValuesInAttributeIt;
	    }
	}
      ++attributeIt;
    }
  const vector<unsigned int>::const_iterator nbOfValuesInNextAttributeIt = nbOfValuesInThisAttributeIt + 1;
  values.reserve(irrelevantIndex);
  for (unsigned int valueId = 0; valueId != *nbOfValuesInThisAttributeIt; ++valueId)
    {
      values.push_back(new Value(valueId, sizeOfAValue, nbOfValuesInNextAttributeIt, nbOfValuesPerAttribute.end(), noisesInIntersections));
    }
  epsilonVector.push_back(epsilon * noisePerUnit);
  labelsVector.push_back(labels);
}

Attribute::Attribute(const Attribute& parentAttribute): id(parentAttribute.id), values(), potentialIndex(parentAttribute.potentialIndex), irrelevantIndex(parentAttribute.irrelevantIndex), absentIndex(parentAttribute.absentIndex)
{
}

Attribute::~Attribute()
{
  for (const Value* value : values)
    {
      delete value;
    }
}

Attribute* Attribute::clone() const
{
  return new Attribute(*this);
}

ostream& operator<<(ostream& out, const Attribute& attribute)
{
  attribute.printValues(attribute.values.begin(), attribute.values.begin() + attribute.irrelevantIndex, out);
  return out;
}

void Attribute::subtractSelfLoopsFromPotentialNoise(const unsigned int totalMembershipDueToSelfLoopsOnASymmetricValue, const unsigned int nbOfSymmetricElements, const vector<Attribute*>::const_iterator attributeIt, const vector<Attribute*>::const_iterator attributeEnd)
{
  const unsigned int totalMembershipDueToSelfLoopsOnIntersectionOfValueWithSymmetricValue = totalMembershipDueToSelfLoopsOnASymmetricValue / irrelevantIndex;
  const unsigned int totalMembershipDueToSelfLoopsOnValue = totalMembershipDueToSelfLoopsOnIntersectionOfValueWithSymmetricValue * nbOfSymmetricElements;
  for (Value* value : values)
    {
      value->subtractPotentialNoise(totalMembershipDueToSelfLoopsOnValue);
    }
  unsigned int intersectionIndex = 0;
  for (vector<Attribute*>::const_iterator subsequentAttributeIt = attributeIt; ++subsequentAttributeIt != attributeEnd; ++intersectionIndex)
    {
      if ((*subsequentAttributeIt)->symmetric())
	{
	  for (Value* value : values)
	    {
	      value->subtractPotentialNoiseAtIntersections(totalMembershipDueToSelfLoopsOnIntersectionOfValueWithSymmetricValue, intersectionIndex);
	    }
	}
      else
	{
	  const unsigned int totalMembershipDueToSelfLoopsOnIntersectionOfValueWithNonSymmetricValue = totalMembershipDueToSelfLoopsOnValue / (*subsequentAttributeIt)->irrelevantIndex;
	  for (Value* value : values)
	    {
	      value->subtractPotentialNoiseAtIntersections(totalMembershipDueToSelfLoopsOnIntersectionOfValueWithNonSymmetricValue, intersectionIndex);
	    }
	}
    }
}

void Attribute::setPresentIntersections(const vector<Attribute*>::const_iterator parentAttributeIt, const vector<Attribute*>::const_iterator parentAttributeEnd, const unsigned int presentAttributeId)
{
  values.reserve((*parentAttributeIt)->values.size());
  const vector<Attribute*>::const_iterator parentNextAttributeIt = parentAttributeIt + 1;
  unsigned int newId = 0;
  vector<Value*>::const_iterator parentValueIt = (*parentAttributeIt)->values.begin();
  vector<Value*>::const_iterator end = parentValueIt + potentialIndex;
  for (; parentValueIt != end; ++parentValueIt)
    {
      values.push_back(createPresentChildValue(**parentValueIt, newId++, parentNextAttributeIt, parentAttributeEnd, presentAttributeId));
    }
  if (isEnumeratedAttribute(presentAttributeId))
    {
      values.push_back(createChosenValue(**parentValueIt++, newId, parentNextAttributeIt, parentAttributeEnd));
      ++potentialIndex;
    }
  newId = 0;
  end = (*parentAttributeIt)->values.end();
  for (; parentValueIt != end; ++parentValueIt)
    {
      values.push_back(createPotentialOrAbsentChildValue(**parentValueIt, newId++, parentNextAttributeIt, parentAttributeEnd, presentAttributeId));
    }
}

Value* Attribute::createPresentChildValue(const Value& parentValue, const unsigned int newId, const vector<Attribute*>::const_iterator parentNextAttributeIt, const vector<Attribute*>::const_iterator parentAttributeEnd, const unsigned int presentAttributeId) const
{
  Value* childValue = new Value(parentValue, newId);
  vector<vector<unsigned int>>::iterator noiseInIntersectionWithPresentValuesIt = childValue->getIntersectionsBeginWithPresentValues();
  vector<vector<unsigned int>>::const_iterator parentNoiseInIntersectionWithPresentValuesIt = parentValue.getIntersectionsBeginWithPresentValues();
  for (vector<Attribute*>::const_iterator parentAttributeIt = parentNextAttributeIt; parentAttributeIt != parentAttributeEnd; ++parentAttributeIt)
    {
      (*parentAttributeIt)->setPresentChildValuePresentIntersections(*parentNoiseInIntersectionWithPresentValuesIt++, *noiseInIntersectionWithPresentValuesIt++, presentAttributeId);
    }
  return childValue;
}

void Attribute::setPresentChildValuePresentIntersections(const vector<unsigned int>& noiseInIntersectionWithPresentValues, vector<unsigned int>& childNoiseInIntersectionWithPresentValues, const unsigned int presentAttributeId) const
{
  const vector<Value*>::const_iterator end = values.end();
  if (isEnumeratedAttribute(presentAttributeId))
    {
      childNoiseInIntersectionWithPresentValues.reserve(values.size() - potentialIndex - 1);
      for (vector<Value*>::const_iterator valueIt = values.begin() + potentialIndex + 1; valueIt != end; ++valueIt)
	{
	  childNoiseInIntersectionWithPresentValues.push_back(noiseInIntersectionWithPresentValues[(*valueIt)->getPresentIntersectionId()]);
	}
      return;      
    }
  childNoiseInIntersectionWithPresentValues.reserve(values.size() - potentialIndex);
  for (vector<Value*>::const_iterator valueIt = values.begin() + potentialIndex; valueIt != end; ++valueIt)
    {
      childNoiseInIntersectionWithPresentValues.push_back(noiseInIntersectionWithPresentValues[(*valueIt)->getPresentIntersectionId()]);
    }
}

Value* Attribute::createChosenValue(const Value& parentValue, const unsigned int newId, const vector<Attribute*>::const_iterator parentNextAttributeIt, const vector<Attribute*>::const_iterator parentAttributeEnd) const
{
  Value* chosenValue = new Value(parentValue, newId);
  vector<vector<unsigned int>>::iterator noiseInIntersectionWithPresentValuesIt = chosenValue->getIntersectionsBeginWithPresentValues();
  for (vector<Attribute*>::const_iterator parentAttributeIt = parentNextAttributeIt; parentAttributeIt != parentAttributeEnd; ++parentAttributeIt)
    {
      (*parentAttributeIt)->setChosenChildValuePresentIntersections(*noiseInIntersectionWithPresentValuesIt++);
    }
  return chosenValue;
}

void Attribute::setChosenChildValuePresentIntersections(vector<unsigned int>& childNoiseInIntersectionWithPresentValues) const
{
  childNoiseInIntersectionWithPresentValues.resize(values.size() - potentialIndex);
}

Value* Attribute::createPotentialOrAbsentChildValue(const Value& parentValue, const unsigned int newId, const vector<Attribute*>::const_iterator parentNextAttributeIt, const vector<Attribute*>::const_iterator parentAttributeEnd, const unsigned int presentAttributeId) const
{
  Value* childValue = new Value(parentValue, newId);
  vector<vector<unsigned int>>::iterator noiseInIntersectionWithPresentValuesIt = childValue->getIntersectionsBeginWithPresentValues();
  vector<vector<unsigned int>>::const_iterator parentNoiseInIntersectionWithPresentValuesIt = parentValue.getIntersectionsBeginWithPresentValues();
  for (vector<Attribute*>::const_iterator parentAttributeIt = parentNextAttributeIt; parentAttributeIt != parentAttributeEnd; ++parentAttributeIt)
    {
      (*parentAttributeIt)->setPotentialOrAbsentChildValuePresentIntersections(*parentNoiseInIntersectionWithPresentValuesIt++, *noiseInIntersectionWithPresentValuesIt++, presentAttributeId);
    }  
  return childValue;
}

void Attribute::setPotentialOrAbsentChildValuePresentIntersections(const vector<unsigned int>& noiseInIntersectionWithPresentValues, vector<unsigned int>& childNoiseInIntersectionWithPresentValues, const unsigned int presentAttributeId) const
{
  const vector<Value*>::const_iterator end = values.begin() + potentialIndex;
  if (isEnumeratedAttribute(presentAttributeId))
    {
      childNoiseInIntersectionWithPresentValues.reserve(potentialIndex + 1);
      for (vector<Value*>::const_iterator valueIt = values.begin(); valueIt != end; ++valueIt)
	{
	  childNoiseInIntersectionWithPresentValues.push_back(noiseInIntersectionWithPresentValues[(*valueIt)->getPresentIntersectionId()]);
	}
      childNoiseInIntersectionWithPresentValues.push_back(0);
      return;
    }
  childNoiseInIntersectionWithPresentValues.reserve(potentialIndex);
  for (vector<Value*>::const_iterator valueIt = values.begin(); valueIt != end; ++valueIt)
    {
      childNoiseInIntersectionWithPresentValues.push_back(noiseInIntersectionWithPresentValues[(*valueIt)->getPresentIntersectionId()]);
    }
}

void Attribute::setPresentAndPotentialIntersections(const vector<Attribute*>::const_iterator parentAttributeIt, const vector<Attribute*>::const_iterator attributeIt, const vector<Attribute*>::const_iterator attributeEnd)
{
  const vector<Attribute*>::const_iterator parentNextAttributeIt = parentAttributeIt + 1;
  const vector<Attribute*>::const_iterator nextAttributeIt = attributeIt + 1;
  unsigned int newId = 0;
  vector<Value*>::const_iterator parentValueIt = (*parentAttributeIt)->values.begin();
  vector<Value*>::iterator end = values.begin() + irrelevantIndex;
  vector<Value*>::iterator valueIt = values.begin();
  for (; valueIt != end; ++valueIt)
    {
      vector<vector<unsigned int>>::iterator presentAndPotentialIntersectionsIt = (*valueIt)->setPresentAndPotentialIntersectionId(newId++);
      for (; (*parentValueIt)->getDataId() != (*valueIt)->getDataId(); ++parentValueIt)
	{
	}
      vector<vector<unsigned int>>::iterator parentPresentAndPotentialIntersectionsIt = (*parentValueIt)->getIntersectionsBeginWithPresentAndPotentialValues();
      vector<Attribute*>::const_iterator subsequentParentAttributeIt = parentNextAttributeIt;
      for (vector<Attribute*>::const_iterator subsequentAttributeIt = nextAttributeIt; subsequentAttributeIt != attributeEnd; ++subsequentAttributeIt)
	{
	  (*subsequentAttributeIt)->setPresentAndPotentialIntersectionsWithPresentAndPotentialValues(**subsequentParentAttributeIt, *parentPresentAndPotentialIntersectionsIt, *presentAndPotentialIntersectionsIt);
	  (*subsequentAttributeIt)->setPresentAndPotentialIntersectionsWithAbsentValues(**subsequentParentAttributeIt++, *parentPresentAndPotentialIntersectionsIt++, *presentAndPotentialIntersectionsIt++);
	}
      ++parentValueIt;
    }
  end = values.end();
  for (valueIt = values.begin() + absentIndex; valueIt != end; ++valueIt)
    {
      vector<vector<unsigned int>>::iterator presentAndPotentialIntersectionsIt = (*valueIt)->setPresentAndPotentialIntersectionId(newId++);
      for (; (*parentValueIt)->getDataId() != (*valueIt)->getDataId(); ++parentValueIt)
	{
	}
      vector<vector<unsigned int>>::iterator parentPresentAndPotentialIntersectionsIt = (*parentValueIt)->getIntersectionsBeginWithPresentAndPotentialValues();
      vector<Attribute*>::const_iterator subsequentParentAttributeIt = parentNextAttributeIt;
      for (vector<Attribute*>::const_iterator subsequentAttributeIt = nextAttributeIt; subsequentAttributeIt != attributeEnd; ++subsequentAttributeIt)
	{
	  (*subsequentAttributeIt)->setPresentAndPotentialIntersectionsWithPresentAndPotentialValues(**subsequentParentAttributeIt++, *parentPresentAndPotentialIntersectionsIt++, *presentAndPotentialIntersectionsIt++);
	}
      ++parentValueIt;
    }
}

void Attribute::setPresentAndPotentialIntersectionsWithPresentAndPotentialValues(const Attribute& parentAttribute, const vector<unsigned int>& parentNoiseInIntersectionWithPresentAndPotentialValues, vector<unsigned int>& noiseInIntersectionWithPresentAndPotentialValues) const
{
  noiseInIntersectionWithPresentAndPotentialValues.reserve(values.size() - absentIndex + irrelevantIndex);
  vector<Value*>::const_iterator parentValueIt = parentAttribute.values.begin();
  const vector<Value*>::const_iterator end = values.begin() + irrelevantIndex;
  for (vector<Value*>::const_iterator valueIt = values.begin(); valueIt != end; ++valueIt)
    {
      for (; (*parentValueIt)->getDataId() != (*valueIt)->getDataId(); ++parentValueIt)
	{
	}
      noiseInIntersectionWithPresentAndPotentialValues.push_back(parentNoiseInIntersectionWithPresentAndPotentialValues[(*parentValueIt)->getPresentAndPotentialIntersectionId()]);
      ++parentValueIt;
    }
}

void Attribute::setPresentAndPotentialIntersectionsWithAbsentValues(const Attribute& parentAttribute, const vector<unsigned int>& parentNoiseInIntersectionWithPresentAndPotentialValues, vector<unsigned int>& noiseInIntersectionWithPresentAndPotentialValues) const
{
  vector<Value*>::const_iterator parentValueIt = parentAttribute.absentBegin();
  const vector<Value*>::const_iterator end = values.end();
  for (vector<Value*>::const_iterator valueIt = values.begin() + absentIndex; valueIt != end; ++valueIt)
    {
      for (; (*parentValueIt)->getDataId() != (*valueIt)->getDataId(); ++parentValueIt)
	{
	}
      noiseInIntersectionWithPresentAndPotentialValues.push_back(parentNoiseInIntersectionWithPresentAndPotentialValues[(*parentValueIt)->getPresentAndPotentialIntersectionId()]);
      ++parentValueIt;
    }
}

const unsigned int Attribute::getId() const
{
  return id;
}

const bool Attribute::symmetric() const
{
  return false;
}

Attribute* Attribute::thisOrFirstSymmetricAttribute()
{
  return this;
}

const bool Attribute::isEnumeratedAttribute(const unsigned int enumeratedAttributeId) const
{
  return enumeratedAttributeId == id;
}

vector<unsigned int> Attribute::getPresentAndPotentialDataIds() const
{
  vector<unsigned int> dataIds;
  dataIds.reserve(irrelevantIndex);
  const vector<Value*>::const_iterator end = values.begin() + irrelevantIndex;
  for (vector<Value*>::const_iterator valueIt = values.begin(); valueIt != end; ++valueIt)
    {
      dataIds.push_back((*valueIt)->getDataId());
    }
  return dataIds;
}

vector<unsigned int> Attribute::getIrrelevantDataIds() const
{
  vector<unsigned int> dataIds;
  dataIds.reserve(absentIndex - irrelevantIndex);
  const vector<Value*>::const_iterator end = values.begin() + absentIndex;
  for (vector<Value*>::const_iterator valueIt = values.begin() + irrelevantIndex; valueIt != end; ++valueIt)
    {
      dataIds.push_back((*valueIt)->getDataId());
    }
  return dataIds;
}

vector<Value*>::const_iterator Attribute::presentBegin() const
{
  return values.begin();
}

vector<Value*>::const_iterator Attribute::presentEnd() const
{
  return values.begin() + potentialIndex;
}

vector<Value*>::const_iterator Attribute::potentialBegin() const
{
  return values.begin() + potentialIndex;
}

vector<Value*>::const_iterator Attribute::potentialEnd() const
{
  return values.begin() + irrelevantIndex;
}

vector<Value*>::const_iterator Attribute::irrelevantBegin() const
{
  return values.begin() + irrelevantIndex;
}

vector<Value*>::const_iterator Attribute::irrelevantEnd() const
{
  return values.begin() + absentIndex;
}

vector<Value*>::const_iterator Attribute::absentBegin() const
{
  return values.begin() + absentIndex;
}

vector<Value*>::const_iterator Attribute::absentEnd() const
{
  return values.end();
}

vector<Value*>::iterator Attribute::presentBegin()
{
  return values.begin();
}

vector<Value*>::iterator Attribute::presentEnd()
{
  return values.begin() + potentialIndex;
}

vector<Value*>::iterator Attribute::potentialBegin()
{
  return values.begin() + potentialIndex;
}

vector<Value*>::iterator Attribute::potentialEnd()
{
  return values.begin() + irrelevantIndex;
}

vector<Value*>::iterator Attribute::irrelevantBegin()
{
  return values.begin() + irrelevantIndex;
}

vector<Value*>::iterator Attribute::irrelevantEnd()
{
  return values.begin() + absentIndex;
}

vector<Value*>::iterator Attribute::absentBegin()
{
  return values.begin() + absentIndex;
}

vector<Value*>::iterator Attribute::absentEnd()
{
  return values.end();
}

const unsigned int Attribute::sizeOfPresent() const
{
  return potentialIndex;
}

const unsigned int Attribute::sizeOfPresentAndPotential() const
{
  return absentIndex;
}

const bool Attribute::irrelevantEmpty() const
{
  return irrelevantIndex == absentIndex;
}

const unsigned int Attribute::globalSize() const
{
  return values.size();
}

const double Attribute::totalPresentAndPotentialNoise() const
{
  double totalNoise = 0;
  const vector<Value*>::const_iterator end = values.begin() + irrelevantIndex;
  for (vector<Value*>::const_iterator valueIt = values.begin(); valueIt != end; ++valueIt)
    {
      totalNoise += (*valueIt)->getPresentAndPotentialNoise();
    }
  return totalNoise;
}

const double Attribute::averagePresentAndPotentialNoise() const
{
  return totalPresentAndPotentialNoise() / values.size();
}

const unsigned int Attribute::getChosenValueDataId() const
{
  return values[potentialIndex]->getDataId();
}

Value& Attribute::getChosenPresentValue() const
{
  return *values[potentialIndex - 1];
}

pair<const bool, vector<unsigned int>> Attribute::setChosenValueAbsent()
{
  --irrelevantIndex;
  Value*& lastPotentialValue = values[--absentIndex];
  swap(values[potentialIndex], lastPotentialValue);
  return pair<const bool, vector<unsigned int>>(false, vector<unsigned int> {lastPotentialValue->getDataId()});
}

const unsigned int Attribute::getChosenAbsentValueDataId() const
{
  return values[irrelevantIndex]->getDataId();
}

void Attribute::repositionChosenPresentValue()
{
  if (id == orderedAttributeId)
    {
      repositionChosenPresentValueInOrderedAttribute();
    }
}

void Attribute::repositionChosenPresentValueInOrderedAttribute()
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
      return;
    }
  rotate(valuesBegin, valueIt, valueIt + 1);
}

void Attribute::keepChosenAbsentValue(const bool isValuePotentiallyPreventingClosedness)
{
  if (id == orderedAttributeId)
    {
      keepChosenAbsentValueInOrderedAttribute(isValuePotentiallyPreventingClosedness);
      return;
    }
#ifdef DETECT_NON_EXTENSION_ELEMENTS
  if (!(isValuePotentiallyPreventingClosedness && isClosedVector[id]))
#else
  if (!isClosedVector[id])
#endif
    {
      Value*& lastPotentialValue = values[irrelevantIndex];
      delete lastPotentialValue;
      lastPotentialValue = values.back();
      values.pop_back();
    }
}

void Attribute::keepChosenAbsentValueInOrderedAttribute(const bool isValuePotentiallyPreventingClosedness)
{
  const vector<Value*>::iterator valuesBegin = values.begin();
  vector<Value*>::iterator potentialBegin = valuesBegin + potentialIndex;
#ifdef DETECT_NON_EXTENSION_ELEMENTS
  if (isValuePotentiallyPreventingClosedness && isClosedVector[id])
#else
  if (isClosedVector[id])
#endif
    {
      vector<Value*>::iterator valueIt = valuesBegin + irrelevantIndex;
      if (valueIt != potentialBegin)
	{
	  rotate(potentialBegin, potentialBegin + 1, valueIt);
	}
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
  swap(*potentialBegin, values[irrelevantIndex]);
  delete *potentialBegin;
  values.erase(potentialBegin);
}

// It should be called after initialization only (values is ordered)
vector<vector<unsigned int>>::iterator Attribute::getIntersectionsBeginWithPotentialValues(const unsigned int valueId)
{
  return values[valueId]->getIntersectionsBeginWithPresentAndPotentialValues();
}

// It should be called after initialization only (values is ordered)
void Attribute::decrementPotentialNoise(const unsigned int valueId)
{
  values[valueId]->decrementPotentialNoise();
}

// It should be called after initialization only (values is ordered)
void Attribute::subtractPotentialNoise(const unsigned int valueId, const unsigned int noise)
{
  values[valueId]->subtractPotentialNoise(noise);
}

void Attribute::printValues(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, ostream& out) const
{
  bool isFirstElement = true;
  vector<string>& labels = labelsVector[id];
  for (vector<Value*>::const_iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      if (isFirstElement)
	{
	  isFirstElement = false;
	}
      else
	{
	  out << outputElementSeparator;
	}
      out << labels[(*valueIt)->getDataId()];
      if (isNoisePrinted)
	{
	  out << elementNoiseSeparator << static_cast<float>((*valueIt)->getPresentAndPotentialNoise()) / noisePerUnit;
	}
    }
  if (isFirstElement)
    {
      out << emptySetString;
    }
}

void Attribute::printValueFromDataId(const unsigned int valueDataId, ostream& out) const
{
  out << labelsVector[id][valueDataId];
}

#ifdef DEBUG
void Attribute::printPresent(ostream& out) const
{
  printValues(values.begin(), values.begin() + potentialIndex, out);
}

void Attribute::printPotential(ostream& out) const
{
  printValues(values.begin() + potentialIndex, values.begin() + absentIndex, out);
}

void Attribute::printAbsent(ostream& out) const
{
  printValues(values.begin() + absentIndex, values.end(), out);
}

void Attribute::printChosenValue(ostream& out) const
{
  out << labelsVector[id][values[potentialIndex]->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id];
}
#endif

#ifdef ASSERT
void Attribute::printValue(const Value& value, ostream& out) const
{
  out << labelsVector[id][value.getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id];
}
#endif

#if defined DEBUG || defined VERBOSE_ELEMENT_CHOICE || defined ASSERT
void Attribute::setInternal2ExternalAttributeOrder(const vector<unsigned int>& internal2ExternalAttributeOrderParam)
{
  internal2ExternalAttributeOrder = internal2ExternalAttributeOrderParam;
}
#endif

Attribute* Attribute::chooseValue(const vector<Attribute*>::iterator attributeBegin, const vector<Attribute*>::iterator attributeEnd)
{
  vector<Attribute*>::const_iterator attributeIt = attributeBegin;
#ifdef TWO_MODE_ELEMENT_CHOICE
  for (vector<unsigned int>::const_iterator epsilonIt = epsilonVector.begin(); attributeIt != attributeEnd; ++attributeIt)
    {
      unsigned int presentArea = noisePerUnit;
      for (vector<Attribute*>::const_iterator otherAttributeIt = attributeBegin; otherAttributeIt != attributeEnd; ++otherAttributeIt)
  	{
  	  if (otherAttributeIt != attributeIt)
  	    {
  	      presentArea *= (*otherAttributeIt)->potentialIndex;
  	    }
  	}
      if (presentArea > *epsilonIt++)
  	{
  	  break;
  	}
    }  
  if (attributeIt == attributeEnd)
    {
#ifdef VERBOSE_ELEMENT_CHOICE
      cout << "Currently too few present values to exceed any epsilon" << endl;
#endif
      Attribute* bestAttribute = nullptr;
      float bestCost = numeric_limits<float>::infinity();
      vector<Attribute*> orderedAttributes(attributeBegin, attributeEnd);
      sort(orderedAttributes.begin(), orderedAttributes.end(), lessPresentAndPotentialValues);
      const vector<Attribute*>::iterator orderedAttributeEnd = orderedAttributes.end();
      for (vector<Attribute*>::iterator orderedAttributeIt = orderedAttributes.begin(); orderedAttributeIt != orderedAttributeEnd; ++orderedAttributeIt)
  	{
  	  Attribute* currentAttribute = *orderedAttributeIt;
#ifdef VERBOSE_ELEMENT_CHOICE
  	  cout << "Multiplicative partitions of attribute " << internal2ExternalAttributeOrder[currentAttribute->id] << "'s floor(epsilon) + 1: " << epsilonVector[currentAttribute->id] / noisePerUnit + 1 << endl;
#endif	  
  	  vector<Attribute*>::iterator nextAttributeIt = orderedAttributes.erase(orderedAttributeIt);
  	  vector<unsigned int> factorization;
  	  factorization.reserve(maxId - 2);
  	  multiplicativePartition(epsilonVector[currentAttribute->id] / noisePerUnit + 1, maxId - 1, 1, orderedAttributes, factorization, bestCost, bestAttribute);
  	  orderedAttributes.insert(nextAttributeIt, currentAttribute);
  	}
      bestAttribute = bestAttribute->thisOrFirstSymmetricAttribute();
      bestAttribute->chooseValue(bestAttribute->getAppealAndIndexOfValueToChoose(attributeBegin, attributeEnd).second);
#ifdef VERBOSE_ELEMENT_CHOICE
      cout << labelsVector[bestAttribute->id][bestAttribute->values[bestAttribute->potentialIndex]->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[bestAttribute->id] << " chosen" << endl;
#endif
      return bestAttribute;
    }
#ifdef VERBOSE_ELEMENT_CHOICE
  cout << "Enough present values to exceed some epsilon" << endl;
#endif
#endif
  for (attributeIt = attributeBegin; (*attributeIt)->potentialIndex == (*attributeIt)->irrelevantIndex; ++attributeIt)
    {
    }
  Attribute* bestAttribute = *attributeIt;
  pair<double, unsigned int> appealAndIndexOfValueToChoose = bestAttribute->getAppealAndIndexOfValueToChoose(attributeBegin, attributeEnd);
#ifdef VERBOSE_ELEMENT_CHOICE
  cout << "Appeal of "  << labelsVector[bestAttribute->id][bestAttribute->values[appealAndIndexOfValueToChoose.second]->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[bestAttribute->id] << ": " << appealAndIndexOfValueToChoose.first << endl;
#endif
  while (++attributeIt != attributeEnd)
    {
      if ((*attributeIt)->potentialIndex != (*attributeIt)->irrelevantIndex)
	{
          const pair<double, unsigned int> appealAndIndexOfValueToChooseInAttribute = (*attributeIt)->getAppealAndIndexOfValueToChoose(attributeBegin, attributeEnd);
#ifdef VERBOSE_ELEMENT_CHOICE
	  cout << "Appeal of "  << labelsVector[(*attributeIt)->id][(*attributeIt)->values[appealAndIndexOfValueToChooseInAttribute.second]->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[(*attributeIt)->id] << ": " << appealAndIndexOfValueToChooseInAttribute.first << endl;
#endif
	  if (appealAndIndexOfValueToChooseInAttribute.first > appealAndIndexOfValueToChoose.first)
	    {
              appealAndIndexOfValueToChoose = appealAndIndexOfValueToChooseInAttribute;
	      bestAttribute = *attributeIt;
            }
        }
    }
  bestAttribute->chooseValue(appealAndIndexOfValueToChoose.second);
#ifdef VERBOSE_ELEMENT_CHOICE
  cout << labelsVector[bestAttribute->id][bestAttribute->values[bestAttribute->potentialIndex]->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[bestAttribute->id] << " chosen" << endl;
#endif
  return bestAttribute;
}

pair<double, unsigned int> Attribute::getAppealAndIndexOfValueToChoose(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const
{
  unsigned int presentArea = 1;
  unsigned int presentAndPotentialArea = 1;
  unsigned int testedArea = 0;
  for (vector<Attribute*>::const_iterator potentialAttributeIt = attributeBegin; potentialAttributeIt != attributeEnd; ++potentialAttributeIt)
    {
      if ((*potentialAttributeIt)->id != id)
      	{
	  presentArea *= (*potentialAttributeIt)->potentialIndex;
	  presentAndPotentialArea *= (*potentialAttributeIt)->irrelevantIndex;
	  unsigned int yesFactor = (*potentialAttributeIt)->irrelevantIndex - (*potentialAttributeIt)->potentialIndex;
      	  for (vector<Attribute*>::const_iterator presentAttributeIt = attributeBegin; presentAttributeIt != attributeEnd; ++presentAttributeIt)
      	    {
      	      if ((*presentAttributeIt)->id != id && presentAttributeIt != potentialAttributeIt)
      		{
      		  yesFactor *= (*presentAttributeIt)->potentialIndex;
      		}
      	    }
	  testedArea += yesFactor;
	}
    }
  if (testedArea == 0)
    {
#if ENUMERATION_PROCESS == 1
      if (presentArea == 0)
	{
#endif
#ifdef TWO_MODE_ELEMENT_CHOICE
	  return pair<double, unsigned int>(0, potentialIndex);
#endif
	  if (potentialIndex == 0)
	    {
	      return pair<double, unsigned int>(0, getAppealAndIndexOfValueToChoose(1, 0).second);
	    }
	  return pair<double, unsigned int>(-1, getAppealAndIndexOfValueToChoose(1, 0).second);
#if ENUMERATION_PROCESS == 1
	}
      return getAppealAndIndexOfValueToChoose(1, 0);
#endif
    }
#if ENUMERATION_PROCESS == 0
  return pair<double, unsigned int>(testedArea, getAppealAndIndexOfValueToChoose(1, 0).second);
#endif
  const double presentAndPotentialCoeff = static_cast<double>(testedArea) / (presentAndPotentialArea - presentArea);
  return getAppealAndIndexOfValueToChoose(-presentAndPotentialCoeff + maxId - 1, presentAndPotentialCoeff);
}

pair<double, unsigned int> Attribute::getAppealAndIndexOfValueToChoose(const double presentCoeff, const double presentAndPotentialCoeff) const
{
  vector<Value*>::const_iterator valueToChooseIt;
  double appealOfValueToChoose = -1;
  const vector<Value*>::const_iterator end = values.begin() + irrelevantIndex;
  for (vector<Value*>::const_iterator potentialValueIt = values.begin() + potentialIndex; potentialValueIt != end; ++potentialValueIt)
    {
      const double appealOfValue = presentCoeff * (*potentialValueIt)->getPresentNoise() + presentAndPotentialCoeff * (*potentialValueIt)->getPresentAndPotentialNoise();
      if (appealOfValue > appealOfValueToChoose)
	{
	  appealOfValueToChoose = appealOfValue;
	  valueToChooseIt = potentialValueIt;
	}
    }
  return pair<double, unsigned int>(appealOfValueToChoose, valueToChooseIt - values.begin());
}

void Attribute::chooseValue(const unsigned int indexOfValue)
{
  if (id == orderedAttributeId)
    {
      const vector<Value*>::iterator valueIt = values.begin() + indexOfValue;
      rotate(values.begin() + potentialIndex, valueIt, valueIt + 1);
      return;
    }
  swap(values[potentialIndex], values[indexOfValue]);
}

void Attribute::setPotentialValueIrrelevant(const vector<Value*>::iterator potentialValueIt)
{
  swap(*potentialValueIt, values[--irrelevantIndex]);
}

const bool Attribute::findIrrelevantValuesAndCheckTauContiguity(const vector<Attribute*>::iterator attributeBegin, const vector<Attribute*>::iterator attributeEnd)
{
  // A potential value is irrelevant if at least one previous present values is not extensible with it
  vector<Value*>::iterator potentialEnd = values.begin() + irrelevantIndex;
  for (vector<Value*>::iterator potentialValueIt = values.begin() + potentialIndex; potentialValueIt != potentialEnd; )
    {
      if (valueDoesNotExtendPresent(**potentialValueIt, attributeBegin, attributeEnd))
	{
#ifdef DEBUG
	  cout << labelsVector[id][(*potentialValueIt)->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id] << " will never be present nor extend any future pattern" << endl;
#endif
	  swap(*potentialValueIt, *--potentialEnd);
	  --irrelevantIndex;
	}
      else
	{
	  ++potentialValueIt;
	}
    }
  return false;
}

#ifdef MIN_SIZE_ELEMENT_PRUNING
const bool Attribute::presentAndPotentialIrrelevant(const unsigned int presentAndPotentialIrrelevancyThreshold) const
{
  const vector<Value*>::const_iterator end = values.begin() + potentialIndex;
  vector<Value*>::const_iterator presentValueIt = values.begin();
  for (; presentValueIt != end && (*presentValueIt)->getPresentAndPotentialNoise() <= presentAndPotentialIrrelevancyThreshold; ++presentValueIt)
    {
    }
#ifdef DEBUG
  if (presentValueIt != end)
    {
      cout << labelsVector[id][(*presentValueIt)->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id] << " contains too much potential noise given the size constraints -> Prune!" << endl;
    }
#endif
  return presentValueIt != end;
}

pair<bool, vector<unsigned int>> Attribute::findPresentAndPotentialIrrelevantValuesAndCheckTauContiguity(const unsigned int presentAndPotentialIrrelevancyThreshold)
{
  vector<unsigned int> newIrrelevantValueDataIds;
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
	}
      else
	{
	  ++potentialValueIt;
	}
    }
  return pair<bool, vector<unsigned int>>(false, newIrrelevantValueDataIds);
}

const bool Attribute::presentAndPotentialIrrelevantValue(const Value& value, const unsigned int presentAndPotentialIrrelevancyThreshold) const
{
#ifdef DEBUG
  if (value.getPresentAndPotentialNoise() > presentAndPotentialIrrelevancyThreshold)
    {
      cout << "Given the minimal size constraints, " << labelsVector[id][value.getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id] << " will never be present nor extend any future pattern" << endl;
    }
#endif
  return value.getPresentAndPotentialNoise() > presentAndPotentialIrrelevancyThreshold;
}

void Attribute::presentAndPotentialCleanAbsent(const unsigned int presentAndPotentialIrrelevancyThreshold)
{
  const vector<Value*>::iterator absentBegin = values.begin() + absentIndex;
  for (vector<Value*>::iterator absentValueIt = absentBegin; absentValueIt != values.end(); )
    {
      if ((*absentValueIt)->getPresentAndPotentialNoise() > presentAndPotentialIrrelevancyThreshold)
	{
	  removeAbsentValue(absentValueIt);
	}
      else
	{
	  ++absentValueIt;
	}
    }
}

void Attribute::sortPotentialIrrelevantAndAbsent()
{
  if (id == orderedAttributeId)
    {
      const vector<Value*>::iterator irrelevantBegin = values.begin() + irrelevantIndex;
      sort(values.begin() + potentialIndex, irrelevantBegin, Value::smallerDataId);
      const vector<Value*>::iterator absentBegin = values.begin() + absentIndex;
      sort(irrelevantBegin, absentBegin, Value::smallerDataId);
      sort(absentBegin, values.end(), Value::smallerDataId);
    }
}

const unsigned int Attribute::minSizeIrrelevancyThreshold(const vector<unsigned int>& minSizes, const double minArea, const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const
{
  // Sum epsilon with the number of non-self-loops tuples in the maximal pattern and subtract the number of non-self-loops tuples in the minimal pattern
  bool isActualMinAreaActually0 = false;
  unsigned int minNbOfSymmetricElements = 0;
  unsigned int maxNbOfSymmetricElements = 0;
  vector<unsigned int> productsOfSubsequentMinSizes;
  productsOfSubsequentMinSizes.reserve(maxId - 1);
  unsigned int actualMinArea = 1;
  vector<unsigned int> minSizesOfOrthogonalAttributes;
  minSizesOfOrthogonalAttributes.reserve(maxId - 1);
  unsigned int actualMaxArea = 1;
  vector<Attribute*> orthogonalAttributes;
  orthogonalAttributes.reserve(maxId - 1);
  vector<Attribute*>::const_iterator attributeIt = attributeBegin;
  for (; *attributeIt != this; ++attributeIt)
    {
      orthogonalAttributes.push_back(*attributeIt);
    }
  while (++attributeIt != attributeEnd)
    {
      orthogonalAttributes.push_back(*attributeIt);
    }
  const vector<Attribute*>::const_reverse_iterator rend = orthogonalAttributes.rend();
  for (vector<Attribute*>::const_reverse_iterator orthogonalAttributeIt = orthogonalAttributes.rbegin(); orthogonalAttributeIt != rend; ++orthogonalAttributeIt)
    {
      const unsigned int minSize = max((*orthogonalAttributeIt)->potentialIndex, minSizes[(*orthogonalAttributeIt)->id]);
      if (minSize == 0)
	{
	  minSizesOfOrthogonalAttributes.push_back(1);
	  isActualMinAreaActually0 = true;
	}
      else
	{
	  minSizesOfOrthogonalAttributes.push_back(minSize);
	}
      actualMinArea *= minSizesOfOrthogonalAttributes.back();
      productsOfSubsequentMinSizes.push_back(actualMinArea);
      actualMaxArea *= (*orthogonalAttributeIt)->absentIndex;
      if ((*orthogonalAttributeIt)->symmetric() && (*orthogonalAttributeIt)->absentIndex > maxNbOfSymmetricElements) // absentIndex can differ between symmetric attributes!
      	{
      	  minNbOfSymmetricElements = minSize;
      	  maxNbOfSymmetricElements = (*orthogonalAttributeIt)->absentIndex;
      	}
    }
  if (maxNbOfSymmetricElements == 0)
    {
      if (actualMinArea < minArea / irrelevantIndex)
  	{
  	  actualMinArea = numeric_limits<unsigned int>::max();
  	  orthogonalAttributes.front()->computeMinArea(minArea / irrelevantIndex, minSizesOfOrthogonalAttributes.rbegin(), productsOfSubsequentMinSizes.rbegin() + 1, orthogonalAttributes.begin(), orthogonalAttributes.end(), 1, actualMinArea);
  	  return epsilonVector[id] + (actualMaxArea - actualMinArea) * noisePerUnit;
  	}
      if (isActualMinAreaActually0)
  	{
  	  return epsilonVector[id] + (actualMaxArea - ceil(minArea / irrelevantIndex)) * noisePerUnit;
  	}
      return epsilonVector[id] + (actualMaxArea - actualMinArea) * noisePerUnit;
    }
  if (actualMinArea < minArea / irrelevantIndex)
    {
      actualMinArea = numeric_limits<unsigned int>::max();
      computeMinAreaWithSymmetricAttributes(minArea / irrelevantIndex, minSizesOfOrthogonalAttributes.rbegin(), productsOfSubsequentMinSizes.rbegin() + 1, orthogonalAttributes, actualMinArea);
      return epsilonVector[id] + (actualMaxArea - actualMaxArea / maxNbOfSymmetricElements - actualMinArea) * noisePerUnit;
    }
  if (isActualMinAreaActually0)
    {
      return epsilonVector[id] + (actualMaxArea - ceil(minArea / irrelevantIndex)) * noisePerUnit;
    }
  return epsilonVector[id] + (actualMaxArea - actualMaxArea / maxNbOfSymmetricElements - actualMinArea + actualMinArea / minNbOfSymmetricElements) * noisePerUnit;
}

void Attribute::computeMinArea(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>::const_iterator thisIt, const vector<Attribute*>::const_iterator attributeEnd, const unsigned int currentArea, unsigned int& minArea) const
{
  const vector<Attribute*>::const_iterator nextIt = thisIt + 1;
  if (nextIt == attributeEnd)
    {
      const unsigned int factor = ceil(number);
      if (factor <= irrelevantIndex && factor * currentArea < minArea)
	{
	  minArea = factor * currentArea;
	}
      return;
    }
  const vector<unsigned int>::const_reverse_iterator nextMinSizesIt = minSizeIt + 1;
  const vector<unsigned int>::const_reverse_iterator nextProductOfSubsequentMinSizesIt = productOfSubsequentMinSizesIt + 1;
  const unsigned int end = min(static_cast<unsigned int>(ceil(number / *productOfSubsequentMinSizesIt)), irrelevantIndex);
  for (unsigned int factor = *minSizeIt; factor <= end; ++factor)
    {
      (*nextIt)->computeMinArea(number / factor, nextMinSizesIt, nextProductOfSubsequentMinSizesIt, nextIt, attributeEnd, factor * currentArea, minArea);
    }
}

void Attribute::computeMinAreaWithSymmetricAttributes(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>& orthogonalAttributes, unsigned int& minArea) const
{
  orthogonalAttributes.front()->computeMinAreaOnNonSymmetricValue(number, minSizeIt, productOfSubsequentMinSizesIt, orthogonalAttributes.begin(), orthogonalAttributes.end(), 0, 1, minArea);
}

void Attribute::computeMinAreaOnNonSymmetricValue(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>::const_iterator thisIt, const vector<Attribute*>::const_iterator attributeEnd, const unsigned int nbOfSymmetricElements, const unsigned int currentArea, unsigned int& minArea) const
{
  const vector<Attribute*>::const_iterator nextIt = thisIt + 1;
  if (nextIt == attributeEnd)
    {
      terminateMinAreaComputationWithSymmetricAttributes(number, nbOfSymmetricElements, currentArea, minArea);
      return;
    }
  const vector<unsigned int>::const_reverse_iterator nextMinSizesIt = minSizeIt + 1;
  const vector<unsigned int>::const_reverse_iterator nextProductOfSubsequentMinSizesIt = productOfSubsequentMinSizesIt + 1;
  const unsigned int end = min(static_cast<unsigned int>(ceil(number / *productOfSubsequentMinSizesIt)), irrelevantIndex);
  for (unsigned int factor = *minSizeIt; factor <= end; ++factor)
    {
      (*nextIt)->computeMinAreaOnNonSymmetricValue(number / factor, nextMinSizesIt, nextProductOfSubsequentMinSizesIt, nextIt, attributeEnd, nbOfSymmetricElements, factor * currentArea, minArea);
    }
}

void Attribute::computeMinAreaOnSymmetricValue(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>::const_iterator thisIt, const vector<Attribute*>::const_iterator attributeEnd, const unsigned int nbOfSymmetricElements, const unsigned int currentArea, unsigned int& minArea) const
{
  const vector<Attribute*>::const_iterator nextIt = thisIt + 1;
  if (nextIt == attributeEnd)
    {
      terminateMinAreaComputationWithSymmetricAttributes(number, nbOfSymmetricElements, currentArea, minArea);
      return;
    }
  const vector<unsigned int>::const_reverse_iterator nextMinSizesIt = minSizeIt + 1;
  const vector<unsigned int>::const_reverse_iterator nextProductOfSubsequentMinSizesIt = productOfSubsequentMinSizesIt + 1;
  const unsigned int end = min(static_cast<unsigned int>(ceil(number / *productOfSubsequentMinSizesIt)), irrelevantIndex);
  for (unsigned int factor = *minSizeIt; factor <= end; ++factor)
    {
      (*nextIt)->computeMinAreaOnSymmetricValue(number / factor, nextMinSizesIt, nextProductOfSubsequentMinSizesIt, nextIt, attributeEnd, nbOfSymmetricElements, factor * currentArea, minArea);
    }
}

void Attribute::terminateMinAreaComputationWithSymmetricAttributes(const float number, const unsigned int nbOfSymmetricElements, const unsigned int currentArea, unsigned int& minArea) const
{
  const unsigned int factor = ceil(number);
  if (factor <= irrelevantIndex)
    {
      unsigned int area = factor * currentArea;
      area -= area / nbOfSymmetricElements;
      if (area < minArea)
	{
	  minArea = area;
	}
    }
}
#endif

const bool Attribute::valueDoesNotExtendPresent(const Value& value, const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const
{
  const vector<unsigned int>::const_iterator thisEpsilonIt = epsilonVector.begin() + id;
  if (value.getPresentNoise() > *thisEpsilonIt)
    {
      return true;
    }
  vector<Attribute*>::const_iterator attributeIt = attributeBegin;
  vector<unsigned int>::const_iterator epsilonIt = epsilonVector.begin() + (*attributeIt)->id;
  for (unsigned int intersectionIndex = id; epsilonIt != thisEpsilonIt && value.extendsPastPresent((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd(), *epsilonIt, --intersectionIndex); ++epsilonIt)
    {
      ++attributeIt;
    }
  if (epsilonIt != thisEpsilonIt)
    {
      return true;
    }
  for (unsigned int reverseAttributeIndex = maxId - id; ++attributeIt != attributeEnd && value.extendsFuturePresent((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd(), *(++epsilonIt), --reverseAttributeIndex); )
    {
    }
  return attributeIt != attributeEnd;
}

vector<unsigned int> Attribute::eraseIrrelevantValues()
{
  vector<unsigned int> dataIds;
  dataIds.reserve(absentIndex - irrelevantIndex);
  const vector<Value*>::iterator begin = values.begin() + irrelevantIndex;
  const vector<Value*>::iterator end = values.begin() + absentIndex;
  for (vector<Value*>::iterator irrelevantValueIt = begin; irrelevantValueIt != end; ++irrelevantValueIt)
    {
      dataIds.push_back((*irrelevantValueIt)->getDataId());
      delete *irrelevantValueIt;
    }
  values.erase(begin, end);
  absentIndex = irrelevantIndex;
  if (id == orderedAttributeId)
    {
      sort(dataIds.begin(), dataIds.end());
    }
  return dataIds;
}

const bool Attribute::unclosed(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const
{
  const vector<Value*>::const_iterator absentEnd = values.end();
  vector<Value*>::const_iterator absentValueIt = values.begin() + irrelevantIndex;
  for (; absentValueIt != absentEnd && valueDoesNotExtendPresentAndPotential(**absentValueIt, attributeBegin, attributeEnd); ++absentValueIt)
    {
    }
#ifdef DEBUG
  if (absentValueIt != absentEnd)
    {
      cout << labelsVector[id][(*absentValueIt)->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id] << " extends any future pattern -> Prune!" << endl;
    }
#endif
  return absentValueIt != absentEnd;
}

const bool Attribute::valueDoesNotExtendPresentAndPotential(const Value& value, const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const
{
  const vector<unsigned int>::const_iterator thisEpsilonIt = epsilonVector.begin() + id;
  if (value.getPresentAndPotentialNoise() > *thisEpsilonIt)
    {
      return true;
    }
  vector<Attribute*>::const_iterator attributeIt = attributeBegin;
  vector<unsigned int>::const_iterator epsilonIt = epsilonVector.begin() + (*attributeIt)->id;
  for (unsigned int intersectionIndex = id; epsilonIt != thisEpsilonIt && value.extendsPastPresentAndPotential((*attributeIt)->presentBegin(), (*attributeIt)->potentialEnd(), *epsilonIt, --intersectionIndex); ++epsilonIt)
    {
      ++attributeIt;
    }
  if (epsilonIt != thisEpsilonIt)
    {
      return true;
    }
  for (unsigned int reverseAttributeIndex = maxId - id; ++attributeIt != attributeEnd && value.extendsFuturePresentAndPotential((*attributeIt)->presentBegin(), (*attributeIt)->potentialEnd(), *(++epsilonIt), --reverseAttributeIndex); )
    {
    }
  return attributeIt != attributeEnd;
}

void Attribute::cleanAndSortAbsent(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd)
{
  const vector<Value*>::iterator absentBegin = values.begin() + absentIndex;
  for (vector<Value*>::iterator absentValueIt = absentBegin; absentValueIt != values.end(); )
    {
      if (valueDoesNotExtendPresent(**absentValueIt, attributeBegin, attributeEnd))
	{
	  Attribute::removeAbsentValue(absentValueIt);
	}
      else
	{
	  ++absentValueIt;
	}
    }
  sort(absentBegin, values.end(), Value::smallerDataId);
}

void Attribute::sortPotential()
{
  const vector<Value*>::iterator irrelevantBegin = values.begin() + irrelevantIndex;
  sort(values.begin() + potentialIndex, irrelevantBegin, Value::smallerDataId);
  if (id == orderedAttributeId)
    {
      sort(irrelevantBegin, values.begin() + absentIndex, Value::smallerDataId);
    }
}

void Attribute::sortPotentialAndAbsentButChosenValue(const unsigned int presentAttributeId)
{
  const vector<Value*>::iterator absentBegin = values.begin() + absentIndex;
  sort(absentBegin, values.end(), Value::smallerDataId);
  if (isEnumeratedAttribute(presentAttributeId))
    {
      sort(values.begin() + potentialIndex + 1, absentBegin, Value::smallerDataId);
      return;
    }
  sort(values.begin() + potentialIndex, absentBegin, Value::smallerDataId);
}

void Attribute::removeAbsentValue(vector<Value*>::iterator& absentValueIt)
{
#ifdef DEBUG
  cout << labelsVector[id][(*absentValueIt)->getDataId()] << " in attribute " << internal2ExternalAttributeOrder[id] << " will never extend any future pattern" << endl;
#endif
  delete *absentValueIt;
  *absentValueIt = values.back();
  values.pop_back();
}

const bool Attribute::finalizable() const
{
  if (isClosedVector[id] || potentialIndex == irrelevantIndex)
    {
      const unsigned int epsilon = epsilonVector[id];
      const vector<Value*>::const_iterator end = values.begin() + irrelevantIndex;
      vector<Value*>::const_iterator valueIt = values.begin();
      for (; valueIt != end && (*valueIt)->getPresentAndPotentialNoise() <= epsilon; ++valueIt)
	{
	}
      return valueIt == end;
    }
  return false;
}

vector<unsigned int> Attribute::finalize()
{
  vector<unsigned int> dataIdsOfValuesSetPresent;
  dataIdsOfValuesSetPresent.reserve(irrelevantIndex - potentialIndex);
  const vector<Value*>::const_iterator potentialEnd = values.begin() + irrelevantIndex;
  for (vector<Value*>::const_iterator potentialValueIt = values.begin() + potentialIndex; potentialValueIt != potentialEnd; ++potentialValueIt)
    {
      dataIdsOfValuesSetPresent.push_back((*potentialValueIt)->getDataId());
    }
  return dataIdsOfValuesSetPresent;
}

const unsigned int Attribute::lastAttributeId()
{
  return maxId - 1;
}

const vector<unsigned int>& Attribute::getEpsilonVector()
{
  return epsilonVector;
}

void Attribute::setIsClosedVectorAndIsStorageAllDense(const vector<bool>& isClosedVectorParam, const bool isStorageAllDense)
{
  isClosedVector = isClosedVectorParam;
  if (isStorageAllDense)
    {
      orderedAttributeId = numeric_limits<unsigned int>::max();
      return;
    }
  orderedAttributeId = maxId - 1;
}

void Attribute::multiplicativePartition(const float number, const unsigned int nbOfFactors, const unsigned int begin, const vector<Attribute*>& orderedAttributes, vector<unsigned int>& factorization, float& bestCost, Attribute*& bestAttribute)
{
  if (nbOfFactors == 1)
    {
      const unsigned int factor = ceil(number);
      if (begin <= factor)
	{
	  Attribute* firstAttribute = nullptr;
	  float cost = 1;
	  bool isFirstSymmetric = true;
	  vector<Attribute*> attributes = orderedAttributes;
	  factorization.push_back(factor);
#ifdef VERBOSE_ELEMENT_CHOICE
	  cout << "  ";
	  for (const unsigned int f : factorization)
	    {
	      cout << f << ' ';
	    }
#endif
	  const vector<unsigned int>::const_reverse_iterator factorREnd = factorization.rend();
	  for (vector<unsigned int>::const_reverse_iterator factorIt = factorization.rbegin(); factorIt != factorREnd; ++factorIt)
	    {
	      const vector<Attribute*>::iterator attributeEnd = attributes.end();
	      vector<Attribute*>::iterator attributeIt = attributes.begin();
	      for (; attributeIt != attributeEnd && (*attributeIt)->irrelevantIndex < *factorIt; ++attributeIt)
		{
		}
	      if (attributeIt == attributeEnd)
		{
#ifdef VERBOSE_ELEMENT_CHOICE
		  cout << "cannot be made" << endl;
#endif
		  factorization.pop_back();
		  return;
		}
	      if ((*attributeIt)->symmetric())
		{
		  if (isFirstSymmetric)
		    {
		      isFirstSymmetric = false;
		      cost *= math::binomial_coefficient<float>((*attributeIt)->irrelevantIndex, *factorIt);
		    }
		}
	      else
		{
		  cost *= math::binomial_coefficient<float>((*attributeIt)->irrelevantIndex, *factorIt);
		}
	      if (firstAttribute == nullptr && (*attributeIt)->potentialIndex < *factorIt)
		{
		  firstAttribute = *attributeIt;
		}
	      attributes.erase(attributeIt);
	    }
#ifdef VERBOSE_ELEMENT_CHOICE
	  cout << "first choosing attribute " << internal2ExternalAttributeOrder[firstAttribute->id] << " leads to " << cost << " nodes exceeding epsilon" << endl;
#endif
	  if (cost < bestCost)
	    {
	      bestCost = cost;
	      bestAttribute = firstAttribute;
	    }
	  factorization.pop_back();
	}
      return;
    }
  const unsigned int end = ceil(pow(number, 1. / nbOfFactors)) + 1;
  for (unsigned int factor = begin; factor != end; ++factor)
    {
      factorization.push_back(factor);
      multiplicativePartition(number / factor, nbOfFactors - 1, factor, orderedAttributes, factorization, bestCost, bestAttribute);
      factorization.pop_back();
    }
}

void Attribute::setOutputFormat(const char* outputElementSeparatorParam, const char* emptySetStringParam, const char* elementNoiseSeparatorParam, const bool isNoisePrintedParam)
{
  outputElementSeparator = outputElementSeparatorParam;
  emptySetString = emptySetStringParam;
  elementNoiseSeparator = elementNoiseSeparatorParam;
  isNoisePrinted = isNoisePrintedParam;
}

const bool Attribute::noisePrinted()
{
  return isNoisePrinted;
}

void Attribute::printOutputElementSeparator(ostream& out)
{
  out << outputElementSeparator;
}

void Attribute::printEmptySetString(ostream& out)
{
  out << emptySetString;
}

void Attribute::printNoise(const float noise, ostream& out)
{
  out << elementNoiseSeparator << noise;
}

#ifdef DEBUG_HA
void Attribute::printValuesFromDataIds(const vector<unsigned int>& dimension, const unsigned int dimensionId, ostream& out)
{
  const vector<string>& labels = labelsVector[dimensionId];
  bool isFirst = true;
  for (const unsigned int elementId : dimension)
    {
      if (isFirst)
	{
	  isFirst = false;
	}
      else
	{
	  out << outputElementSeparator;
	}
      out << labels[elementId];
    }
}
#endif

const bool Attribute::lessPresentAndPotentialValues(const Attribute* attribute, const Attribute* otherAttribute)
{
  return attribute->irrelevantIndex < otherAttribute->irrelevantIndex;
}

const bool Attribute::lessAppealingIrrelevant(const Attribute* attribute, const Attribute* otherAttribute)
{
  return (attribute->absentIndex - attribute->irrelevantIndex) * otherAttribute->values.size() < (otherAttribute->absentIndex - otherAttribute->irrelevantIndex) * attribute->values.size();
}
