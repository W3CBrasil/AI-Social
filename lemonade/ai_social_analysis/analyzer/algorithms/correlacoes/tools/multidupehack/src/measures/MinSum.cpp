// Copyright 2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MinSum.h"

vector<unsigned int> MinSum::dimensionIds;
AbstractTupleValueData* MinSum::tupleValues;
float MinSum::threshold;

MinSum::MinSum(const string& tupleValueFileName, const char* dimensionSeparatorChars, const char* elementSeparatorChars, const vector<unordered_map<string, unsigned int>>& labels2Ids, const vector<unsigned int>& dimensionOrder, const vector<unsigned int>& cardinalities, const float densityThreshold, const float thresholdParam): TupleMeasure(), minSum(0)
{
  ifstream tupleValueFile(tupleValueFileName.c_str());
  if (!tupleValueFile)
    {
      throw NoFileException(tupleValueFileName.c_str());
    }
  // Parsing the dimension ids of the tuples
  unsigned int lineNb = 0;
  vector<unsigned int> internalDimensionIds;
  internalDimensionIds.reserve(dimensionOrder.size());
  while (internalDimensionIds.empty() && !tupleValueFile.eof())
    {
      ++lineNb;
      string dataString;
      getline(tupleValueFile, dataString);
#ifdef VERBOSE_PARSER
      cout << tupleValueFileName << ':' << lineNb << ": " << dataString << endl;
#endif
      unsigned int dimensionId;
      istringstream ss(dataString);
      while (ss >> dimensionId)
	{
	  if (dimensionId >= dimensionOrder.size())
	    {
	      throw DataFormatException(tupleValueFileName.c_str(), lineNb, ("dimension " + lexical_cast<string>(dimensionId) + " does not exist! (at most " + lexical_cast<string>(dimensionOrder.size() - 1) + " expected)").c_str());
	    }
	  internalDimensionIds.push_back(dimensionOrder[dimensionId]);
	}
    }
  if (tupleValueFile.eof())
    {
      throw DataFormatException(tupleValueFileName.c_str(), lineNb, "no dimension ids found!");
    }
  dimensionIds = internalDimensionIds;
  sort(dimensionIds.begin(), dimensionIds.end());
  vector<unsigned int>::const_iterator dimensionIdIt = dimensionIds.begin();
  while (++dimensionIdIt != dimensionIds.end() && *dimensionIdIt != *(dimensionIdIt - 1))
    {
    }
  if (dimensionIdIt != dimensionIds.end())
    {
      throw DataFormatException(tupleValueFileName.c_str(), lineNb, "dimensions must all be different!");
    }
  // Compute the tuple dimension ids, indicate the dimensions as relevant to the measure, which is monotone
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
  // Initialize tupleValues
  if (dimensionIds.size() == 1)
    {
      tupleValues = new TupleValueSparseTube();
    }
  else
    {
      tupleValues = new TupleValueTrie(dimensionIds.begin(), dimensionIds.end(), cardinalities);
    }
  // Parsing the tuple values
  const unsigned int lastDimensionCardinality = cardinalities[dimensionIds.back()];
  const unsigned int sizeThreshold = lastDimensionCardinality * densityThreshold * sizeof(double) / (sizeof(unsigned int) + sizeof(double));
  const char_separator<char> elementSeparator(elementSeparatorChars);
  const char_separator<char> dimensionSeparator(dimensionSeparatorChars);
  while (!tupleValueFile.eof())
    {
      ++lineNb;
      string dataString;
      getline(tupleValueFile, dataString);
#ifdef VERBOSE_PARSER
      cout << tupleValueFileName << ':' << lineNb << ": " << dataString << endl;
#endif
      tokenizer<char_separator<char>> tupleValue(dataString, dimensionSeparator);
      tokenizer<char_separator<char>>::const_iterator tupleValueIt = tupleValue.begin();
      if (tupleValueIt != tupleValue.end())
	{
	  // Parsing the n-set
	  vector<vector<unsigned int>> nSet(tupleDimensionIds.size());
	  bool isCoverEmpty = false;
	  vector<unsigned int>::const_iterator tupleDimensionIdIt = tupleDimensionIds.begin();
	  for (const unsigned int internalDimensionId : internalDimensionIds)
	    {
	      vector<unsigned int>& elements = nSet[*tupleDimensionIdIt++];
	      const unordered_map<string, unsigned int>& labels2IdsView = labels2Ids[internalDimensionId];
	      tokenizer<char_separator<char>> elementTokens(*tupleValueIt, elementSeparator);
	      for (const string& element : elementTokens)
		{
		  const unordered_map<string, unsigned int>::const_iterator labels2IdsViewIt = labels2IdsView.find(element);
		  if (labels2IdsViewIt == labels2IdsView.end())
		    {
		      cerr << "Warning: ignoring " << element << " at line " << lineNb << " of " << tupleValueFileName << " because it is absent from the corresponding dimension of the input data" << endl;
		    }
		  else
		    {
		      if (labels2IdsViewIt->second != numeric_limits<unsigned int>::max())
			{
			  elements.push_back(labels2IdsViewIt->second);
			}
		    }
		}
	      if (++tupleValueIt == tupleValue.end())
		{
		  delete tupleValues;
		  throw DataFormatException(tupleValueFileName.c_str(), lineNb, ("less than the expected " + lexical_cast<string>(dimensionIds.size() + 2) + " dimensions (a " + lexical_cast<string>(dimensionIds.size()) + "-set followed by a value)!").c_str());
		}
	      if (elements.empty())
		{
		  isCoverEmpty = true;
		}
	    }
	  // Parsing the value
          double value;
	  try
	    {
	      value = lexical_cast<double>(*tupleValueIt);
	      if (value < 0)
		{
		  throw bad_lexical_cast();
		}
	    }
	  catch (bad_lexical_cast &)
	    {
	      delete tupleValues;
	      throw DataFormatException(tupleValueFileName.c_str(), lineNb, (*tupleValueIt + " should be a positive double!").c_str());
	    }
	  if (!isfinite(value))
	    {
	      delete tupleValues;
	      throw DataFormatException(tupleValueFileName.c_str(), lineNb, "value must be finite!");
	    }
          if (++tupleValueIt != tupleValue.end())
	    {
	      delete tupleValues;
	      throw DataFormatException(tupleValueFileName.c_str(), lineNb, ("more than the expected " + lexical_cast<string>(dimensionIds.size() + 2) + " dimensions (a " + lexical_cast<string>(dimensionIds.size()) + "-set followed by a value)!").c_str());
	    }
	  if (!isCoverEmpty && value != 0)
	    {
	      // Storing the tuple values
	      if (tupleValues->setTupleValues(nSet.begin(), value, sizeThreshold, lastDimensionCardinality))
		{
		  TupleValueDenseTube* newTupleValues = new TupleValueDenseTube(static_cast<TupleValueSparseTube*>(tupleValues)->getDenseRepresentation(lastDimensionCardinality));
		  delete tupleValues;
		  tupleValues = newTupleValues;
		}
	    }
	}
    }
  tupleValues->sortTubesAndSetSum(minSum);
  threshold = thresholdParam;
}

MinSum* MinSum::clone() const
{
  return new MinSum(*this);
}

const bool MinSum::monotone() const
{
  return true;
}

const bool MinSum::violationAfterPresentAndPotentialDecreased(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent)
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
  // dimensionIdOfElementsSetPresent is a dimension of the tuples, update the sum
  tupleValues->decreaseSum(present(), potential(), elementsSetAbsent, dimensionIds.begin(), dimensionIdIt, minSum);
#ifdef DEBUG
  if (minSum < threshold)
    {
      cout << threshold << "-minimal sum constraint cannot be satisfied -> Prune!" << endl;
    }
#endif
  return minSum < threshold;
}

const float MinSum::optimisticValue() const
{
  return minSum;
}

void MinSum::deleteTupleValues()
{
  if (!dimensionIds.empty())
    {
      delete tupleValues;
    }
}
