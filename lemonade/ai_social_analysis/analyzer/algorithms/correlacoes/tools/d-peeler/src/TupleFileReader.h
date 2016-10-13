// Copyright 2012,2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef TUPLE_FILE_READER_H_
#define TUPLE_FILE_READER_H_

#include <map>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/dynamic_bitset.hpp>

#include "../Parameters.h"
#include "vector_hash.h"
#include "UsageException.h"
#include "DataFormatException.h"
#include "NoFileException.h"

#ifdef VERBOSE_PARSER
#include <iostream>
#endif

using namespace std;
using namespace boost;

class TupleFileReader
{
 public:
  TupleFileReader(const vector<unsigned int>& symDimensionIdsParam,
                  const char* inputDimensionSeparator,
                  const char* inputItemSeparator,
                  vector<map<const string, const unsigned int>>& labels2Ids);

  TupleFileReader();

  ~TupleFileReader() = default;

  /// Opens a data file for reading n-sets
  /// \param nSetFileName path to the file to read from
  /// \param labels2Ids a vector of one map for each dimension that relates labels of items to identifiers.
  /// This parameter is updated during the parsing of the first data file (i.e. before the second call to
  /// start()). After that, it is used to translate the items in the next files to integers.
  void start(const char* nSetFileName)
        throw(NoFileException, UsageException, DataFormatException);

  /// Returns the next tuple in the file.
  /// The size of the vector is the dimensionality of the data set.
  /// If the end of file was reached, an empty vector is returned.
  vector<unsigned int> next() throw(DataFormatException);

  /// Copy the common map from labels to ids to all symmetric dimensions
  /// to the vector of maps passed in the constructor
  void copySymmetricDimensionsLabels2Ids();

  vector<vector<unsigned int> > nextSet();
  void startOverFromNextLine();

  void printTuplesInFirstDimensionHyperplane(ostream& out, const unsigned int firstDimensionHyperplaneId, const map<vector<unsigned int>, double>::const_iterator begin, const map<vector<unsigned int>, double>::const_iterator end, const string& outputDimensionSeparator) const;
  /* WARNING: front, next and printTuplesInFirstDimensionHyperplane cannot be called after the three following methods (move semantics) */
  string captureLabel(const unsigned int dimensionId, const unsigned int itemId) const;
  vector<map<const string, const unsigned int> > captureLabels2Ids(); /* when no dimension is symmetric */
  vector<map<const string, const unsigned int> > captureLabels2Ids(const unsigned int firstSymmetricDimensionId); /* when some dimensions are symmetric */

 protected:
  string nSetFileName;
  ifstream nSetFile;
  vector<unsigned int> symDimensionIds;
  char_separator<char> inputDimensionSeparator;
  char_separator<char> inputItemSeparator;
  unsigned int lineNb;
  vector<vector<string> > ids2Labels;
  vector<map<const string, const unsigned int> >& labels2Ids;
  vector<string> symIds2Labels;
  map<const string, const unsigned int> symLabels2Ids;
  vector<vector<unsigned int> > nSet;
  vector<vector<unsigned int>::const_iterator> tupleIts;

  /// Stores whether the end of the input file was found during a read operation,
  /// i.e. there are no more n-sets to read.
  bool eofFound;

  /// Stores whether labels should be captured from the current file or not.
  /// This is true while parsing the first file, and false after that.
  bool shouldCaptureLabels;

  /// The next permutation of the symmetric dimensions to use.
  /// All permutations are output before the next tuple is fetched.
  vector<unsigned int> symDimensionsPermutation;

  void init() throw(UsageException, DataFormatException);
  void nextNSet() throw(DataFormatException);
  const bool parseLine(const tokenizer<char_separator<char> >::const_iterator dimensionBegin, const tokenizer<char_separator<char> >::const_iterator dimensionEnd) throw(DataFormatException); /* returns whether the line is to be disconsidered (recursive calls to nextNSet can lead to a stack overflow) */
  void nextTuple();
};

#endif /*TUPLE_FILE_READER_H_*/
