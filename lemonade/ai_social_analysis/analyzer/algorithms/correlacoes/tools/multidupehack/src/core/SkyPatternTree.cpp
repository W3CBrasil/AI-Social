// Copyright 2007,2008,2009,2010,2011,2012,2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "SkyPatternTree.h"

unsigned int SkyPatternTree::nonMinSizeMeasuresIndex;
vector<IndistinctSkyPatterns*> SkyPatternTree::skyPatterns;
unordered_set<IndistinctSkyPatterns*, skyPatternHasher, skyPatternEqual> SkyPatternTree::minSizeSkyline;

bool SkyPatternTree::isSomeOptimizedMeasureNotMonotone;
bool SkyPatternTree::isIntermediateSkylinePrinted;

SkyPatternTree::SkyPatternTree(const char* dataFileName, const float densityThreshold, const double shiftMultiplier, const vector<double>& epsilonVectorParam, const vector<unsigned int>& cliqueDimensionsParam, const vector<double>& tauVectorParam, const vector<unsigned int>& minSizesParam, const unsigned int minAreaParam, const bool isReductionOnly, const bool isAgglomerationParam, const vector<unsigned int>& unclosedDimensions, const char* inputElementSeparator, const char* inputDimensionSeparator, const char* outputFileName, const char* outputDimensionSeparatorParam, const char* patternSizeSeparatorParam, const char* sizeSeparatorParam, const char* sizeAreaSeparatorParam, const bool isSizePrintedParam, const bool isAreaPrintedParam, const bool isIntermediateSkylinePrintedParam) : Tree(dataFileName, densityThreshold, shiftMultiplier, epsilonVectorParam, cliqueDimensionsParam, tauVectorParam, minSizesParam, minAreaParam, isReductionOnly, isAgglomerationParam, unclosedDimensions, inputElementSeparator, inputDimensionSeparator, outputFileName, outputDimensionSeparatorParam, patternSizeSeparatorParam, sizeSeparatorParam, sizeAreaSeparatorParam, isSizePrintedParam, isAreaPrintedParam), measuresToMaximize()
{
  isIntermediateSkylinePrinted = isIntermediateSkylinePrintedParam;
}

// Constructor of a left subtree
SkyPatternTree::SkyPatternTree(const SkyPatternTree& parent, const unsigned int presentAttributeId, const vector<Measure*>& mereConstraintsParam, const vector<Measure*>& measuresToMaximizeParam): Tree(parent, presentAttributeId, mereConstraintsParam), measuresToMaximize(std::move(measuresToMaximizeParam))
{
}

SkyPatternTree::~SkyPatternTree()
{
  deleteMeasures(measuresToMaximize);
}

const bool SkyPatternTree::leftSubtree(const Attribute& presentAttribute)
{
  const unsigned int presentAttributeId = presentAttribute.getId();
  const unsigned int valueId = presentAttribute.getChosenValueDataId();
  vector<Measure*> childMereConstraints = childMeasures(mereConstraints, presentAttributeId, valueId);
  if (childMereConstraints.size() != mereConstraints.size())
    {
      return true;
    }
  vector<Measure*> childMeasuresToMaximize = childMeasures(measuresToMaximize, presentAttributeId, valueId);
  if (childMeasuresToMaximize.size() != measuresToMaximize.size())
    {
      deleteMeasures(childMereConstraints);
      return true;
    }
  if (dominated(childMeasuresToMaximize))
    {
      deleteMeasures(childMereConstraints);
      deleteMeasures(childMeasuresToMaximize);
      return true;
    }
  SkyPatternTree leftChild(*this, presentAttributeId, childMereConstraints, childMeasuresToMaximize);
  leftChild.setPresent(presentAttributeId, attributes);
  return leftChild.isEnumeratedElementPotentiallyPreventingClosedness;
}

void SkyPatternTree::initMeasures(const vector<unsigned int>& maxSizesParam, const int maxArea, const vector<unsigned int>& maximizedSizeDimensionsParam, const vector<unsigned int>& minimizedSizeDimensionsParam, const bool isAreaMaximized, const bool isAreaMinimized, const vector<string>& groupFileNames, const vector<unsigned int>& groupMinSizesParam, const vector<unsigned int>& groupMaxSizes, const vector<vector<float>>& groupMinRatios, const vector<vector<float>>& groupMinPiatetskyShapiros, const vector<vector<float>>& groupMinLeverages, const vector<vector<float>>& groupMinForces, const vector<vector<float>>& groupMinYulesQs, const vector<vector<float>>& groupMinYulesYs, const char* groupElementSeparator, const char* groupDimensionElementsSeparator, vector<unsigned int>& groupMaximizedSizes, const vector<unsigned int>& groupMinimizedSizes, const vector<vector<float>>& groupMaximizedRatios, const vector<vector<float>>& groupMaximizedPiatetskyShapiros, const vector<vector<float>>& groupMaximizedLeverages, const vector<vector<float>>& groupMaximizedForces, const vector<vector<float>>& groupMaximizedYulesQs, const vector<vector<float>>& groupMaximizedYulesYs, const char* sumValueFileName, const float minSum, const char* valueElementSeparator, const char* valueDimensionSeparator, const bool isSumMaximized, const char* slopePointFileName, const float minSlope, const char* pointElementSeparator, const char* pointDimensionSeparator, const bool isSlopeMaximized, const float densityThreshold)
{
  // Helper variables
  const unsigned int n = attributes.size();
  vector<unsigned int> cardinalities;
  cardinalities.reserve(n);
  for (const Attribute* attribute : attributes)
    {
      cardinalities.push_back(attribute->sizeOfPresentAndPotential());
    }
  vector<Measure*> nonMinSizeMeasuresToMaximize;
  try
    {
      // Get the maximal sizes in the internal order of the attributes
      vector<unsigned int> maxSizes(cardinalities);
      if (!maxSizesParam.empty())
	{
	  if (maxSizesParam.size() > n)
	    {
	      throw UsageException(("Sizes option should provide at most " + lexical_cast<string>(n) + " sizes!").c_str());
	    }
	  vector<unsigned int>::const_iterator external2InternalAttributeOrderIt = external2InternalAttributeOrder.begin();
	  for (const unsigned int maxSize : maxSizesParam)
	    {
	      maxSizes[*external2InternalAttributeOrderIt] = maxSize;
	      ++external2InternalAttributeOrderIt;
	    }
	  setMinParametersInClique(maxSizes);
	}
      // Initializing measures in increasing cost to update them
      // Initializing minimizedSizeDimensions and maximizedSizeDimensions
      if (!maximizedSizeDimensionsParam.empty() && maximizedSizeDimensionsParam.back() >= n)
	{
	  throw UsageException(("sky-s option should provide attribute ids between 0 and " + lexical_cast<string>(n - 1)).c_str());
	}
      if (!minimizedSizeDimensionsParam.empty() && minimizedSizeDimensionsParam.back() >= n)
	{
	  throw UsageException(("sky-S option should provide attribute ids between 0 and " + lexical_cast<string>(n - 1)).c_str());
	}
      vector<unsigned int> minimizedSizeDimensions;
      minimizedSizeDimensions.reserve(minimizedSizeDimensionsParam.size());
      for (const unsigned int minimizedSizeDimension : minimizedSizeDimensionsParam)
	{
	  minimizedSizeDimensions.push_back(external2InternalAttributeOrder[minimizedSizeDimension]);
	}
      vector<unsigned int> maximizedSizeDimensions;
      maximizedSizeDimensions.reserve(maximizedSizeDimensionsParam.size());
      for (const unsigned int maximizedSizeDimension : maximizedSizeDimensionsParam)
	{
	  maximizedSizeDimensions.push_back(external2InternalAttributeOrder[maximizedSizeDimension]);
	}
      sort(maximizedSizeDimensions.begin(), maximizedSizeDimensions.end());
      IndistinctSkyPatterns::setParametersToComputePresentAndPotentialIrrelevancyThresholds(maximizedSizeDimensions, isAreaMaximized);
      // Initializing MinSize measures
      unsigned int minAreaAccordingToSizes = 1;
      vector<unsigned int>::const_iterator maximizedSizeDimensionIt = maximizedSizeDimensions.begin();
      for (unsigned int attributeId = 0; attributeId != n; ++attributeId)
	{
	  if (maximizedSizeDimensionIt != maximizedSizeDimensions.end() && *maximizedSizeDimensionIt == attributeId)
	    {
	      measuresToMaximize.push_back(new MinSize(attributeId, cardinalities[attributeId], minSizes[attributeId]));
	      ++maximizedSizeDimensionIt;
	    }
	  else
	    {
	      const unsigned int minSize = minSizes[attributeId];
	      minAreaAccordingToSizes *= minSize;
	      if (minSize != 0)
		{
		  mereConstraints.push_back(new MinSize(attributeId, cardinalities[attributeId], minSize));
		}
	    }
	}
      // Initializing MaxSize measures
      sort(minimizedSizeDimensions.begin(), minimizedSizeDimensions.end());
      vector<unsigned int>::const_iterator minimizedSizeDimensionIt = minimizedSizeDimensions.begin();
      for (unsigned int attributeId = 0; attributeId != n; ++attributeId)
	{
	  if (minimizedSizeDimensionIt != minimizedSizeDimensions.end() && *minimizedSizeDimensionIt == attributeId)
	    {
	      nonMinSizeMeasuresToMaximize.push_back(new MaxSize(attributeId, maxSizes[attributeId]));
	      ++minimizedSizeDimensionIt;
	    }
	  else
	    {
	      const unsigned int maxSize = maxSizes[attributeId];
	      if (maxSize < cardinalities[attributeId])
		{
		  mereConstraints.push_back(new MaxSize(attributeId, maxSize));
		}
	    }
	}
      // Initializing minArea measure
      if (isAreaMaximized && maximizedSizeDimensions.size() != n)
	{
	  measuresToMaximize.push_back(new MinArea(cardinalities, minArea));
	}
      else
	{
	  if (minArea > minAreaAccordingToSizes)
	    {
	      mereConstraints.push_back(new MinArea(cardinalities, minArea));
	    }
	}
      // Initializing maxArea measure
      if (isAreaMinimized)
	{
	  if (maxArea == -1)
	    {
	      nonMinSizeMeasuresToMaximize.push_back(new MaxArea(n, numeric_limits<unsigned int>::max()));
	    }
	  else
	    {
	      nonMinSizeMeasuresToMaximize.push_back(new MaxArea(n, maxArea));
	    }
	}
      else
	{
	  if (maxArea != -1)
	    {
	      mereConstraints.push_back(new MaxArea(n, maxArea));
	    }
	}
      if (!groupFileNames.empty())
	{
	  // Initializing groups
	  GroupMeasure::initGroups(groupFileNames, groupElementSeparator, groupDimensionElementsSeparator, cardinalities, labels2Ids, external2InternalAttributeOrder);
	  // groupMinSizes is to be modified according to the diagonals of MinGroupCoverRatios, MinGroupCoverPiatetskyShapiros, MinGroupCoverLeverages, MinGroupCoverForces, MinGroupCoverYulesQ and MinGroupCoverYulesY
	  vector<unsigned int> groupMinSizes = groupMinSizesParam;
	  groupMinSizes.resize(groupFileNames.size());
	  // Initializing MinGroupCoverRatio measures
	  if (!groupMaximizedRatios.empty())
	    {
	      unsigned int rowId = 0;
	      for (const vector<float>& maximizedRow : groupMaximizedRatios)
		{
		  if (maximizedRow.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with sky-gr option provides " + lexical_cast<string>(maximizedRow.size()) + " values but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float maximizedRatio : maximizedRow)
		    {
		      if (maximizedRatio != 0)
			{
			  if (rowId == columnId)
			    {
			      const vector<unsigned int>::iterator groupMaximizedSizeIt = lower_bound(groupMaximizedSizes.begin(), groupMaximizedSizes.end(), rowId);
			      if (groupMaximizedSizeIt == groupMaximizedSizes.end() || *groupMaximizedSizeIt != rowId)
				{
				  groupMaximizedSizes.insert(groupMaximizedSizeIt, rowId);
				}
			    }
			  else
			    {
			      if (groupMinRatios.size() > rowId && groupMinRatios[rowId].size() > columnId)
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverRatio(rowId, columnId, groupMinRatios[rowId][columnId]));
				}
			      else
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverRatio(rowId, columnId, 0));
				}
			    }
			}
		      ++columnId;
		    }
		  ++rowId;
		}
	    }
	  if (!groupMinRatios.empty())
	    {
	      vector<unsigned int>::iterator groupMinSizeIt = groupMinSizes.begin();
	      unsigned int rowId = 0;
	      for (const vector<float>& row : groupMinRatios)
		{
		  if (row.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with gr option provides " + lexical_cast<string>(row.size()) + " ratios but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float ratio : row)
		    {
		      if (ratio > 0)
			{
			  if (rowId == columnId)
			    {
			      const unsigned int groupMinSizeAccordingToMatrix = ratio;
			      if (groupMinSizeAccordingToMatrix > *groupMinSizeIt)
				{
				  *groupMinSizeIt = groupMinSizeAccordingToMatrix;
				}
			    }
			  else
			    {
			      if (!(groupMaximizedRatios.size() > rowId && groupMaximizedRatios[rowId].size() > columnId && groupMaximizedRatios[rowId][columnId] != 0))
				{
				  mereConstraints.push_back(new MinGroupCoverRatio(rowId, columnId, ratio));
				}
			    }
			}
		      ++columnId;
		    }
		  ++groupMinSizeIt;
		  ++rowId;
		}
	    }
	  // Initializing MinGroupCoverPiatetskyShapiro measures
	  if (!groupMaximizedPiatetskyShapiros.empty())
	    {
	      unsigned int rowId = 0;
	      for (const vector<float>& maximizedRow : groupMaximizedPiatetskyShapiros)
		{
		  if (maximizedRow.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with sky-gps option provides " + lexical_cast<string>(maximizedRow.size()) + " values but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float maximizedPiatetskyShapiro : maximizedRow)
		    {
		      if (maximizedPiatetskyShapiro != 0)
			{
			  if (rowId == columnId)
			    {
			      const vector<unsigned int>::iterator groupMaximizedSizeIt = lower_bound(groupMaximizedSizes.begin(), groupMaximizedSizes.end(), rowId);
			      if (groupMaximizedSizeIt == groupMaximizedSizes.end() || *groupMaximizedSizeIt != rowId)
				{
				  groupMaximizedSizes.insert(groupMaximizedSizeIt, rowId);
				}
			    }
			  else
			    {
			      if (groupMinPiatetskyShapiros.size() > rowId && groupMinPiatetskyShapiros[rowId].size() > columnId)
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverPiatetskyShapiro(rowId, columnId, groupMinPiatetskyShapiros[rowId][columnId]));
				}
			      else
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverPiatetskyShapiro(rowId, columnId, -numeric_limits<float>::infinity()));
				}
			    }
			}
		      ++columnId;
		    }
		  ++rowId;
		}
	    }
	  if (!groupMinPiatetskyShapiros.empty())
	    {
	      vector<unsigned int>::iterator groupMinSizeIt = groupMinSizes.begin();
	      unsigned int rowId = 0;
	      for (const vector<float>& row : groupMinPiatetskyShapiros)
		{
		  if (row.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with gps option provides " + lexical_cast<string>(row.size()) + " Piatetsky-Shapiro's measures but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float piatetskyShapiro : row)
		    {
		      if (rowId == columnId)
			{
			  const int groupMinSizeAccordingToMatrix = piatetskyShapiro;
			  if (groupMinSizeAccordingToMatrix > static_cast<int>(*groupMinSizeIt))
			    {
			      *groupMinSizeIt = groupMinSizeAccordingToMatrix;
			    }
			}
		      else
			{
			  if (-GroupMeasure::maxCoverOfGroup(rowId) < piatetskyShapiro * GroupMeasure::maxCoverOfGroup(columnId) && !(groupMaximizedPiatetskyShapiros.size() > rowId && groupMaximizedPiatetskyShapiros[rowId].size() > columnId && groupMaximizedPiatetskyShapiros[rowId][columnId] != 0))
			    {
			      mereConstraints.push_back(new MinGroupCoverPiatetskyShapiro(rowId, columnId, piatetskyShapiro));
			    }
			}
		      ++columnId;
		    }
		  ++groupMinSizeIt;
		  ++rowId;
		}
	    }
	  // Initializing MinGroupCoverLeverage measures
	  if (!groupMaximizedLeverages.empty())
	    {
	      unsigned int rowId = 0;
	      for (const vector<float>& maximizedRow : groupMaximizedLeverages)
		{
		  if (maximizedRow.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with sky-gl option provides " + lexical_cast<string>(maximizedRow.size()) + " values but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float maximizedLeverage : maximizedRow)
		    {
		      if (maximizedLeverage != 0)
			{
			  if (rowId == columnId)
			    {
			      const vector<unsigned int>::iterator groupMaximizedSizeIt = lower_bound(groupMaximizedSizes.begin(), groupMaximizedSizes.end(), rowId);
			      if (groupMaximizedSizeIt == groupMaximizedSizes.end() || *groupMaximizedSizeIt != rowId)
				{
				  groupMaximizedSizes.insert(groupMaximizedSizeIt, rowId);
				}
			    }
			  else
			    {
			      if (groupMinLeverages.size() > rowId && groupMinLeverages[rowId].size() > columnId)
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverLeverage(rowId, columnId, groupMinLeverages[rowId][columnId]));
				}
			      else
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverLeverage(rowId, columnId, -numeric_limits<float>::infinity()));
				}
			    }
			}
		      ++columnId;
		    }
		  ++rowId;
		}
	    }
	  if (!groupMinLeverages.empty())
	    {
	      vector<unsigned int>::iterator groupMinSizeIt = groupMinSizes.begin();
	      unsigned int rowId = 0;
	      for (const vector<float>& row : groupMinLeverages)
		{
		  if (row.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with gl option provides " + lexical_cast<string>(row.size()) + " leverages but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float leverage : row)
		    {
		      if (rowId == columnId)
			{
			  const int groupMinSizeAccordingToMatrix = leverage;
			  if (groupMinSizeAccordingToMatrix > static_cast<int>(*groupMinSizeIt))
			    {
			      *groupMinSizeIt = groupMinSizeAccordingToMatrix;
			    }
			}
		      else
			{
			  if (-GroupMeasure::maxCoverOfGroup(rowId) < leverage * GroupMeasure::maxCoverOfGroup(columnId) && !(groupMaximizedLeverages.size() > rowId && groupMaximizedLeverages[rowId].size() > columnId && groupMaximizedLeverages[rowId][columnId] != 0))
			    {
			      mereConstraints.push_back(new MinGroupCoverLeverage(rowId, columnId, leverage));
			    }
			}
		      ++columnId;
		    }
		  ++groupMinSizeIt;
		  ++rowId;
		}
	    }
	  // Initializing MinGroupCoverForce measures
	  if (!groupMaximizedForces.empty())
	    {
	      unsigned int rowId = 0;
	      for (const vector<float>& maximizedRow : groupMaximizedForces)
		{
		  if (maximizedRow.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with sky-gf option provides " + lexical_cast<string>(maximizedRow.size()) + " values but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float maximizedForce : maximizedRow)
		    {
		      if (maximizedForce != 0)
			{
			  if (rowId == columnId)
			    {
			      const vector<unsigned int>::iterator groupMaximizedSizeIt = lower_bound(groupMaximizedSizes.begin(), groupMaximizedSizes.end(), rowId);
			      if (groupMaximizedSizeIt == groupMaximizedSizes.end() || *groupMaximizedSizeIt != rowId)
				{
				  groupMaximizedSizes.insert(groupMaximizedSizeIt, rowId);
				}
			    }
			  else
			    {
			      if (groupMinForces.size() > rowId && groupMinForces[rowId].size() > columnId)
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverForce(rowId, columnId, groupMinForces[rowId][columnId]));
				}
			      else
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverForce(rowId, columnId, 0));
				}
			    }
			}
		      ++columnId;
		    }
		  ++rowId;
		}
	    }
	  if (!groupMinForces.empty())
	    {
	      vector<unsigned int>::iterator groupMinSizeIt = groupMinSizes.begin();
	      unsigned int rowId = 0;
	      for (const vector<float>& row : groupMinForces)
		{
		  if (row.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with gf option provides " + lexical_cast<string>(row.size()) + " forces but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float force : row)
		    {
		      if (force > 0)
			{
			  if (rowId == columnId)
			    {
			      const unsigned int groupMinSizeAccordingToMatrix = force;
			      if (groupMinSizeAccordingToMatrix > *groupMinSizeIt)
				{
				  *groupMinSizeIt = groupMinSizeAccordingToMatrix;
				}
			    }
			  else
			    {
			      if (!(groupMaximizedForces.size() > rowId && groupMaximizedForces[rowId].size() > columnId && groupMaximizedForces[rowId][columnId] != 0))
				{
				  mereConstraints.push_back(new MinGroupCoverForce(rowId, columnId, force));
				}
			    }
			}
		      ++columnId;
		    }
		  ++groupMinSizeIt;
		  ++rowId;
		}
	    }
	  // Initializing MinGroupCoverYulesQ measures
	  if (!groupMaximizedYulesQs.empty())
	    {
	      unsigned int rowId = 0;
	      for (const vector<float>& maximizedRow : groupMaximizedYulesQs)
		{
		  if (maximizedRow.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with sky-gyq option provides " + lexical_cast<string>(maximizedRow.size()) + " values but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float maximizedYulesQ : maximizedRow)
		    {
		      if (maximizedYulesQ != 0)
			{
			  if (rowId == columnId)
			    {
			      const vector<unsigned int>::iterator groupMaximizedSizeIt = lower_bound(groupMaximizedSizes.begin(), groupMaximizedSizes.end(), rowId);
			      if (groupMaximizedSizeIt == groupMaximizedSizes.end() || *groupMaximizedSizeIt != rowId)
				{
				  groupMaximizedSizes.insert(groupMaximizedSizeIt, rowId);
				}
			    }
			  else
			    {
			      if (groupMinYulesQs.size() > rowId && groupMinYulesQs[rowId].size() > columnId)
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverYulesQ(rowId, columnId, groupMinYulesQs[rowId][columnId]));
				}
			      else
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverYulesQ(rowId, columnId, -1));
				}
			    }
			}
		      ++columnId;
		    }
		  ++rowId;
		}
	    }
	  if (!groupMinYulesQs.empty())
	    {
	      vector<unsigned int>::iterator groupMinSizeIt = groupMinSizes.begin();
	      unsigned int rowId = 0;
	      for (const vector<float>& row : groupMinYulesQs)
		{
		  if (row.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with gyq option provides " + lexical_cast<string>(row.size()) + " Yule's Q measures but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float yulesQ : row)
		    {
		      if (rowId == columnId)
			{
			  if (yulesQ > 0)
			    {
			      const unsigned int groupMinSizeAccordingToMatrix = yulesQ;
			      if (groupMinSizeAccordingToMatrix > *groupMinSizeIt)
				{
				  *groupMinSizeIt = groupMinSizeAccordingToMatrix;
				}
			    }
			}
		      else
			{
			  if (yulesQ > -1 && !(groupMaximizedYulesQs.size() > rowId && groupMaximizedYulesQs[rowId].size() > columnId && groupMaximizedYulesQs[rowId][columnId] != 0))
			    {
			      mereConstraints.push_back(new MinGroupCoverYulesQ(rowId, columnId, yulesQ));
			    }
			}
		      ++columnId;
		    }
		  ++groupMinSizeIt;
		  ++rowId;
		}
	    }
	  // Initializing MinGroupCoverYulesY measures
	  if (!groupMaximizedYulesYs.empty())
	    {
	      unsigned int rowId = 0;
	      for (const vector<float>& maximizedRow : groupMaximizedYulesYs)
		{
		  if (maximizedRow.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with sky-gyy option provides " + lexical_cast<string>(maximizedRow.size()) + " values but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float maximizedYulesY : maximizedRow)
		    {
		      if (maximizedYulesY != 0)
			{
			  if (rowId == columnId)
			    {
			      const vector<unsigned int>::iterator groupMaximizedSizeIt = lower_bound(groupMaximizedSizes.begin(), groupMaximizedSizes.end(), rowId);
			      if (groupMaximizedSizeIt == groupMaximizedSizes.end() || *groupMaximizedSizeIt != rowId)
				{
				  groupMaximizedSizes.insert(groupMaximizedSizeIt, rowId);
				}
			    }
			  else
			    {
			      if (groupMinYulesYs.size() > rowId && groupMinYulesYs[rowId].size() > columnId)
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverYulesY(rowId, columnId, groupMinYulesYs[rowId][columnId]));
				}
			      else
				{
				  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCoverYulesY(rowId, columnId, -1));
				}
			    }
			}
		      ++columnId;
		    }
		  ++rowId;
		}
	    }
	  if (!groupMinYulesYs.empty())
	    {
	      vector<unsigned int>::iterator groupMinSizeIt = groupMinSizes.begin();
	      unsigned int rowId = 0;
	      for (const vector<float>& row : groupMinYulesYs)
		{
		  if (row.size() > groupFileNames.size())
		    {
		      throw UsageException(("row " + lexical_cast<string>(rowId) + " of file set with gyy option provides " + lexical_cast<string>(row.size()) + " Yule's Y measures but groups option only defines " + lexical_cast<string>(groupFileNames.size()) + " groups!").c_str());
		    }
		  unsigned int columnId = 0;
		  for (const float yulesY : row)
		    {
		      if (rowId == columnId)
			{
			  if (yulesY > 0)
			    {
			      const unsigned int groupMinSizeAccordingToMatrix = yulesY;
			      if (groupMinSizeAccordingToMatrix > *groupMinSizeIt)
				{
				  *groupMinSizeIt = groupMinSizeAccordingToMatrix;
				}
			    }
			}
		      else
			{
			  if (yulesY > -1 && !(groupMaximizedYulesYs.size() > rowId && groupMaximizedYulesYs[rowId].size() > columnId && groupMaximizedYulesYs[rowId][columnId] != 0))
			    {
			      mereConstraints.push_back(new MinGroupCoverYulesY(rowId, columnId, yulesY));
			    }
			}
		      ++columnId;
		    }
		  ++groupMinSizeIt;
		  ++rowId;
		}
	    }
	  // Initializing MinGroupCover measures
	  const vector<unsigned int>::const_iterator end = groupMinSizes.end();
	  vector<unsigned int>::const_iterator groupMinSizeIt = groupMinSizes.begin();
	  vector<unsigned int>::const_iterator groupMaximizedSizeIt = groupMaximizedSizes.begin();
	  for (unsigned int groupId = 0; groupMinSizeIt != end; ++groupId)
	    {
	      if (groupMaximizedSizeIt != groupMaximizedSizes.end() && *groupMaximizedSizeIt == groupId)
		{
		  nonMinSizeMeasuresToMaximize.push_back(new MinGroupCover(groupId, *groupMinSizeIt));
		  ++groupMaximizedSizeIt;
		}
	      else
		{
		  if (*groupMinSizeIt != 0)
		    {
		      mereConstraints.push_back(new MinGroupCover(groupId, *groupMinSizeIt));
		    }
		}
	      ++groupMinSizeIt;
	    }
	  // Initializing MaxGroupCover measures
	  vector<unsigned int>::const_iterator groupMinimizedSizeIt = groupMinimizedSizes.begin();
	  unsigned int groupId = 0;
	  for (const unsigned int groupMaxSize : groupMaxSizes)
	    {
	      if (groupMinimizedSizeIt != groupMinimizedSizes.end() && *groupMinimizedSizeIt == groupId)
		{
		  nonMinSizeMeasuresToMaximize.push_back(new MaxGroupCover(groupId, groupMaxSize));
		  ++groupMinimizedSizeIt;
		}
	      else
		{
		  if (groupMaxSize < GroupMeasure::maxCoverOfGroup(groupId))
		    {
		      mereConstraints.push_back(new MaxGroupCover(groupId, groupMaxSize));
		    }
		}
	      ++groupId;
	    }
	  for (; groupMinimizedSizeIt != groupMinimizedSizes.end(); ++groupMinimizedSizeIt)
	    {
	      nonMinSizeMeasuresToMaximize.push_back(new MaxGroupCover(*groupMinimizedSizeIt, GroupMeasure::maxCoverOfGroup(*groupMinimizedSizeIt)));
	    }
	  if (groupMinSizesParam.empty() && groupMaximizedSizes.empty() && groupMaxSizes.empty() && groupMinimizedSizes.empty() && groupMinRatios.empty() && groupMaximizedRatios.empty() && groupMinPiatetskyShapiros.empty() && groupMaximizedPiatetskyShapiros.empty() && groupMinLeverages.empty() && groupMaximizedLeverages.empty() && groupMinForces.empty() && groupMaximizedForces.empty() && groupMinYulesQs.empty() && groupMaximizedYulesQs.empty() && groupMinYulesYs.empty() && groupMaximizedYulesYs.empty())
	    {
	      for (groupId = 0; groupId != groupFileNames.size(); ++groupId)
		{
		  mereConstraints.push_back(new MinGroupCover(groupId, GroupMeasure::maxCoverOfGroup(groupId)));
		}
	    }
	  GroupMeasure::allMeasuresSet();
	}
      // Initializing min sum measure
      const string sumValueFileNameString(sumValueFileName);
      if (!sumValueFileNameString.empty())
	{
	  if (isSumMaximized)
	    {
	      nonMinSizeMeasuresToMaximize.push_back(new MinSum(sumValueFileNameString, valueDimensionSeparator, valueElementSeparator, labels2Ids, external2InternalAttributeOrder, cardinalities, densityThreshold, minSum));
	    }
	  else
	    {
	      mereConstraints.push_back(new MinSum(sumValueFileNameString, valueDimensionSeparator, valueElementSeparator, labels2Ids, external2InternalAttributeOrder, cardinalities, densityThreshold, minSum));
	    }
	}
      // Initializing min slope measure
      const string slopePointFileNameString(slopePointFileName);
      if (!slopePointFileNameString.empty())
	{
	  if (isSlopeMaximized)
	    {
	      nonMinSizeMeasuresToMaximize.push_back(new MinSlope(slopePointFileNameString, pointDimensionSeparator, pointElementSeparator, labels2Ids, external2InternalAttributeOrder, cardinalities, densityThreshold, minSlope));
	    }
	  else
	    {
	      mereConstraints.push_back(new MinSlope(slopePointFileNameString, pointDimensionSeparator, pointElementSeparator, labels2Ids, external2InternalAttributeOrder, cardinalities, densityThreshold, minSlope));
	    }
	}
    }
  catch (std::exception& e)
    {
      outputFile.close();
      delete data;
      rethrow_exception(current_exception());
    }
  labels2Ids.clear();
  TupleMeasure::allMeasuresSet(cardinalities);
  stable_partition(mereConstraints.begin(), mereConstraints.end(), monotone);
  nonMinSizeMeasuresIndex = measuresToMaximize.size();
  stable_partition(nonMinSizeMeasuresToMaximize.begin(), nonMinSizeMeasuresToMaximize.end(), monotone);
  measuresToMaximize.insert(measuresToMaximize.end(), nonMinSizeMeasuresToMaximize.begin(), nonMinSizeMeasuresToMaximize.end());
  isSomeOptimizedMeasureNotMonotone = !measuresToMaximize.back()->monotone();
}

void SkyPatternTree::terminate(const double maximalNbOfCandidateAgglomerates)
{
  if (isAgglomeration)
    {
      for (IndistinctSkyPatterns* indistinctSkyPatterns : skyPatterns)
	{
	  indistinctSkyPatterns->toNodes(data);
	  delete indistinctSkyPatterns;
	}
    }
  else
    {
      for (IndistinctSkyPatterns* indistinctSkyPatterns : skyPatterns)
	{
#ifdef OUTPUT
	  printNSets(indistinctSkyPatterns->getPatterns(), outputFile);
#endif
	  delete indistinctSkyPatterns;
	}
    }
  Tree::terminate(maximalNbOfCandidateAgglomerates);
}

const bool SkyPatternTree::violationAfterAdding(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent)
{
  if (Tree::violationAfterAdding(dimensionIdOfElementsSetPresent, elementsSetPresent))
    {
      return true;
    }
  for (Measure* measure : measuresToMaximize)
    {
      if (measure->violationAfterAdding(dimensionIdOfElementsSetPresent, elementsSetPresent))
	{
	  isEnumeratedElementPotentiallyPreventingClosedness = true;
	  return true;
	}
    }
  return false;
}

const bool SkyPatternTree::violationAfterRemoving(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent)
{
  if (Tree::violationAfterRemoving(dimensionIdOfElementsSetAbsent, elementsSetAbsent))
    {
      return true;
    }
  for (Measure* measure : measuresToMaximize)
    {
      if (measure->violationAfterRemoving(dimensionIdOfElementsSetAbsent, elementsSetAbsent))
	{
	  if (!measure->monotone())
	    {
	      isEnumeratedElementPotentiallyPreventingClosedness = true;
	    }
	  return true;
	}
    }
  return false;
}

const bool SkyPatternTree::dominated()
{
  if (dominated(measuresToMaximize))
    {
      if (isSomeOptimizedMeasureNotMonotone)
	{
	  isEnumeratedElementPotentiallyPreventingClosedness = true;
	}
      return true;
    }
  return false;
}

const bool SkyPatternTree::dominated(const vector<Measure*>& measuresToMaximize)
{
  minSizeSkyline.clear();
  vector<unsigned int> minSizeMeasures;
  minSizeMeasures.reserve(nonMinSizeMeasuresIndex);
  vector<Measure*>::const_iterator measureIt = measuresToMaximize.begin();
  vector<Measure*>::const_iterator end = measureIt + nonMinSizeMeasuresIndex;
  for (; measureIt != end; ++measureIt)
    {
      minSizeMeasures.push_back((*measureIt)->optimisticValue());
    }
  vector<float> maximizedMeasures;
  maximizedMeasures.reserve(measuresToMaximize.size() - nonMinSizeMeasuresIndex);
  end = measuresToMaximize.end();
  for (; measureIt != end; ++measureIt)
    {
      maximizedMeasures.push_back((*measureIt)->optimisticValue());
    }
  for (IndistinctSkyPatterns* indistinctSkyPatterns : skyPatterns)
    {
      if (indistinctSkyPatterns->indistinctOrDominates(maximizedMeasures))
	{
	  if (indistinctSkyPatterns->minSizeIndistinctOrDominates(minSizeMeasures))
	    {
	      if (indistinctSkyPatterns->minSizeDistinct(minSizeMeasures) || indistinctSkyPatterns->distinct(maximizedMeasures))
		{
#ifdef DEBUG
		  cout << "Dominated by the measures " << *indistinctSkyPatterns << " associated with a closed " << external2InternalAttributeOrder.size() << "-set in the current skyline -> Prune!" << endl;
#endif
		  return true;
		}
	      minSizeSkyline.clear();
	      minSizeSkyline.insert(indistinctSkyPatterns);
	      return false;
	    }
	  minSizeSkyline.insert(indistinctSkyPatterns);
	}
    }
  return false;
}

#ifdef MIN_SIZE_ELEMENT_PRUNING
// TODO: same optimization as in Tree (extensively consider all patterns that barely satisfy the min area constraint)
vector<unsigned int> SkyPatternTree::minSizeIrrelevancyThresholds() const
{
 if (IndistinctSkyPatterns::noSizeOrAreaMaximized() || minSizeSkyline.empty())
    {
      return Tree::minSizeIrrelevancyThresholds();
    }
  const unsigned int n = attributes.size();
  // Compute the minimal and the maximal sizes of a pattern, the area of the minimal pattern and the minimal/maximal number of symmetric elements
  vector<unsigned int> minimalPatternSizes;
  minimalPatternSizes.reserve(n);
  vector<unsigned int> maximalPatternSizes;
  maximalPatternSizes.reserve(n);
  unsigned int minimalPatternArea = 1;
  unsigned int minNbOfSymmetricElements;
  unsigned int maxNbOfSymmetricElements;
  vector<Attribute*>::const_iterator attributeIt = attributes.begin();
  vector<unsigned int>::const_iterator minSizeIt = minSizes.begin();
  if (lastSymmetricAttributeId == 0)
    {
      minNbOfSymmetricElements = 0;
      maxNbOfSymmetricElements = 0;
      for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	{
	  const unsigned int size = max(*minSizeIt, (*attributeIt)->sizeOfPresent());
	  minimalPatternArea *= size;
	  minimalPatternSizes.push_back(size);
	  maximalPatternSizes.push_back((*attributeIt)->sizeOfPresentAndPotential());
	  ++attributeIt;
	  ++minSizeIt;
	}
    }
  else
    {
      minNbOfSymmetricElements = numeric_limits<unsigned int>::max();
      maxNbOfSymmetricElements = numeric_limits<unsigned int>::max();
      double minNbOfSymmetricElementsAccordingToArea = minArea;
      for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	{
	  unsigned int size = max(*minSizeIt, (*attributeIt)->sizeOfPresent());
	  minimalPatternArea *= size;
	  minimalPatternSizes.push_back(size);
	  size = (*attributeIt)->sizeOfPresentAndPotential();
	  if (dimensionId < firstSymmetricAttributeId || dimensionId > lastSymmetricAttributeId)
	    {
	      minNbOfSymmetricElementsAccordingToArea /= size;
	    }
	  else
	    {
	      if (size < maxNbOfSymmetricElements)
		{
		  maxNbOfSymmetricElements = size;
		}
	      if (minimalPatternSizes.back() < minNbOfSymmetricElements)
		{
		  minNbOfSymmetricElements = minimalPatternSizes.back();
		}
	    }
	  maximalPatternSizes.push_back(size);
	  ++attributeIt;
	  ++minSizeIt;
	}
      // TODO: check whether round-off errors are problematic (round instead of ceil?)
      minNbOfSymmetricElementsAccordingToArea = ceil(pow(minNbOfSymmetricElementsAccordingToArea, 1. / (lastSymmetricAttributeId - firstSymmetricAttributeId + 1)));
      if (minNbOfSymmetricElementsAccordingToArea > minNbOfSymmetricElements)
	{
	  minNbOfSymmetricElements = minNbOfSymmetricElementsAccordingToArea;
	}
      vector<unsigned int>::iterator minimalPatternSizeIt = minimalPatternSizes.begin() + firstSymmetricAttributeId;
      for (unsigned int symmetricAttributeId = firstSymmetricAttributeId; symmetricAttributeId <= lastSymmetricAttributeId; ++symmetricAttributeId)
	{
	  *minimalPatternSizeIt++ = minNbOfSymmetricElements;
	}
    }
  // Sum the epsilons with the number of non-self-loops tuples in the maximal pattern and compute the number of non-self-loops tuples in the minimal pattern
  vector<unsigned int> nonSelfLoopTuplesInMinimalPattern;
  nonSelfLoopTuplesInMinimalPattern.reserve(n);
  vector<unsigned int> thresholds(Attribute::getEpsilonVector());
  vector<unsigned int>::iterator thresholdIt = thresholds.begin();
  attributeIt = attributes.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      *thresholdIt++ += Attribute::noisePerUnit * IndistinctSkyPatterns::nbOfNonSelfLoopTuplesInHyperplaneOfPattern(maximalPatternSizes, dimensionId, maxNbOfSymmetricElements);
      // TODO: check whether round-off errors are problematic (round instead of ceil?)
      nonSelfLoopTuplesInMinimalPattern.push_back(max(static_cast<double>(IndistinctSkyPatterns::nbOfNonSelfLoopTuplesInHyperplaneOfPattern(minimalPatternSizes, dimensionId, minNbOfSymmetricElements)), ceil(minArea / ((*attributeIt)->sizeOfPresentAndPotential()))));
    }
  // Additionnaly consider the non-domination by any sky-pattern
  if (lastSymmetricAttributeId != 0)
    {
      vector<unsigned int>::iterator minimalPatternSizeIt = minimalPatternSizes.begin() + firstSymmetricAttributeId;
      for (unsigned int symmetricAttributeId = firstSymmetricAttributeId; symmetricAttributeId <= lastSymmetricAttributeId; ++symmetricAttributeId)
	{
	  *minimalPatternSizeIt++ = maxNbOfSymmetricElements;
	}
    }
  for (const IndistinctSkyPatterns* indistinctSkyPatterns : minSizeSkyline)
    {
      vector<unsigned int>::iterator nonSelfLoopTuplesInMinimalPatternIt = nonSelfLoopTuplesInMinimalPattern.begin();
      const vector<unsigned int> tmpVector = indistinctSkyPatterns->minNbOfNonSelfLoopsTuplesInANonDominatedPattern(minimalPatternSizes, maximalPatternSizes, minimalPatternArea);
      for (const unsigned int tmp : tmpVector)
	{
	  if (tmp > *nonSelfLoopTuplesInMinimalPatternIt)
	    {
	      *nonSelfLoopTuplesInMinimalPatternIt = tmp;
	    }
	  ++nonSelfLoopTuplesInMinimalPatternIt;
	}
    }
  // Subtract the number of non-self-loops tuples in the minimal pattern to get the thresholds
  thresholdIt = thresholds.begin();
  for (vector<unsigned int>::const_iterator nonSelfLoopTuplesInMinimalPatternIt = nonSelfLoopTuplesInMinimalPattern.begin(); nonSelfLoopTuplesInMinimalPatternIt != nonSelfLoopTuplesInMinimalPattern.end(); ++nonSelfLoopTuplesInMinimalPatternIt)
    {
      *thresholdIt++ -= Attribute::noisePerUnit * *nonSelfLoopTuplesInMinimalPatternIt;
    }
  return thresholds;
}
#endif

void SkyPatternTree::validPattern() const
{
  vector<unsigned int> minSizeMeasures;
  minSizeMeasures.reserve(nonMinSizeMeasuresIndex);
  vector<Measure*>::const_iterator measureIt = measuresToMaximize.begin();
  vector<Measure*>::const_iterator end = measureIt + nonMinSizeMeasuresIndex;
  for (; measureIt != end; ++measureIt)
    {
      minSizeMeasures.push_back((*measureIt)->optimisticValue());
    }
  vector<float> maximizedMeasures;
  maximizedMeasures.reserve(measuresToMaximize.size() - nonMinSizeMeasuresIndex);
  end = measuresToMaximize.end();
  for (; measureIt != end; ++measureIt)
    {
      maximizedMeasures.push_back((*measureIt)->optimisticValue());
    }
  vector<vector<unsigned int>> skyPattern;
  skyPattern.reserve(attributes.size());
  for (const Attribute* attribute : attributes)
    {
      skyPattern.push_back(attribute->getPresentAndPotentialDataIds());
    }
  vector<IndistinctSkyPatterns*>::iterator indistinctSkyPatternsIt = skyPatterns.begin();
  while (indistinctSkyPatternsIt != skyPatterns.end())
    {
      if ((*indistinctSkyPatternsIt)->minSizeIndistinctOrDominatedBy(minSizeMeasures) && (*indistinctSkyPatternsIt)->indistinctOrDominatedBy(maximizedMeasures))
	{
	  if ((*indistinctSkyPatternsIt)->minSizeDistinct(minSizeMeasures) || (*indistinctSkyPatternsIt)->distinct(maximizedMeasures))
	    {
#ifdef DEBUG
	      cout << "Removing from the current skyline the now dominated closed " << attributes.size() << "-set(s) associated with the measures " << **indistinctSkyPatternsIt << endl;
#endif
	      delete *indistinctSkyPatternsIt;
	      *indistinctSkyPatternsIt = skyPatterns.back();
	      skyPatterns.pop_back();
	    }
	  else
	    {
#ifdef DEBUG
	      cout << "Inserting the pattern in the existing class of closed " << attributes.size() << "-set(s) associated with the measures " << **indistinctSkyPatternsIt << endl;
#endif
	      (*indistinctSkyPatternsIt)->insert(skyPattern);
	      break;
	    }
	}
      else
	{
#ifdef DEBUG
	  cout << "The existing class of closed " << attributes.size() << "-set(s) associated with the measures " << **indistinctSkyPatternsIt << " is not dominated" << endl;
#endif
	  ++indistinctSkyPatternsIt;
	}
    }
  if (indistinctSkyPatternsIt == skyPatterns.end())
    {
      skyPatterns.push_back(new IndistinctSkyPatterns(skyPattern, maximizedMeasures));
#ifdef DEBUG
      cout << "Inserting the pattern in the current skyline as the first member of a new class of closed " << attributes.size() << "-set(s) associated with the measures " << *(skyPatterns.back()) << endl;
#endif
    }
  if (isIntermediateSkylinePrinted)
    {
      cout << "************************** intermediate sky-patterns **************************" << endl;
      for (IndistinctSkyPatterns* indistinctSkyPatterns : skyPatterns)
	{
	  printNSets(indistinctSkyPatterns->getPatterns(), cout);
	}
    }
}

void SkyPatternTree::printNSets(const vector<vector<vector<unsigned int>>>& nSets, ostream& out) const
{
  for (const vector<vector<unsigned int>>& nSet : nSets)
    {
      bool isFirst = true;
      for (const unsigned int internalAttributeId : external2InternalAttributeOrder)
	{
	  if (isFirst)
	    {
	      isFirst = false;
	    }
	  else
	    {
	      out << outputDimensionSeparator;
	    }
	  bool isFirstElement = true;
	  const Attribute& attribute = *attributes[internalAttributeId];
	  for (unsigned int elementId : nSet[internalAttributeId])
	    {
	      if (isFirstElement)
		{
		  isFirstElement = false;
		}
	      else
		{
		  Attribute::printOutputElementSeparator(out);
		}
	      attribute.printValueFromDataId(elementId, out);
	    }
	  if (isFirstElement)
	    {
	      Attribute::printEmptySetString(out);
	    }
	}
      if (isSizePrinted)
	{
	  out << patternSizeSeparator;
	  isFirst = true;
	  for (const unsigned int internalAttributeId : external2InternalAttributeOrder)
	    {
	      if (isFirst)
		{
		  isFirst = false;
		}
	      else
		{
		  out << sizeSeparator;
		}
	      out << nSet[internalAttributeId].size();
	    }
	}
      if (isAreaPrinted)
	{
	  unsigned int area = 1;
	  for (const vector<unsigned int>& dimension : nSet)
	    {
	      area *= dimension.size();
	    }
	  out << sizeAreaSeparator << area;
	}
      out << endl;
    }
}
