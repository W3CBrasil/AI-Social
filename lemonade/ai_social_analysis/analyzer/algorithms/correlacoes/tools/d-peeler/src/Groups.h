// Copyright 2012,2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef GROUP_H_
#define GROUP_H_

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/dynamic_bitset.hpp>

#include "../Parameters.h"
#include "NoFileException.h"
#include "DataFormatException.h"
#include "UsageException.h"

using namespace boost;
using namespace std;

class Groups
{
protected:
  vector<unsigned int> sizeOfInterPresent;
  vector<unsigned int> sizeOfInterPresentAndPotential;

  /* These three static members are never modified after initialized */
  static vector<vector<dynamic_bitset<> > > groups;
  static vector<vector<float> > minRatios;
  static vector<unsigned int> maxSizes;

public:
  Groups();
  Groups(const Groups& otherGroups);
  Groups(const vector<string>& groupFileNames, const char* groupItemSeparator, const char* groupDimensionItemsSeparator, const vector<map<const string, const unsigned int> >& labels2Ids, const vector<unsigned int>& dimensionOrder, const vector<unsigned int>& groupMinSizes, const vector<unsigned int>& groupMaxSizes, const char* groupMinRatiosFileName) throw(UsageException, DataFormatException, NoFileException);
  ~Groups() throw();

  void setPresent(const int dimensionId, const unsigned int itemId);
  void setPresent(const int dimensionId, const vector<unsigned int>& itemIds);
  void setAbsent(const int dimensionId, const unsigned int itemId);
  void setAbsent(const vector<vector<unsigned int> >& itemIds);
  const bool constraintsSatisfied() const; /* PERF?: The relevant constraints could, instead, be checked in every setPresent/setAbsent method */
};

#endif /*GROUP_H_*/
