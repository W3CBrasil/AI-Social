// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015 Loïc (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "Trie.h"

NoNoiseTube Trie::noNoiseTube = NoNoiseTube();

Trie::Trie(const Trie& otherTrie): hyperplanes()
{
  copy(otherTrie);
}

Trie::Trie(Trie&& otherTrie): hyperplanes(std::move(otherTrie.hyperplanes))
{
}

Trie::Trie(const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator cardinalityEnd): hyperplanes()
{
  const unsigned int cardinality = *cardinalityIt;
  hyperplanes.reserve(cardinality);
  if (cardinalityIt + 2 == cardinalityEnd)
    {
      if (Attribute::noisePerUnit == 1)
	{
	  for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
	    {
	      hyperplanes.push_back(new SparseCrispTube());
	    }
	  return;
	}
      for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
	{
	  hyperplanes.push_back(new SparseFuzzyTube());
	}
      return;
    }
  const vector<unsigned int>::const_iterator nbOfElementsInNextDiIt = cardinalityIt + 1;
  for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
    {
      hyperplanes.push_back(new Trie(nbOfElementsInNextDiIt, cardinalityEnd));
    }
}

Trie::~Trie()
{
  for (AbstractData* hyperplane : hyperplanes)
    {
      if (hyperplane != &noNoiseTube)
	{
	  delete hyperplane;
	}
    }
}

Trie* Trie::clone() const
{
  return new Trie(*this);
}

Trie& Trie::operator=(const Trie& otherTrie)
{
  copy(otherTrie);
  return *this;
}

Trie& Trie::operator=(Trie&& otherTrie)
{
  hyperplanes = std::move(otherTrie.hyperplanes);
  return *this;
}

ostream& operator<<(ostream& out, const Trie& trie)
{
  vector<unsigned int> prefix;
  trie.print(prefix, out);
  return out;
}

void Trie::copy(const Trie& otherTrie)
{
  hyperplanes.reserve(otherTrie.hyperplanes.size());
  for (const AbstractData* hyperplane : otherTrie.hyperplanes)
    {
      hyperplanes.push_back(hyperplane->clone());
    }
}

void Trie::print(vector<unsigned int>& prefix, ostream& out) const
{
  unsigned int hyperplaneId = 0;
  for (AbstractData* hyperplane : hyperplanes)
    {
      prefix.push_back(hyperplaneId++);
      hyperplane->print(prefix, out);
      prefix.pop_back();
    }
}

void Trie::setSelfLoops(const unsigned int firstSymmetricAttributeId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute)
{
  setSelfLoopsBeforeSymmetricAttributes(firstSymmetricAttributeId, lastSymmetricAttributeId, sizeOfLastAttribute, 0);
}

void Trie::setSelfLoopsBeforeSymmetricAttributes(const unsigned int firstSymmetricAttributeId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute, const unsigned int dimensionId)
{
  const unsigned int nextDimensionId = dimensionId + 1;
  if (dimensionId == firstSymmetricAttributeId)
    {
      unsigned int hyperplaneId = 0;
      for (AbstractData*& hyperplane : hyperplanes)
	{
	  setSelfLoopsInSymmetricAttributes(hyperplane, hyperplaneId++, lastSymmetricAttributeId, sizeOfLastAttribute, nextDimensionId);
	}
      return;
    }
  for (AbstractData* hyperplane : hyperplanes)
    {
      static_cast<Trie*>(hyperplane)->setSelfLoopsBeforeSymmetricAttributes(firstSymmetricAttributeId, lastSymmetricAttributeId, sizeOfLastAttribute, nextDimensionId);
    }
}

void Trie::setSelfLoopsInSymmetricAttributes(AbstractData*& hyperplane, const unsigned int hyperplaneId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute, const unsigned int nextDimensionId)
{
  if (hyperplane->setSelfLoopsInSymmetricAttributes(hyperplaneId, lastSymmetricAttributeId, sizeOfLastAttribute, nextDimensionId))
    {
      delete hyperplane;
      if (Attribute::noisePerUnit == 1)
	{
	  hyperplane = new DenseCrispTube(sizeOfLastAttribute, hyperplaneId);
	  return;
	}
      hyperplane = new DenseFuzzyTube(sizeOfLastAttribute, hyperplaneId);
    }
}

const bool Trie::setSelfLoopsInSymmetricAttributes(const unsigned int hyperplaneId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute, const unsigned int dimensionId)
{
  AbstractData*& hyperplane = hyperplanes[hyperplaneId];
  if (dimensionId == lastSymmetricAttributeId)
    {
      if (dynamic_cast<Tube*>(hyperplane))
	{
	  delete hyperplane;
	  hyperplane = &noNoiseTube;
	  return false;
	}
      static_cast<Trie*>(hyperplane)->setSelfLoopsAfterSymmetricAttributes();
      return false;
    }
  setSelfLoopsInSymmetricAttributes(hyperplane, hyperplaneId, lastSymmetricAttributeId, sizeOfLastAttribute, dimensionId + 1);
  return false;
}

void Trie::setSelfLoopsAfterSymmetricAttributes()
{
  const vector<AbstractData*>::iterator end = hyperplanes.end();
  vector<AbstractData*>::iterator hyperplaneIt = hyperplanes.begin();
  if (dynamic_cast<Tube*>(*hyperplaneIt))
    {
      for (; hyperplaneIt != end; ++hyperplaneIt)
	{
	  delete *hyperplaneIt;
	  *hyperplaneIt = &noNoiseTube;
	}
      return;
    }
  for (; hyperplaneIt != end; ++hyperplaneIt)
    {
      static_cast<Trie*>(*hyperplaneIt)->setSelfLoopsAfterSymmetricAttributes();
    }
}

void Trie::setHyperplane(const unsigned int hyperplaneOldId, const unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator begin, const unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator end, const vector<unsigned int>& attributeOrder, const vector<vector<unsigned int>>& oldIds2NewIds, vector<Attribute*>& attributes)
{
  const vector<Attribute*>::iterator nextAttributeIt = attributes.begin() + 1;
  const unsigned int hyperplaneId = oldIds2NewIds.front().at(hyperplaneOldId);
  AbstractData*& hyperplane = hyperplanes[hyperplaneId];
  for (unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator tupleIt = begin; tupleIt != end; ++tupleIt)
    {
      vector<vector<vector<unsigned int>>::iterator> intersectionIts;
      intersectionIts.reserve(attributeOrder.size());
      intersectionIts.push_back(attributes.front()->getIntersectionsBeginWithPotentialValues(hyperplaneId));
      const unsigned int membership = ceil(tupleIt->second * Attribute::noisePerUnit); // ceil to guarantee that every pattern to be returned is returned
      setTuple(hyperplane, tupleIt->first, membership, attributeOrder.begin(), ++oldIds2NewIds.begin(), nextAttributeIt, intersectionIts);
      attributes.front()->subtractPotentialNoise(hyperplaneId, membership);
    }
  sortTubes();
}

void Trie::setTuple(AbstractData*& hyperplane, const vector<unsigned int>& tuple, const unsigned int membership, vector<unsigned int>::const_iterator attributeIdIt, vector<vector<unsigned int>>::const_iterator oldIds2NewIdsIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts)
{
  if (hyperplane->setTuple(tuple, membership, attributeIdIt, oldIds2NewIdsIt, nextAttributeIt, intersectionIts))
    {
      AbstractData* newHyperplane;
      if (Attribute::noisePerUnit == 1)
	{
	  newHyperplane = new DenseCrispTube(static_cast<SparseCrispTube*>(hyperplane)->getDenseRepresentation((*nextAttributeIt)->sizeOfPresentAndPotential()));
	}
      else
	{
	  newHyperplane = new DenseFuzzyTube(static_cast<SparseFuzzyTube*>(hyperplane)->getDenseRepresentation((*nextAttributeIt)->sizeOfPresentAndPotential()));
	}
      delete hyperplane;
      hyperplane = newHyperplane;
    }
}

const bool Trie::setTuple(const vector<unsigned int>& tuple, const unsigned int membership, vector<unsigned int>::const_iterator attributeIdIt, vector<vector<unsigned int>>::const_iterator oldIds2NewIdsIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts)
{
  const unsigned int element = oldIds2NewIdsIt->at(tuple[*attributeIdIt]);
  (*attributeIt)->subtractPotentialNoise(element, membership);
  for (vector<vector<unsigned int>>::iterator& intersectionIt : intersectionIts)
    {
      (*intersectionIt)[element] -= membership;
      ++intersectionIt;
    }
  intersectionIts.push_back((*attributeIt)->getIntersectionsBeginWithPotentialValues(element));
  setTuple(hyperplanes[element], tuple, membership, ++attributeIdIt, ++oldIds2NewIdsIt, attributeIt + 1, intersectionIts);
  return false;
}

void Trie::sortTubes()
{
  for (AbstractData* hyperplane : hyperplanes)
    {
      hyperplane->sortTubes();
    }
}

void Trie::setPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeBegin) const
{
  vector<vector<vector<unsigned int>>::iterator> intersectionIts;
  intersectionIts.reserve(Attribute::lastAttributeId());
  setPresent(presentAttributeIt, attributeBegin, intersectionIts);
}

void Trie::setSymmetricPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeBegin) const
{
  vector<vector<vector<unsigned int>>::iterator> intersectionIts;
  intersectionIts.reserve(Attribute::lastAttributeId());
  setSymmetricPresent(presentAttributeIt, attributeBegin, intersectionIts);
}

void Trie::setAbsent(const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator attributeBegin) const
{
  vector<vector<vector<unsigned int>>::iterator> intersectionIts;
  intersectionIts.reserve(Attribute::lastAttributeId());
  setAbsent(absentAttributeIt, absentValueDataIds, attributeBegin, intersectionIts);
}

void Trie::setSymmetricAbsent(const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator attributeBegin) const
{
  vector<vector<vector<unsigned int>>::iterator> intersectionIts;
  intersectionIts.reserve(Attribute::lastAttributeId());
  setSymmetricAbsent(absentAttributeIt, attributeBegin, intersectionIts);
}

const unsigned int Trie::setPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Attribute*>::iterator nextAttributeIt = attributeIt + 1;
  vector<vector<vector<unsigned int>>::iterator> nextIntersectionIts(incrementIterators(intersectionIts));
  if (attributeIt == presentAttributeIt)
    {
      Value& presentValue = (*presentAttributeIt)->getChosenPresentValue();
      nextIntersectionIts.push_back(presentValue.getIntersectionsBeginWithPresentValues());
      return hyperplanes[presentValue.getDataId()]->setPresentAfterPresentValueMet(nextAttributeIt, nextIntersectionIts);
    }
  presentFixPotentialOrAbsentValues(**attributeIt, presentAttributeIt, nextAttributeIt, intersectionIts);
  return presentFixPresentValues(**attributeIt, presentAttributeIt, nextAttributeIt, nextIntersectionIts);
}

const unsigned int Trie::setSymmetricPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Attribute*>::iterator nextAttributeIt = attributeIt + 1;
  vector<vector<vector<unsigned int>>::iterator> nextIntersectionIts(incrementIterators(intersectionIts));
  if (attributeIt == presentAttributeIt)
    {
      // *this necessarily relates to the first symmetric attribute
      presentFixPotentialOrAbsentValues(**attributeIt, nextAttributeIt, nextAttributeIt, intersectionIts);
      const unsigned int newNoise = presentFixPresentValues(**attributeIt, nextAttributeIt, nextAttributeIt, nextIntersectionIts);
      Value& presentValue = (*presentAttributeIt)->getChosenPresentValue();
      nextIntersectionIts.push_back(presentValue.getIntersectionsBeginWithPresentValues());
      return newNoise + hyperplanes[presentValue.getDataId()]->setSymmetricPresentAfterPresentValueMet(nextAttributeIt, nextIntersectionIts);
    }
  presentFixPotentialOrAbsentValuesBeforeSymmetricAttributes(**attributeIt, presentAttributeIt, nextAttributeIt, intersectionIts);
  return presentFixPresentValuesBeforeSymmetricAttributes(**attributeIt, presentAttributeIt, nextAttributeIt, nextIntersectionIts);
}

const unsigned int Trie::setPresentAfterPresentValueMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Attribute*>::iterator nextAttributeIt = attributeIt + 1;
  vector<vector<vector<unsigned int>>::iterator> nextIntersectionIts(incrementIterators(intersectionIts));
  presentFixPotentialOrAbsentValuesAfterPresentValueMet(**attributeIt, nextAttributeIt, intersectionIts);
  return presentFixPresentValuesAfterPresentValueMet((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd(), nextAttributeIt, nextIntersectionIts);
}

const unsigned int Trie::setSymmetricPresentAfterPresentValueMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  // *this necessarily relates to the second symmetric attribute
  const vector<Attribute*>::iterator nextAttributeIt = attributeIt + 1;
  vector<vector<vector<unsigned int>>::iterator> nextIntersectionIts(incrementIterators(intersectionIts));
  presentFixPotentialOrAbsentValuesAfterPresentValueMet(**attributeIt, nextAttributeIt, intersectionIts);
  // The last present value actually is the value set present and there is no noise to be found at the intersection of a vertex (seen as an outgoing vertex) and itself (seen as an ingoing vertex)
  return presentFixPresentValuesAfterPresentValueMet((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd() - 1, nextAttributeIt, nextIntersectionIts);
}

const unsigned int Trie::setPresentAfterPotentialOrAbsentUsed(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const
{
  if (attributeIt == presentAttributeIt)
    {
      const Value& presentValue = (*presentAttributeIt)->getChosenPresentValue();
      const unsigned int newNoiseInHyperplane = hyperplanes[presentValue.getDataId()]->setPresentAfterPresentValueMetAndPotentialOrAbsentUsed(attributeIt + 1, potentialOrAbsentValueIntersectionIt + 1);
      (*potentialOrAbsentValueIntersectionIt)[presentValue.getPresentIntersectionId()] += newNoiseInHyperplane;
      return newNoiseInHyperplane;
    }
  return presentFixPresentValuesAfterPotentialOrAbsentUsed(**attributeIt, presentAttributeIt, attributeIt + 1, potentialOrAbsentValueIntersectionIt, potentialOrAbsentValueIntersectionIt + 1);
}

const unsigned int Trie::setSymmetricPresentAfterPotentialOrAbsentUsed(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const
{
  if (attributeIt == presentAttributeIt)
    {
      // *this necessarily relates to the first symmetric attribute
      const Value& presentValue = (*presentAttributeIt)->getChosenPresentValue();
      const unsigned int newNoiseInHyperplane = hyperplanes[presentValue.getDataId()]->setSymmetricPresentAfterPresentValueMetAndPotentialOrAbsentUsed(attributeIt + 1, potentialOrAbsentValueIntersectionIt + 1);
      (*potentialOrAbsentValueIntersectionIt)[presentValue.getPresentIntersectionId()] += newNoiseInHyperplane;
      return newNoiseInHyperplane + presentFixPresentValuesAfterPotentialOrAbsentUsed(**attributeIt, presentAttributeIt + 1, attributeIt + 1, potentialOrAbsentValueIntersectionIt, potentialOrAbsentValueIntersectionIt + 1);
    }
  return presentFixPresentValuesBeforeSymmetricAttributesAfterPotentialOrAbsentUsed(**attributeIt, presentAttributeIt, attributeIt + 1, potentialOrAbsentValueIntersectionIt, potentialOrAbsentValueIntersectionIt + 1);
}

const unsigned int Trie::setPresentAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const
{
  return presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd(), attributeIt + 1, potentialOrAbsentValueIntersectionIt, potentialOrAbsentValueIntersectionIt + 1);
}

const unsigned int Trie::setSymmetricPresentAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const
{
  // *this necessarily relates to the second symmetric attribute
  // The last present value actually is the value set present and there is no noise to be found at the intersection of a vertex (seen as an outgoing vertex) and itself (seen as an ingoing vertex)
  return presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd() - 1, attributeIt + 1, potentialOrAbsentValueIntersectionIt, potentialOrAbsentValueIntersectionIt + 1);
}

const unsigned int Trie::presentFixPresentValues(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const
{
  unsigned int newNoise = 0;
  const vector<Value*>::iterator end = currentAttribute.presentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.presentBegin(); valueIt != end; ++valueIt)
    {
      nextIntersectionIts.push_back((*valueIt)->getIntersectionsBeginWithPresentValues());
      const unsigned int newNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setPresent(presentAttributeIt, nextAttributeIt, nextIntersectionIts);
      nextIntersectionIts.pop_back();
      (*valueIt)->addPresentNoise(newNoiseInHyperplane);
      newNoise += newNoiseInHyperplane;
    }
  return newNoise;
}

const unsigned int Trie::presentFixPresentValuesBeforeSymmetricAttributes(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const
{
  unsigned int newNoise = 0;
  const vector<Value*>::iterator end = currentAttribute.presentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.presentBegin(); valueIt != end; ++valueIt)
    {
      nextIntersectionIts.push_back((*valueIt)->getIntersectionsBeginWithPresentValues());
      // Since this is before the symmetric attributes, hyperplanes necessarily are tries
      const unsigned int newNoiseInHyperplane = static_cast<Trie*>(hyperplanes[(*valueIt)->getDataId()])->setSymmetricPresent(presentAttributeIt, nextAttributeIt, nextIntersectionIts);
      nextIntersectionIts.pop_back();
      (*valueIt)->addPresentNoise(newNoiseInHyperplane);
      newNoise += newNoiseInHyperplane;
    }
  return newNoise;
}

const unsigned int Trie::presentFixPresentValuesAfterPresentValueMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const
{
  unsigned int newNoise = 0;
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      nextIntersectionIts.push_back((*valueIt)->getIntersectionsBeginWithPresentValues());
      const unsigned int newNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setPresentAfterPresentValueMet(nextAttributeIt, nextIntersectionIts);
      nextIntersectionIts.pop_back();
      (*valueIt)->addPresentNoise(newNoiseInHyperplane);
      newNoise += newNoiseInHyperplane;
    }
  return newNoise;
}

const unsigned int Trie::presentFixPresentValuesAfterPotentialOrAbsentUsed(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextPotentialOrAbsentValueIntersectionIt) const
{
  unsigned int newNoise = 0;
  const vector<Value*>::iterator end = currentAttribute.presentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.presentBegin(); valueIt != end; ++valueIt)
    {
      const unsigned int newNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setPresentAfterPotentialOrAbsentUsed(presentAttributeIt, nextAttributeIt, nextPotentialOrAbsentValueIntersectionIt);
      (*potentialOrAbsentValueIntersectionIt)[(*valueIt)->getPresentIntersectionId()] += newNoiseInHyperplane;
      newNoise += newNoiseInHyperplane;
    }
  return newNoise;
}

const unsigned int Trie::presentFixPresentValuesBeforeSymmetricAttributesAfterPotentialOrAbsentUsed(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextPotentialOrAbsentValueIntersectionIt) const
{
  unsigned int newNoise = 0;
  const vector<Value*>::iterator end = currentAttribute.presentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.presentBegin(); valueIt != end; ++valueIt)
    {
      // Since this is before the symmetric attributes, hyperplanes necessarily are tries
      const unsigned int newNoiseInHyperplane = static_cast<Trie*>(hyperplanes[(*valueIt)->getDataId()])->setSymmetricPresentAfterPotentialOrAbsentUsed(presentAttributeIt, nextAttributeIt, nextPotentialOrAbsentValueIntersectionIt);
      (*potentialOrAbsentValueIntersectionIt)[(*valueIt)->getPresentIntersectionId()] += newNoiseInHyperplane;
      newNoise += newNoiseInHyperplane;
    }
  return newNoise;
}

const unsigned int Trie::presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextPotentialOrAbsentValueIntersectionIt) const
{
  unsigned int newNoise = 0;
  for (vector<Value*>::const_iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int newNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setPresentAfterPresentValueMetAndPotentialOrAbsentUsed(nextAttributeIt, nextPotentialOrAbsentValueIntersectionIt);
      (*potentialOrAbsentValueIntersectionIt)[(*valueIt)->getPresentIntersectionId()] += newNoiseInHyperplane;
      newNoise += newNoiseInHyperplane;
    }
  return newNoise;
}

void Trie::presentFixPotentialOrAbsentValues(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator end = currentAttribute.absentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.potentialBegin(); valueIt != end; ++valueIt)
    {
      const unsigned int newNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setPresentAfterPotentialOrAbsentUsed(presentAttributeIt, nextAttributeIt, (*valueIt)->getIntersectionsBeginWithPresentValues());
      (*valueIt)->addPresentNoise(newNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] += newNoiseInHyperplane;
	}
    }
}

void Trie::presentFixPotentialOrAbsentValuesBeforeSymmetricAttributes(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator end = currentAttribute.absentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.potentialBegin(); valueIt != end; ++valueIt)
    {
      // Since this is before the symmetric attributes, hyperplanes necessarily are tries
      const unsigned int newNoiseInHyperplane = static_cast<Trie*>(hyperplanes[(*valueIt)->getDataId()])->setSymmetricPresentAfterPotentialOrAbsentUsed(presentAttributeIt, nextAttributeIt, (*valueIt)->getIntersectionsBeginWithPresentValues());
      (*valueIt)->addPresentNoise(newNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] += newNoiseInHyperplane;
	}
    }
}

void Trie::presentFixPotentialOrAbsentValuesAfterPresentValueMet(Attribute& currentAttribute, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator end = currentAttribute.absentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.potentialBegin(); valueIt != end; ++valueIt)
    {
      const unsigned int newNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setPresentAfterPresentValueMetAndPotentialOrAbsentUsed(nextAttributeIt, (*valueIt)->getIntersectionsBeginWithPresentValues());
      (*valueIt)->addPresentNoise(newNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] += newNoiseInHyperplane;
	}
    }
}

const unsigned int Trie::setAbsent(const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Attribute*>::iterator nextAttributeIt = attributeIt + 1;
  vector<vector<vector<unsigned int>>::iterator> nextIntersectionIts(incrementIterators(intersectionIts));
  if (attributeIt == absentAttributeIt)
    {
      unsigned int oldNoise = 0;
      for (const unsigned int absentValueDataId : absentValueDataIds)
	{
	  oldNoise += hyperplanes[absentValueDataId]->setAbsentAfterAbsentValuesMet(nextAttributeIt, nextIntersectionIts);
	}
      return oldNoise;
    }
  absentFixAbsentValues(**attributeIt, absentAttributeIt, absentValueDataIds, nextAttributeIt, intersectionIts);
  return absentFixPresentOrPotentialValues(**attributeIt, absentAttributeIt, absentValueDataIds, nextAttributeIt, intersectionIts, nextIntersectionIts);
}

const unsigned int Trie::setSymmetricAbsent(const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Attribute*>::iterator nextAttributeIt = attributeIt + 1;
  vector<vector<vector<unsigned int>>::iterator> nextIntersectionIts(incrementIterators(intersectionIts));
  if (attributeIt == absentAttributeIt)
    {
      // *this necessarily relates to the first symmetric attribute
      const unsigned int absentValueDataId = (*absentAttributeIt)->getChosenAbsentValueDataId();
      const vector<unsigned int> absentValueDataIds {absentValueDataId};
      absentFixAbsentValuesInFirstSymmetricAttribute(**attributeIt, absentValueDataIds, nextAttributeIt, intersectionIts);
      return hyperplanes[absentValueDataId]->setSymmetricAbsentAfterAbsentValueMet(nextAttributeIt, nextIntersectionIts) + absentFixPresentOrPotentialValues(**attributeIt, nextAttributeIt, absentValueDataIds, nextAttributeIt, intersectionIts, nextIntersectionIts);
    }
  absentFixAbsentValuesBeforeSymmetricAttributes(**attributeIt, absentAttributeIt, nextAttributeIt, intersectionIts);
  return absentFixPresentOrPotentialValuesBeforeSymmetricAttributes(**attributeIt, absentAttributeIt, nextAttributeIt, intersectionIts, nextIntersectionIts);
}

const unsigned int Trie::setAbsentAfterAbsentValuesMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Attribute*>::iterator nextAttributeIt = attributeIt + 1;
  vector<vector<vector<unsigned int>>::iterator> nextIntersectionIts(incrementIterators(intersectionIts));
  absentFixAbsentValuesAfterAbsentValuesMet((*attributeIt)->absentBegin(), (*attributeIt)->absentEnd(), nextAttributeIt, intersectionIts);
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMet(**attributeIt, nextAttributeIt, intersectionIts, nextIntersectionIts);
}

const unsigned int Trie::setSymmetricAbsentAfterAbsentValueMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  // *this necessarily relates to the second symmetric attribute
  const vector<Attribute*>::iterator nextAttributeIt = attributeIt + 1;
  vector<vector<vector<unsigned int>>::iterator> nextIntersectionIts(incrementIterators(intersectionIts));
  // The first absent value actually is the value set absent and there is no noise to be found at the intersection of a vertex (seen as an outgoing vertex) and itself (seen as an ingoing vertex)
  absentFixAbsentValuesAfterAbsentValuesMet((*attributeIt)->absentBegin() + 1, (*attributeIt)->absentEnd(), nextAttributeIt, intersectionIts);
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMet(**attributeIt, nextAttributeIt, intersectionIts, nextIntersectionIts);
}

const unsigned int Trie::setAbsentAfterAbsentUsed(const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const
{
  const vector<Attribute*>::iterator nextAttributeIt = attributeIt + 1;
  const vector<vector<unsigned int>>::iterator nextAbsentValueIntersectionIt = absentValueIntersectionIt + 1;
  if (attributeIt == absentAttributeIt)
    {
      unsigned int oldNoise = 0;
      for (const unsigned int absentValueDataId : absentValueDataIds)
	{
	  oldNoise += hyperplanes[absentValueDataId]->setAbsentAfterAbsentValuesMetAndAbsentUsed(nextAttributeIt, nextAbsentValueIntersectionIt);
	}
      return oldNoise;
    }
  return absentFixPresentOrPotentialValuesAfterAbsentUsed(**attributeIt, absentAttributeIt, absentValueDataIds, nextAttributeIt, absentValueIntersectionIt, nextAbsentValueIntersectionIt);
}

const unsigned int Trie::setSymmetricAbsentAfterAbsentUsed(const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const
{
  const vector<Attribute*>::iterator nextAttributeIt = attributeIt + 1;
  const vector<vector<unsigned int>>::iterator nextAbsentValueIntersectionIt = absentValueIntersectionIt + 1;
  if (attributeIt == absentAttributeIt)
    {
      // *this necessarily relates to the first symmetric attribute
      const unsigned int absentValueDataId = (*absentAttributeIt)->getChosenAbsentValueDataId();
      const vector<unsigned int> absentValueDataIds {absentValueDataId};
      return hyperplanes[absentValueDataId]->setSymmetricAbsentAfterAbsentValueMetAndAbsentUsed(nextAttributeIt, nextAbsentValueIntersectionIt) + absentFixPresentOrPotentialValuesAfterAbsentUsed(**attributeIt, nextAttributeIt, absentValueDataIds, nextAttributeIt, absentValueIntersectionIt, nextAbsentValueIntersectionIt);
    }
  return absentFixPresentOrPotentialValuesBeforeSymmetricAttributesAfterAbsentUsed(**attributeIt, absentAttributeIt, nextAttributeIt, absentValueIntersectionIt, nextAbsentValueIntersectionIt);
}

const unsigned int Trie::setAbsentAfterAbsentValuesMetAndAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const
{
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(**attributeIt, attributeIt + 1, absentValueIntersectionIt, absentValueIntersectionIt + 1);
}

const unsigned int Trie::setSymmetricAbsentAfterAbsentValueMetAndAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const
{
  // *this necessarily relates to the second symmetric attribute
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(**attributeIt, attributeIt + 1, absentValueIntersectionIt, absentValueIntersectionIt + 1);
}

const unsigned int Trie::absentFixPresentOrPotentialValues(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const
{
  unsigned int oldNoise = 0;
  vector<Value*>::iterator end = currentAttribute.potentialEnd();
  vector<Value*>::iterator valueIt = currentAttribute.presentBegin();
  for (; valueIt != end; ++valueIt)
    {
      nextIntersectionIts.push_back((*valueIt)->getIntersectionsBeginWithPresentAndPotentialValues());
      const unsigned int oldNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setAbsent(absentAttributeIt, absentValueDataIds, nextAttributeIt, nextIntersectionIts);
      nextIntersectionIts.pop_back();
      (*valueIt)->subtractPotentialNoise(oldNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] -= oldNoiseInHyperplane;
	}
      oldNoise += oldNoiseInHyperplane;
    }
  end = currentAttribute.irrelevantEnd();
  for (; valueIt != end; ++valueIt)
    {
      oldNoise += hyperplanes[(*valueIt)->getDataId()]->setAbsent(absentAttributeIt, absentValueDataIds, nextAttributeIt, nextIntersectionIts);
    }
  return oldNoise;
}

const unsigned int Trie::absentFixPresentOrPotentialValuesBeforeSymmetricAttributes(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const
{
  unsigned int oldNoise = 0;
  vector<Value*>::iterator end = currentAttribute.potentialEnd();
  vector<Value*>::iterator valueIt = currentAttribute.presentBegin();
  for (; valueIt != end; ++valueIt)
    {
      nextIntersectionIts.push_back((*valueIt)->getIntersectionsBeginWithPresentAndPotentialValues());
      // Since this is before the symmetric attributes, hyperplanes necessarily are tries
      const unsigned int oldNoiseInHyperplane = static_cast<Trie*>(hyperplanes[(*valueIt)->getDataId()])->setSymmetricAbsent(absentAttributeIt, nextAttributeIt, nextIntersectionIts);
      nextIntersectionIts.pop_back();
      (*valueIt)->subtractPotentialNoise(oldNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] -= oldNoiseInHyperplane;
	}
      oldNoise += oldNoiseInHyperplane;
    }
  end = currentAttribute.irrelevantEnd();
  for (; valueIt != end; ++valueIt)
    {
      oldNoise += static_cast<Trie*>(hyperplanes[(*valueIt)->getDataId()])->setSymmetricAbsent(absentAttributeIt, nextAttributeIt, nextIntersectionIts);
    }
  return oldNoise;
}

const unsigned int Trie::absentFixPresentOrPotentialValuesAfterAbsentValuesMet(Attribute& currentAttribute, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const
{
  unsigned int oldNoise = 0;
  vector<Value*>::iterator end = currentAttribute.potentialEnd();
  vector<Value*>::iterator valueIt = currentAttribute.presentBegin();
  for (; valueIt != end; ++valueIt)
    {
      nextIntersectionIts.push_back((*valueIt)->getIntersectionsBeginWithPresentAndPotentialValues());
      const unsigned int oldNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setAbsentAfterAbsentValuesMet(nextAttributeIt, nextIntersectionIts);
      nextIntersectionIts.pop_back();
      (*valueIt)->subtractPotentialNoise(oldNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] -= oldNoiseInHyperplane;
	}
      oldNoise += oldNoiseInHyperplane;
    }
  end = currentAttribute.irrelevantEnd();
  for (; valueIt != end; ++valueIt)
    {
      oldNoise += hyperplanes[(*valueIt)->getDataId()]->setAbsentAfterAbsentValuesMet(nextAttributeIt, nextIntersectionIts);
    }
  return oldNoise;
}

const unsigned int Trie::absentFixPresentOrPotentialValuesAfterAbsentUsed(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextAbsentValueIntersectionIt) const
{
  unsigned int oldNoise = 0;
  vector<Value*>::iterator end = currentAttribute.potentialEnd();
  vector<Value*>::iterator valueIt = currentAttribute.presentBegin();
  for (; valueIt != end; ++valueIt)
    {
      const unsigned int oldNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setAbsentAfterAbsentUsed(absentAttributeIt, absentValueDataIds, nextAttributeIt, nextAbsentValueIntersectionIt);
      (*absentValueIntersectionIt)[(*valueIt)->getPresentAndPotentialIntersectionId()] -= oldNoiseInHyperplane;
      oldNoise += oldNoiseInHyperplane;
    }
  end = currentAttribute.irrelevantEnd();
  for (; valueIt != end; ++valueIt)
    {
      oldNoise += hyperplanes[(*valueIt)->getDataId()]->setAbsentAfterAbsentUsed(absentAttributeIt, absentValueDataIds, nextAttributeIt, nextAbsentValueIntersectionIt);
    }
  return oldNoise;
}

const unsigned int Trie::absentFixPresentOrPotentialValuesBeforeSymmetricAttributesAfterAbsentUsed(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextAbsentValueIntersectionIt) const
{
  unsigned int oldNoise = 0;
  vector<Value*>::iterator end = currentAttribute.potentialEnd();
  vector<Value*>::iterator valueIt = currentAttribute.presentBegin();
  for (; valueIt != end; ++valueIt)
    {
      // Since this is before the symmetric attributes, hyperplanes necessarily are tries
      const unsigned int oldNoiseInHyperplane = static_cast<Trie*>(hyperplanes[(*valueIt)->getDataId()])->setSymmetricAbsentAfterAbsentUsed(absentAttributeIt, nextAttributeIt, nextAbsentValueIntersectionIt);
      (*absentValueIntersectionIt)[(*valueIt)->getPresentAndPotentialIntersectionId()] -= oldNoiseInHyperplane;
      oldNoise += oldNoiseInHyperplane;
    }
  end = currentAttribute.irrelevantEnd();
  for (; valueIt != end; ++valueIt)
    {
      oldNoise += static_cast<Trie*>(hyperplanes[(*valueIt)->getDataId()])->setSymmetricAbsentAfterAbsentUsed(absentAttributeIt, nextAttributeIt, nextAbsentValueIntersectionIt);
    }
  return oldNoise;
}

const unsigned int Trie::absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(Attribute& currentAttribute, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextAbsentValueIntersectionIt) const
{
  unsigned int oldNoise = 0;
  vector<Value*>::iterator end = currentAttribute.potentialEnd();
  vector<Value*>::iterator valueIt = currentAttribute.presentBegin();
  for (; valueIt != end; ++valueIt)
    {
      const unsigned int oldNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setAbsentAfterAbsentValuesMetAndAbsentUsed(nextAttributeIt, nextAbsentValueIntersectionIt);
      (*absentValueIntersectionIt)[(*valueIt)->getPresentAndPotentialIntersectionId()] -= oldNoiseInHyperplane;
      oldNoise += oldNoiseInHyperplane;
    }
  end = currentAttribute.irrelevantEnd();
  for (; valueIt != end; ++valueIt)
    {
      oldNoise += hyperplanes[(*valueIt)->getDataId()]->setAbsentAfterAbsentValuesMetAndAbsentUsed(nextAttributeIt, nextAbsentValueIntersectionIt);
    }
  return oldNoise;
}

void Trie::absentFixAbsentValues(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator end = currentAttribute.absentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.absentBegin(); valueIt != end; ++valueIt)
    {
      const unsigned int oldNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setAbsentAfterAbsentUsed(absentAttributeIt, absentValueDataIds, nextAttributeIt, (*valueIt)->getIntersectionsBeginWithPresentAndPotentialValues());
      (*valueIt)->subtractPotentialNoise(oldNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] -= oldNoiseInHyperplane;
	}
    }
}

void Trie::absentFixAbsentValuesBeforeSymmetricAttributes(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator end = currentAttribute.absentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.absentBegin(); valueIt != end; ++valueIt)
    {
      // Since this is before the symmetric attributes, hyperplanes necessarily are tries
      const unsigned int oldNoiseInHyperplane = static_cast<Trie*>(hyperplanes[(*valueIt)->getDataId()])->setSymmetricAbsentAfterAbsentUsed(absentAttributeIt, nextAttributeIt, (*valueIt)->getIntersectionsBeginWithPresentAndPotentialValues());
      (*valueIt)->subtractPotentialNoise(oldNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] -= oldNoiseInHyperplane;
	}
    }
}

void Trie::absentFixAbsentValuesInFirstSymmetricAttribute(Attribute& currentAttribute, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator end = currentAttribute.absentEnd();
  // The first absent value actually is the value set absent and there is no noise to be found at the intersection of a vertex (seen as an outgoing vertex) and itself (seen as an ingoing vertex)
  for (vector<Value*>::iterator valueIt = currentAttribute.absentBegin(); ++valueIt != end; )
    {
      const unsigned int oldNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setAbsentAfterAbsentUsed(nextAttributeIt, absentValueDataIds, nextAttributeIt, (*valueIt)->getIntersectionsBeginWithPresentAndPotentialValues());
      (*valueIt)->subtractPotentialNoise(oldNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] -= oldNoiseInHyperplane;
	}
    }
}

void Trie::absentFixAbsentValuesAfterAbsentValuesMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int oldNoiseInHyperplane = hyperplanes[(*valueIt)->getDataId()]->setAbsentAfterAbsentValuesMetAndAbsentUsed(nextAttributeIt, (*valueIt)->getIntersectionsBeginWithPresentAndPotentialValues());
      (*valueIt)->subtractPotentialNoise(oldNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] -= oldNoiseInHyperplane;
	}
    }
}

const double Trie::noiseSum(const vector<vector<unsigned int>>& nSet) const
{
  double noise = 0;
  const vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      noise += hyperplanes[id]->noiseSum(nextDimensionIt);
    }
  return noise;
}

const unsigned int Trie::noiseSum(const vector<vector<unsigned int>>::const_iterator dimensionIt) const
{
  unsigned int noise = 0;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      noise += hyperplanes[id]->noiseSum(nextDimensionIt);
    }
  return noise;
}

#ifdef ASSERT
const unsigned int Trie::noiseSumOnPresent(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const
{
  const vector<Attribute*>::const_iterator nextAttributeIt = attributeIt + 1;
  if (attributeIt == valueAttributeIt)
    {
      return hyperplanes[value.getDataId()]->noiseSumOnPresent(valueAttributeIt, value, nextAttributeIt);
    }
  unsigned int noise = 0;
  const vector<Value*>::const_iterator end = (*attributeIt)->presentEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      noise += hyperplanes[(*valueIt)->getDataId()]->noiseSumOnPresent(valueAttributeIt, value, nextAttributeIt);
    }
  return noise;
}

const unsigned int Trie::noiseSumOnPresentAndPotential(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const
{
  const vector<Attribute*>::const_iterator nextAttributeIt = attributeIt + 1;
  if (attributeIt == valueAttributeIt)
    {
      return hyperplanes[value.getDataId()]->noiseSumOnPresentAndPotential(valueAttributeIt, value, nextAttributeIt);
    }
  unsigned int noise = 0;
  const vector<Value*>::const_iterator end = (*attributeIt)->irrelevantEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      noise += hyperplanes[(*valueIt)->getDataId()]->noiseSumOnPresentAndPotential(valueAttributeIt, value, nextAttributeIt);
    }
  return noise;
}

const unsigned int Trie::noiseSumOnPresent(const vector<Attribute*>::const_iterator firstValueAttributeIt, const Value& firstValue, const vector<Attribute*>::const_iterator secondValueAttributeIt, const Value& secondValue, const vector<Attribute*>::const_iterator attributeIt) const
{
  const vector<Attribute*>::const_iterator nextAttributeIt = attributeIt + 1;
  if (attributeIt == firstValueAttributeIt)
    {
      return hyperplanes[firstValue.getDataId()]->noiseSumOnPresent(secondValueAttributeIt, secondValue, nextAttributeIt);
    }
  unsigned int noise = 0;
  const vector<Value*>::const_iterator end = (*attributeIt)->presentEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      noise += static_cast<Trie*>(hyperplanes[(*valueIt)->getDataId()])->noiseSumOnPresent(firstValueAttributeIt, firstValue, secondValueAttributeIt, secondValue, nextAttributeIt);
    }
  return noise;
}

const unsigned int Trie::noiseSumOnPresentAndPotential(const vector<Attribute*>::const_iterator firstValueAttributeIt, const Value& firstValue, const vector<Attribute*>::const_iterator secondValueAttributeIt, const Value& secondValue, const vector<Attribute*>::const_iterator attributeIt) const
{
  const vector<Attribute*>::const_iterator nextAttributeIt = attributeIt + 1;
  if (attributeIt == firstValueAttributeIt)
    {
      return hyperplanes[firstValue.getDataId()]->noiseSumOnPresentAndPotential(secondValueAttributeIt, secondValue, nextAttributeIt);
    }
  unsigned int noise = 0;
  const vector<Value*>::const_iterator end = (*attributeIt)->irrelevantEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      noise += static_cast<Trie*>(hyperplanes[(*valueIt)->getDataId()])->noiseSumOnPresentAndPotential(firstValueAttributeIt, firstValue, secondValueAttributeIt, secondValue, nextAttributeIt);
    }
  return noise;
}
#endif

vector<vector<vector<unsigned int>>::iterator> Trie::incrementIterators(const vector<vector<vector<unsigned int>>::iterator>& iterators)
{
  vector<vector<vector<unsigned int>>::iterator> nextIterators;
  nextIterators.reserve(iterators.size());
  for (const vector<vector<unsigned int>>::iterator it : iterators)
    {
      nextIterators.push_back(it + 1);
    }
  return nextIterators;
}
