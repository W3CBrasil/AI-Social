// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015,2016 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include <string>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/binomial.hpp>

#include "Value.h"

#ifdef DEBUG
#include <iostream>
#endif

using namespace boost;

class Attribute
{
 public:

  /* /\* debug *\/ */
  /* void printPresentAndPotentialIntersectionsWithEnumeratedValueInNextAttribute(const Attribute& nextAttribute) const; */
  
  static unsigned int noisePerUnit; /* 1 if the data is crisp */

#ifdef DEBUG
  void printPresent(ostream& out) const;
  void printPotential(ostream& out) const;
  void printAbsent(ostream& out) const;
  void printChosenValue(ostream& out) const;
#endif
#ifdef ASSERT
  void printValue(const Value& value, ostream& out) const;
#endif

#if defined DEBUG || defined VERBOSE_ELEMENT_CHOICE || defined ASSERT
  static void setInternal2ExternalAttributeOrder(const vector<unsigned int>& internal2ExternalAttributeOrder);
#endif

  Attribute();
  Attribute(Attribute&& otherAttribute) = delete;
  Attribute(const vector<unsigned int>& nbOfValuesPerAttribute, const double epsilon, const vector<string>& labels);
  virtual ~Attribute();

  virtual Attribute* clone() const;
  virtual void subtractSelfLoopsFromPotentialNoise(const unsigned int totalMembershipDueToSelfLoopsOnASymmetricValue, const unsigned int nbOfSymmetricElements, const vector<Attribute*>::const_iterator attributeIt, const vector<Attribute*>::const_iterator attributeEnd);
  void setPresentIntersections(const vector<Attribute*>::const_iterator parentAttributeIt, const vector<Attribute*>::const_iterator parentAttributeEnd, const unsigned int presentAttributeId);
  void setPresentAndPotentialIntersections(const vector<Attribute*>::const_iterator parentAttributeIt, const vector<Attribute*>::const_iterator attributeIt, const vector<Attribute*>::const_iterator attributeEnd);
  
  Attribute& operator=(const Attribute& otherAttribute) = delete;
  Attribute& operator=(Attribute&& otherAttribute) = delete;
  friend ostream& operator<<(ostream& out, const Attribute& attribute);
  void printValueFromDataId(const unsigned int valueDataId, ostream& out) const;

  const unsigned int getId() const;
  virtual const bool symmetric() const;
  virtual Attribute* thisOrFirstSymmetricAttribute();
  vector<unsigned int> getPresentAndPotentialDataIds() const;
  vector<unsigned int> getIrrelevantDataIds() const;
  vector<Value*>::const_iterator presentBegin() const;
  vector<Value*>::const_iterator presentEnd() const;
  vector<Value*>::const_iterator potentialBegin() const;
  vector<Value*>::const_iterator potentialEnd() const;
  vector<Value*>::const_iterator irrelevantBegin() const;
  vector<Value*>::const_iterator irrelevantEnd() const;
  vector<Value*>::const_iterator absentBegin() const;
  vector<Value*>::const_iterator absentEnd() const;
  vector<Value*>::iterator presentBegin();
  vector<Value*>::iterator presentEnd();
  vector<Value*>::iterator potentialBegin();
  vector<Value*>::iterator potentialEnd();
  vector<Value*>::iterator irrelevantBegin();
  vector<Value*>::iterator irrelevantEnd();
  vector<Value*>::iterator absentBegin();
  vector<Value*>::iterator absentEnd();
  const unsigned int sizeOfPresent() const;
  const unsigned int sizeOfPresentAndPotential() const;
  const bool irrelevantEmpty() const;
  const unsigned int globalSize() const;
  const double totalPresentAndPotentialNoise() const;
  const double averagePresentAndPotentialNoise() const;

  const unsigned int getChosenValueDataId() const; /* to be called after chooseValue (on the same attribute or on its child) */
  Value& getChosenPresentValue() const; /* to be called after setPresentIntersections (on the same object) */
  virtual void repositionChosenPresentValue();	/* to be called after setPresentIntersections (on the same object) */
  virtual pair<const bool, vector<unsigned int>> setChosenValueAbsent(); /* to be called after chooseValue (on the same object) */
  const unsigned int getChosenAbsentValueDataId() const; /* to be called after setChosenValueAbsent (on the same object) */
  virtual void keepChosenAbsentValue(const bool isValuePotentiallyPreventingClosedness); /* to be called after setChosenValueAbsent (on the same object) */
  void setPotentialValueIrrelevant(const vector<Value*>::iterator potentialValueIt);
  vector<unsigned int> eraseIrrelevantValues(); /* returns the sorted data ids of the erased elements */

  // WARNING: These two methods should be called after initialization only (all values ordered in potential)
  vector<vector<unsigned int>>::iterator getIntersectionsBeginWithPotentialValues(const unsigned int valueId);
  void decrementPotentialNoise(const unsigned int valueId);
  void subtractPotentialNoise(const unsigned int valueId, const unsigned int noise);

  virtual const bool findIrrelevantValuesAndCheckTauContiguity(const vector<Attribute*>::iterator attributeBegin, const vector<Attribute*>::iterator attributeEnd);

#ifdef MIN_SIZE_ELEMENT_PRUNING
  const bool presentAndPotentialIrrelevant(const unsigned int presentAndPotentialIrrelevancyThreshold) const;
  virtual pair<bool, vector<unsigned int>> findPresentAndPotentialIrrelevantValuesAndCheckTauContiguity(const unsigned int presentAndPotentialIrrelevancyThreshold);
  virtual void presentAndPotentialCleanAbsent(const unsigned int presentAndPotentialIrrelevancyThreshold);
  virtual void sortPotentialIrrelevantAndAbsent();
  const unsigned int minSizeIrrelevancyThreshold(const vector<unsigned int>& minSizes, const double minArea, const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const;
  virtual void computeMinAreaOnNonSymmetricValue(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>::const_iterator thisIt, const vector<Attribute*>::const_iterator attributeEnd, const unsigned int nbOfSymmetricElements, const unsigned int currentArea, unsigned int& minArea) const;
  virtual void computeMinAreaOnSymmetricValue(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>::const_iterator thisIt, const vector<Attribute*>::const_iterator attributeEnd, const unsigned int nbOfSymmetricElements, const unsigned int currentArea, unsigned int& minArea) const;
#endif

  virtual const bool unclosed(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const;
  const bool valueDoesNotExtendPresentAndPotential(const Value& value, const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const;
  virtual void cleanAndSortAbsent(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd);
  void removeAbsentValue(vector<Value*>::iterator& valueIt);
  virtual void sortPotential();
  virtual void sortPotentialAndAbsentButChosenValue(const unsigned int presentAttributeId);
  virtual const bool finalizable() const;
  virtual vector<unsigned int> finalize(); /* returns the original ids of the elements moved to present */

  static void setIsClosedVectorAndIsStorageAllDense(const vector<bool>& isClosedVector, const bool isStorageAllDense);
  static void setOutputFormat(const char* outputElementSeparator, const char* emptySetString, const char* elementNoiseSeparator, const bool isNoisePrinted);

  static const unsigned int lastAttributeId();
  static const vector<unsigned int>& getEpsilonVector();
  static Attribute* chooseValue(const vector<Attribute*>::iterator attributeBegin, const vector<Attribute*>::iterator attributeEnd);

  static const bool noisePrinted();
  static void printOutputElementSeparator(ostream& out);
  static void printEmptySetString(ostream& out);
  static void printNoise(const float noise, ostream& out);

  static const bool lessAppealingIrrelevant(const Attribute* attribute, const Attribute* otherAttribute);

#ifdef DEBUG_HA
  static void printValuesFromDataIds(const vector<unsigned int>& dimension, const unsigned int dimensionId, ostream& out);
#endif

 protected:
  unsigned int id;
  vector<Value*> values;	/* present, then potential, then irrelevant, then absent */
  unsigned int potentialIndex;
  unsigned int irrelevantIndex;
  unsigned int absentIndex;

  static unsigned int maxId;
  static unsigned int orderedAttributeId;
  static vector<unsigned int> epsilonVector;
  static vector<vector<vector<unsigned int>>> nbOfValuesToReachEpsilonVector;
  static vector<bool> isClosedVector;
  static vector<vector<string>> labelsVector;
  static bool isDensestValuePreferred;

  static string outputElementSeparator;
  static string emptySetString;
  static string elementNoiseSeparator;
  static bool isNoisePrinted;

#if defined DEBUG || defined VERBOSE_ELEMENT_CHOICE || defined ASSERT
  static vector<unsigned int> internal2ExternalAttributeOrder;
#endif

  Attribute(const Attribute& parentAttribute);

  virtual const bool isEnumeratedAttribute(const unsigned int enumeratedAttributeId) const;
  void printValues(const vector<Value*>::const_iterator begin, const vector<Value*>::const_iterator end, ostream& out) const;

  Value* createPresentChildValue(const Value& parentValue, const unsigned int newId, const vector<Attribute*>::const_iterator parentNextAttributeIt, const vector<Attribute*>::const_iterator parentAttributeEnd, const unsigned int presentAttributeId) const;
  void setPresentChildValuePresentIntersections(const vector<unsigned int>& noiseInIntersectionWithPresentValues, vector<unsigned int>& childNoiseInIntersectionWithPresentValues, const unsigned int presentAttributeId) const;
  Value* createChosenValue(const Value& parentValue, const unsigned int newId, const vector<Attribute*>::const_iterator parentNextAttributeIt, const vector<Attribute*>::const_iterator parentAttributeEnd) const;
  void setChosenChildValuePresentIntersections(vector<unsigned int>& childNoiseInIntersectionWithPresentValues) const;
  Value* createPotentialOrAbsentChildValue(const Value& parentValue, const unsigned int newId, const vector<Attribute*>::const_iterator parentNextAttributeIt, const vector<Attribute*>::const_iterator parentAttributeEnd, const unsigned int presentAttributeId) const;
  void setPotentialOrAbsentChildValuePresentIntersections(const vector<unsigned int>& noiseInIntersectionWithPresentAndPotentialValues, vector<unsigned int>& childNoiseInIntersectionWithPresentAndPotentialValues, const unsigned int presentAttributeId) const;
  void setPresentAndPotentialIntersectionsWithPresentAndPotentialValues(const Attribute& parentAttribute, const vector<unsigned int>& parentNoiseInIntersectionWithPresentAndPotentialValues, vector<unsigned int>& noiseInIntersectionWithPresentAndPotentialValues) const;
  void setPresentAndPotentialIntersectionsWithAbsentValues(const Attribute& parentAttribute, const vector<unsigned int>& parentNoiseInIntersectionWithPresentAndPotentialValues, vector<unsigned int>& noiseInIntersectionWithPresentAndPotentialValues) const;

  pair<double, unsigned int> getAppealAndIndexOfValueToChoose(const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const; /* returns the appeal and the index of the best value to enumerate */
  virtual pair<double, unsigned int> getAppealAndIndexOfValueToChoose(const double presentCoeff, const double presentAndPotentialCoeff) const;
  virtual void chooseValue(const unsigned int indexOfValue);

  const bool valueDoesNotExtendPresent(const Value& value, const vector<Attribute*>::const_iterator attributeBegin, const vector<Attribute*>::const_iterator attributeEnd) const;
  void repositionChosenPresentValueInOrderedAttribute();
  void keepChosenAbsentValueInOrderedAttribute(const bool isValuePotentiallyPreventingClosedness); /* to be called after setChosenValueAbsent (on the same object) */

#ifdef MIN_SIZE_ELEMENT_PRUNING
  const bool presentAndPotentialIrrelevantValue(const Value& value, const unsigned int presentAndPotentialIrrelevancyThreshold) const;
  void computeMinArea(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>::const_iterator thisIt, const vector<Attribute*>::const_iterator attributeEnd, const unsigned int currentArea, unsigned int& minArea) const;
  virtual void computeMinAreaWithSymmetricAttributes(const float number, const vector<unsigned int>::const_reverse_iterator minSizeIt, const vector<unsigned int>::const_reverse_iterator productOfSubsequentMinSizesIt, const vector<Attribute*>& orthogonalAttributes, unsigned int& minArea) const;
  void terminateMinAreaComputationWithSymmetricAttributes(const float number, const unsigned int nbOfSymmetricElements, const unsigned int currentArea, unsigned int& minArea) const;
#endif

  static const bool lessPresentAndPotentialValues(const Attribute* attribute, const Attribute* otherAttribute);
  static void multiplicativePartition(const float number, const unsigned int nbOfSubsequentFactors, const unsigned int begin, const vector<Attribute*>& orderedAttributes, vector<unsigned int>& factorization, float& bestCost, Attribute*& bestAttribute);
};

#endif /*ATTRIBUTE_H_*/
