// Copyright 2012,2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "NoisyTupleFileReader.h"

NoisyTupleFileReader::NoisyTupleFileReader(const char* noisyNSetFileNameParam, const vector<unsigned int>& symDimensionIdsParam, const vector<unsigned int>& numDimensionIdsParam, const char* inputDimensionSeparator, const char* inputElementSeparator) : noisyNSetFileName(noisyNSetFileNameParam), noisyNSetFile(noisyNSetFileName), symDimensionIds(symDimensionIdsParam), numDimensionIds(numDimensionIdsParam), minMembership(1. / numeric_limits<unsigned int>::max()), inputDimensionSeparator(inputDimensionSeparator), inputElementSeparator(inputElementSeparator), lineNb(0), ids2Labels(), labels2Ids(), symIds2Labels(), symLabels2Ids(), nSet(), membership(0), tupleIts()
{
  if (!noisyNSetFile)
    {
      throw NoFileException(noisyNSetFileNameParam);
    }
  init();
}

void NoisyTupleFileReader::init()
{
  if (noisyNSetFile.eof())
    {
      noisyNSetFile.close();
      membership = 0;
      return;
    }
  ++lineNb;
  string noisyNSetString;
  getline(noisyNSetFile, noisyNSetString);
  tokenizer<char_separator<char>> dimensions(noisyNSetString, inputDimensionSeparator);
  tokenizer<char_separator<char>>::const_iterator dimensionIt = dimensions.begin();
  if (dimensionIt == dimensions.end())
    {
      init();
      return;
    }
  unsigned int n = 0;
  vector<unsigned int>::const_iterator symDimensionIdIt = symDimensionIds.begin();
  while (++dimensionIt != dimensions.end())
    {
      if (symDimensionIdIt != symDimensionIds.end() && *symDimensionIdIt == n)
	{
	  ++symDimensionIdIt;
	}
      ++n;
    }
  if (symDimensionIdIt != symDimensionIds.end())
    {
      throw UsageException(("clique option should provide attribute ids between 0 and " + lexical_cast<string>(n - 1)).c_str());
    }
  ids2Labels.resize(n);
  labels2Ids.resize(n);
  nSet.resize(n);
  tupleIts.resize(n);
#ifdef VERBOSE_PARSER
  cout << noisyNSetFileName << ':' << lineNb << ": " << noisyNSetString << endl;
#endif
  if (parseLine(dimensions.begin(), dimensions.end()))
    {
      nextNSet();
    }
}

const vector<string>& NoisyTupleFileReader::getIds2Labels(const unsigned int dimensionId) const
{
  return ids2Labels[dimensionId];
}

vector<unsigned int> NoisyTupleFileReader::getCardinalities() const
{
  vector<unsigned int> cardinalities;
  cardinalities.reserve(ids2Labels.size());
  for (const vector<string>& ids2LabelsInDimensions : ids2Labels)
    {
      if (ids2LabelsInDimensions.empty())
	{
	  // Symmetric dimension
	  cardinalities.push_back(symIds2Labels.size());
	}
      else
	{
	  // Non-symmetric dimension
	  cardinalities.push_back(ids2LabelsInDimensions.size());
	}
    }
  return cardinalities;
}

pair<vector<unsigned int>, double> NoisyTupleFileReader::next()
{
  if (membership == 0)
    {
      return pair<vector<unsigned int>, double>(vector<unsigned int>(), 0);
    }
  if (!symDimensionIds.empty())
    {
      // Checking whether the tuple is a self loop
      vector<unsigned int>::const_iterator symDimensionIdIt = symDimensionIds.begin();
      const unsigned int symmetricElement = *tupleIts[*symDimensionIdIt];
      while (++symDimensionIdIt != symDimensionIds.end() && *tupleIts[*symDimensionIdIt] == symmetricElement)
	{
	}
      if (symDimensionIdIt == symDimensionIds.end())
	{
	  // It is a self loop
	  nextTuple();
	  return next();
	}
    }
  pair<vector<unsigned int>, double> noisyTuple(vector<unsigned int>(), membership);
  noisyTuple.first.reserve(tupleIts.size());
  for (const vector<unsigned int>::const_iterator tupleIt : tupleIts)
    {
      noisyTuple.first.push_back(*tupleIt);
    }
  nextTuple();
  return noisyTuple;
}

void NoisyTupleFileReader::setMinMembership(const double minMembershipParam)
{
  minMembership = minMembershipParam;
}

void NoisyTupleFileReader::startOverFromNextLine()
{
  for (vector<string>& ids2LabelsInDimension : ids2Labels)
    {
      ids2LabelsInDimension.clear();
    }
  for (unordered_map<string, unsigned int>& labels2IdsInDimension : labels2Ids)
    {
      labels2IdsInDimension.clear();
    }
  symIds2Labels.clear();
  symLabels2Ids.clear();
  nextNSet();
}

void NoisyTupleFileReader::printTuplesInFirstDimensionHyperplane(ostream& out, const unsigned int firstDimensionHyperplaneId, const unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator begin, const unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator end, const string& outputDimensionSeparator) const
{
  string prefix;
  if (ids2Labels.front().empty())
    {
      // Symmetric dimension
      prefix = symIds2Labels[firstDimensionHyperplaneId] + outputDimensionSeparator;
    }
  else
    {
      // Non-symmetric dimension
      prefix = ids2Labels.front()[firstDimensionHyperplaneId] + outputDimensionSeparator;
    }
  for (unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator tupleIt = begin; tupleIt != end; ++tupleIt)
    {
      if (tupleIt != begin)
	{
	  out << endl;
	}
      out << prefix;
      vector<vector<string>>::const_iterator ids2LabelsIt = ids2Labels.begin();
      for (const unsigned int element : tupleIt->first)
	{
	  if ((++ids2LabelsIt)->empty())
	    {
	      // Symmetric dimension
	      out << symIds2Labels[element] << outputDimensionSeparator;
	    }
	  else
	    {
	      // Non-symmetric dimension
	      out << (*ids2LabelsIt)[element] << outputDimensionSeparator;
	    }
	}
      out << tupleIt->second;
    }
}

vector<string> NoisyTupleFileReader::setNewIdsAndGetLabels(const unsigned int dimensionId, const vector<unsigned int>& oldIds2NewIds, const unsigned int nbOfValidLabels)
{
  vector<string> labels(nbOfValidLabels);
  for (pair<const string, unsigned int>& label2Id : labels2Ids[dimensionId])
    {
      label2Id.second = oldIds2NewIds[label2Id.second];
      if (label2Id.second != numeric_limits<unsigned int>::max())
	{
	  labels[label2Id.second] = label2Id.first;
	}
    }
  return labels;
}

vector<string> NoisyTupleFileReader::setNewIdsAndGetSymmetricLabels(const vector<unsigned int>& oldIds2NewIds, const unsigned int nbOfValidLabels)
{
  vector<string> labels(nbOfValidLabels);
  for (pair<const string, unsigned int>& label2Id : symLabels2Ids)
    {
      label2Id.second = oldIds2NewIds[label2Id.second];
      if (label2Id.second != numeric_limits<unsigned int>::max())
	{
	  labels[label2Id.second] = label2Id.first;
	}
    }
  return labels;
}

unordered_map<string, unsigned int>&& NoisyTupleFileReader::captureLabels2Ids(const unsigned int dimensionId)
{
  return move(labels2Ids[dimensionId]);
}

unordered_map<string, unsigned int>&& NoisyTupleFileReader::captureSymmetricLabels2Ids()
{
  return move(symLabels2Ids);
}

void NoisyTupleFileReader::nextNSet()
{
  bool isLineToBeDisconsidered = true;
  while (isLineToBeDisconsidered)
    {
      if (noisyNSetFile.eof())
	{
	  noisyNSetFile.close();
	  membership = 0;
	  return;
	}
      ++lineNb;
      string noisyNSetString;
      getline(noisyNSetFile, noisyNSetString);
      tokenizer<char_separator<char>> dimensions(noisyNSetString, inputDimensionSeparator);
      if (dimensions.begin() != dimensions.end())
	{
#ifdef VERBOSE_PARSER
	  cout << noisyNSetFileName << ':' << lineNb << ": " << noisyNSetString << endl;
#endif
	  isLineToBeDisconsidered = parseLine(dimensions.begin(), dimensions.end());
	}
    }
}

const bool NoisyTupleFileReader::parseLine(const tokenizer<char_separator<char>>::const_iterator dimensionBegin, const tokenizer<char_separator<char>>::const_iterator dimensionEnd)
{
  tokenizer<char_separator<char>>::const_iterator dimensionIt = dimensionBegin;
  tokenizer<char_separator<char>>::const_iterator membershipIt = dimensionBegin;
  while (++dimensionIt != dimensionEnd)
    {
      membershipIt = dimensionIt;
    }
  try
    {
      membership = lexical_cast<double>(*membershipIt);
      if (membership < 0 || membership > 1)
	{
	  throw bad_lexical_cast();
	}
    }
  catch (bad_lexical_cast &)
    {
      throw DataFormatException(noisyNSetFileName.c_str(), lineNb, ("the membership, " + *membershipIt + ", should be a double in [0, 1]!").c_str());
    }
  unsigned int dimensionId = 0;
  vector<vector<string>>::iterator ids2LabelsIt = ids2Labels.begin();
  vector<unsigned int>::const_iterator symDimensionIdIt = symDimensionIds.begin();
  dimensionIt = dimensionBegin;
  vector<vector<unsigned int>>::iterator nSetIt = nSet.begin();
  for (unordered_map<string, unsigned int>& labels2IdsInDimension : labels2Ids)
    {
      nSetIt->clear();
      tokenizer<char_separator<char>> elements(*dimensionIt, inputElementSeparator);
      if (symDimensionIdIt != symDimensionIds.end() && *symDimensionIdIt == dimensionId)
	{
	  // Symmetric dimension
	  ++symDimensionIdIt;
	  for (const string& element : elements)
	    {
	      const pair<unordered_map<string, unsigned int>::const_iterator, bool> label2Id = symLabels2Ids.insert(pair<string, unsigned int>(element, symIds2Labels.size()));
	      if (label2Id.second)
		{
		  symIds2Labels.push_back(element);
		}
	      nSetIt->push_back(label2Id.first->second);
	    }
	}
      else
	{
	  // Non-symmetric dimension
	  for (const string& element : elements)
	    {
	      const pair<unordered_map<string, unsigned int>::const_iterator, bool> label2Id = labels2IdsInDimension.insert(pair<string, unsigned int>(element, ids2LabelsIt->size()));
	      if (label2Id.second)
		{
		  if (binary_search(numDimensionIds.begin(), numDimensionIds.end(), dimensionId))
		    {
		      try
			{
			  lexical_cast<double>(element);
			}
		      catch (bad_lexical_cast &)
			{
			  throw DataFormatException(noisyNSetFileName.c_str(), lineNb, (element + ", in dimension " + lexical_cast<string>(dimensionId) + " (almost-contiguous according to tau option) should be a double!").c_str());
			}
		    }
		  ids2LabelsIt->push_back(element);
		}
	      nSetIt->push_back(label2Id.first->second);
	    }
	}
      if (++dimensionIt == dimensionEnd)
	{
	  throw DataFormatException(noisyNSetFileName.c_str(), lineNb, ("less than the expected " + lexical_cast<string>(nSet.size()) + " dimensions!").c_str());
	}
      ++ids2LabelsIt;
      ++nSetIt;
      ++dimensionId;
    }
  if (dimensionIt != membershipIt)
    {
      throw DataFormatException(noisyNSetFileName.c_str(), lineNb, ("more than the expected " + lexical_cast<string>(nSet.size()) + " dimensions!").c_str());
    }
  if (membership < minMembership)
    {
      return true;
    }
  vector<vector<unsigned int>::const_iterator>::iterator tupleItsIt = tupleIts.begin();
  for (const vector<unsigned int>& dimension : nSet)
    {
      *tupleItsIt++ = dimension.begin();
    }
  return false;
}

void NoisyTupleFileReader::nextTuple()
{
  // Little-endian-like
  vector<vector<unsigned int>::const_iterator>::iterator tupleItsIt = tupleIts.begin();
  for (vector<vector<unsigned int>>::const_iterator nSetIt = nSet.begin(); nSetIt != nSet.end() && ++*tupleItsIt == nSetIt->end(); ++nSetIt)
    {
      *tupleItsIt++ = nSetIt->begin();
    }
  if (tupleItsIt == tupleIts.end())
    {
      nextNSet();
    }
}
