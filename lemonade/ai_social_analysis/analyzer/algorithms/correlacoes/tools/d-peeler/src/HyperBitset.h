// Copyright 2007-2014 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef HYPERBITSET_H_
#define HYPERBITSET_H_

#include "AbstractHyperBitset.h"

class HyperBitset : public AbstractHyperBitset
{
 public:
  HyperBitset();
  HyperBitset(const HyperBitset& otherHyperBitset);
  HyperBitset(const vector<unsigned int>& nbOfElementsInDi,
              const bool value = false,
              AbstractHyperBitset* lastDimensionBitset = nullptr);
  ~HyperBitset();

  HyperBitset& operator=(const HyperBitset& otherHyperBitset);

  vector<AbstractHyperBitset*>& getHyperPlans();

  unsigned int nbOfDimensions() const;
  unsigned int size() const;
  void setValues(const vector<vector<unsigned int> >& bits, const bool value = true);
  void setValue(const vector<unsigned int> &tuple, const bool value = true);

  void setValue(vector<unsigned int>::const_iterator begin,
                vector<unsigned int>::const_iterator end,
                bool value);

  void setValue(const bool value = true);
  unsigned int count() const;
  vector<vector<vector<unsigned int> > > unidimensionalNRectangles(const vector<vector<unsigned int> >::iterator& elementsIt, const vector<vector<unsigned int> >::const_iterator& firstElementsIt, const vector<vector<unsigned int> >::const_iterator& pastTheLastElementsIt) const;
  bool includedIn(AbstractHyperBitset* otherAbstractHyperBitset) const;
  bool enough0On(int& threshold, const vector<vector<unsigned int> >::const_iterator& elementsIt) const;
  bool trueOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const;
  bool falseOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const;
  bool trueOnPotentialBits(const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::const_iterator& potentialElementsIt, bool isPotentialElementUsed = false) const;
  unsigned int countOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const;
  unsigned int countAndCropOn(const vector<vector<unsigned int> >::const_iterator& elementsIt, const vector<vector<bool> >::iterator& coveringElementsIt) const;
  bool setPresent(const vector<vector<unsigned int> >::iterator& newAbsentElementsIt, const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::iterator& potentialElementsIt, const bool isPotentialNotUsed = true) const;

  bool densityLimitViolated() const;
  AbstractHyperBitset* cloneAsDenserHyperBitset() const;

 protected:

  vector<AbstractHyperBitset*> hyperPlans;
  AbstractHyperBitset* sharedLastDimensionBitset;

  HyperBitset* clone() const;
  void copy(const HyperBitset& otherHyperBitset);

  /// If necessary, recreates a hyperplane with a denser representation to fit more values in it
  void recreateToFit(unsigned int hyperplaneIndex);

  HyperBitset& bitwiseAndEqual(AbstractHyperBitset* otherAbstractHyperBitset);
};

#endif /*HYPERBITSET_H_*/
