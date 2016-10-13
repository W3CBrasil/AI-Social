// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015,2016 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef SYMMETRIC_ATTRIBUTE_H_
#define SYMMETRIC_ATTRIBUTE_H_

#include "Attribute.h"

/* Although the code in Tree works for any number of symmetric attributes, this implementation is specific to two such attributes */
class SymmetricAttribute final: public Attribute
{
 public:
  SymmetricAttribute();
  SymmetricAttribute(Attribute&& otherSymmetricAttribute) = delete;
  SymmetricAttribute(const vector<unsigned int>& nbOfValuesPerAttribute, const double epsilon, const vector<string>& labels);
  
  SymmetricAttribute* clone() const;

  SymmetricAttribute& operator=(const SymmetricAttribute& otherSymmetricAttribute) = delete;
  SymmetricAttribute& operator=(SymmetricAttribute&& otherSymmetricAttribute) = delete;

  void setSymmetricAttribute(SymmetricAttribute* symmetricAttribute);
  const bool symmetric() const;
  SymmetricAttribute* thisOrFirstSymmetricAttribute();

  void subtractSelfLoopsFromPotentialNoise(const unsigned int totalMembershipDueToSelfLoopsOnASymmetricValue, const unsigned int nbOfSymmetricElements, const vector<Attribute*>::const_iterator attributeIt, const vector<Attribute*>::const_iterator attributeEnd);
  void repositionChosenPresentValue();
  pair<const bool, vector<unsigned int>> setChosenValueAbsent(); /* to be called after chooseValue (on the same object) */
  void keepChosenAbsentValue(const bool isValuePotentiallyPreventingClosedness); /* to be called after setChosenValueAbsent (on the same object) */

  const bool findIrrelevantValuesAndCheckTauContiguity(const vector<Attribute*>::iterator attributeBegin, const vector<Attribute*>::iterator attributeEnd);

#ifdef MIN_SIZE_ELEMENT_PRUNING
  pair<bool, vector<unsigned int>> findPresentAndPotentialIrrelevantValuesAndCheckTauContiguity(const unsigned int presentAndPotentialIrrelevancyThreshold);
  void presentAndPotentialCleanAbsent(const unsigned int presentAndPotentialIrrelevancyThreshold);
  void sortPotentialIrrelevantAndAbsent();
  void computeMinAreaOnNonSymmetricValue(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>::const_iterator thisIt, const vector<Attribute*>::const_iterator attributeEnd, const unsigned int nbOfSymmetricElements, const unsigned int currentArea, unsigned int& minArea) const;
  void computeMinAreaOnSymmetricValue(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>::const_iterator thisIt, const vector<Attribute*>::const_iterator attributeEnd, const unsigned int nbOfSymmetricElements, const unsigned int currentArea, unsigned int& minArea) const;
#endif

  const bool unclosed(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const;
  void cleanAndSortAbsent(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd);
  void shiftPotential();

 protected:
  SymmetricAttribute* symmetricAttribute;

  SymmetricAttribute(const SymmetricAttribute& parentAttribute);

  const bool isEnumeratedAttribute(const unsigned int enumeratedAttributeId) const;

  pair<double, unsigned int> getAppealAndIndexOfValueToChoose(const double presentCoeff, const double presentAndPotentialCoeff) const;
  void chooseValue(const unsigned int indexOfValue);

  const bool symmetricValuesDoNotExtendPresent(const Value& value, const Value& symmetricValue, const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const; /* must only be called on the first symmetric attribute */
  const bool symmetricValuesDoNotExtendPresentAndPotential(const Value& value, const Value& symmetricValue, const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const; /* must only be called on the first symmetric attribute */

  void keepChosenAbsentValueInOrderedAttribute(const bool isValuePotentiallyPreventingClosedness);  

#ifdef MIN_SIZE_ELEMENT_PRUNING
  void computeMinAreaWithSymmetricAttributes(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>& orthogonalAttributes, unsigned int& minArea) const;
#endif
};

#endif /*SYMMETRIC_ATTRIBUTE_H_*/
