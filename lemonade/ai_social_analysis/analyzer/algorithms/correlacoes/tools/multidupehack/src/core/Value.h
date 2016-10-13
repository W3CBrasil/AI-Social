// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015,2016 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef VALUE_H_
#define VALUE_H_

#include "../../Parameters.h"

#include <vector>

using namespace std;

class Value
{
 public:
  Value(const unsigned int dataId);
  Value(const unsigned int dataId, const unsigned int presentAndPotentialNoise, const vector<unsigned int>::const_iterator nbOfValuesPerAttributeBegin, const vector<unsigned int>::const_iterator nbOfValuesPerAttributeEnd, const vector<unsigned int>& noisesInIntersections);
  Value(const Value& otherValue, const unsigned int presentIntersectionId);

  const unsigned int getPresentIntersectionId() const;
  const unsigned int getPresentAndPotentialIntersectionId() const;
  const unsigned int getDataId() const;
  const unsigned int getPresentNoise() const;
  const unsigned int getPresentAndPotentialNoise() const;
  vector<vector<unsigned int>>::iterator setPresentAndPotentialIntersectionId(const unsigned int presentAndPotentialIntersectionId);
  void incrementPresentNoise();
  void addPresentNoise(const unsigned int noise);
  void decrementPotentialNoise();
  void subtractPotentialNoise(const unsigned int noise);
  vector<vector<unsigned int>>::iterator getIntersectionsBeginWithPresentValues();
  vector<vector<unsigned int>>::const_iterator getIntersectionsBeginWithPresentValues() const;
  vector<vector<unsigned int>>::iterator getIntersectionsBeginWithPresentAndPotentialValues();
  vector<vector<unsigned int>>::const_iterator getIntersectionsBeginWithPresentAndPotentialValues() const;
  void subtractPotentialNoiseAtIntersections(const unsigned int noise, const unsigned int index);
  void subtractPotentialNoiseAtIntersectionsWithSymmetricValue(const unsigned int noise, const unsigned int index);

  const unsigned int getNoiseInPresentIntersectionWith(const unsigned int reverseIndex, const Value& otherValue) const;

  const bool extendsPastPresent(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int intersectionIndex) const;
  const bool extendsFuturePresent(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int reverseAttributeIndex) const;
  const bool extendsPastPresentAndPotential(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int intersectionIndex) const;
  const bool extendsFuturePresentAndPotential(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int reverseAttributeIndex) const;

  const bool symmetricValuesExtendPastPresent(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int intersectionIndex) const;
  const bool symmetricValuesExtendFuturePresent(const Value& symmetricValue, const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int reverseAttributeIndex) const;
  const bool symmetricValuesExtendPastPresentAndPotential(const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int intersectionIndex) const;
  const bool symmetricValuesExtendFuturePresentAndPotential(const Value& symmetricValue, const vector<Value*>::const_iterator valueBegin, const vector<Value*>::const_iterator valueEnd, const unsigned int threshold, const unsigned int reverseAttributeIndex) const;

#ifdef ASSERT
  const unsigned int presentNoiseAtIntersectionWith(const Value& otherValue, const unsigned int intersectionIndex) const;
  const unsigned int presentAndPotentialNoiseAtIntersectionWith(const Value& otherValue, const unsigned int intersectionIndex) const;
#endif

  static const bool smallerDataId(const Value* value, const Value* otherValue);

 protected:
  unsigned int dataId;
  unsigned int presentIntersectionId;
  unsigned int presentAndPotentialIntersectionId;
  unsigned int presentNoise;
  unsigned int presentAndPotentialNoise;
  vector<vector<unsigned int>> intersectionsWithPresentValues;
  vector<vector<unsigned int>> intersectionsWithPresentAndPotentialValues;
};

#endif /*VALUE_H_*/
