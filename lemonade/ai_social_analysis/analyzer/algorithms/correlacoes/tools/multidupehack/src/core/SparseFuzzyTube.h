// Copyright 2010,2011,2012,2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef SPARSE_FUZZY_TUBE_H_
#define SPARSE_FUZZY_TUBE_H_

#include "Tube.h"

class SparseFuzzyTube final : public Tube
{
 public:
  SparseFuzzyTube();

  void print(vector<unsigned int>& prefix, ostream& out) const;
  vector<unsigned int> getDenseRepresentation(const unsigned int nbOfHyperplanes) const;

  const bool setSelfLoopsInSymmetricAttributes(const unsigned int hyperplaneId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute, const unsigned int dimensionId);
  const bool setTuple(const vector<unsigned int>& tuple, const unsigned int membership, vector<unsigned int>::const_iterator attributeIdIt, vector<vector<unsigned int>>::const_iterator oldIds2NewIdsIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts);
  void sortTubes();
  
  const unsigned int setPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int setPresentAfterPotentialOrAbsentUsed(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const;

  const unsigned int noiseSum(const vector<vector<unsigned int>>::const_iterator dimensionIt) const;

#ifdef ASSERT
  const unsigned int noiseSumOnPresent(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const;
  const unsigned int noiseSumOnPresentAndPotential(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const;
#endif

  static void setDensityThreshold(const float densityThreshold);

 protected:
  vector<pair<unsigned int, unsigned int>> tube;

  static float densityThreshold;

  SparseFuzzyTube* clone() const;

  const unsigned int noiseOnValue(const unsigned int valueDataId) const;
  const unsigned int noiseOnValues(const vector<unsigned int>& valueDataIds) const;
  const unsigned int presentFixPresentValuesAfterPresentValueMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end) const;
  const unsigned int presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, vector<unsigned int>& potentialOrAbsentValueIntersection) const;
  void presentFixPotentialOrAbsentValuesAfterPresentValueMet(Attribute& currentAttribute, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  void presentFixPotentialOrAbsentValuesAfterPresentValueMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int absentFixPresentOrPotentialValuesAfterAbsentValuesMet(Attribute& currentAttribute, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int absentFixPresentOrPotentialValuesAfterAbsentValuesMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int noiseOnIrrelevant(const Attribute& currentAttribute) const;
  const unsigned int absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(Attribute& currentAttribute, vector<unsigned int>& absentValueIntersection) const;
  const unsigned int absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, vector<unsigned int>& absentValueIntersection) const;
  void absentFixAbsentValuesAfterAbsentValuesMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
};

#endif /*SPARSE_FUZZY_TUBE_H_*/
