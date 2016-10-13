// Copyright 2010,2011,2012,2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "SparseCrispTube.h"

float SparseCrispTube::densityThreshold;

SparseCrispTube::SparseCrispTube() : tube()
{
}

SparseCrispTube* SparseCrispTube::clone() const
{
  return new SparseCrispTube(*this);
}

void SparseCrispTube::print(vector<unsigned int>& prefix, ostream& out) const
{
  for (const unsigned int hyperplane : tube)
    {
      for (const unsigned int id : prefix)
	{
	  out << id << ' ';
	}
      out << hyperplane << " 1" << endl;
    }
}

dynamic_bitset<> SparseCrispTube::getDenseRepresentation(const unsigned int nbOfHyperplanes) const
{
  dynamic_bitset<> denseRepresentation;
  denseRepresentation.resize(nbOfHyperplanes, true);
  for (const unsigned int presentHyperplaneId : tube)
    {
      denseRepresentation.set(presentHyperplaneId, false);
    }
  return denseRepresentation;
}

const bool SparseCrispTube::setSelfLoopsInSymmetricAttributes(const unsigned int hyperplaneId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute, const unsigned int dimensionId)
{
  // Necessarily symmetric
  if (densityThreshold == 0)
    {
      // *this must be turned into a DenseCrispTube
      return true;
    }
  tube.push_back(hyperplaneId);
  return false;
}

const bool SparseCrispTube::setTuple(const vector<unsigned int>& tuple, const unsigned int membership, vector<unsigned int>::const_iterator attributeIdIt, vector<vector<unsigned int>>::const_iterator oldIds2NewIdsIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts)
{
  const unsigned int element = oldIds2NewIdsIt->at(tuple[*attributeIdIt]);
  (*attributeIt)->decrementPotentialNoise(element);
  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
    {
      --(*intersectionIt)[element];
    }
  tube.push_back(element);
  return tube.size() >= (*attributeIt)->sizeOfPresentAndPotential() * densityThreshold;
}

void SparseCrispTube::sortTubes()
{
  tube.resize(tube.size());
  sort(tube.begin(), tube.end());
}

void SparseCrispTube::setDensityThreshold(const float densityThresholdParam)
{
  densityThreshold = densityThresholdParam / sizeof(unsigned int) / 8;
}

const unsigned int SparseCrispTube::noiseOnValues(const vector<unsigned int>& valueDataIds) const
{
  unsigned int oldNoise = 0;
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  for (const unsigned int valueDataId : valueDataIds)
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, valueDataId);
      if (tubeBegin == tubeEnd || *tubeBegin != valueDataId)
	{
	  ++oldNoise;
	}	  
    }
  return oldNoise;
}

const unsigned int SparseCrispTube::setPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  // *this necessarily relates to the present attribute
  if (binary_search(tube.begin(), tube.end(), (*attributeIt)->getChosenPresentValue().getDataId()))
    {
      return 0;
    }
  return 1;
}

const unsigned int SparseCrispTube::setPresentAfterPotentialOrAbsentUsed(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const
{
  // *this necessarily relates to the present attribute
  const Value& presentValue = (*attributeIt)->getChosenPresentValue();
  if (binary_search(tube.begin(), tube.end(), presentValue.getDataId()))
    {
      return 0;
    }
  ++(*potentialOrAbsentValueIntersectionIt)[presentValue.getPresentIntersectionId()];
  return 1;
}

const unsigned int SparseCrispTube::presentFixPresentValuesAfterPresentValueMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end) const
{
  unsigned int newNoise = 0;
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, valueDataId);
      if (tubeBegin == tubeEnd || *tubeBegin != valueDataId)
	{
	  (*valueIt)->incrementPresentNoise();
	  ++newNoise;
	}	  
    }
  return newNoise;
}

const unsigned int SparseCrispTube::presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, vector<unsigned int>& potentialOrAbsentValueIntersection) const
{
  unsigned int newNoise = 0;
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::const_iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, valueDataId);
      if (tubeBegin == tubeEnd || *tubeBegin != valueDataId)
	{
	  ++potentialOrAbsentValueIntersection[(*valueIt)->getPresentIntersectionId()];
	  ++newNoise;
	}
    }
  return newNoise;
}

void SparseCrispTube::presentFixPotentialOrAbsentValuesAfterPresentValueMet(Attribute& currentAttribute, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator absentBegin = currentAttribute.absentBegin();
  presentFixPotentialOrAbsentValuesAfterPresentValueMet(currentAttribute.potentialBegin(), absentBegin, intersectionIts);
  presentFixPotentialOrAbsentValuesAfterPresentValueMet(absentBegin, currentAttribute.absentEnd(), intersectionIts);
}

void SparseCrispTube::presentFixPotentialOrAbsentValuesAfterPresentValueMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, valueDataId);
      if (tubeBegin == tubeEnd || *tubeBegin != valueDataId)
	{
	  (*valueIt)->incrementPresentNoise();
	  const unsigned int valueId = (*valueIt)->getPresentIntersectionId();
	  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	    {
	      ++(*intersectionIt)[valueId];
	    }
	}
    }
}

const unsigned int SparseCrispTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMet(Attribute& currentAttribute, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator potentialBegin = currentAttribute.potentialBegin();
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMet(currentAttribute.presentBegin(), potentialBegin, intersectionIts) + absentFixPresentOrPotentialValuesAfterAbsentValuesMet(potentialBegin, currentAttribute.potentialEnd(), intersectionIts) + noiseOnIrrelevant(currentAttribute);
}

const unsigned int SparseCrispTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  unsigned int oldNoise = 0;
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, valueDataId);
      if (tubeBegin == tubeEnd || *tubeBegin != valueDataId)
	{
	  (*valueIt)->decrementPotentialNoise();
	  const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
	  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	    {
	      --(*intersectionIt)[valueId];
	    }
	  ++oldNoise;
	}
    }
  return oldNoise;
}

const unsigned int SparseCrispTube::noiseOnIrrelevant(const Attribute& currentAttribute) const
{
  unsigned int oldNoise = 0;
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  vector<Value*>::const_iterator end = currentAttribute.irrelevantEnd();
  for (vector<Value*>::const_iterator valueIt = currentAttribute.irrelevantBegin(); valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, valueDataId);
      if (tubeBegin == tubeEnd || *tubeBegin != valueDataId)
	{
	  ++oldNoise;
	}
    }
  return oldNoise;
}

const unsigned int SparseCrispTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(Attribute& currentAttribute, vector<unsigned int>& absentValueIntersection) const
{
  vector<Value*>::const_iterator potentialBegin = currentAttribute.potentialBegin();
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(currentAttribute.presentBegin(), potentialBegin, absentValueIntersection) + absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(potentialBegin, currentAttribute.potentialEnd(), absentValueIntersection) + noiseOnIrrelevant(currentAttribute);
}

const unsigned int SparseCrispTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, vector<unsigned int>& absentValueIntersection) const
{
  unsigned int oldNoise = 0;
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::const_iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, valueDataId);
      if (tubeBegin == tubeEnd || *tubeBegin != valueDataId)
	{
	  --absentValueIntersection[(*valueIt)->getPresentAndPotentialIntersectionId()];
	  ++oldNoise;
	}
    }  
  return oldNoise;
}

void SparseCrispTube::absentFixAbsentValuesAfterAbsentValuesMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, valueDataId);
      if (tubeBegin == tubeEnd || *tubeBegin != valueDataId)
	{
	  (*valueIt)->decrementPotentialNoise();
	  const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
	  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	    {
	      --(*intersectionIt)[valueId];
	    }
	}
    }
}

const unsigned int SparseCrispTube::noiseSum(const vector<vector<unsigned int>>::const_iterator dimensionIt) const
{
  unsigned int noise = 0;
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  for (const unsigned int id : *dimensionIt)
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, id);
      if (tubeBegin == tubeEnd || *tubeBegin != id)
	{
	  ++noise;
	}
    }
  return noise;
}

#ifdef ASSERT
const unsigned int SparseCrispTube::noiseSumOnPresent(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const
{
  if (attributeIt == valueAttributeIt)
    {
      if (binary_search(tube.begin(), tube.end(), value.getDataId()))
	{
	  return 0;
	}
      return 1;
    }
  unsigned int noise = 0;
  const vector<Value*>::const_iterator end = (*attributeIt)->presentEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      if (!binary_search(tube.begin(), tube.end(), (*valueIt)->getDataId()))
	{
	  ++noise;
	}
    }
  return noise;
}

const unsigned int SparseCrispTube::noiseSumOnPresentAndPotential(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const
{
  if (attributeIt == valueAttributeIt)
    {
      if (binary_search(tube.begin(), tube.end(), value.getDataId()))
	{
	  return 0;
	}
      return 1;
    }
  unsigned int noise = 0;
  vector<Value*>::const_iterator end = (*attributeIt)->irrelevantEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      if (!binary_search(tube.begin(), tube.end(), (*valueIt)->getDataId()))
	{
	  ++noise;
	}
    }
  return noise;
}
#endif
