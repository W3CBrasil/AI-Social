// Copyright 2010,2011,2012,2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "DenseFuzzyTube.h"

DenseFuzzyTube::DenseFuzzyTube(const vector<unsigned int> tubeParam) : tube(tubeParam)
{
}

DenseFuzzyTube::DenseFuzzyTube(const unsigned int size, const unsigned int presentElement) : tube(size, Attribute::noisePerUnit)
{
  tube[presentElement] = 0;
}

DenseFuzzyTube* DenseFuzzyTube::clone() const
{
  return new DenseFuzzyTube(*this);
}

void DenseFuzzyTube::print(vector<unsigned int>& prefix, ostream& out) const
{
  unsigned int hyperplaneId = 0;
  for (const unsigned int noise : tube)
    {
      if (noise != Attribute::noisePerUnit)
	{
	  for (const unsigned int id : prefix)
	    {
	      out << id << ' ';
	    }
	  out << hyperplaneId << ' ' << 1 - static_cast<double>(noise) / Attribute::noisePerUnit << endl;
	}
      ++hyperplaneId;
    }
}

const bool DenseFuzzyTube::setTuple(const vector<unsigned int>& tuple, const unsigned int membership, vector<unsigned int>::const_iterator attributeIdIt, vector<vector<unsigned int>>::const_iterator oldIds2NewIdsIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts)
{
  const unsigned int element = oldIds2NewIdsIt->at(tuple[*attributeIdIt]);
  (*attributeIt)->subtractPotentialNoise(element, membership);
  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
    {
      (*intersectionIt)[element] -= membership;
    }
  tube[element] = Attribute::noisePerUnit - membership;
  return false;
}

const unsigned int DenseFuzzyTube::noiseOnValues(const vector<unsigned int>& valueDataIds) const
{
  unsigned int oldNoise = 0;
  for (const unsigned int valueDataId : valueDataIds)
    {
      oldNoise += tube[valueDataId];
    }
  return oldNoise;
}

const unsigned int DenseFuzzyTube::setPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  // *this necessarily relates to the present attribute
  return tube[(*attributeIt)->getChosenPresentValue().getDataId()];
}

const unsigned int DenseFuzzyTube::setPresentAfterPotentialOrAbsentUsed(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const
{
  // *this necessarily relates to the present attribute
  const Value& presentValue = (*attributeIt)->getChosenPresentValue();
  const unsigned int noise = tube[presentValue.getDataId()];
  (*potentialOrAbsentValueIntersectionIt)[presentValue.getPresentIntersectionId()] += noise;
  return noise;
}

const unsigned int DenseFuzzyTube::presentFixPresentValuesAfterPresentValueMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end) const
{
  unsigned int newNoise = 0;
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int newNoiseInHyperplane = tube[(*valueIt)->getDataId()];
      (*valueIt)->addPresentNoise(newNoiseInHyperplane);
      newNoise += newNoiseInHyperplane;
    }
  return newNoise;
}

const unsigned int DenseFuzzyTube::presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, vector<unsigned int>& potentialOrAbsentValueIntersection) const
{
  unsigned int newNoise = 0;
  for (vector<Value*>::const_iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int newNoiseInHyperplane = tube[(*valueIt)->getDataId()];
      potentialOrAbsentValueIntersection[(*valueIt)->getPresentIntersectionId()] += newNoiseInHyperplane;
      newNoise += newNoiseInHyperplane;
    }
  return newNoise;
}

void DenseFuzzyTube::presentFixPotentialOrAbsentValuesAfterPresentValueMet(Attribute& currentAttribute, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator end = currentAttribute.absentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.potentialBegin(); valueIt != end; ++valueIt)
    {
      const unsigned int newNoiseInHyperplane = tube[(*valueIt)->getDataId()];
      (*valueIt)->addPresentNoise(newNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] += newNoiseInHyperplane;
	}
    }
}

const unsigned int DenseFuzzyTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMet(Attribute& currentAttribute, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  unsigned int oldNoise = 0;
  vector<Value*>::iterator end = currentAttribute.potentialEnd();
  vector<Value*>::iterator valueIt = currentAttribute.presentBegin();
  for (; valueIt != end; ++valueIt)
    {
      const unsigned int oldNoiseInHyperplane = tube[(*valueIt)->getDataId()];
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
      oldNoise += tube[(*valueIt)->getDataId()];
    }
  return oldNoise;
}

const unsigned int DenseFuzzyTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(Attribute& currentAttribute, vector<unsigned int>& absentValueIntersectionIt) const
{
  unsigned int oldNoise = 0;
  vector<Value*>::iterator end = currentAttribute.potentialEnd();
  vector<Value*>::iterator valueIt = currentAttribute.presentBegin();
  for (; valueIt != end; ++valueIt)
    {
      const unsigned int oldNoiseInHyperplane = tube[(*valueIt)->getDataId()];
      absentValueIntersectionIt[(*valueIt)->getPresentAndPotentialIntersectionId()] -= oldNoiseInHyperplane;
      oldNoise += oldNoiseInHyperplane;
    }
  end = currentAttribute.irrelevantEnd();
  for (; valueIt != end; ++valueIt)
    {
      oldNoise += tube[(*valueIt)->getDataId()];
    }
  return oldNoise;
}

void DenseFuzzyTube::absentFixAbsentValuesAfterAbsentValuesMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      const unsigned int oldNoiseInHyperplane = tube[(*valueIt)->getDataId()];
      (*valueIt)->subtractPotentialNoise(oldNoiseInHyperplane);
      const unsigned int valueId = (*valueIt)->getPresentAndPotentialIntersectionId();
      for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
	{
	  (*intersectionIt)[valueId] -= oldNoiseInHyperplane;
	}
    }
}

const unsigned int DenseFuzzyTube::noiseSum(const vector<vector<unsigned int>>::const_iterator dimensionIt) const
{
  unsigned int noise = 0;
  for (const unsigned int id : *dimensionIt)
    {
      noise += tube[id];
    }
  return noise;
}

#ifdef ASSERT
const unsigned int DenseFuzzyTube::noiseSumOnPresent(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const
{
  if (attributeIt == valueAttributeIt)
    {
      return tube[value.getDataId()];
    }
  unsigned int noise = 0;
  const vector<Value*>::const_iterator end = (*attributeIt)->presentEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      noise += tube[(*valueIt)->getDataId()];
    }
  return noise;
}

const unsigned int DenseFuzzyTube::noiseSumOnPresentAndPotential(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const
{
  if (attributeIt == valueAttributeIt)
    {
      return tube[value.getDataId()];
    }
  unsigned int noise = 0;
  vector<Value*>::const_iterator end = (*attributeIt)->irrelevantEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      noise += tube[(*valueIt)->getDataId()];
    }
  return noise;
}
#endif
