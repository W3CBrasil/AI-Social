// Copyright 2007-2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include <iostream>
#include <limits>
#include <fstream>

#include "Tree.h"
#include "TupleFileReader.h"
#include "TupleFileWriter.h"
#include "EmptyItemset.h"
#include "ExAnte.h"

#if ENUMERATION_PROCESS == 1 || ENUMERATION_PROCESS == 2
map<const pair<const unsigned int, const unsigned int>, const float> Tree::itemsAppeals;
#endif

#ifdef DETAILED_TIME
clock_t Tree::parsingTime = 0;
clock_t Tree::miningTime = 0;
clock_t Tree::propagatingTime = 0;
clock_t Tree::maximalityCheckingTime = 0;
clock_t Tree::closureOperationTime = 0;
clock_t Tree::postProcessingTime = 0;
std::chrono::high_resolution_clock::duration Tree::preProcessingTime;
#endif
#ifdef NB_OF_NODES
unsigned int Tree::nbOfNodes = 0;
#endif
#ifdef NB_OF_CLOSED_N_SETS
unsigned int Tree::nbOfClosedNSets = 0;
#endif
#ifdef PRUNING_DETAILS
vector<bool> Tree::nbOfPrunesByMaximalityConstraint;
vector<bool> Tree::nbOfPrunesByMinimalSizeConstraint;
vector<bool> Tree::nbOfPrunesByMaximalSizeConstraint;
vector<bool> Tree::nbOfPrunesByMinimalAreaConstraint;
vector<bool> Tree::nbOfPrunesByMaximalAreaConstraint;
vector<bool> Tree::nbOfPrunesByMinimizationRatio;
#endif
#ifdef AVERAGE_NB_OF_ITEMS_ABSENT_BY_PROPAGATION
unsigned int Tree::nbOfPropagations;
unsigned int Tree::nbOfItemsAbsentByPropagation;
#endif
// #ifdef PROPORTION_OF_ABSENT_ITEMS_CHECKED_FOR_MAXIMALITY
// unsigned int Tree::nbOfItemsCheckedForMaximality = 0;
// unsigned int Tree::nbOfAbsentItemsInDimensionCheckedForMaximality = 0;
// #endif

vector<unsigned int> Tree::dimensionOrder;
vector<unsigned int> Tree::reverseDimensionOrder;
vector<unsigned int> Tree::nbOfElementsInDiInFiles;
vector<unsigned int> Tree::nbOfElementsInDi;
vector<float> Tree::deltaVector;
vector<unsigned int> Tree::minSizes;
vector<unsigned int> Tree::maxSizes;
float Tree::minArea;
float Tree::maxArea;
vector<float> Tree::areaCoefficients;
float Tree::minimizationRatio;
vector<unsigned int> Tree::cliqueDimensions;
AbstractHyperBitset* Tree::lastDimensionEmptyBitset;
AbstractHyperBitset* Tree::data;
unsigned int Tree::n = 0;
vector<map<const unsigned int, const string> > Tree::ids2Labels;

bool Tree::isAMinimization;
ofstream Tree::outputFile;
const char* Tree::outputItemSeparator;
const char* Tree::outputDimensionSeparator;
const char* Tree::patternSizeSeparator;
const char* Tree::sizeSeparator;
const char* Tree::sizeAreaSeparator;
const char* Tree::emptySetString;
bool Tree::isSizePrinted;
bool Tree::isAreaPrinted;

vector<vector<unsigned int> > Tree::present;
vector<unsigned int> Tree::sizes;
multiset<NRectangle> Tree::nRectangles;

namespace {
    bool decreasingOrder(const pair<unsigned int, unsigned int> labeledHyperBitset1, const pair<unsigned int, unsigned int> labeledHyperBitset2)
    {
        return labeledHyperBitset1.second > labeledHyperBitset2.second;
    }

    vector<vector<unsigned int>>
    AssociatedSelfLoops(const vector<unsigned int>& tuple, const vector<unsigned int>& cliqueDimensions)
    {
        vector<vector<unsigned int>> selfLoops(cliqueDimensions.size());

        for (unsigned i = 0; i < cliqueDimensions.size(); i++) {
            // Make the vertices in all symmetric dimensions equal to the vertex in the i-th dimension
            vector<unsigned int>& selfLoop = selfLoops[i];
            selfLoop.resize(tuple.size());

            unsigned vertexId = tuple[cliqueDimensions[i]];
            unsigned symmetricDimensionIndex = 0;

            for (unsigned j = 0; j < tuple.size(); j++) {
                if (symmetricDimensionIndex < cliqueDimensions.size() &&
                        cliqueDimensions[symmetricDimensionIndex] == j) {
                    selfLoop[j] = vertexId;
                    symmetricDimensionIndex++;
                } else {
                    selfLoop[j] = tuple[j];
                }
            }
        }

        return selfLoops;
    }

}

Tree::Tree(): potential(), absent(), groups()
{
}

Tree::Tree(const char* dataFileName, const char* dontCareSetFileName, const vector<float>& deltaVectorParam, const vector<unsigned int>& minSizesParam, const vector<unsigned int>& maxSizesParam, const float minAreaParam, const float maxAreaParam, const vector<float>& areaCoefficientsParam, const float minimizationRatioParam, const vector<string>& groupFileNames, const vector<unsigned int>& groupMinSizes, const vector<unsigned int>& groupMaxSizes, const char* groupMinRatiosFileName, const vector<unsigned int>& cliqueDimensionsParam, const char* inputItemSeparator, const char* inputDimensionSeparator, const char* groupItemSeparator, const char* groupDimensionItemsSeparator, const char* outputFileName, const char* outputItemSeparatorParam, const char* outputDimensionSeparatorParam, const char* patternSizeSeparatorParam, const char* sizeSeparatorParam, const char* sizeAreaSeparatorParam, const char* emptySetStringParam, Tree::Flags flags) throw(UsageException, DataFormatException, NoFileException): potential(), absent(), groups()
{
#ifdef TIME
  clock_t overallStart = clock();
#endif

  isSizePrinted = flags & Tree::IS_SIZE_PRINTED;
  isAreaPrinted = flags & Tree::IS_AREA_PRINTED;
  isAMinimization = flags & Tree::IS_MINIMIZATION;

  bool shouldReduce = !(flags & Tree::NO_DATASET_REDUCTION);
  bool isReductionOnly = flags & Tree::DATASET_REDUCTION_ONLY;

  outputItemSeparator = outputItemSeparatorParam;
  outputDimensionSeparator = outputDimensionSeparatorParam;
  patternSizeSeparator = patternSizeSeparatorParam;
  sizeSeparator = sizeSeparatorParam;
  sizeAreaSeparator = sizeAreaSeparatorParam;
  emptySetString = emptySetStringParam;
  minimizationRatio = minimizationRatioParam;
  minArea = minAreaParam;
  maxArea = maxAreaParam;
#ifdef DETAILED_TIME
  clock_t start = clock();
  std::chrono::high_resolution_clock detailedTimeClock;
#endif

  vector<map<const string, const unsigned int>> labels2Ids;

  vector<unsigned int> sortedCliqueDimensions(cliqueDimensionsParam.begin(),
                                              cliqueDimensionsParam.end());

  sort(sortedCliqueDimensions.begin(), sortedCliqueDimensions.end());

  TupleFileReader reader(sortedCliqueDimensions,
                         inputDimensionSeparator,
                         inputItemSeparator,
                         labels2Ids);

  // Parsing the data file
  auto tuples = parse(reader, dataFileName);
  n = tuples.size() > 0 ? tuples.front().size() : 0;

  reader.copySymmetricDimensionsLabels2Ids();

#ifdef DETAILED_TIME
  parsingTime += clock() - start;
#endif

  if (isAMinimization && !string(dontCareSetFileName).empty())
  {
      // Parsing the don't care set
      auto additionalTuples = parse(reader, dontCareSetFileName);
      tuples.insert(tuples.end(), additionalTuples.begin(), additionalTuples.end());
  }

  if (shouldReduce || isReductionOnly)
  {

#ifdef DETAILED_TIME
  std::chrono::time_point<std::chrono::high_resolution_clock> beforePreProcessing = detailedTimeClock.now();
#endif

      tuples = ExAntePreprocess(tuples, minSizesParam, sortedCliqueDimensions);

#ifdef DETAILED_TIME
      Tree::preProcessingTime = detailedTimeClock.now() - beforePreProcessing;
#endif

      ReassignIdentifiers(tuples, labels2Ids, sortedCliqueDimensions);
  }

  // Initializing ids2Labels
  ids2Labels.resize(n);

  vector<map<const unsigned int, const string> >::iterator ids2LabelsIt = ids2Labels.begin();
  for (vector<map<const string, const unsigned int> >::const_iterator labels2IdsIt = labels2Ids.begin(); labels2IdsIt != labels2Ids.end(); ++labels2IdsIt)
    {
      for (map<const string, const unsigned int>::const_iterator name2IdIt = labels2IdsIt->begin(); name2IdIt != labels2IdsIt->end(); ++name2IdIt)
        {
            if (name2IdIt->second != std::numeric_limits<unsigned int>::max()) {
                ids2LabelsIt->insert(pair<const unsigned int, const string>(name2IdIt->second, name2IdIt->first));
            }
        }
      ++ids2LabelsIt;
    }

  if (isReductionOnly)
  {
      TupleFileWriter writer(outputFileName, ids2Labels, outputDimensionSeparatorParam);
      writer.write(tuples);
      return;
  }

  nbOfElementsInDiInFiles.resize(n, 0);

  for (unsigned i = 0; i < n; i++)
      nbOfElementsInDiInFiles[i] = ids2Labels[i].size();

  // Reordering the dimensions to store the data in a good way (increasing order of cardinals)
  vector<pair<unsigned int, unsigned int> > labelledDimensions;
  labelledDimensions.reserve(n);
  vector<unsigned int>::const_iterator nbOfElementsInDiInFilesIt = nbOfElementsInDiInFiles.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; )
    {
      labelledDimensions.push_back(pair<unsigned int, unsigned int>(*nbOfElementsInDiInFilesIt++, dimensionId++));
    }
#ifdef SORT_DIMENSIONS
  sort(labelledDimensions.begin(), labelledDimensions.end());
#endif
  reverseDimensionOrder.reserve(n);
  dimensionOrder = vector<unsigned int>(n);
  sizes.reserve(n);
  vector<pair<unsigned int, unsigned int> >::const_iterator labelledDimensionIt = labelledDimensions.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      sizes.push_back(labelledDimensionIt->first);
      unsigned int dimensionIdInFile = labelledDimensionIt->second;
      dimensionOrder[dimensionIdInFile] = dimensionId;
      reverseDimensionOrder.push_back(dimensionIdInFile);
      ++labelledDimensionIt;
    }

#ifdef DETAILED_TIME
  start = clock();
#endif

  // Initializing groups
  groups = Groups(groupFileNames, groupItemSeparator, groupDimensionItemsSeparator, labels2Ids, dimensionOrder, groupMinSizes, groupMaxSizes, groupMinRatiosFileName);

#ifdef DETAILED_TIME
  parsingTime += clock() - start;
#endif
  nbOfElementsInDi.reserve(n);
  vector<unsigned int>::const_iterator reverseDimensionOrderIt = reverseDimensionOrder.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      nbOfElementsInDi.push_back(nbOfElementsInDiInFiles[*reverseDimensionOrderIt++]);
    }
  cliqueDimensions.reserve(cliqueDimensionsParam.size());
  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensionsParam.begin(); cliqueDimensionIt != cliqueDimensionsParam.end(); )
    {
      cliqueDimensions.push_back(reverseDimensionOrder[*cliqueDimensionIt++]);
    }
  sort(cliqueDimensions.begin(), cliqueDimensions.end());
  if (areaCoefficientsParam.empty())
    {
      areaCoefficients.resize(n, 1);
    }
  else
    {
      if (areaCoefficientsParam.size() != n)
	{
	  throw UsageException(("ac option should provide " + lexical_cast<string>(n) + " coefficients!").c_str());
	}
      areaCoefficients.reserve(n);
      vector<unsigned int>::const_iterator reverseDimensionOrderIt = reverseDimensionOrder.begin();
      for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	{
	  areaCoefficients.push_back(areaCoefficientsParam[*reverseDimensionOrderIt++]);
	}
    }
  if (minSizesParam.empty())
    {
      minSizes.resize(n, 0);
    }
  else
    {
      if (minSizesParam.size() != n)
	{
	  throw UsageException(("sizes option should provide " + lexical_cast<string>(n) + " sizes!").c_str());
	}
      minSizes.reserve(n);
      reverseDimensionOrderIt = reverseDimensionOrder.begin();
      for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	{
	  minSizes.push_back(minSizesParam[*reverseDimensionOrderIt++]);
	}
      if (!cliqueDimensions.empty())
	{
	  unsigned int max = 0;
	  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); ++cliqueDimensionIt)
	    {
	      unsigned int minSize = minSizes[*cliqueDimensionIt];
	      if (minSize > max)
		{
		  max = minSize;
		}
	    }
	  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); ++cliqueDimensionIt)
	    {
	      minSizes[*cliqueDimensionIt] = max;
	    }
	}
    }
  if (maxSizesParam.empty())
    {
      maxSizes = sizes;
    }
  else
    {
      if (maxSizesParam.size() != n)
	{
	  throw UsageException(("Sizes option should provide " + lexical_cast<string>(n) + " sizes!").c_str());
	}
      maxSizes.reserve(n);
      reverseDimensionOrderIt = reverseDimensionOrder.begin();
      for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	{
	  maxSizes.push_back(maxSizesParam[*reverseDimensionOrderIt++]);
	}
      if (!cliqueDimensions.empty())
	{
	  unsigned int min = UINT_MAX;
	  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); ++cliqueDimensionIt)
	    {
	      unsigned int maxSize = maxSizes[*cliqueDimensionIt];
	      if (maxSize < min)
		{
		  min = maxSize;
		}
	    }
	  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); ++cliqueDimensionIt)
	    {
	      maxSizes[*cliqueDimensionIt] = min;
	    }
	}
    }
  if (maxArea == -1)
    {
      maxArea = 1;
      vector<float>::const_iterator areaCoefficientIt = areaCoefficients.begin();
      for (vector<unsigned int>::const_iterator maxSizeIt = maxSizes.begin(); maxSizeIt != maxSizes.end(); ++maxSizeIt)
	{
	  maxArea *= pow(*maxSizeIt, *areaCoefficientIt++);
	}
    }
  if (isAMinimization)
    {
      vector<unsigned int>::iterator minFrequencyIt = minSizes.begin();
      for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	{
	  *minFrequencyIt = max(*minFrequencyIt++, static_cast<unsigned int>(1));
	}
    }
  if (deltaVectorParam.empty())
    {
      deltaVector.resize(n, 0);
    }
  else
    {
      if (deltaVectorParam.size() != n)
	{
	  throw UsageException(("delta option should provide " + lexical_cast<string>(n) + " coefficients!").c_str());
	}
      deltaVector.reserve(n);
      vector<unsigned int>::const_iterator reverseDimensionOrderIt = reverseDimensionOrder.begin();
      for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	{
	  deltaVector.push_back(deltaVectorParam[*reverseDimensionOrderIt++]);
	}
      if (!cliqueDimensions.empty())
	{
	  float max = 0;
	  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); )
	    {
	      float delta = deltaVector[*cliqueDimensionIt++];
	      if (delta > max)
		{
		  max = delta;
		}
	    }
	  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); )
	    {
	      deltaVector[*cliqueDimensionIt++] = max;
	    }
	}
    }

  lastDimensionEmptyBitset = new EmptyItemset(sizes.back());

  // Initializing data
  data = new HyperBitset(sizes, false, lastDimensionEmptyBitset);

  // Insert tuples from dataset into `data`
  vector<unsigned int> reorderedTuple(n);

  for (const auto& t : tuples)
  {
      for (unsigned i = 0; i < n; i++)
          reorderedTuple[i] = t[reverseDimensionOrder[i]];

      data->setValue(reorderedTuple, true);

      for (const auto& sl : AssociatedSelfLoops(t, sortedCliqueDimensions)) {
          for (unsigned i = 0; i < n; i++)
              reorderedTuple[i] = sl[reverseDimensionOrder[i]];

          data->setValue(reorderedTuple, true);
      }
  }

  // Insert self-loops if we're mining cliques
  setSelfLoops(sortedCliqueDimensions);

  present.resize(n);
  absent.resize(n);
  // Initializing potential (with sorted elements)
#if ENUMERATION_PROCESS == 1 || ENUMERATION_PROCESS == 2
  vector<vector<pair<unsigned int, float> > > densities(n);
  vector<vector<pair<unsigned int, float> > >::iterator dimensionDensitiesIt = densities.begin();
#endif
  unsigned int dataSetSize = 1;
  vector<vector<unsigned int> > projectionOnLabel(n);
  vector<vector<unsigned int> >::iterator projectionOnLabelIt = projectionOnLabel.begin();
  vector<unsigned int>::const_iterator sizeIt = sizes.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      for (unsigned int elementId = 0; elementId != *sizeIt; )
	{
	  projectionOnLabelIt->push_back(elementId++);
	}
      dataSetSize *= *sizeIt++;
      ++projectionOnLabelIt;
    }
  sizeIt = sizes.begin();
  projectionOnLabelIt = projectionOnLabel.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      const vector<unsigned int> projectionOnLabelCopy = *projectionOnLabelIt;
      projectionOnLabelIt->clear();
      vector<pair<unsigned int, unsigned int> > labeledDataView;
      const unsigned int& nbOfElementsInDimension = *sizeIt++;
      labeledDataView.reserve(nbOfElementsInDimension);
      for (unsigned int label = 0; label != nbOfElementsInDimension; )
	{
	  projectionOnLabelIt->push_back(label);
	  labeledDataView.push_back(pair<unsigned int, unsigned int>(label++, data->countOn(projectionOnLabel)));
	  projectionOnLabelIt->pop_back();
	}
      *projectionOnLabelIt = projectionOnLabelCopy;
#if ENUMERATION_PROCESS == 1 || ENUMERATION_PROCESS == 2
      const unsigned int dimensionSize = dataSetSize / nbOfElementsInDimension;
      for (vector<pair<unsigned int, unsigned int> >::iterator dataHyperPlanIt = labeledDataView.begin(); dataHyperPlanIt != labeledDataView.end(); ++dataHyperPlanIt)
	{
#if ENUMERATION_PROCESS == 1
	  itemsAppeals.insert(itemsAppeals.end(), pair<const pair<const unsigned int, const unsigned int>, const float>(pair<const unsigned int, const unsigned int>(dimensionId, dataHyperPlanIt->first), 1 - static_cast<const float>(dataHyperPlanIt->second) / static_cast<const float>(dimensionSize)));
#else
	  itemsAppeals.insert(itemsAppeals.end(), pair<const pair<const unsigned int, const unsigned int>, const float>(pair<const unsigned int, const unsigned int>(dimensionId, dataHyperPlanIt->first), static_cast<const float>(dimensionSize - dataHyperPlanIt->second)));
#endif
	}
#endif
#ifdef SORT_ITEMS
      sort(labeledDataView.begin(), labeledDataView.end(), decreasingOrder);
#endif
      vector<unsigned int> potentialItems;
      potentialItems.reserve(n);
      for (vector<pair<unsigned int, unsigned int> >::const_iterator orderedDataHyperPlanIt = labeledDataView.begin(); orderedDataHyperPlanIt != labeledDataView.end(); ++orderedDataHyperPlanIt)
	{
	  potentialItems.push_back(orderedDataHyperPlanIt->first);
	}
      potential.push_back(potentialItems);
      ++projectionOnLabelIt;
    }
  outputFile.open(outputFileName);
  if (outputFile.fail())
    {
      throw NoFileException(outputFileName);
    }
#ifdef PRUNING_DETAILS
  // Initializing the nbOfPrunes;
  const unsigned int nbOfElementsInAllDimensions = nbOfPotentialElements();
  nbOfPrunesByMaximalityConstraint.resize(nbOfElementsInAllDimensions, false);
  nbOfPrunesByMinimalSizeConstraint.resize(nbOfElementsInAllDimensions, false);
  nbOfPrunesByMaximalSizeConstraint.resize(nbOfElementsInAllDimensions, false);
  nbOfPrunesByMinimalAreaConstraint.resize(nbOfElementsInAllDimensions, false);
  nbOfPrunesByMaximalAreaConstraint.resize(nbOfElementsInAllDimensions, false);
  nbOfPrunesByMinimizationRatio.resize(nbOfElementsInAllDimensions, false);
#endif
#ifdef DETAILED_TIME
  start = clock();
#endif

  // Mining
  if (tuples.size())
    peel(0);
#ifdef DETAILED_TIME
  miningTime += clock() - start;
#endif
  if (isAMinimization)
    {
      // Minimizing
      if (!string(dontCareSetFileName).empty())
	{
#ifdef DETAILED_TIME
	  clock_t start = clock();
#endif
	  // Removing 1s from the don't care set
	  auto dontCareTuples = parse(reader, dontCareSetFileName);

          for (const auto& t : dontCareTuples)
              data->setValue(t, false);

#ifdef DETAILED_TIME
	  parsingTime += clock() - start;
#endif
	}
#ifdef DETAILED_TIME
      clock_t start = clock();
#endif
      const unsigned int& nbOfElementsInLastDimension = sizes.back();
      float maximalCoverRateOfUnidimensionalNRectangles = 0;
      bool isNRectanglesNotContainingUnidimensionalOnes;
      const float coverRateOfUnidimensionalNRectanglesCoveringAllDn = static_cast<float>(nbOfElementsInLastDimension) / static_cast<float>(n);
      if (coverRateOfUnidimensionalNRectanglesCoveringAllDn < minimizationRatio)
	{
	  isNRectanglesNotContainingUnidimensionalOnes = false;
	  insertUnidimensionalNRectangles();
	}
      else
	{
	  isNRectanglesNotContainingUnidimensionalOnes = true;
	  maximalCoverRateOfUnidimensionalNRectangles = static_cast<float>(nbOfElementsInLastDimension - 1) / static_cast<float>(nbOfElementsInLastDimension + n - 2);
	}
      while (!(nRectangles.empty()))
	{
	  multiset<NRectangle>::iterator firstNRectangleIt = nRectangles.begin();
	  NRectangle firstNRectangle = *firstNRectangleIt;
#ifdef DEBUG_MINIMIZATION
	  cout << "Tile before update: " << firstNRectangle << endl;
#endif
	  if (firstNRectangle.update())
	    {
#ifdef DEBUG_MINIMIZATION
	      cout << "Modified when updated: " << firstNRectangle;
#endif
	      if (isNRectanglesNotContainingUnidimensionalOnes)
		{
		  if (firstNRectangle.multidimensional())
		    {
#ifdef DEBUG_MINIMIZATION
		      cout << " -> Reinsert among tiles" << endl;
#endif
		      nRectangles.insert(firstNRectangle);
		    }
#ifdef DEBUG_MINIMIZATION
		  else
		    {
		      cout << " -> Drop" << endl;
		    }
#endif
		}
	      else
		{
		  if (!firstNRectangle.empty())
		    {
#ifdef DEBUG_MINIMIZATION
		      cout << " -> Reinsert among tiles" << endl;
#endif
		      nRectangles.insert(firstNRectangle);
		    }
#ifdef DEBUG_MINIMIZATION
		  else
		    {
		      cout << " -> Drop" << endl;
		    }
#endif
		}
	    }
	  else
	    {
#ifdef DEBUG_MINIMIZATION
	      cout << "Identical when updated: " << firstNRectangle;
#endif
	      if (isNRectanglesNotContainingUnidimensionalOnes && firstNRectangle.coverRate() <= maximalCoverRateOfUnidimensionalNRectangles)
		{
#ifdef DEBUG_MINIMIZATION
		  cout << " -> Reinsert among tiles and add every unidimensional tile" << endl;
#endif
		  nRectangles.insert(firstNRectangle);
		  insertUnidimensionalNRectangles();
		  isNRectanglesNotContainingUnidimensionalOnes = false;
		}
	      else
		{
#ifdef DEBUG_MINIMIZATION
		  cout << " -> Output" << endl;
#endif
		  firstNRectangle.resetValues();
#ifdef OUTPUT
		  outputFile << firstNRectangle << endl;
#endif
		}
	    }
	  nRectangles.erase(firstNRectangleIt);
	}
      if (isNRectanglesNotContainingUnidimensionalOnes)
	{
	  vector<vector<vector<unsigned int> > > remainingUnidimensionalNRectangles = data->unidimensionalNRectangles();
	  while (!remainingUnidimensionalNRectangles.empty())
	    {
	      outputFile << NRectangle(remainingUnidimensionalNRectangles.back()) << endl;
	      remainingUnidimensionalNRectangles.pop_back();
	    }
	}
#ifdef DETAILED_TIME
      postProcessingTime += clock() - start;
#endif
    }
  outputFile.close();
  delete data;
  delete lastDimensionEmptyBitset;
#ifdef NB_OF_CLOSED_N_SETS
#ifdef GNUPLOT
  cout << nbOfClosedNSets << '\t';
#else
  cout << "Nb of closed " << n << "-sets: " << nbOfClosedNSets << endl;
#endif
#endif
#ifdef NB_OF_NODES
#ifdef GNUPLOT
  cout << nbOfNodes << '\t';
#else
  cout << "Nb of valid nodes: " << nbOfNodes << endl;
#endif
#endif
#ifdef PRUNING_DETAILS
#ifdef GNUPLOT
  printNbOfPrunes(nbOfPrunesByMaximalityConstraint);
  cout << '\t';
  printNbOfPrunes(nbOfPrunesByMinimalSizeConstraint);
  cout << '\t';
  printNbOfPrunes(nbOfPrunesByMaximalSizeConstraint);
  cout << '\t';
  printNbOfPrunes(nbOfPrunesByMinimalAreaConstraint);
  cout << '\t';
  printNbOfPrunes(nbOfPrunesByMaximalAreaConstraint);
  cout << '\t';
  printNbOfPrunes(nbOfPrunesByMinimizationRatio);
  cout << '\t';
#else
  cout << "Binary logarithm of the nb of nodes pruned for non-maximality: ";
  printNbOfPrunes(nbOfPrunesByMaximalityConstraint);
  cout << endl << "Binary logarithm of the nb of nodes pruned for unsatisfied minimal size constrainst: ";
  printNbOfPrunes(nbOfPrunesByMinimalSizeConstraint);
  cout << endl << "Binary logarithm of the nb of nodes pruned for unsatisfied maximal size constrainst: ";
  printNbOfPrunes(nbOfPrunesByMaximalSizeConstraint);
  cout << endl << "Binary logarithm of the nb of nodes pruned for unsatisfied minimal area constraint: ";
  printNbOfPrunes(nbOfPrunesByMinimalAreaConstraint);
  cout << endl << "Binary logarithm of the nb of nodes pruned for unsatisfied maximal area constraint: ";
  printNbOfPrunes(nbOfPrunesByMaximalAreaConstraint);
  cout << endl << "Binary logarithm of the nb of nodes pruned for unreached minimization ratio: ";
  printNbOfPrunes(nbOfPrunesByMinimizationRatio);
  cout << endl;
#endif
#endif
#ifdef AVERAGE_NB_OF_ITEMS_ABSENT_BY_PROPAGATION
#ifdef GNUPLOT
  cout << static_cast<float>(nbOfItemsAbsentByPropagation) / static_cast<float>(nbOfPropagations) << '\t';
#else
  cout << "Average nb of elements absent by propagation: " << static_cast<float>(nbOfItemsAbsentByPropagation) / static_cast<float>(nbOfPropagations) << endl;
#endif
#endif
// #ifdef PROPORTION_OF_ABSENT_ITEMS_CHECKED_FOR_MAXIMALITY
// #ifdef GNUPLOT
//   cout << static_cast<float>(nbOfItemsCheckedForMaximality) / static_cast<float>(nbOfAbsentItemsInDimensionCheckedForMaximality) << '\t';
// #else
//   cout << "Proportion of absent items checked for maximality: " << static_cast<float>(nbOfItemsCheckedForMaximality) / static_cast<float>(nbOfAbsentItemsInDimensionCheckedForMaximality) << endl;
// #endif
// #endif
#ifdef TIME
#ifdef GNUPLOT
  cout << (clock() - overallStart) / static_cast<float>(CLOCKS_PER_SEC) << '\t';
#else
  cout << "Total time: " << (clock() - overallStart) / static_cast<float>(CLOCKS_PER_SEC) << 's' << endl;
#endif
#endif
#ifdef DETAILED_TIME
#ifdef GNUPLOT
  cout << parsingTime / static_cast<float>(CLOCKS_PER_SEC) << '\t'
       << (std::chrono::duration_cast<std::chrono::milliseconds>(preProcessingTime).count() / 1000.0) << '\t'
       << miningTime / static_cast<float>(CLOCKS_PER_SEC) << '\t';
  if (isAMinimization)
    {
      cout << postProcessingTime / static_cast<float>(CLOCKS_PER_SEC) << '\t';
    }
  cout << maximalityCheckingTime / static_cast<float>(CLOCKS_PER_SEC) << '\t' << propagatingTime / static_cast<float>(CLOCKS_PER_SEC);
#else
  cout << "Parsing time: " << parsingTime / static_cast<float>(CLOCKS_PER_SEC) << 's' << endl
       << "Pre-processing time: "
            << (std::chrono::duration_cast<std::chrono::milliseconds>(preProcessingTime).count() / 1000.0)
            << "s\n"
       << "Mining time: " << miningTime / static_cast<float>(CLOCKS_PER_SEC) << 's' << endl;
  if (isAMinimization)
    {
      cout << "Minimizing time: " << postProcessingTime / static_cast<float>(CLOCKS_PER_SEC) << 's' << endl;
    }
  cout << "Time spent checking maximality: " << maximalityCheckingTime / static_cast<float>(CLOCKS_PER_SEC) << 's' << endl << "Time spent propagating: " << propagatingTime / static_cast<float>(CLOCKS_PER_SEC) << 's' << endl;
#endif
#endif
#ifdef GNUPLOT
  cout << endl;
#endif
}

// Constructor of a left subtree
Tree::Tree(const Tree& father, const unsigned int dimensionToPeelId, const unsigned int itemToPeelId, const bool isDimensionToPeelInTheClique): potential(father.potential), absent(father.absent), groups(father.groups)
{
  vector<unsigned int>& presentView = present[dimensionToPeelId];
  const vector<unsigned int> oldPresentCardinals = presentCardinals();
  if (isDimensionToPeelInTheClique)
    {
      for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); ++cliqueDimensionIt)
	{
	  present[*cliqueDimensionIt].push_back(itemToPeelId);
	  groups.setPresent(*cliqueDimensionIt, itemToPeelId);
	}
    }
  else
    {
      presentView.push_back(itemToPeelId);
      groups.setPresent(dimensionToPeelId, itemToPeelId);
    }
#ifdef DETAILED_TIME
  clock_t start = clock();
#endif
  const vector<unsigned int> presentViewCopy = presentView;
  presentView.clear();
  presentView.push_back(itemToPeelId);
  vector<vector<unsigned int> > elementsAbsentByPropagation = data->setPresent(present, potential);
  presentView = presentViewCopy;
#ifdef DETAILED_TIME
  propagatingTime += clock() - start;
#endif
  if (isDimensionToPeelInTheClique)
    {
      const unsigned int& otherDimensionInTheCliqueId = cliqueDimensions.back();
      potential[dimensionToPeelId] = potential[otherDimensionInTheCliqueId];
      elementsAbsentByPropagation[dimensionToPeelId] = elementsAbsentByPropagation[otherDimensionInTheCliqueId];
    }
#ifdef AVERAGE_NB_OF_ITEMS_ABSENT_BY_PROPAGATION
  ++nbOfPropagations;
  for (vector<unsigned int>::const_iterator elementsAbsentByPropagationIt = elementsAbsentByPropagation.begin(); elementsAbsentByPropagationIt != elementsAbsentByPropagation.end(); ++elementsAbsentByPropagationIt)
    {
      nbOfItemsAbsentByPropagation += elementsAbsentByPropagationIt->size();
    }
#endif
#ifdef DEBUG_EXTRACTION
  cout << "Elements absent by propagation: ";
  printNVector(elementsAbsentByPropagation);
  cout << endl;
#endif
  float area = 1;
  vector<float>::const_iterator areaCoefficientIt = areaCoefficients.begin();
  vector<unsigned int>::const_iterator minSizeIt = minSizes.begin();
  vector<unsigned int> newSizes = sizes;
  vector<unsigned int>::iterator sizeIt = newSizes.begin();
  vector<vector<unsigned int> >::iterator absentIt = absent.begin();
  vector<float>::const_iterator deltaIt = deltaVector.begin();
  vector<vector<unsigned int> >::const_iterator elementsAbsentByPropagationIt = elementsAbsentByPropagation.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      if (!elementsAbsentByPropagationIt->empty())
	{
	  *sizeIt -= elementsAbsentByPropagationIt->size();
	  if (*sizeIt < *minSizeIt)
	    {
#ifdef PRUNING_DETAILS
	      incrementNbOfPrunes(nbOfPrunesByMinimalSizeConstraint, nbOfPotentialElements());
#endif
#ifdef DEBUG_EXTRACTION
	      cout << "After propagation, the minimal size constraint is not satisfied on dimension " << reverseDimensionOrder[dimensionId] << " -> Pruned!" << endl;
#endif
	      if (isDimensionToPeelInTheClique)
		{
		  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); )
		    {
		      present[*cliqueDimensionIt++].pop_back();
		    }
		}
	      else
		{
		  presentView.pop_back();
		}
	      return;
	    }
	  if (*deltaIt != 0)
	    {
	      vector<unsigned int>::const_iterator dimensionInTheCliqueIt = find(cliqueDimensions.begin(), cliqueDimensions.end(), dimensionId);
	      if (dimensionInTheCliqueIt == cliqueDimensions.end() || dimensionInTheCliqueIt == cliqueDimensions.begin())
		{
		  for (vector<unsigned int>::const_iterator elementAbsentByPropagationIt = elementsAbsentByPropagationIt->begin(); elementAbsentByPropagationIt != elementsAbsentByPropagationIt->end(); )
		    {
		      absentIt->push_back(*elementAbsentByPropagationIt++);
		    }
		}
	    }
	}
      area *= pow(*sizeIt++, *areaCoefficientIt++);
      ++elementsAbsentByPropagationIt;
      ++deltaIt;
      ++absentIt;
      ++minSizeIt;
    }
  if (area >= minArea)
    {
      sizes = newSizes;
      removeIrrelevantElementsFromAbsent(dimensionToPeelId, vector<unsigned int>(1, itemToPeelId), isDimensionToPeelInTheClique);
      groups.setAbsent(elementsAbsentByPropagation);
      fetchElementsInClosureOnEveryDimension(isDimensionToPeelInTheClique);
#ifdef DEBUG_EXTRACTION
      cout << "Node after:" << endl << *this << endl;
#endif
      if (constraintsSatisfied())
	{
	  if (deltaMaximalOnEveryDimension())
	    {
	      peel(nextDimension());
	    }
	}
      keepPresentPrefixes(oldPresentCardinals);
      vector<vector<unsigned int> >::const_iterator elementsAbsentByPropagationIt = elementsAbsentByPropagation.begin();
      sizeIt = sizes.begin();
      for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	{
	  *sizeIt += elementsAbsentByPropagationIt->size();
	  ++elementsAbsentByPropagationIt;
	  ++sizeIt;
	}
    }
  else
    {
#ifdef PRUNING_DETAILS
      incrementNbOfPrunes(nbOfPrunesByMinimalAreaConstraint, nbOfPotentialElements());
#ifdef DEBUG_EXTRACTION
      cout << "After propagation, the minimal area constraint is not satisfied -> Pruned!" << endl;
#endif
#else
#ifdef DEBUG_EXTRACTION
      cout << "After propagation, the minimal area constraint is not satisfied -> Pruned!" << endl;
#endif
#endif
      if (isDimensionToPeelInTheClique)
	{
	  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); )
	    {
	      present[*cliqueDimensionIt++].pop_back();
	    }
	}
      else
	{
	  presentView.pop_back();
	}
    }
}

Tree::~Tree()
{
}

void Tree::printDimension(const vector<unsigned int>& dimension, const map<const unsigned int, const string>& dimensionIds2Labels, ostream& out)
{
  bool isFirstItem = true;
  for (vector<unsigned int>::const_iterator dimensionIt = dimension.begin(); dimensionIt != dimension.end(); )
    {
      if (isFirstItem)
	{
	  isFirstItem = false;
	}
      else
	{
	  out << outputItemSeparator;
	}

      out << dimensionIds2Labels.find(*dimensionIt++)->second;
    }
  if (isFirstItem)
    {
      out << emptySetString;
    }
}

void Tree::printNVector(const vector<vector<unsigned int> >& nVector, ostream& out)
{
  vector<map<const unsigned int, const string> >::const_iterator ids2LabelsIt = ids2Labels.begin();
  vector<unsigned int>::const_iterator nbOfElementsInDiInFilesIt = nbOfElementsInDiInFiles.begin();
  vector<unsigned int>::const_iterator dimensionOrderIt = dimensionOrder.begin();
  bool isFirstDimension = true;
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      if (isFirstDimension)
	{
	  isFirstDimension = false;
	}
      else
	{
	  out << outputDimensionSeparator;
	}
      printDimension(nVector[*dimensionOrderIt++], *ids2LabelsIt, out);
      ++ids2LabelsIt;
    }
}

vector<vector<unsigned int>>
Tree::parse(TupleFileReader& reader,
            const char* dataFileName)
{
  reader.start(dataFileName);

  vector<vector<unsigned>> tuples;

  for (vector<unsigned int> tuple = reader.next();
       tuple.size();
       tuple = reader.next())
  {
      tuples.push_back(tuple);
  }

  return tuples;
}

void Tree::goodLeaf() const
{
#ifdef DEBUG_EXTRACTION
  cout << "************************* closed " << n << "-set *************************" << endl;
  printNVector(present);
  cout << endl << "****************************************************************" << endl;
#endif
#ifdef NB_OF_CLOSED_N_SETS
  ++nbOfClosedNSets;
#endif
  if (isAMinimization)
    {
      vector<unsigned int>::const_iterator sizeIt = sizes.begin();
      unsigned int dimensionId = 1;
      for (; dimensionId != n && *sizeIt++ == 1; ++dimensionId)
	{
	}
      if (dimensionId != n || *sizeIt == nbOfElementsInDi.back())
	{
	  nRectangles.insert(NRectangle(present));
	}
    }
#ifdef OUTPUT
  else
    {
      printNVector(present, outputFile);
      if (isSizePrinted)
	{
	  outputFile << patternSizeSeparator;
	  bool isFirstSize = true;
	  vector<unsigned int>::const_iterator dimensionOrderIt = dimensionOrder.begin();
	  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	    {
	      if (isFirstSize)
		{
		  isFirstSize = false;
		}
	      else
		{
		  outputFile << sizeSeparator;
		}
	      outputFile << sizes[*dimensionOrderIt++];
	    }
	}
      if (isAreaPrinted)
	{
	  float area = 1;
	  vector<float>::const_iterator areaCoefficientIt = areaCoefficients.begin();
	  vector<unsigned int>::const_iterator sizeIt = sizes.begin();
	  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	    {
	      area *= pow(*sizeIt++, *areaCoefficientIt++);
	    }
	  outputFile << sizeAreaSeparator << area;
	}
      outputFile << endl;
    }
#endif
}

void Tree::peel(const int dimensionToPeelId)
{
#ifdef NB_OF_NODES
  ++nbOfNodes;
#endif
  if (dimensionToPeelId == INT_MIN)
    {
      unsigned int remainingPotentialDimensionId = 0;
      vector<vector<unsigned int> >::iterator potentialElementsIt = potential.begin();
      for (; remainingPotentialDimensionId != n && potentialElementsIt->empty(); ++remainingPotentialDimensionId)
	{
	  ++potentialElementsIt;
	}
      if (remainingPotentialDimensionId == n)
	{
	  goodLeaf();
	}
      else
	{
	  vector<unsigned int>& presentElements = present[remainingPotentialDimensionId];
	  unsigned int oldNbOfPresentElements = presentElements.size();
	  presentElements.insert(presentElements.end(), potentialElementsIt->begin(), potentialElementsIt->end());
	  potentialElementsIt->clear();
	  unsigned int minimizationSize = 0;
	  vector<vector<unsigned int> >::const_iterator presentIt = present.begin();
	  vector<unsigned int>::const_iterator nbOfElementsInDiIt = nbOfElementsInDi.begin();
	  float area = 1;
	  vector<float>::const_iterator areaCoefficientIt = areaCoefficients.begin();
	  vector<unsigned int>::const_iterator sizeIt = sizes.begin();
	  vector<float>::const_iterator deltaIt = deltaVector.begin();
	  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
	    {
	      if (*deltaIt != 0 && dimensionId != remainingPotentialDimensionId && !deltaMaximalOn(dimensionId))
		{
		  presentElements.resize(oldNbOfPresentElements);
		  return;
		}
	      if (*sizeIt != *nbOfElementsInDiIt++)
		{
		  minimizationSize += presentIt->size();
		}
	      area *= pow(*sizeIt++, *areaCoefficientIt++);
	      ++presentIt;
	      ++deltaIt;
	    }
	  if (area >= static_cast<float>(minimizationSize) * minimizationRatio)
	    {
	      goodLeaf();
	    }
	  presentElements.resize(oldNbOfPresentElements);
	}
    }
  else
    {
      unsigned int itemToPeelId = potential[dimensionToPeelId].back();
      bool isDimensionToPeelInTheClique = !cliqueDimensions.empty() && static_cast<unsigned int>(dimensionToPeelId) == cliqueDimensions.front();
      if (isDimensionToPeelInTheClique)
	{
	  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); )
	    {
	      potential[*cliqueDimensionIt++].pop_back();
	    }
	}
      else
	{
	  potential[dimensionToPeelId].pop_back();
	}
#ifdef DEBUG_EXTRACTION
      cout << "Constructing left child (dimension peeled: " << reverseDimensionOrder[dimensionToPeelId] << ", item present: " << ids2Labels[reverseDimensionOrder[dimensionToPeelId]][itemToPeelId] << ')' << endl << "Node before:" << endl << *this << endl;
#endif
      Tree(*this, dimensionToPeelId, itemToPeelId, isDimensionToPeelInTheClique);
      unsigned int& sizeView = sizes[dimensionToPeelId];
      if (sizeView > minSizes[dimensionToPeelId])
	{
	  float area = 1;
	  vector<float>::const_iterator areaCoefficientIt = areaCoefficients.begin();
	  vector<unsigned int>::const_iterator sizeIt = sizes.begin();
	  for (unsigned int dimensionId = 0; dimensionId < n; ++dimensionId)
	    {
	      if (dimensionId == static_cast<unsigned int>(dimensionToPeelId) || (isDimensionToPeelInTheClique && find(cliqueDimensions.begin(), cliqueDimensions.end(), dimensionId) != cliqueDimensions.end()))
		{
		  const unsigned int futureSize = *sizeIt - 1;
		  area *= pow(futureSize, *areaCoefficientIt);
		}
	      else
		{
		  area *= pow(*sizeIt, *areaCoefficientIt);
		}
	      ++areaCoefficientIt;
	      ++sizeIt;
	    }
	  if (area >= minArea)
	    {
#ifdef DEBUG_EXTRACTION
	      cout << "Constructing right child (dimension peeled: " << reverseDimensionOrder[dimensionToPeelId] << ", item absent: " << ids2Labels[reverseDimensionOrder[dimensionToPeelId]][itemToPeelId] << ')' << endl << "Node before:" << endl << *this << endl;
#endif
	      float nbOf0ToReachForDeltaMaximality = deltaVector[dimensionToPeelId];
	      if (nbOf0ToReachForDeltaMaximality == 0)
		{
		  vector<unsigned int>& potentialView = potential[dimensionToPeelId];
		  const vector<unsigned int> potentialViewCopy = potentialView;
		  potentialView.clear();
		  vector<unsigned int>& presentView = present[dimensionToPeelId];
		  const vector<unsigned int> presentViewCopy = presentView;
		  presentView.clear();
		  presentView.push_back(itemToPeelId);
		  bool isNotApplicable = data->trueOnPotentialBits(present, potential);
		  presentView = presentViewCopy;
		  potentialView = potentialViewCopy;
		  if (isNotApplicable || !deltaMaximalOnEveryDimension())
		    {
#ifdef PRUNING_DETAILS
		      incrementNbOfPrunes(nbOfPrunesByMaximalityConstraint, nbOfPotentialElements());
#endif
#ifdef DEBUG_EXTRACTION
		      if (isNotApplicable)
			{
			  cout << "Item " << ids2Labels[reverseDimensionOrder[dimensionToPeelId]][itemToPeelId] << " of dimension " << reverseDimensionOrder[dimensionToPeelId] << " is not applicable -> Pruned!" <<  endl;
			}
#endif
		      return;
		    }
		}
	      else
		{
		  vector<vector<unsigned int> >::const_iterator presentIt = present.begin();
		  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
		    {
		      if (dimensionId != static_cast<unsigned int>(dimensionToPeelId))
			{
			  nbOf0ToReachForDeltaMaximality *= static_cast<float>(presentIt->size());
			}
		      ++presentIt;
		    }
		  vector<vector<unsigned int> > presentUnionPotential(n);
		  vector<vector<unsigned int> >::iterator presentUnionPotentialElementsIt = presentUnionPotential.begin();
		  vector<vector<unsigned int> >::const_iterator presentElementsIt = present.begin();
		  vector<vector<unsigned int> >::const_iterator potentialElementsIt = potential.begin();
		  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
		    {
		      if (dimensionId == static_cast<unsigned int>(dimensionToPeelId))
			{
			  presentUnionPotentialElementsIt->push_back(itemToPeelId);
			}
		      else
			{
			  presentUnionPotentialElementsIt->insert(presentUnionPotentialElementsIt->end(), presentElementsIt->begin(), presentElementsIt->end());
			  presentUnionPotentialElementsIt->insert(presentUnionPotentialElementsIt->end(), potentialElementsIt->begin(), potentialElementsIt->end());
			}
		      ++potentialElementsIt;
		      ++presentElementsIt;
		      ++presentUnionPotentialElementsIt;
		    }
		  if (!data->enough0On(static_cast<unsigned int>(floor(nbOf0ToReachForDeltaMaximality)), presentUnionPotential))
		    {
#ifdef PRUNING_DETAILS
		      incrementNbOfPrunes(nbOfPrunesByMaximalityConstraint, nbOfPotentialElements());
#endif
#ifdef DEBUG_EXTRACTION
		      cout << "Item " << ids2Labels[reverseDimensionOrder[dimensionToPeelId]][itemToPeelId] << " of dimension " << reverseDimensionOrder[dimensionToPeelId] << " is not applicable -> Pruned!" <<  endl;
#endif
		      return;
		    }
		}
	      if (isDimensionToPeelInTheClique)
		{
		  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); ++cliqueDimensionIt)
		    {
		      --sizes[*cliqueDimensionIt];
		      groups.setAbsent(*cliqueDimensionIt, itemToPeelId);
		    }
		}
	      else
		{
		  --sizeView;
		  groups.setAbsent(dimensionToPeelId, itemToPeelId);
		}
	      absent[dimensionToPeelId].push_back(itemToPeelId);
	      const vector<unsigned int> oldPresentCardinals = presentCardinals();
	      fetchElementsInClosureExceptOn(dimensionToPeelId, isDimensionToPeelInTheClique);
#ifdef DEBUG_EXTRACTION
	      cout << "Node after:" << endl << *this << endl;
#endif
	      if (deltaMaximalOnEveryDimension() && constraintsSatisfied())
		{
		  peel(nextDimension());
		}
	      keepPresentPrefixes(oldPresentCardinals);
	      if (isDimensionToPeelInTheClique)
		{
		  for (vector<unsigned int>::const_iterator cliqueDimensionIt = cliqueDimensions.begin(); cliqueDimensionIt != cliqueDimensions.end(); )
		    {
		      ++sizes[*cliqueDimensionIt++];
		    }
		}
	      else
		{
		  ++sizeView;
		}
	    }
#ifdef PRUNING_DETAILS
#ifdef DEBUG_EXTRACTION
	  else
	    {
	      incrementNbOfPrunes(nbOfPrunesByMinimalAreaConstraint, nbOfPotentialElements());
	      cout << "If one more element in dimension " << reverseDimensionOrder[dimensionToPeelId] << " is set absent, the minimal area constraint would not be satisfied -> No right child!" << endl;
	    }
#endif
#else
#ifdef DEBUG_EXTRACTION
	  else
	    {
	      cout << "If one more element in dimension " << reverseDimensionOrder[dimensionToPeelId] << " is set absent, the minimal area constraint would not be satisfied -> No right child!" << endl;
	    }
#endif
#endif
	}
#ifdef PRUNING_DETAILS
      else
	{
	  incrementNbOfPrunes(nbOfPrunesByMinimalSizeConstraint, nbOfPotentialElements());
#ifdef DEBUG_EXTRACTION
	  cout << "If one more element in dimension " << reverseDimensionOrder[dimensionToPeelId] << " is set absent, the minimal size constraint would not be satisfied -> No right child!" << endl;
#endif
	}
#else
#ifdef DEBUG_EXTRACTION
      else
	{
	  cout << "If one more element in dimension " << reverseDimensionOrder[dimensionToPeelId] << " is set absent, the minimal size constraint would not be satisfied -> No right child!" << endl;
	}
#endif
#endif
    }
}

int Tree::nextDimension() const
{
  unsigned int nbOfEmptyDimensions = 0;
  unsigned int notEmptyDimensionId = 0;
  bool isFirstNotEmptyDimensionNotFound = true;
  vector<vector<unsigned int> >::const_iterator potentialElementsIt = potential.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      if(potentialElementsIt->empty())
	{
#ifdef VERBOSE_DIM_CHOICE
	  cout << "Dimension " << reverseDimensionOrder[dimensionId] << " does not contain any more potential elements" << endl;
#endif
	  ++nbOfEmptyDimensions;
	}
      else
	{
	  if (isFirstNotEmptyDimensionNotFound)
	    {
	      isFirstNotEmptyDimensionNotFound = false;
	      notEmptyDimensionId = dimensionId;
	    }
	}
      ++potentialElementsIt;
    }
  if (nbOfEmptyDimensions >= n - 1)
    {
      return INT_MIN;
    }
  unsigned int dimensionToPeelId = notEmptyDimensionId;
  float maxAppeal = getAppeal(notEmptyDimensionId);
#ifdef VERBOSE_DIM_CHOICE
  cout << "Appeal of dimension " << reverseDimensionOrder[notEmptyDimensionId] << ": " << maxAppeal << endl;
#endif
  potentialElementsIt = potential.begin() + notEmptyDimensionId + 1;
  for (++notEmptyDimensionId; notEmptyDimensionId != n; ++notEmptyDimensionId)
    {
      if (find(cliqueDimensions.begin(), cliqueDimensions.end(), notEmptyDimensionId) == cliqueDimensions.end() || notEmptyDimensionId == cliqueDimensions.front())
	{
	  if (!potentialElementsIt->empty())
	    {
	      float appeal = getAppeal(notEmptyDimensionId);
#ifdef VERBOSE_DIM_CHOICE
	      cout << "Appeal of dimension " << reverseDimensionOrder[notEmptyDimensionId] << ": " << appeal << endl;
#endif
	      if (appeal > maxAppeal)
		{
		  dimensionToPeelId = notEmptyDimensionId;
		  maxAppeal = appeal;
		}
	    }
#ifdef VERBOSE_DIM_CHOICE
	  else
	    {
	      cout << "Dimension " << reverseDimensionOrder[notEmptyDimensionId] << " does not contain any more potential elements" << endl;
	    }
#endif
	}
      ++potentialElementsIt;
    }
  return dimensionToPeelId;
}

float Tree::getAppeal(const unsigned int dimensionId) const
{
#if ENUMERATION_PROCESS == 0
  return 0;
#endif
#if ENUMERATION_PROCESS == 1 || ENUMERATION_PROCESS == 2
  return itemsAppeals[pair<const unsigned int, const unsigned int>(dimensionId, potential[dimensionId].back())];
#endif
#if ENUMERATION_PROCESS == 3
  return -potential[dimensionId].size();
#endif
#if ENUMERATION_PROCESS == 4
  float appeal = 0;
  vector<vector<unsigned int> >::const_iterator potentialElementsIt = potential.begin();
  for (unsigned int i = 0; i != n; ++i)
    {
      if (i != dimensionId)
	{
	  float yesFactor = static_cast<float>(potentialElementsIt->size());
	  vector<vector<unsigned int> >::const_iterator presentElementsIt = present.begin();
	  for (unsigned int j = 0; j != n; ++j)
	    {
	      if (j != dimensionId && j != i)
		{
		  yesFactor *= static_cast<float>(presentElementsIt->size());
		}
	      ++presentElementsIt;
	    }
	  appeal += yesFactor;
	}
      ++potentialElementsIt;
    }
  if (appeal != 0)
    {
      return appeal;
    }
  return -!present[dimensionId].empty();
#endif
}

bool Tree::deltaMaximalOn(const unsigned int dimensionId)
{
#ifdef DETAILED_TIME
  clock_t start = clock();
#endif
  const vector<unsigned int>& currentAbsent = absent[dimensionId];
  vector<unsigned int>::const_iterator HElementIt = currentAbsent.begin();
  float nbOf0ToReachForDeltaMaximality = deltaVector[dimensionId];
  if (nbOf0ToReachForDeltaMaximality == 0)
    {
// #ifdef PROPORTION_OF_ABSENT_ITEMS_CHECKED_FOR_MAXIMALITY
//       nbOfItemsCheckedForMaximality += currentAbsent.size();
//       nbOfAbsentItemsInDimensionCheckedForMaximality += dataViews[dimensionId].size() - present[dimensionId].size() - potential[dimensionId].size();
// #endif
      vector<unsigned int>& potentialView = potential[dimensionId];
      const vector<unsigned int> potentialViewCopy = potentialView;
      potentialView.clear();
      vector<unsigned int>& presentView = present[dimensionId];
      const vector<unsigned int> presentViewCopy = presentView;
      presentView.clear();
      for (; HElementIt != currentAbsent.end(); ++HElementIt)
	{
	  presentView.push_back(*HElementIt);
	  if (data->trueOnPotentialBits(present, potential))
	    {
	      break;
	    }
	  presentView.pop_back();
	}
      presentView = presentViewCopy;
      potentialView = potentialViewCopy;
    }
  else
    {
// #ifdef PROPORTION_OF_ABSENT_ITEMS_CHECKED_FOR_MAXIMALITY
//       nbOfItemsCheckedForMaximality += currentAbsent.size();
//       nbOfAbsentItemsInDimensionCheckedForMaximality += currentAbsent.size();
// #endif
      vector<vector<unsigned int> >::const_iterator presentIt = present.begin();
      for (unsigned int d = 0; d != n; )
	{
	  if (d++ != dimensionId)
	    {
	      nbOf0ToReachForDeltaMaximality *= static_cast<float>(presentIt->size());
	    }
	  ++presentIt;
	}
      const unsigned int threshold = static_cast<const unsigned int>(floor(nbOf0ToReachForDeltaMaximality));
      vector<vector<unsigned int> > presentUnionPotential(n);
      vector<vector<unsigned int> >::iterator presentUnionPotentialElementsIt = presentUnionPotential.begin();
      vector<vector<unsigned int> >::const_iterator presentElementsIt = present.begin();
      vector<vector<unsigned int> >::const_iterator potentialElementsIt = potential.begin();
      for (unsigned int d = 0; d != n; )
	{
	  if (d++ != dimensionId)
	    {
	      presentUnionPotentialElementsIt->insert(presentUnionPotentialElementsIt->end(), presentElementsIt->begin(), presentElementsIt->end());
	      presentUnionPotentialElementsIt->insert(presentUnionPotentialElementsIt->end(), potentialElementsIt->begin(), potentialElementsIt->end());
	    }
	  ++potentialElementsIt;
	  ++presentElementsIt;
	  ++presentUnionPotentialElementsIt;
	}
      vector<unsigned int>& presentUnionPotentialView = presentUnionPotential[dimensionId];
      for (; HElementIt != currentAbsent.end(); ++HElementIt)
	{
	  presentUnionPotentialView.push_back(*HElementIt);
	  if (!data->enough0On(threshold, presentUnionPotential))
	    {
	      break;
	    }
	  presentUnionPotentialView.pop_back();
	}
    }
#ifdef DETAILED_TIME
  maximalityCheckingTime += clock() - start;
#endif
#ifdef DEBUG_EXTRACTION
  if (HElementIt != currentAbsent.end())
    {
      cout << ids2Labels[reverseDimensionOrder[dimensionId]][*HElementIt] << " (in absent[" << reverseDimensionOrder[dimensionId] << "]) can extend the node -> Pruned!" << endl;
    }
#endif
  return HElementIt == currentAbsent.end();
}

bool Tree::deltaMaximalOnEveryDimension()
{
  unsigned int dimensionId = 0;
  for (; dimensionId != n && deltaMaximalOn(dimensionId); ++dimensionId)
    {
    }
  return dimensionId == n;
}

bool Tree::deltaMaximalExceptOn(const unsigned int dimensionNotCheckedId)
{
  unsigned int dimensionId = 0;
  for (; dimensionId != n && (dimensionId == dimensionNotCheckedId || deltaMaximalOn(dimensionId)); ++dimensionId)
    {
    }
  return dimensionId == n;
}

void Tree::removeIrrelevantElementsFromAbsent(const unsigned int dimensionWithNewElementsId, const vector<unsigned int> newItemIds, const bool isDimensionToPeelInTheClique)
{
  vector<vector<unsigned int> > newPresentBits;
  newPresentBits.reserve(n);
  vector<vector<unsigned int> >::const_iterator presentElementsIt = present.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      if (dimensionId == dimensionWithNewElementsId)
	{
	  newPresentBits.push_back(newItemIds);
	}
      else
	{
	  newPresentBits.push_back(*presentElementsIt);
	}
      ++presentElementsIt;
    }
  vector<vector<unsigned int> >::iterator absentIt = absent.begin();
  vector<vector<unsigned int> >::iterator newPresentBitsIt = newPresentBits.begin();
  vector<float>::const_iterator deltaIt = deltaVector.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      if (*deltaIt++ == 0 && (dimensionId != dimensionWithNewElementsId || isDimensionToPeelInTheClique))
	{
	  vector<unsigned int> newPresentBitsCopy = *newPresentBitsIt;
	  newPresentBitsIt->clear();
	  for (vector<unsigned int>::iterator absentElementIt = absentIt->begin(); absentElementIt != absentIt->end(); )
	    {
	      newPresentBitsIt->push_back(*absentElementIt);
	      if (data->trueOn(newPresentBits))
		{
		  ++absentElementIt;
		}
	      else
		{
#ifdef DEBUG_EXTRACTION
		  cout << ids2Labels[reverseDimensionOrder[dimensionId]][*absentElementIt] << " removed from absent[" << reverseDimensionOrder[dimensionId] << "] since it will never extend the node" << endl;
#endif
		  *absentElementIt = absentIt->back();
		  absentIt->pop_back();
		}
	      newPresentBitsIt->pop_back();
	    }
	  *newPresentBitsIt = newPresentBitsCopy;
	}
      ++absentIt;
      ++newPresentBitsIt;
    }
}

void Tree::fetchElementsInClosureOn(const unsigned int dimensionId, const bool isDimensionToPeelInTheClique)
{
#ifdef DETAILED_TIME
  clock_t start = clock();
#endif
  vector<unsigned int>& potentialView = potential[dimensionId];
  vector<unsigned int> newPotentialView = potentialView;
  potentialView.clear();
  vector<unsigned int>& presentView = present[dimensionId];
  vector<unsigned int> newItemIds;
  vector<unsigned int> newPresentView = presentView;
  presentView.clear();
  for (vector<unsigned int>::iterator potentialElementIt = newPotentialView.begin(); potentialElementIt != newPotentialView.end(); )
    {
      presentView.push_back(*potentialElementIt);
      if (data->trueOnPotentialBits(present, potential))
	{
#ifdef DEBUG_EXTRACTION
	  const unsigned int dimensionInFiles = reverseDimensionOrder[dimensionId];
	  cout << ids2Labels[reverseDimensionOrder[dimensionId]][*potentialElementIt] << " is moved from potential[" << dimensionInFiles << "] to present[" << dimensionInFiles << "] since it will always be present" << endl;
#endif
	  newItemIds.push_back(*potentialElementIt);
	  *potentialElementIt = newPotentialView.back();
	  newPotentialView.pop_back();
	}
      else
	{
	  ++potentialElementIt;
	}
      presentView.pop_back();
    }
  presentView = newPresentView;
  presentView.insert(presentView.end(), newItemIds.begin(), newItemIds.end());
  potentialView = newPotentialView;
#ifdef DETAILED_TIME
  closureOperationTime += clock() - start;
#endif
  removeIrrelevantElementsFromAbsent(dimensionId, newItemIds, isDimensionToPeelInTheClique);
  groups.setPresent(dimensionId, newItemIds);
}

void Tree::fetchElementsInClosureOnEveryDimension(const bool isDimensionToPeelInTheClique)
{
  for (unsigned int dimensionId = 0; dimensionId != n ; ++dimensionId)
    {
      fetchElementsInClosureOn(dimensionId, isDimensionToPeelInTheClique);
    }
}

void Tree::fetchElementsInClosureExceptOn(const unsigned int dimensionNotCheckedId, const bool isDimensionToPeelInTheClique)
{
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      if (dimensionId != dimensionNotCheckedId)
	{
	  fetchElementsInClosureOn(dimensionId, isDimensionToPeelInTheClique);
	}
    }
}

vector<unsigned int> Tree::presentCardinals() const
{
  vector<unsigned int> presentCardinalsVector;
  presentCardinalsVector.reserve(n);
  vector<vector<unsigned int> >::const_iterator presentElementsIt = present.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      presentCardinalsVector.push_back(presentElementsIt->size());
      ++presentElementsIt;
    }
  return presentCardinalsVector;
}

void Tree::keepPresentPrefixes(const vector<unsigned int> indices)
{
  vector<unsigned int>::const_iterator indexIt = indices.begin();
  vector<vector<unsigned int> >::iterator presentElementsIt = present.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      presentElementsIt->resize(*indexIt++);
      ++presentElementsIt;
    }
}

// Here, no crazyness of mixing the enumeration and constraint checking to save micro-seconds
// Constraints checked: maximal sizes, maximal area and minimization ratio
#ifdef PRUNING_DETAILS
const bool Tree::constraintsSatisfied()
#else
const bool Tree::constraintsSatisfied() const
#endif
{
#ifdef DEBUG_EXTRACTION
  unsigned int dimensionId = 0;
#endif
  unsigned int minimizationSize = 0;
  vector<unsigned int>::const_iterator nbOfElementsInDiIt = nbOfElementsInDi.begin();
  float area = 1;
  float maxPossibleArea = 1;
  vector<float>::const_iterator areaCoefficientIt = areaCoefficients.begin();
  vector<unsigned int>::iterator sizeIt = sizes.begin();
  vector<unsigned int>::const_iterator maxSizeIt = maxSizes.begin();
  for (vector<vector<unsigned int> >::const_iterator presentIt = present.begin(); presentIt != present.end(); ++presentIt)
    {
      const unsigned int presentSize = presentIt->size();
      if (presentSize > *maxSizeIt++)
	{
#ifdef DEBUG_EXTRACTION
	  cout << "After propagation, the maximal size constraint is not satisfied on dimension " << reverseDimensionOrder[dimensionId] << " -> Pruned!" << endl;
#endif
#ifdef PRUNING_DETAILS
	  incrementNbOfPrunes(nbOfPrunesByMaximalSizeConstraint, nbOfPotentialElements());
#endif
	  return false;
	}
      maxPossibleArea *= pow(presentSize, *areaCoefficientIt);
      if (*sizeIt != *nbOfElementsInDiIt++)
	{
	  minimizationSize += presentSize;
	}
      area *= pow(*sizeIt++, *areaCoefficientIt++);
    }
  if (maxPossibleArea > maxArea)
    {
#ifdef DEBUG_EXTRACTION
      cout << "After propagation, the maximal area constraint is not satisfied -> Pruned!" << endl;
#endif
#ifdef PRUNING_DETAIL
      incrementNbOfPrunes(nbOfPrunesByMaximalAreaConstraint, nbOfPotentialElements());
#endif
      return false;
    }
  if (area < static_cast<float>(minimizationSize) * minimizationRatio)
    {
#ifdef DEBUG_EXTRACTION
      cout << "The minimization ratio cannot be met -> Pruned!" << endl;
#endif
#ifdef PRUNING_DETAIL
      incrementNbOfPrunes(nbOfPrunesByMinimizationRatio, nbOfPotentialElements());
#endif
      return false;
    }
  return groups.constraintsSatisfied();
}

void Tree::insertUnidimensionalNRectangles()
{
  for (vector<vector<vector<unsigned int> > > remainingUnidimensionalNRectangles = data->unidimensionalNRectangles(); !remainingUnidimensionalNRectangles.empty(); remainingUnidimensionalNRectangles.pop_back())
    {
      nRectangles.insert(NRectangle(remainingUnidimensionalNRectangles.back()));
    }
}

#ifdef DEBUG_EXTRACTION
ostream& operator<<(ostream& out, const Tree& tree)
{
  out << "present: ";
  tree.printNVector(tree.present, out);
  out << endl << "potential: ";
  tree.printNVector(tree.potential, out);
  out << endl << "absent: ";
  tree.printNVector(tree.absent, out);
  return out;
}
#endif

#ifdef PRUNING_DETAILS
unsigned int Tree::nbOfPotentialElements() const
{
  unsigned int total = 0;
  vector<vector<unsigned int> >::const_iterator potentialElementsIt = potential.begin();
  for (unsigned int dimensionId = 0; dimensionId != n; ++dimensionId)
    {
      total += potentialElementsIt->size();
      ++potentialElementsIt;
    }
  return total;
}

void Tree::incrementNbOfPrunes(vector<bool>& nbOfPrunes, const unsigned int index)
{
  if (nbOfPrunes[index])
    {
      nbOfPrunes[index] = false;
      incrementNbOfPrunes(nbOfPrunes, index + 1);
    }
  else
    {
      nbOfPrunes[index] = true;
    }
}

void Tree::printNbOfPrunes(const vector<bool>& nbOfPrunes, ostream& out)
{
  vector<bool>::const_reverse_iterator nbOfPrunesIt = nbOfPrunes.rbegin();
  for (unsigned int power = nbOfPrunes.size() - 1; power != 0; --power)
    {
      if (*nbOfPrunesIt)
	{
	  out << power;
	  return;
	}
      ++nbOfPrunesIt;
    }
  if (nbOfPrunes.front())
    {
      out << '1';
    }
  else
    {
      out << '0';
    }
}

#endif

namespace
{
    void
    SetAllSelfLoops(unsigned dimension,
                    vector<unsigned int>& tuple,
                    unsigned int symmetricDimensionsValue,
                    const vector<bool>& isDimensionSymmetric)
    {
        if (dimension == Tree::n) {
            Tree::data->setValue(tuple, true);
        } else {
            if (isDimensionSymmetric[dimension]) {
                tuple[Tree::dimensionOrder[dimension]] = symmetricDimensionsValue;
                SetAllSelfLoops(dimension + 1, tuple, symmetricDimensionsValue,
                        isDimensionSymmetric);
            } else {
                for (unsigned int i = 0; i < Tree::ids2Labels[dimension].size(); i++) {
                    tuple[Tree::dimensionOrder[dimension]] = i;
                    SetAllSelfLoops(dimension + 1, tuple, symmetricDimensionsValue,
                            isDimensionSymmetric);
                }
            }
        }
    }
}

void
Tree::setSelfLoops(const vector<unsigned int>& cliqueDimensions)
{
    if (!cliqueDimensions.size())
        return;

    vector<unsigned int> tuple(n);
    vector<bool> isDimensionSymmetric(n);

    for (unsigned i = 0; i < n; i++) {
        if (std::find(cliqueDimensions.begin(), cliqueDimensions.end(), i)
                != cliqueDimensions.end()) {
            isDimensionSymmetric[i] = true;
        }
    }

    for (unsigned int symmetricDimensionsValue = 0;
            symmetricDimensionsValue < ids2Labels[cliqueDimensions.front()].size();
            symmetricDimensionsValue++) {
        SetAllSelfLoops(0, tuple, symmetricDimensionsValue, isDimensionSymmetric);
    }
}


