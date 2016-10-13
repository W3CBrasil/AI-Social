// Copyright 2010,2011,2012,2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "Tube.h"

Tube::~Tube()
{
}

const bool Tube::setSelfLoopsInSymmetricAttributes(const unsigned int hyperplaneId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute, const unsigned int dimensionId)
{
  // Never called because self loops are inserted first, the tubes are initially sparse and the method is overridden in those classes
  return false;
}

const unsigned int Tube::setPresentAfterPresentValueMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  presentFixPotentialOrAbsentValuesAfterPresentValueMet(**attributeIt, intersectionIts);
  return presentFixPresentValuesAfterPresentValueMet((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd());
}

const unsigned int Tube::setSymmetricPresentAfterPresentValueMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  presentFixPotentialOrAbsentValuesAfterPresentValueMet(**attributeIt, intersectionIts);
  // The last present value actually is the value set present and there is no noise to be found at the intersection of a vertex (seen as an outgoing vertex) and itself (seen as an ingoing vertex)
  return presentFixPresentValuesAfterPresentValueMet((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd() - 1);
}

const unsigned int Tube::setPresentAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const
{
  return presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd(), *potentialOrAbsentValueIntersectionIt);
}

const unsigned int Tube::setSymmetricPresentAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const
{
  // *this necessarily relates to the second symmetric attribute
  // The last present value actually is the value set present and there is no noise to be found at the intersection of a vertex (seen as an outgoing vertex) and itself (seen as an ingoing vertex)
  return presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed((*attributeIt)->presentBegin(), (*attributeIt)->presentEnd() - 1, *potentialOrAbsentValueIntersectionIt);
}

const unsigned int Tube::setAbsent(const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  // *this necessarily relates to the absent attribute
  return noiseOnValues(absentValueDataIds);
}

const unsigned int Tube::setAbsentAfterAbsentUsed(const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const
{
  // *this necessarily relates to the absent attribute
  return noiseOnValues(absentValueDataIds);
}

const unsigned int Tube::setAbsentAfterAbsentValuesMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  absentFixAbsentValuesAfterAbsentValuesMet((*attributeIt)->absentBegin(), (*attributeIt)->absentEnd(), intersectionIts);
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMet(**attributeIt, intersectionIts);
}

const unsigned int Tube::setSymmetricAbsentAfterAbsentValueMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const
{
  // The first absent value actually is the value set absent and there is no noise to be found at the intersection of a vertex (seen as an outgoing vertex) and itself (seen as an ingoing vertex)
  absentFixAbsentValuesAfterAbsentValuesMet((*attributeIt)->absentBegin() + 1, (*attributeIt)->absentEnd(), intersectionIts);
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMet(**attributeIt, intersectionIts);
}

const unsigned int Tube::setAbsentAfterAbsentValuesMetAndAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const
{
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(**attributeIt, *absentValueIntersectionIt);
}

const unsigned int Tube::setSymmetricAbsentAfterAbsentValueMetAndAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const
{
  return absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(**attributeIt, *absentValueIntersectionIt);
}
