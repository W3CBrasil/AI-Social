// Copyright 2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MinSlope.h"

vector<unsigned int> MinSlope::dimensionIds;
AbstractTuplePointData* MinSlope::tuplePoints;
float MinSlope::threshold;

MinSlope::MinSlope(const string& tuplePointFileName, const char* dimensionSeparatorChars, const char* elementSeparatorChars, const vector<unordered_map<string, unsigned int>>& labels2Ids, const vector<unsigned int>& dimensionOrder, const vector<unsigned int>& cardinalities, const float densityThreshold, const float thresholdParam): TupleMeasure(), minSums(), maxSums(), minSlope(numeric_limits<float>::infinity())
{
  ifstream tuplePointFile(tuplePointFileName.c_str());
  if (!tuplePointFile)
    {
      throw NoFileException(tuplePointFileName.c_str());
    }
  // Parsing the dimension ids of the tuples
  unsigned int lineNb = 0;
  vector<unsigned int> internalDimensionIds;
  internalDimensionIds.reserve(dimensionOrder.size());
  while (internalDimensionIds.empty() && !tuplePointFile.eof())
    {
      ++lineNb;
      string dataString;
      getline(tuplePointFile, dataString);
#ifdef VERBOSE_PARSER
      cout << tuplePointFileName << ':' << lineNb << ": " << dataString << endl;
#endif
      unsigned int dimensionId;
      istringstream ss(dataString);
      while (ss >> dimensionId)
	{
	  if (dimensionId >= dimensionOrder.size())
	    {
	      throw DataFormatException(tuplePointFileName.c_str(), lineNb, ("dimension " + lexical_cast<string>(dimensionId) + " does not exist! (at most " + lexical_cast<string>(dimensionOrder.size() - 1) + " expected)").c_str());
	    }
	  internalDimensionIds.push_back(dimensionOrder[dimensionId]);
	}
    }
  if (tuplePointFile.eof())
    {
      throw DataFormatException(tuplePointFileName.c_str(), lineNb, "no dimension ids found!");
    }
  dimensionIds = internalDimensionIds;
  sort(dimensionIds.begin(), dimensionIds.end());
  vector<unsigned int>::const_iterator dimensionIdIt = dimensionIds.begin();
  while (++dimensionIdIt != dimensionIds.end() && *dimensionIdIt != *(dimensionIdIt - 1))
    {
    }
  if (dimensionIdIt != dimensionIds.end())
    {
      throw DataFormatException(tuplePointFileName.c_str(), lineNb, "dimensions must all be different!");
    }
  // Compute the tuple dimension ids, indicate the dimensions as relevant to the measure, which is both monotone and anti-monotone
  vector<unsigned int> tupleDimensionIds;
  tupleDimensionIds.reserve(internalDimensionIds.size());
  relevantDimensions.resize(dimensionOrder.size());
  relevantDimensionsForMonotoneMeasures.resize(dimensionOrder.size());
  for (const unsigned int internalDimensionId : internalDimensionIds)
    {
      relevantDimensions[internalDimensionId] = true;
      relevantDimensionsForMonotoneMeasures[internalDimensionId] = true;
      unsigned int tupleDimensionId = 0;
      for (vector<unsigned int>::const_iterator dimensionIdIt = dimensionIds.begin(); *dimensionIdIt != internalDimensionId; ++dimensionIdIt)
	{
	  ++tupleDimensionId;
	}
      tupleDimensionIds.push_back(tupleDimensionId);
    }
  // Initialize tuplePoints
  if (dimensionIds.size() == 1)
    {
      tuplePoints = new TuplePointSparseTube();
    }
  else
    {
      tuplePoints = new TuplePointTrie(dimensionIds.begin(), dimensionIds.end(), cardinalities);
    }
  // Parsing the tuple points
  const unsigned int lastDimensionCardinality = cardinalities[dimensionIds.back()];
  const unsigned int sizeThreshold = lastDimensionCardinality * densityThreshold * 2 * sizeof(double) / (sizeof(unsigned int) + 2 * sizeof(double));
  const char_separator<char> elementSeparator(elementSeparatorChars);
  const char_separator<char> dimensionSeparator(dimensionSeparatorChars);
  while (!tuplePointFile.eof())
    {
      ++lineNb;
      string dataString;
      getline(tuplePointFile, dataString);
#ifdef VERBOSE_PARSER
      cout << tuplePointFileName << ':' << lineNb << ": " << dataString << endl;
#endif
      tokenizer<char_separator<char>> tuplePoint(dataString, dimensionSeparator);
      tokenizer<char_separator<char>>::const_iterator tuplePointIt = tuplePoint.begin();
      if (tuplePointIt != tuplePoint.end())
	{
	  // Parsing the n-set
	  vector<vector<unsigned int>> nSet(tupleDimensionIds.size());
	  bool isCoverEmpty = false;
	  vector<unsigned int>::const_iterator tupleDimensionIdIt = tupleDimensionIds.begin();
	  for (const unsigned int internalDimensionId : internalDimensionIds)
	    {
	      vector<unsigned int>& elements = nSet[*tupleDimensionIdIt++];
	      const unordered_map<string, unsigned int>& labels2IdsView = labels2Ids[internalDimensionId];
	      tokenizer<char_separator<char>> elementTokens(*tuplePointIt, elementSeparator);
	      for (const string& element : elementTokens)
		{
		  const unordered_map<string, unsigned int>::const_iterator labels2IdsViewIt = labels2IdsView.find(element);
		  if (labels2IdsViewIt == labels2IdsView.end())
		    {
		      cerr << "Warning: ignoring " << element << " at line " << lineNb << " of " << tuplePointFileName << " because it is absent from the corresponding dimension of the input data" << endl;
		    }
		  else
		    {
		      if (labels2IdsViewIt->second != numeric_limits<unsigned int>::max())
			{
			  elements.push_back(labels2IdsViewIt->second);
			}
		    }
		}
	      if (++tuplePointIt == tuplePoint.end())
		{
		  delete tuplePoints;
		  throw DataFormatException(tuplePointFileName.c_str(), lineNb, ("less than the expected " + lexical_cast<string>(dimensionIds.size() + 2) + " dimensions (a " + lexical_cast<string>(dimensionIds.size()) + "-set followed by a point)!").c_str());
		}
	      if (elements.empty())
		{
		  isCoverEmpty = true;
		}
	    }
	  // Parsing the point
          pair<double, double> point;
	  try
	    {
	      point.first = lexical_cast<double>(*tuplePointIt);
	    }
	  catch (bad_lexical_cast &)
	    {
	      delete tuplePoints;
	      throw DataFormatException(tuplePointFileName.c_str(), lineNb, (*tuplePointIt + " (abscissa of the point) should be a double!").c_str());
	    }
	  if (!isfinite(point.first))
	    {
	      delete tuplePoints;
	      throw DataFormatException(tuplePointFileName.c_str(), lineNb, "abscissa of the point must be finite!");
	    }
          if (++tuplePointIt == tuplePoint.end())
	    {
	      delete tuplePoints;
	      throw DataFormatException(tuplePointFileName.c_str(), lineNb, ("less than the expected " + lexical_cast<string>(dimensionIds.size() + 2) + " dimensions (a " + lexical_cast<string>(dimensionIds.size()) + "-set followed by a point)!").c_str());
	    }
	  try
	    {
	      point.second = lexical_cast<double>(*tuplePointIt);
	    }
	  catch (bad_lexical_cast &)
	    {
	      delete tuplePoints;
	      throw DataFormatException(tuplePointFileName.c_str(), lineNb, (*tuplePointIt + " (ordinate of the point) should be a double!").c_str());
	    }
	  if (!isfinite(point.second))
	    {
	      delete tuplePoints;
	      throw DataFormatException(tuplePointFileName.c_str(), lineNb, "ordinate of the point must be finite!");
	    }
          if (++tuplePointIt != tuplePoint.end())
	    {
	      delete tuplePoints;
	      throw DataFormatException(tuplePointFileName.c_str(), lineNb, ("more than the expected " + lexical_cast<string>(dimensionIds.size() + 2) + " dimensions (a " + lexical_cast<string>(dimensionIds.size()) + "-set followed by a point)!").c_str());
	    }
          if (!isCoverEmpty)
	    {
	      // Storing the tuple points
	      if (tuplePoints->setTuplePoints(nSet.begin(), point, sizeThreshold, lastDimensionCardinality))
		{
		  TuplePointDenseTube* newTuplePoints = new TuplePointDenseTube(static_cast<TuplePointSparseTube*>(tuplePoints)->getDenseRepresentation(lastDimensionCardinality));
		  delete tuplePoints;
		  tuplePoints = newTuplePoints;
		}
	    }
	}
    }
  tuplePoints->sortTubesAndTranslateToPositiveQuadrant();
  tuplePoints->setSlopeSums(maxSums);
  threshold = thresholdParam;
}

MinSlope* MinSlope::clone() const
{
  return new MinSlope(*this);
}

const bool MinSlope::violationAfterPresentIncreased(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent)
{
  // Test whether dimensionIdOfElementsSetPresent is a dimension of the tuples
  vector<unsigned int>::const_iterator dimensionIdIt = dimensionIds.begin();
  for (; dimensionIdIt != dimensionIds.end() && *dimensionIdIt < dimensionIdOfElementsSetPresent; ++dimensionIdIt)
    {
    }
  if (dimensionIdIt == dimensionIds.end() || *dimensionIdIt != dimensionIdOfElementsSetPresent)
    {
      return false;
    }
  // dimensionIdOfElementsSetPresent is a dimension of the tuples, update the sums
  tuplePoints->increaseSlopeSums(present(), elementsSetPresent, dimensionIds.begin(), dimensionIdIt, minSums);
  computeOptimisticValue();
#ifdef DEBUG
  if (minSlope < threshold)
    {
      cout << threshold << "-minimal slope constraint cannot be satisfied -> Prune!" << endl;
    }
#endif
  return minSlope < threshold;
}

const bool MinSlope::violationAfterPresentAndPotentialDecreased(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent)
{
  // Test whether dimensionIdOfElementsSetPresent is a dimension of the tuples
  vector<unsigned int>::const_iterator dimensionIdIt = dimensionIds.begin();
  for (; dimensionIdIt != dimensionIds.end() && *dimensionIdIt < dimensionIdOfElementsSetAbsent; ++dimensionIdIt)
    {
    }
  if (dimensionIdIt == dimensionIds.end() || *dimensionIdIt != dimensionIdOfElementsSetAbsent)
    {
      return false;
    }
  // dimensionIdOfElementsSetPresent is a dimension of the tuples, update the sums
  tuplePoints->decreaseSlopeSums(present(), potential(), elementsSetAbsent, dimensionIds.begin(), dimensionIdIt, maxSums);
  computeOptimisticValue();
#ifdef DEBUG
  if (minSlope < threshold)
    {
      cout << threshold << "-minimal slope constraint cannot be satisfied -> Prune!" << endl;
    }
#endif
  return minSlope < threshold;
}

const float MinSlope::optimisticValue() const
{
  return minSlope;
}

void MinSlope::computeOptimisticValue()
{
  double denominator = minSums.sumX * minSums.sumX - maxSums.nbOfPoints * maxSums.sumXSquared;
  if (denominator > 0)
    {
      const double numerator = maxSums.sumX * maxSums.sumY - minSums.nbOfPoints * minSums.sumXY;
      if (numerator > 0)
	{
	  minSlope = numerator / denominator;
	  return;
	}
      minSlope = numerator / (maxSums.sumX * maxSums.sumX - minSums.nbOfPoints * minSums.sumXSquared);
      return;
    }
  denominator = maxSums.sumX * maxSums.sumX - minSums.nbOfPoints * minSums.sumXSquared;
  if (denominator < 0)
    {
      const double numerator = minSums.sumX * minSums.sumY - maxSums.nbOfPoints * maxSums.sumXY;
      if (numerator < 0)
	{
	  minSlope = numerator / denominator;
	  return;
	}
      minSlope = numerator / (minSums.sumX * minSums.sumX - maxSums.nbOfPoints * maxSums.sumXSquared);
    }
}

void MinSlope::deleteTuplePoints()
{
  if (!dimensionIds.empty())
    {
      delete tuplePoints;
    }
}
