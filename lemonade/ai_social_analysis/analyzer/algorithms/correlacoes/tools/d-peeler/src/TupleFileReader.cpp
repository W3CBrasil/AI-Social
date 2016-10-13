// Copyright 2012,2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "TupleFileReader.h"

TupleFileReader::TupleFileReader(const vector<unsigned int>& symDimensionIdsParam,
                                 const char* inputDimensionSeparator,
                                 const char* inputItemSeparator,
                                 vector<map<const string, const unsigned int>>& labels2IdsParam)
    : nSetFileName(),
      nSetFile(),
      symDimensionIds(symDimensionIdsParam),
      inputDimensionSeparator(inputDimensionSeparator),
      inputItemSeparator(inputItemSeparator),
      lineNb(0),
      ids2Labels(),
      labels2Ids(labels2IdsParam),
      symIds2Labels(),
      symLabels2Ids(),
      nSet(),
      tupleIts(),
      eofFound(false),
      shouldCaptureLabels(true),
      symDimensionsPermutation()
{
}

void TupleFileReader::init() throw(UsageException, DataFormatException)
{
  if (nSetFile.eof())
    {
      nSetFile.close();
      eofFound = true;
      return;
    }
  ++lineNb;
  string nSetString;
  getline(nSetFile, nSetString);
  tokenizer<char_separator<char> > dimensions(nSetString, inputDimensionSeparator);
  tokenizer<char_separator<char> >::const_iterator dimensionIt = dimensions.begin();
  if (dimensionIt == dimensions.end())
    {
      init();
      return;
    }
  unsigned int n = 0;
  vector<unsigned int>::const_iterator symDimensionIdIt = symDimensionIds.begin();
  for (; dimensionIt != dimensions.end(); dimensionIt++, n++)
    {
      if (symDimensionIdIt != symDimensionIds.end() && *symDimensionIdIt == n)
	{
	  ++symDimensionIdIt;
	}
    }
  if (symDimensionIdIt != symDimensionIds.end())
    {
      throw UsageException(("clique option should provide attribute ids between 0 and " + lexical_cast<string>(n - 1)).c_str());
    }

  symDimensionsPermutation.resize(symDimensionIds.size());

  for (unsigned i = 0; i < symDimensionsPermutation.size(); i++)
      symDimensionsPermutation[i] = i;

  if (shouldCaptureLabels)
  {
    labels2Ids.resize(n);
    ids2Labels.resize(n);
    nSet.resize(n);
    tupleIts.resize(n);
  }
  else if (n != labels2Ids.size())
  {
      throw DataFormatException(nSetFileName.c_str(),
                                lineNb,
                                (string("Expected ") +
                                 nSetFileName +
                                 " to contain a " +
                                 lexical_cast<string>(labels2Ids.size()) +
                                 "-dimensional data set,  but its data is " +
                                 lexical_cast<string>(n) +
                                 "-dimensional instead.").c_str());
  }


#ifdef VERBOSE_PARSER
  cout << nSetFileName << ':' << lineNb << ": " << nSetString << endl;
#endif
  if (parseLine(dimensions.begin(), dimensions.end()))
    {
      nextNSet();
    }
}

void TupleFileReader::start(const char* nSetFileName)
                       throw(NoFileException, UsageException, DataFormatException)
{
    this->nSetFileName = nSetFileName;

    if (nSetFile.is_open())
        shouldCaptureLabels = false;

    nSetFile.open(nSetFileName);

    if (nSetFile.fail())
    {
      throw NoFileException(nSetFileName);
    }

    lineNb = 0;
    eofFound = false;
    init();
}

namespace
{
    vector<unsigned int>
    ApplySymmetricDimensionsPermutation(const vector<unsigned int> &tuple,
                                        const vector<unsigned int> &symDimensions,
                                        const vector<unsigned int> &permutation)
    {
        vector<unsigned int> newTuple(tuple.size());
        unsigned symDimensionIndex = 0;

        for (unsigned i = 0; i < tuple.size(); i++) {
            if (symDimensionIndex < symDimensions.size() &&
                    symDimensions[symDimensionIndex] == i) {
                newTuple[i] = tuple[symDimensions[permutation[symDimensionIndex]]];
                symDimensionIndex++;
            } else {
                newTuple[i] = tuple[i];
            }
        }

        return newTuple;
    }
}

vector<unsigned int> TupleFileReader::next() throw(DataFormatException)
{
    if (eofFound)
        return vector<unsigned int>();

    if (!symDimensionIds.empty())
    {
        // Checking whether the tuple is a self loop
        vector<unsigned int>::const_iterator symDimensionIdIt = symDimensionIds.begin();
        const unsigned int symmetricItem = *tupleIts[*symDimensionIdIt];
        while (++symDimensionIdIt != symDimensionIds.end() && *tupleIts[*symDimensionIdIt] == symmetricItem)
        {
        }
        if (symDimensionIdIt == symDimensionIds.end())
        {
            // It is a self loop
            nextTuple();
            return next();
        }
    }
    vector<unsigned int> tuple;
    tuple.reserve(tupleIts.size());
    for (const vector<unsigned int>::const_iterator tupleIt : tupleIts)
    {
        tuple.push_back(*tupleIt);
    }

    tuple = ApplySymmetricDimensionsPermutation(tuple, symDimensionIds, symDimensionsPermutation);

    // Advance the symmetric dimensions permutation of the current tuple
    // or reset the permutation and go to the next tuple
    if (!next_permutation(symDimensionsPermutation.begin(), symDimensionsPermutation.end())) {
        nextTuple();
    }

    return tuple;
}

void TupleFileReader::startOverFromNextLine()
{
  if (shouldCaptureLabels)
  {
    for (vector<string>& ids2LabelsInDimension : ids2Labels)
        {
        ids2LabelsInDimension.clear();
        }
    for (map<const string, const unsigned int>& labels2IdsInDimension : labels2Ids)
        {
        labels2IdsInDimension.clear();
        }
    symIds2Labels.clear();
    symLabels2Ids.clear();
  }
  nextNSet();
}

void TupleFileReader::printTuplesInFirstDimensionHyperplane(ostream& out, const unsigned int firstDimensionHyperplaneId, const map<vector<unsigned int>, double>::const_iterator begin, const map<vector<unsigned int>, double>::const_iterator end, const string& outputDimensionSeparator) const
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
  for (map<vector<unsigned int>, double>::const_iterator tupleIt = begin; tupleIt != end; ++tupleIt)
    {
      if (tupleIt != begin)
	{
	  out << endl;
	}
      out << prefix;
      vector<vector<string> >::const_iterator ids2LabelsIt = ids2Labels.begin();
      for (const unsigned int item : tupleIt->first)
	{
	  if ((++ids2LabelsIt)->empty())
	    {
	      // Symmetric dimension
	      out << symIds2Labels[item] << outputDimensionSeparator;
	    }
	  else
	    {
	      // Non-symmetric dimension
	      out << (*ids2LabelsIt)[item] << outputDimensionSeparator;
	    }
	}
      out << tupleIt->second;
    }
}

string TupleFileReader::captureLabel(const unsigned int dimensionId, const unsigned int itemId) const
{
  if (ids2Labels[dimensionId].empty())
    {
      return std::move(symIds2Labels[itemId]);
    }
  return std::move(ids2Labels[dimensionId][itemId]);
}

vector<map<const string, const unsigned int> > TupleFileReader::captureLabels2Ids()
{
  return std::move(labels2Ids);
}

vector<map<const string, const unsigned int> > TupleFileReader::captureLabels2Ids(const unsigned int firstSymmetricDimensionId)
{
  labels2Ids[firstSymmetricDimensionId] = std::move(symLabels2Ids);
  return std::move(labels2Ids);
}

void TupleFileReader::nextNSet() throw(DataFormatException)
{
  bool isLineToBeDisconsidered = true;
  while (isLineToBeDisconsidered)
    {
      if (nSetFile.eof())
	{
	  nSetFile.close();
          eofFound = true;
	  return;
	}
      ++lineNb;
      string nSetString;
      getline(nSetFile, nSetString);
      tokenizer<char_separator<char> > dimensions(nSetString, inputDimensionSeparator);
      if (dimensions.begin() == dimensions.end())
	{
	  nextNSet();
	  return;
	}
#ifdef VERBOSE_PARSER
      cout << nSetFileName << ':' << lineNb << ": " << nSetString << endl;
#endif
      isLineToBeDisconsidered = parseLine(dimensions.begin(), dimensions.end());
    }
}

const bool TupleFileReader::parseLine(const tokenizer<char_separator<char> >::const_iterator dimensionBegin, const tokenizer<char_separator<char> >::const_iterator dimensionEnd) throw(DataFormatException)
{
  tokenizer<char_separator<char> >::const_iterator dimensionIt = dimensionBegin;
  unsigned int dimensionId = 0;
  vector<vector<string> >::iterator ids2LabelsIt = ids2Labels.begin();
  vector<unsigned int>::const_iterator symDimensionIdIt = symDimensionIds.begin();
  dimensionIt = dimensionBegin;
  vector<vector<unsigned int> >::iterator nSetIt = nSet.begin();
  for (map<const string, const unsigned int>& labels2IdsInDimension : labels2Ids)
    {
      nSetIt->clear();
      tokenizer<char_separator<char> > items(*dimensionIt, inputItemSeparator);
      if (symDimensionIdIt != symDimensionIds.end() && *symDimensionIdIt == dimensionId)
	{
	  // Symmetric dimension
	  ++symDimensionIdIt;
	  for (const string& item : items)
	    {
	      const pair<map<const string, const unsigned int>::const_iterator, bool> label2Id = symLabels2Ids.insert(pair<const string, const unsigned int>(item, symIds2Labels.size()));
	      if (label2Id.second)
		{
		  symIds2Labels.push_back(item);
		}
	      nSetIt->push_back(label2Id.first->second);
	    }
	}
      else
	{
	  // Non-symmetric dimension
	  for (const string& item : items)
	    {
	      const pair<map<const string, const unsigned int>::const_iterator, bool> label2Id = labels2IdsInDimension.insert(pair<const string, const unsigned int>(item, ids2LabelsIt->size()));
	      if (label2Id.second)
		{
		  ids2LabelsIt->push_back(item);
		}
	      nSetIt->push_back(label2Id.first->second);
	    }
	}
      if (dimensionIt++ == dimensionEnd)
	{
	  throw DataFormatException(nSetFileName.c_str(), lineNb, ("less than the expected " + lexical_cast<string>(nSet.size()) + " dimensions!").c_str());
	}
      ++ids2LabelsIt;
      ++nSetIt;
      ++dimensionId;
    }
  vector<vector<unsigned int>::const_iterator>::iterator tupleItsIt = tupleIts.begin();
  for (const vector<unsigned int>& dimension : nSet)
    {
      *tupleItsIt++ = dimension.begin();
    }
  return false;
}

void TupleFileReader::nextTuple()
{
  // Little-endian-like
  vector<vector<unsigned int>::const_iterator>::iterator tupleItsIt = tupleIts.begin();
  for (vector<vector<unsigned int> >::const_iterator nSetIt = nSet.begin(); nSetIt != nSet.end() && ++*tupleItsIt == nSetIt->end(); ++nSetIt)
    {
      *tupleItsIt++ = nSetIt->begin();
    }
  if (tupleItsIt == tupleIts.end())
    {
      nextNSet();
    }
}

void TupleFileReader::copySymmetricDimensionsLabels2Ids()
{
    for (unsigned int i = 0; i < symDimensionIds.size(); i++) {
        labels2Ids[symDimensionIds[i]] = symLabels2Ids;
    }
}
