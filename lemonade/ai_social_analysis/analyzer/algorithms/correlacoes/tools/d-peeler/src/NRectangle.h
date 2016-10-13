// Copyright 2007-2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef NRECTANGLE_H_
#define NRECTANGLE_H_

#include <vector>
#include <boost/dynamic_bitset.hpp>

#include "Tree.h"

using namespace boost;
using namespace std;

class NRectangle
{
 public:
  NRectangle();
  NRectangle(const NRectangle& otherNRectangle);
  NRectangle(const vector<vector<unsigned int> >& bitsParam);
  ~NRectangle();

  NRectangle& operator=(const NRectangle& otherNRectangle);
  bool operator<(const NRectangle& otherNRectangle) const;
  friend ostream& operator<<(ostream& out, const NRectangle& nRectangle);

  bool empty() const;
  bool multidimensional() const;
  float coverRate() const;
  void setValues(const bool value = true) const;
  void resetValues() const;
  bool update();

 protected:
  vector<vector<unsigned int> > present;
  unsigned int coveredArea;
  unsigned int nbOfLogicOperators;

  void copy(const NRectangle& otherNRectangle);
};

#endif /*NRECTANGLE_H_*/
