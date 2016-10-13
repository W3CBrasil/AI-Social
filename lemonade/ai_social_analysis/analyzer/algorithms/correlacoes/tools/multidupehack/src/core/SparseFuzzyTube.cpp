// Copyright 2010,2011,2012,2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "SparseFuzzyTube.h"

float SparseFuzzyTube::densityThreshold;

SparseFuzzyTube::SparseFuzzyTube() : tube()
{
}

SparseFuzzyTube* SparseFuzzyTube::clone() const
{
  return new SparseFuzzyTube(*this);
}

void SparseFuzzyTube::print(vector<unsigned int>& prefix, ostream& out) const
{
  for (const pair<unsigned int, unsigned int>& hyperplane : tube)
    {
      for (const unsigned int id : prefix)
	{
	  out << id << ' ';
	}
      out << hyperplane.first << ' ' << 1 - static_cast<double>(hyperplane.second) / Attribute::noisePerUnit << endl;
    }
}

vector<unsigned int> SparseFuzzyTube::getDenseRepresentation(const unsigned int nbOfHyperplanes) const
{
  vector<unsigned int> denseRepresentation(nbOfHyperplanes, Attribute::noisePerUnit);
  for (const pair<unsigned int, unsigned int>& entry : tube)
    {
      denseRepresentation[entry.first] = entry.second;
    }
  return denseRepresentation;
}

const bool SparseFuzzyTube::setSelfLoopsInSymmetricAttributes(const unsigned int hyperplaneId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute, const unsigned int dimensionId)
{
  // Necessarily symmetric
  if (densityThreshold == 0)
    {
      // *this must be turned into a DenseFuzzyTube
      return true;
    }
  tube.push_back(pair<unsigned int, unsigned int>(hyperplaneId, 0));
  return false;
}

const bool SparseFuzzyTube::setTuple(const vector<unsigned int>& tuple, const unsigned int membership, vector<unsigned int>::const_iterator attributeIdIt, vector<vector<unsigned int>>::const_iterator oldIds2NewIdsIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts)
{
  const unsigned int element = oldIds2NewIdsIt->at(tuple[*attributeIdIt]);
  (*attributeIt)->subtractPotentialNoise(element, membership);
  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
    {
      (*intersectionIt)[element] -= membership;
    }
  tube.push_back(pair<unsigned int, unsigned int>(element, Attribute::noisePerUnit - membership));
  return tube.size() >= (*attributeIt)->sizeOfPresentAndPotential() * densityThreshold;
}

void SparseFuzzyTube::sortTubes()
{
  tube.resize(tube.size());
  sort(tube.begin(), tube.end());
}

void SparseFuzzyTube::setDensityThreshold(const float densityThresholdParam)
{
  densityThreshold = densityThresholdParam / 2;
}

const unsigned int SparseFuzzyTube::noiseOnValue(const unsigned int valueDataId) const
{
  const vector<pair<unsigned int, unsigned int>>::const_iterator tubeEnd = tube.end();
  const vector<pair<unsigned int, unsigned int>>::const_iterator noisyTupleIt = lower_bound(tube.begin(), tubeEnd, pair<unsigned int, unsigned int>(valueDataId, 0));
  if (noisyTupleIt != tubeEnd && noisyTupleIt->first == valueDataId)
    {
      return noisyTupleIt->second;
    }
  return Attribute::noisePerUnit;
}

const unsigned int SparseFuzzyTube::noiseOnValues(const vector<unsigned int>& valueDataIds) const
{
  unsigned int oldNoise = 0;
  const vector<pair<unsigned int, unsigned int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, unsigned int>>::const_iterator tubeBegin = tube.begin();
  for (const unsigned int valueDataId : valueDataIds)
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, unsigned int>(valueDataId, 0));
      if (tubeBegin != tubeEnd && tubeBegin->first == valueDataId)
	{
	  oldNoise += tubeBegin->second;
	}
      else
	{
	  oldNoise += Attribute::noisePerUnit;
	}
    }
  return oldNoise;
}

const unsigned int SparseFuzzyTube::setPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  // *this necessarily relates to the present attribute
  return noiseOnValue((*attributeIt)->getChosenPresentValue().getDataId());
}

const unsigned int SparseFuzzyTube::setPresentAfterPotentialOrAbsentUsed(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const
{
  // *this necessarily relates to the present attribute
  const Value& presentValue = (*attributeIt)->getChosenPresentValue();
  const unsigned int noise = noiseOnValue(presentValue.getDataId());
  (*potentialOrAbsentValueIntersectionIt)[presentValue.getPresentIntersectionId()] += noise;
  return noise;
}

const unsigned int SparseFuzzyTube::presentFixPresentValuesAfterPresentValueMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end) const
{
  unsigned int newNoise = 0;
  const vector<pair<unsigned int, unsigned int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, unsigned int>>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, unsigned int>(valueDataId, 0));
      if (tubeBegin != tubeEnd && tubeBegin->first == valueDataId)
	{
	  (*valueIt)->addPresentNoise(tubeBegin->second);
	  newNoise += tubeBegin->second;
	}
      else
	{
	  (*valueIt)->addPresentNoise(Attribute::noisePerUnit);
	  newNoise += Attribute::noisePerUnit;	  
	}
    }
  return newNoise;
}

const unsigned int SparseFuzzyTube::presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, vector<unsigned int>& potentialOrAbsentValueIntersection) const
{
  unsigned int newNoise = 0;
  const vector<pair<unsigned int, unsigned int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, unsigned int>>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::const_iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, unsigned int>(valueDataId, 0));
      if (tubeBegin != tubeEnd && tubeBegin->first == valueDataId)
	{
	  potentialOrAbsentValueIntersection[(*valueIt)->getPresentIntersectionId()] += tubeBegin->second;
	  newNoise += tubeBegin->second;
	}
      else
	{
	  potentialOrAbsentValueIntersection[(*valueIt)->getPresentIntersectionId()] += Attribute::noisePerUnit;
	  newNoise += Attribute::noisePerUnit;
	}
    }
  return newNoise;
}

void SparseFuzzyTube::presentFixPotentialOrAbsentValuesAfterPresentValueMet(Attribute& currentAttribute, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator absentBegin = currentAttribute.absentBegin();
  presentFixPotentialOrAbsentValuesAfterPresentValueMet(currentAttribute.potentialBegin(), absentBegin, intersectionIts);
  presentFixPotentialOrAbsentValuesAfterPresentValueMet(absentBegin, currentAttribute.absentEnd(), intersectionIts);
}

void SparseFuzzyTube::presentFixPotentialOrAbsentValuesAfterPresentValueMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<pair<unsigned int, unsigned int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, unsigned int>>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueId = (*valueIt)->getPresentIntersectionId();
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, unsigned int>(valueDataId, 0));
      if (tubeBegin != tubeEnd && tubeBegin->first == valueDataId)
	{
	  (*valueIt)->addPresentNoise(tubeBegin->second);
	  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	    {
	      (*intersectionIt)[valueId] += tubeBegin->second;
	    }
	}
      else
	{
	  (*valueIt)->addPresentNoise(Attribute::noisePerUnit);
	  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	    {
	      (*intersectionIt)[valueId] += Attribute::noisePerUnit;
	    }
	}
    }  
}

const unsigned int SparseFuzzyTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMet(Attribute& currentAttribute, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator potentialBegin = currentAttribute.potentialBegin();
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMet(currentAttribute.presentBegin(), potentialBegin, intersectionIts) + absentFixPresentOrPotentialValuesAfterAbsentValuesMet(potentialBegin, currentAttribute.potentialEnd(), intersectionIts) + noiseOnIrrelevant(currentAttribute);
}

const unsigned int SparseFuzzyTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  unsigned int oldNoise = 0;
  const vector<pair<unsigned int, unsigned int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, unsigned int>>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, unsigned int>(valueDataId, 0));
      if (tubeBegin != tubeEnd && tubeBegin->first == valueDataId)
      	{
      	  (*valueIt)->subtractPotentialNoise(tubeBegin->second);
      	  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
      	    {
      	      (*intersectionIt)[valueId] -= tubeBegin->second;
      	    }
      	  oldNoise += tubeBegin->second;
      	}
      else
      	{
      	  (*valueIt)->subtractPotentialNoise(Attribute::noisePerUnit);
      	  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
      	    {
      	      (*intersectionIt)[valueId] -= Attribute::noisePerUnit;
      	    }
      	  oldNoise += Attribute::noisePerUnit;
      	}
    }
  return oldNoise;
}

const unsigned int SparseFuzzyTube::noiseOnIrrelevant(const Attribute& currentAttribute) const
{
  unsigned int oldNoise = 0;
  const vector<pair<unsigned int, unsigned int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, unsigned int>>::const_iterator tubeBegin = tube.begin();
  vector<Value*>::const_iterator end = currentAttribute.irrelevantEnd();
  for (vector<Value*>::const_iterator valueIt = currentAttribute.irrelevantBegin(); valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId(); 
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, unsigned int>(valueDataId, 0));
      if (tubeBegin != tubeEnd && tubeBegin->first == valueDataId)
	{
	  oldNoise += tubeBegin->second;
	}
      else
	{
	  oldNoise += Attribute::noisePerUnit;
	}
    }
  return oldNoise;
}

const unsigned int SparseFuzzyTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(Attribute& currentAttribute, vector<unsigned int>& absentValueIntersection) const
{
  const vector<Value*>::const_iterator potentialBegin = currentAttribute.potentialBegin();
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(currentAttribute.presentBegin(), potentialBegin, absentValueIntersection) + absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(potentialBegin, currentAttribute.potentialEnd(), absentValueIntersection) + noiseOnIrrelevant(currentAttribute);
}

const unsigned int SparseFuzzyTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, vector<unsigned int>& absentValueIntersection) const
{
  unsigned int oldNoise = 0;
  const vector<pair<unsigned int, unsigned int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, unsigned int>>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::const_iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueDataId = (*valueIt)->getDataId();
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, unsigned int>(valueDataId, 0));
      if (tubeBegin != tubeEnd && tubeBegin->first == valueDataId)
	{
	  absentValueIntersection[(*valueIt)->getPresentAndPotentialIntersectionId()] -= tubeBegin->second;
	  oldNoise += tubeBegin->second;
	}
      else
	{
	  absentValueIntersection[(*valueIt)->getPresentAndPotentialIntersectionId()] -= Attribute::noisePerUnit;
	  oldNoise += Attribute::noisePerUnit;
	}
    }
  return oldNoise;
}

void SparseFuzzyTube::absentFixAbsentValuesAfterAbsentValuesMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<pair<unsigned int, unsigned int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, unsigned int>>::const_iterator tubeBegin = tube.begin();
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
      const unsigned int valueDataId = (*valueIt)->getDataId(); 
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, unsigned int>(valueDataId, 0));
      if (tubeBegin != tubeEnd && tubeBegin->first == valueDataId)
	{
	  (*valueIt)->subtractPotentialNoise(tubeBegin->second);
	  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	    {
	      (*intersectionIt)[valueId] -= tubeBegin->second;
	    }
	}
      else
	{
	  (*valueIt)->subtractPotentialNoise(Attribute::noisePerUnit);
	  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	    {
	      (*intersectionIt)[valueId] -= Attribute::noisePerUnit;
	    }
	}
    }
}

const unsigned int SparseFuzzyTube::noiseSum(const vector<vector<unsigned int>>::const_iterator dimensionIt) const
{
  unsigned int noise = 0;
  const vector<pair<unsigned int, unsigned int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, unsigned int>>::const_iterator tubeBegin = tube.begin();
  for (const unsigned int id : *dimensionIt)
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, unsigned int>(id, 0));
      if (tubeBegin != tubeEnd && tubeBegin->first == id)
	{
	  noise += tubeBegin->second;
	}
      else
	{
	  noise += Attribute::noisePerUnit;
	}
    }
  return noise;
}

#ifdef ASSERT
const unsigned int SparseFuzzyTube::noiseSumOnPresent(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const
{
  if (attributeIt == valueAttributeIt)
    {
      return noiseOnValue(value.getDataId());
    }
  unsigned int noise = 0;
  const vector<Value*>::const_iterator end = (*attributeIt)->presentEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      noise += noiseOnValue((*valueIt)->getDataId());
    }
  return noise;
}

const unsigned int SparseFuzzyTube::noiseSumOnPresentAndPotential(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const
{
  if (attributeIt == valueAttributeIt)
    {
      return noiseOnValue(value.getDataId());
    }
  unsigned int noise = 0;
  vector<Value*>::const_iterator end = (*attributeIt)->irrelevantEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      noise += noiseOnValue((*valueIt)->getDataId());
    }
  return noise;
}
#endif
