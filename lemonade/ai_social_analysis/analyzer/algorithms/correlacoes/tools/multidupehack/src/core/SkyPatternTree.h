// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef SKY_PATTERN_TREE_H_
#define SKY_PATTERN_TREE_H_

#include "Tree.h"

struct skyPatternHasher
{
  size_t operator()(const IndistinctSkyPatterns* indistinctSkyPattern) const
  {
    const vector<unsigned int> minSizeMeasures = indistinctSkyPattern->getMinSizeMeasures();
    return boost::hash_range(minSizeMeasures.begin(), minSizeMeasures.end());
  }
};

struct skyPatternEqual
{
  bool operator()(const IndistinctSkyPatterns* indistinctSkyPattern1, const IndistinctSkyPatterns* indistinctSkyPattern2) const
  {
    return *indistinctSkyPattern1 == *indistinctSkyPattern2;
  }
};

class SkyPatternTree final : public Tree
{
 public:
  SkyPatternTree() = delete;
  SkyPatternTree(const SkyPatternTree&) = delete;
  SkyPatternTree(SkyPatternTree&&) = delete;
  SkyPatternTree(const char* dataFileName, const float densityThreshold, const double shiftMultiplier, const vector<double>& epsilonVector, const vector<unsigned int>& cliqueDimensions, const vector<double>& tauVector, const vector<unsigned int>& minSizes, const unsigned int minArea, const bool isReductionOnly, const bool isAgglomeration, const vector<unsigned int>& unclosedDimensions, const char* inputElementSeparator, const char* inputDimensionSeparator, const char* outputFileName, const char* outputDimensionSeparator, const char* patternSizeSeparator, const char* sizeSeparator, const char* sizeAreaSeparator, const bool isSizePrinted, const bool isAreaPrinted, const bool isIntermediateSkylinePrinted);

  ~SkyPatternTree();

  SkyPatternTree& operator=(const SkyPatternTree&) = delete;
  SkyPatternTree& operator=(SkyPatternTree&&) = delete;

  void initMeasures(const vector<unsigned int>& maxSizes, const int maxArea, const vector<unsigned int>& maximizedSizeDimensions, const vector<unsigned int>& minimizedSizeDimensions, const bool isAreaMaximized, const bool isAreaMinimized, const vector<string>& groupFileNames, const vector<unsigned int>& groupMinSizes, const vector<unsigned int>& groupMaxSizes, const vector<vector<float>>& groupMinRatios, const vector<vector<float>>& groupMinPiatetskyShapiros, const vector<vector<float>>& groupMinLeverages, const vector<vector<float>>& groupMinForces, const vector<vector<float>>& groupMinYulesQs, const vector<vector<float>>& groupMinYulesYs, const char* groupElementSeparator, const char* groupDimensionElementsSeparator, vector<unsigned int>& groupMaximizedSizes, const vector<unsigned int>& groupMinimizedSizes, const vector<vector<float>>& groupMaximizedRatios, const vector<vector<float>>& groupMaximizedPiatetskyShapiros, const vector<vector<float>>& groupMaximizedLeverages, const vector<vector<float>>& groupMaximizedForces, const vector<vector<float>>& groupMaximizedYulesQs, const vector<vector<float>>& groupMaximizedYulesYs, const char* sumValueFileName, const float minSum, const char* valueElementSeparator, const char* valueDimensionSeparator, const bool isSumMaximized, const char* slopePointFileName, const float minSlope, const char* pointElementSeparator, const char* pointDimensionSeparator, const bool isSlopeMaximized, const float densityThreshold);
  void terminate(const double maximalNbOfCandidateAgglomerates);

 protected:
  vector<Measure*> measuresToMaximize;

  static unsigned int nonMinSizeMeasuresIndex;
  static vector<IndistinctSkyPatterns*> skyPatterns;
  static unordered_set<IndistinctSkyPatterns*, skyPatternHasher, skyPatternEqual> minSizeSkyline;

  static bool isSomeOptimizedMeasureNotMonotone;
  static bool isIntermediateSkylinePrinted;

  SkyPatternTree(const SkyPatternTree& parent, const unsigned int presentAttributeId, const vector<Measure*>& mereConstraints, const vector<Measure*>& measuresToMaximize);

  const bool leftSubtree(const Attribute& presentAttribute);

  const bool violationAfterAdding(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent);
  const bool violationAfterRemoving(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent);
  const bool dominated();
#ifdef MIN_SIZE_ELEMENT_PRUNING
  vector<unsigned int> minSizeIrrelevancyThresholds() const;
#endif
  void validPattern() const;
  void printNSets(const vector<vector<vector<unsigned int>>>& nSets, ostream& out) const;

  static const bool dominated(const vector<Measure*>& measuresToMaximize);
};

#endif /*SKY_PATTERN_TREE_H_*/
