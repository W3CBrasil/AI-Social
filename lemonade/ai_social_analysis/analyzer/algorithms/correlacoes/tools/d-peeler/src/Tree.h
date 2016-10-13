// Copyright 2007-2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef TREE_H_
#define TREE_H_

#include <set>
#include <utility>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <chrono>

#include "../Parameters.h"
#include "HyperBitset.h"
#include "NRectangle.h"
#include "Groups.h"

class NRectangle;
class TupleFileReader;

class Tree
{
 public:

  enum Flags {
      NO_DATASET_REDUCTION   = 1 << 0,
      DATASET_REDUCTION_ONLY = 1 << 1,
      IS_MINIMIZATION        = 1 << 2,
      IS_SIZE_PRINTED        = 1 << 3,
      IS_AREA_PRINTED        = 1 << 4
  };

  static vector<unsigned int> dimensionOrder;
  static vector<unsigned int> nbOfElementsInDiInFiles;
  static vector<unsigned int> nbOfElementsInDi;
  static vector<float> deltaVector;
  static vector<unsigned int> minSizes;
  static vector<unsigned int> maxSizes;
  static float minArea;
  static float maxArea;
  static vector<float> areaCoefficients;
  static float minimizationRatio;
  static vector<unsigned int> cliqueDimensions;
  static AbstractHyperBitset* data;
  static AbstractHyperBitset* lastDimensionEmptyBitset;
  static unsigned int n;
  static vector<map<const unsigned int, const string> > ids2Labels;

  static bool isAMinimization;
  static ofstream outputFile;
  static const char* outputItemSeparator;
  static const char* outputDimensionSeparator;
  static const char* emptySetString;
  static const char* patternSizeSeparator;
  static const char* sizeSeparator;
  static const char* sizeAreaSeparator;
  static bool isSizePrinted;
  static bool isAreaPrinted;

  Tree();
  Tree(const char* dataFileName,
       const char* dontCareSetFileName,
       const vector<float>& deltaVector,
       const vector<unsigned int>& minSizes,
       const vector<unsigned int>& maxSizes,
       const float minArea,
       const float maxArea,
       const vector<float>& areaCoefficients,
       const float minimizationRatio,
       const vector<string>& groupFileNames,
       const vector<unsigned int>& groupMinSizes,
       const vector<unsigned int>& groupMaxSizes,
       const char* groupMinRatiosFileName,
       const vector<unsigned int>& cliqueDimensions,
       const char* inputItemSeparator,
       const char* inputDimensionSeparator,
       const char* groupItemSeparator,
       const char* groupDimensionItemsSeparator,
       const char* outputFileName,
       const char* outputItemSeparator,
       const char* outputDimensionSeparator,
       const char* patternSizeSeparator,
       const char* sizeSeparator,
       const char* sizeAreaSeparator,
       const char* emptySetString,
       Tree::Flags flags)
  throw(UsageException, DataFormatException, NoFileException);

  ~Tree();

  static void printNVector(const vector<vector<unsigned int> >& nVector, ostream& out = cout);

 protected:
  // PERF + CLEAN: Create a struct Dimension to avoid the vector on the views and iterate through everything
  vector<vector<unsigned int> > potential;
  vector<vector<unsigned int> > absent;

  Groups groups;

  static vector<vector<unsigned int> > present;
  static vector<unsigned int> sizes;

  static multiset<NRectangle> nRectangles;

#if ENUMERATION_PROCESS == 1 || ENUMERATION_PROCESS == 2
  static map<const pair<const unsigned int, const unsigned int>, const float> itemsAppeals;
#endif

  // CLEAN: All these static attributes should not be static
#ifdef DETAILED_TIME
  static clock_t parsingTime;
  static clock_t miningTime;
  static clock_t postProcessingTime;
  static clock_t propagatingTime;
  static clock_t maximalityCheckingTime;
  static clock_t closureOperationTime;
  static std::chrono::high_resolution_clock::duration preProcessingTime;
#endif
#ifdef NB_OF_NODES
  static unsigned int nbOfNodes;
#endif
#ifdef NB_OF_CLOSED_N_SETS
  static unsigned int nbOfClosedNSets;
#endif
#ifdef PRUNING_DETAILS
  static vector<bool> nbOfPrunesByMaximalityConstraint;
  static vector<bool> nbOfPrunesByMinimalSizeConstraint;
  static vector<bool> nbOfPrunesByMaximalSizeConstraint;
  static vector<bool> nbOfPrunesByMinimalAreaConstraint;
  static vector<bool> nbOfPrunesByMaximalAreaConstraint;
  static vector<bool> nbOfPrunesByMinimizationRatio;

  unsigned int nbOfPotentialElements() const;
  void incrementNbOfPrunes(vector<bool>& nbOfPrunes, const unsigned int index);
  static void printNbOfPrunes(const vector<bool>& nbOfPrunes, ostream& out = cout);
#endif

#ifdef AVERAGE_NB_OF_ITEMS_ABSENT_BY_PROPAGATION
  static unsigned int nbOfPropagations;
  static unsigned int nbOfItemsAbsentByPropagation;
#endif

/* #ifdef PROPORTION_OF_ABSENT_ITEMS_CHECKED_FOR_MAXIMALITY */
/*   static unsigned int nbOfItemsCheckedForMaximality; */
/*   static unsigned int nbOfAbsentItemsInDimensionCheckedForMaximality; */
/* #endif */

  Tree(const Tree& father, const unsigned int dimensionToPeelId, const unsigned int itemToPeelId, const bool isDimensionToPeelInTheClique);

  void peel(const int dimensionId);
  void goodLeaf() const;
  int nextDimension() const;
  float getAppeal(const unsigned int dimensionId) const;
  bool deltaMaximalOn(const unsigned int dimensionId);
  bool deltaMaximalOnEveryDimension();
  bool deltaMaximalExceptOn(const unsigned int dimensionNotCheckedId);
  void removeIrrelevantElementsFromAbsent(const unsigned int dimensionId, const vector<unsigned int> newItemIds, const bool isDimensionToPeelInTheClique);
  void fetchElementsInClosureOn(const unsigned int dimensionId, const bool isDimensionToPeelInTheClique);
  void fetchElementsInClosureOnEveryDimension(const bool isDimensionToPeelInTheClique);
  void fetchElementsInClosureExceptOn(const unsigned int dimensionNotCheckedId, const bool isDimensionToPeelInTheClique);
  vector<unsigned int> presentCardinals() const;
  void keepPresentPrefixes(const vector<unsigned int> indices);
#ifdef PRUNING_DETAILS
  const bool constraintsSatisfied();
#else
  const bool constraintsSatisfied() const;
#endif

  static void printDimension(const vector<unsigned int>& dimension, const map<const unsigned int, const string>& dimensionIds2Labels, ostream& out);

  /// Reads a data set contained in a file and returns the tuples in it.
  /// \param reader object to be used to read the file
  /// \param dataFileName path to the file to be parsed
  /// \return a vector containing the tuples from the file.
  static vector<vector<unsigned int>>
  parse(TupleFileReader& reader,
        const char* dataFileName);

  static void setSelfLoops(const vector<unsigned int>& cliqueDimensions);

  static void insertUnidimensionalNRectangles();

#ifdef DEBUG_EXTRACTION
  friend ostream& operator<<(ostream& out, const Tree& tree);
#endif
  static vector<unsigned int> reverseDimensionOrder;
};

#endif /*TREE_H_*/
