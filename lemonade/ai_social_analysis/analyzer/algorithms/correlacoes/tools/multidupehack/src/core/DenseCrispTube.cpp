// Copyright 2010,2011,2012,2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "DenseCrispTube.h"

DenseCrispTube::DenseCrispTube(const dynamic_bitset<>& tubeParam) : tube(tubeParam)
{
}

DenseCrispTube::DenseCrispTube(const unsigned int size, const unsigned int presentElement) : tube(size, true)
{
  tube.set(presentElement, false);
}

DenseCrispTube* DenseCrispTube::clone() const
{
  return new DenseCrispTube(*this);
}

void DenseCrispTube::print(vector<unsigned int>& prefix, ostream& out) const
{
  unsigned int hyperplaneId = 0;
  for (dynamic_bitset<>::size_type absentHyperplaneId = tube.find_first(); absentHyperplaneId != dynamic_bitset<>::npos; absentHyperplaneId = tube.find_next(absentHyperplaneId))
    {
      for (; hyperplaneId != absentHyperplaneId; ++hyperplaneId)
	{
	  for (const unsigned int id : prefix)
	    {
	      out << id << ' ';
	    }
	  out << hyperplaneId << " 1" << endl;	  
	}
      ++hyperplaneId;
    }
}

const bool DenseCrispTube::setTuple(const vector<unsigned int>& tuple, const unsigned int membership, vector<unsigned int>::const_iterator attributeIdIt, vector<vector<unsigned int>>::const_iterator oldIds2NewIdsIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts)
{
  const unsigned int element = oldIds2NewIdsIt->at(tuple[*attributeIdIt]);
  (*attributeIt)->decrementPotentialNoise(element);
  for (vector<vector<unsigned int>>::iterator intersectionIt : intersectionIts)
    {
      --(*intersectionIt)[element];
    }
  tube.set(element, false);
  return false;
}

const unsigned int DenseCrispTube::noiseOnValues(const vector<unsigned int>& valueDataIds) const
{
  unsigned int oldNoise = 0;
  for (const unsigned int valueDataId : valueDataIds)
    {
      if (tube[valueDataId])
	{
	  ++oldNoise;
	}
    }
  return oldNoise;
}

const unsigned int DenseCrispTube::setPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  // *this necessarily relates to the present attribute
  if (tube[(*attributeIt)->getChosenPresentValue().getDataId()])
    {
      return 1;
    }
  return 0;
}

const unsigned int DenseCrispTube::setPresentAfterPotentialOrAbsentUsed(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const
{
  // *this necessarily relates to the present attribute
  const Value& presentValue = (*attributeIt)->getChosenPresentValue();
  if (tube[presentValue.getDataId()])
    {
      ++(*potentialOrAbsentValueIntersectionIt)[presentValue.getPresentIntersectionId()];
      return 1;
    }
  return 0;
}

const unsigned int DenseCrispTube::presentFixPresentValuesAfterPresentValueMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end) const
{
  unsigned int newNoise = 0;
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      if (tube[(*valueIt)->getDataId()])
	{
	  (*valueIt)->incrementPresentNoise();
	  ++newNoise;
	}
    }
  return newNoise;
}

const unsigned int DenseCrispTube::presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, vector<unsigned int>& potentialOrAbsentValueIntersection) const
{
  unsigned int newNoise = 0;
  for (vector<Value*>::const_iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      if (tube[(*valueIt)->getDataId()])
	{
	  ++potentialOrAbsentValueIntersection[(*valueIt)->getPresentIntersectionId()];
	  ++newNoise;
	}
    }
  return newNoise;
}

void DenseCrispTube::presentFixPotentialOrAbsentValuesAfterPresentValueMet(Attribute& currentAttribute, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  const vector<Value*>::iterator end = currentAttribute.absentEnd();
  for (vector<Value*>::iterator valueIt = currentAttribute.potentialBegin(); valueIt != end; ++valueIt)
    {
      if (tube[(*valueIt)->getDataId()])
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

const unsigned int DenseCrispTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMet(Attribute& currentAttribute, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  unsigned int oldNoise = 0;
  vector<Value*>::iterator end = currentAttribute.potentialEnd();
  vector<Value*>::iterator valueIt = currentAttribute.presentBegin();
  for (; valueIt != end; ++valueIt)
    {
      if (tube[(*valueIt)->getDataId()])
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
  end = currentAttribute.irrelevantEnd();
  for (; valueIt != end; ++valueIt)
    {
      if (tube[(*valueIt)->getDataId()])
	{
	  ++oldNoise;
	}
    }
  return oldNoise;
}

const unsigned int DenseCrispTube::absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(Attribute& currentAttribute, vector<unsigned int>& absentValueIntersection) const
{
  unsigned int oldNoise = 0;
  vector<Value*>::iterator end = currentAttribute.potentialEnd();
  vector<Value*>::iterator valueIt = currentAttribute.presentBegin();
  for (; valueIt != end; ++valueIt)
    {
      if (tube[(*valueIt)->getDataId()])
	{
	  --absentValueIntersection[(*valueIt)->getPresentAndPotentialIntersectionId()];
	  ++oldNoise;
	}
    }
  end = currentAttribute.irrelevantEnd();
  for (; valueIt != end; ++valueIt)
    {
      if (tube[(*valueIt)->getDataId()])
	{
	  ++oldNoise;
	}
    }
  return oldNoise;
}

void DenseCrispTube::absentFixAbsentValuesAfterAbsentValuesMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  for (vector<Value*>::iterator valueIt = begin; valueIt != end; ++valueIt)
    {
      if (tube[(*valueIt)->getDataId()])
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

const unsigned int DenseCrispTube::noiseSum(const vector<vector<unsigned int>>::const_iterator dimensionIt) const
{
  unsigned int noise = 0;
  for (const unsigned int id : *dimensionIt)
    {
      if (tube[id])
	{
	  ++noise;
	}
    }  
  return noise;
}

#ifdef ASSERT
const unsigned int DenseCrispTube::noiseSumOnPresent(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const
{
  if (attributeIt == valueAttributeIt)
    {
      if (tube[value.getDataId()])
	{
	  return 1;
	}
      return 0;
    }
  unsigned int noise = 0;
  const vector<Value*>::const_iterator end = (*attributeIt)->presentEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      if (tube[(*valueIt)->getDataId()])
	{
	  ++noise;
	}
    }
  return noise;
}

const unsigned int DenseCrispTube::noiseSumOnPresentAndPotential(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const
{
  if (attributeIt == valueAttributeIt)
    {
      if (tube[value.getDataId()])
	{
	  return 1;
	}
      return 0;
    }
  unsigned int noise = 0;
  const vector<Value*>::const_iterator end = (*attributeIt)->irrelevantEnd();
  for (vector<Value*>::const_iterator valueIt = (*attributeIt)->presentBegin(); valueIt != end; ++valueIt)
    {
      if (tube[(*valueIt)->getDataId()])
	{
	  ++noise;
	}
    }
  return noise;
}
#endif
