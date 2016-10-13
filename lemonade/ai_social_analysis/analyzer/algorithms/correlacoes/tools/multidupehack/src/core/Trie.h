// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef TRIE_H_
#define TRIE_H_

#include <unordered_map>

#include "../utilities/vector_hash.h"
#include "NoNoiseTube.h"
#include "SparseCrispTube.h"
#include "DenseCrispTube.h"
#include "SparseFuzzyTube.h"
#include "DenseFuzzyTube.h"

class Trie final : public AbstractData
{
 public:
  Trie() = default;
  Trie(const Trie& otherTrie);
  Trie(Trie&& otherTrie);
  Trie(const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator cardinalityEnd);

  ~Trie();

  Trie& operator=(const Trie& otherTrie);
  Trie& operator=(Trie&& otherTrie);
  friend ostream& operator<<(ostream& out, const Trie& trie);

  void print(vector<unsigned int>& prefix, ostream& out) const;
  const unsigned int depth() const;
  void setSelfLoops(const unsigned int firstSymmetricAttributeId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute);
  const bool setSelfLoopsInSymmetricAttributes(const unsigned int hyperplaneId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute, const unsigned int dimensionId);
  void setHyperplane(const unsigned int hyperplaneOldId, const unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator begin, const unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator end, const vector<unsigned int>& attributeOrder, const vector<vector<unsigned int>>& oldIds2NewIds, vector<Attribute*>& attributes);
  const bool setTuple(const vector<unsigned int>& tuple, const unsigned int membership, vector<unsigned int>::const_iterator attributeIdIt, vector<vector<unsigned int>>::const_iterator oldIds2NewIdsIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts);
  void sortTubes();

  void setPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeBegin) const;
  void setSymmetricPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeBegin) const;
  void setSymmetricAbsent(const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator attributeBegin) const;
  void setAbsent(const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator attributeBegin) const;

  const unsigned int setPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int setPresentAfterPresentValueMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int setSymmetricPresentAfterPresentValueMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int setPresentAfterPotentialOrAbsentUsed(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const;
  const unsigned int setPresentAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const;
  const unsigned int setSymmetricPresentAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const;
  const unsigned int setAbsent(const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int setAbsentAfterAbsentValuesMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int setSymmetricAbsentAfterAbsentValueMet(const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int setAbsentAfterAbsentUsed(const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const;
  const unsigned int setAbsentAfterAbsentValuesMetAndAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const;
  const unsigned int setSymmetricAbsentAfterAbsentValueMetAndAbsentUsed(const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const;

  const double noiseSum(const vector<vector<unsigned int>>& nSet) const;
  const unsigned int noiseSum(const vector<vector<unsigned int>>::const_iterator dimensionIt) const;

#ifdef ASSERT
  const unsigned int noiseSumOnPresent(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const;
  const unsigned int noiseSumOnPresentAndPotential(const vector<Attribute*>::const_iterator valueAttributeIt, const Value& value, const vector<Attribute*>::const_iterator attributeIt) const;
  const unsigned int noiseSumOnPresent(const vector<Attribute*>::const_iterator firstValueAttributeIt, const Value& firstValue, const vector<Attribute*>::const_iterator secondValueAttributeIt, const Value& secondValue, const vector<Attribute*>::const_iterator attributeIt) const;
  const unsigned int noiseSumOnPresentAndPotential(const vector<Attribute*>::const_iterator firstValueAttributeIt, const Value& firstValue, const vector<Attribute*>::const_iterator secondValueAttributeIt, const Value& secondValue, const vector<Attribute*>::const_iterator attributeIt) const;
#endif

 protected:
  vector<AbstractData*> hyperplanes;

  static NoNoiseTube noNoiseTube;

  Trie* clone() const;
  void copy(const Trie& otherTrie);

  void setSelfLoopsBeforeSymmetricAttributes(const unsigned int firstSymmetricAttributeId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute, const unsigned int dimensionId);
  void setSelfLoopsInSymmetricAttributes(AbstractData*& hyperplane, const unsigned int hyperplaneId, const unsigned int lastSymmetricAttributeId, const unsigned int sizeOfLastAttribute, const unsigned int nextDimensionId);
  void setSelfLoopsAfterSymmetricAttributes();
  void setTuple(AbstractData*& hyperplane, const vector<unsigned int>& tuple, const unsigned int membership, vector<unsigned int>::const_iterator attributeIdIt, vector<vector<unsigned int>>::const_iterator oldIds2NewIdsIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts);

  const unsigned int setSymmetricPresent(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int setSymmetricPresentAfterPotentialOrAbsentUsed(const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt) const;

  const unsigned int presentFixPresentValues(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const;
  const unsigned int presentFixPresentValuesBeforeSymmetricAttributes(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const;
  const unsigned int presentFixPresentValuesAfterPresentValueMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const;
  const unsigned int presentFixPresentValuesAfterPotentialOrAbsentUsed(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextPotentialOrAbsentValueIntersectionIt) const;
  const unsigned int presentFixPresentValuesBeforeSymmetricAttributesAfterPotentialOrAbsentUsed(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextPotentialOrAbsentValueIntersectionIt) const;
  const unsigned int presentFixPresentValuesAfterPresentValueMetAndPotentialOrAbsentUsed(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator potentialOrAbsentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextPotentialOrAbsentValueIntersectionIt) const;
  void presentFixPotentialOrAbsentValues(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  void presentFixPotentialOrAbsentValuesBeforeSymmetricAttributes(Attribute& currentAttribute, const vector<Attribute*>::iterator presentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  void presentFixPotentialOrAbsentValuesAfterPresentValueMet(Attribute& currentAttribute, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;

  const unsigned int setSymmetricAbsent(const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator attributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  const unsigned int setSymmetricAbsentAfterAbsentUsed(const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator attributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt) const;

  const unsigned int absentFixPresentOrPotentialValues(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const;
  const unsigned int absentFixPresentOrPotentialValuesBeforeSymmetricAttributes(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const;
  const unsigned int absentFixPresentOrPotentialValuesAfterAbsentValuesMet(Attribute& currentAttribute, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts, vector<vector<vector<unsigned int>>::iterator>& nextIntersectionIts) const;
  const unsigned int absentFixPresentOrPotentialValuesAfterAbsentUsed(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextAbsentValueIntersectionIt) const;
  const unsigned int absentFixPresentOrPotentialValuesBeforeSymmetricAttributesAfterAbsentUsed(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextAbsentValueIntersectionIt) const;
  const unsigned int absentFixPresentOrPotentialValuesAfterAbsentValuesMetAndAbsentUsed(Attribute& currentAttribute, const vector<Attribute*>::iterator nextAttributeIt, const vector<vector<unsigned int>>::iterator absentValueIntersectionIt, const vector<vector<unsigned int>>::iterator nextAbsentValueIntersectionIt) const;
  void absentFixAbsentValues(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  void absentFixAbsentValuesBeforeSymmetricAttributes(Attribute& currentAttribute, const vector<Attribute*>::iterator absentAttributeIt, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  void absentFixAbsentValuesInFirstSymmetricAttribute(Attribute& currentAttribute, const vector<unsigned int>& absentValueDataIds, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;
  void absentFixAbsentValuesAfterAbsentValuesMet(const vector<Value*>::iterator begin, const vector<Value*>::iterator end, const vector<Attribute*>::iterator nextAttributeIt, vector<vector<vector<unsigned int>>::iterator>& intersectionIts) const;

  static vector<vector<vector<unsigned int>>::iterator> incrementIterators(const vector<vector<vector<unsigned int>>::iterator>& iterators);
};

#endif /*TRIE_H_*/
