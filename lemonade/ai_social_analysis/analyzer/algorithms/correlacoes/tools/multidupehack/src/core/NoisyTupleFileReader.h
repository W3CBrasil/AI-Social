// Copyright 2012,2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef NOISY_TUPLE_FILE_READER_H_
#define NOISY_TUPLE_FILE_READER_H_

#include <map>
#include <unordered_map>
#include <fstream>
#include <boost/tokenizer.hpp>

#include "../../Parameters.h"
#include "../utilities/vector_hash.h"
#include "../utilities/UsageException.h"
#include "../utilities/DataFormatException.h"
#include "../utilities/NoFileException.h"

#ifdef VERBOSE_PARSER
#include <iostream>
#endif

using namespace boost;

class NoisyTupleFileReader
{
 public:
  NoisyTupleFileReader(const char* noisyNSetFileName, const vector<unsigned int>& symDimensionIdsParam, const vector<unsigned int>& numDimensionIdsParam, const char* inputDimensionSeparator, const char* inputElementSeparator);

  const vector<string>& getIds2Labels(const unsigned int dimensionId) const;
  vector<unsigned int> getCardinalities() const;

  pair<vector<unsigned int>, double> next();
  void setMinMembership(const double minMembership);
  void startOverFromNextLine();

  void printTuplesInFirstDimensionHyperplane(ostream& out, const unsigned int firstDimensionHyperplaneId, const unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator begin, const unordered_map<vector<unsigned int>, double, vector_hash<unsigned int>>::const_iterator end, const string& outputDimensionSeparator) const;
  /* WARNING: next and printTuplesInFirstDimensionHyperplane cannot be called after the following methods */
  vector<string> setNewIdsAndGetLabels(const unsigned int dimensionId, const vector<unsigned int>& oldIds2NewIds, const unsigned int nbOfValidLabels);
  vector<string> setNewIdsAndGetSymmetricLabels(const vector<unsigned int>& oldIds2NewIds, const unsigned int nbOfValidLabels);
  unordered_map<string, unsigned int>&& captureLabels2Ids(const unsigned int dimensionId);
  unordered_map<string, unsigned int>&& captureSymmetricLabels2Ids();

 protected:
  string noisyNSetFileName;
  ifstream noisyNSetFile;
  vector<unsigned int> symDimensionIds;
  vector<unsigned int> numDimensionIds;
  double minMembership;
  char_separator<char> inputDimensionSeparator;
  char_separator<char> inputElementSeparator;
  unsigned int lineNb;
  vector<vector<string>> ids2Labels;
  vector<unordered_map<string, unsigned int>> labels2Ids;
  vector<string> symIds2Labels;
  unordered_map<string, unsigned int> symLabels2Ids;
  vector<vector<unsigned int>> nSet;
  double membership;
  vector<vector<unsigned int>::const_iterator> tupleIts;

  /* The two following method set membership to 1 if and only if the end of file is met */
  void init();
  void nextNSet();
  const bool parseLine(const tokenizer<char_separator<char>>::const_iterator dimensionBegin, const tokenizer<char_separator<char>>::const_iterator dimensionEnd); /* returns whether the line is to be disconsidered (recursive calls to nextNSet can lead to a stack overflow) */
  void nextTuple();
};

#endif /*NOISY_TUPLE_FILE_READER_H_*/
