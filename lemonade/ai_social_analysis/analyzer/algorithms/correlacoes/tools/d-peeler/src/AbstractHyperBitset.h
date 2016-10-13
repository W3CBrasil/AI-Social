// Copyright 2007-2014 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef ABSTRACTHYPERBITSET_H_
#define ABSTRACTHYPERBITSET_H_

#include <vector>
#include <boost/dynamic_bitset.hpp>

using namespace boost;
using namespace std;

class AbstractHyperBitset
{
 public:
  AbstractHyperBitset();
  virtual ~AbstractHyperBitset();
  virtual AbstractHyperBitset* clone() const = 0;

  AbstractHyperBitset& operator&=(AbstractHyperBitset& otherAbstractHyperBitset);
  bool operator<(AbstractHyperBitset& otherAbstractHyperBitset) const;

  virtual AbstractHyperBitset& bitwiseAndEqual(AbstractHyperBitset* otherAbstractHyperBitset) = 0;
  virtual unsigned int nbOfDimensions() const = 0;
  virtual unsigned int size() const = 0;
  virtual float density() const;
  virtual void setValues(const vector<vector<unsigned int> >& bits, const bool value = true) = 0;
  virtual void setValue(const vector<unsigned int> &tuple, const bool value = true) = 0;

  /// Sets the value of a single tuple
  /// \param begin An iterator pointing to the first dimension of the tuple
  /// \param end An iterator pointing to one past the last dimension of the tuple
  /// \param value Whether the tuple is present (true) or absent (false)
  virtual void setValue(vector<unsigned int>::const_iterator begin, vector<unsigned int>::const_iterator end, bool value) = 0;

  virtual void setValue(const bool value = true) = 0;
  void resetValue();
  void resetValues(const vector<vector<unsigned int> >& bits);
  virtual unsigned int count() const = 0;
  vector<vector<vector<unsigned int> > > unidimensionalNRectangles() const;
  virtual vector<vector<vector<unsigned int> > > unidimensionalNRectangles(const vector<vector<unsigned int> >::iterator& elementsIt, const vector<vector<unsigned int> >::const_iterator& firstElementsIt, const vector<vector<unsigned int> >::const_iterator& pastTheLastElementsIt) const = 0;
  virtual bool includedIn(AbstractHyperBitset* otherAbstractHyperBitset) const = 0;
  bool enough0On(const unsigned int threshold, const vector<vector<unsigned int> >& subset) const;
  virtual bool enough0On(int& threshold, const vector<vector<unsigned int> >::const_iterator& elementsIt) const = 0;
  bool trueOn(const vector<vector<unsigned int> >& subset) const;
  virtual bool trueOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const = 0;
  bool falseOn(const vector<vector<unsigned int> >& subset) const;
  virtual bool falseOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const = 0;
  bool trueOnPotentialBits(const vector<vector<unsigned int> >& present, const vector<vector<unsigned int> >& potential) const;
  virtual bool trueOnPotentialBits(const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::const_iterator& potentialElementsIt, bool isPotentialElementUsed = false) const = 0;
  unsigned int countOn(const vector<vector<unsigned int> >& subset) const;
  virtual unsigned int countOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const = 0;
  pair<unsigned int, vector<vector<unsigned int> > > countAndCropOn(const vector<vector<unsigned int> >& subset, const vector<unsigned int>& nbOfElementsInDi) const;
  virtual unsigned int countAndCropOn(const vector<vector<unsigned int> >::const_iterator& elementsIt, const vector<vector<bool> >::iterator& coveringElementsIt) const = 0;
  vector<vector<unsigned int> > setPresent(const vector<vector<unsigned int> >& present, vector<vector<unsigned int> >& potential) const;
  virtual bool setPresent(const vector<vector<unsigned int> >::iterator& newAbsentElementsIt, const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::iterator& potentialElementsIt, const bool isPotentialNotUsed = true) const = 0;

  /// Returns whether this HyperBitset is storing more items than it should based on its density constraints
  /// This indicates that this HyperBitset should be copied into a denser
  /// representation of it, using copyToDenserHyperBitset()
  virtual bool densityLimitViolated() const = 0;

  /// Copy this HyperBitset to a denser representation of it
  /// This will be used to obtain a new HyperBitset when the HyperBitset's density
  /// limit is reached.
  /// The caller is responsible for freeing the returned pointer.
  virtual AbstractHyperBitset* cloneAsDenserHyperBitset() const = 0;

 protected:
};

#endif /*ABSTRACTHYPERBITSET_H_*/
