// Copyright 2007-2014 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include <boost/lexical_cast.hpp>
#include <exception>

#include "Itemset.h"

Itemset::Itemset():bitset()
{
}

Itemset::Itemset(const Itemset& otherItemset): AbstractHyperBitset(), bitset()
{
  copy(otherItemset);
}

Itemset::Itemset(const dynamic_bitset<>& bitsetParam): bitset(bitsetParam)
{
}

Itemset::Itemset(const unsigned int& size, const bool value): bitset()
{
  bitset.resize(size, value);
}

Itemset::~Itemset()
{
}

Itemset& Itemset::operator=(const Itemset& otherItemset)
{
  copy(otherItemset);
  return *this;
}

Itemset* Itemset::clone() const
{
  return new Itemset(*this);
}

void Itemset::copy(const Itemset& otherItemset)
{
  bitset = otherItemset.bitset;
}

Itemset& Itemset::bitwiseAndEqual(AbstractHyperBitset* otherAbstractHyperBitset)
{
  bitset &= static_cast<Itemset*>(otherAbstractHyperBitset)->bitset;
  return *this;
}

bool Itemset::includedIn(AbstractHyperBitset* otherAbstractHyperBitset) const
{
  return (bitset - static_cast<Itemset*>(otherAbstractHyperBitset)->bitset).none();
}

dynamic_bitset<>& Itemset::getBitset()
{
  return bitset;
}

unsigned int Itemset::nbOfDimensions() const
{
  return 1;
}

unsigned int Itemset::size() const
{
  return bitset.size();
}

void Itemset::setValue(const bool value)
{
  bitset.set(value);
}

void Itemset::setValue(const unsigned int& elementId, const bool value)
{
  bitset.set(elementId, value);
}

void Itemset::setValue(vector<unsigned int>::const_iterator begin,
                       vector<unsigned int>::const_iterator end,
                       bool value)
{
    if (begin + 1 == end)
        setValue(*begin, value);
    else
        throw std::invalid_argument(string("Setting a n-tuple"
                                    "in an 1-dimensional Itemset"));
}


void Itemset::setValue(const std::vector< unsigned int >& tuple, const bool value)
{
    if (tuple.size() == 1)
        setValue(tuple.front(), value);
    else
        throw std::invalid_argument(string("Setting a ") +
                                    lexical_cast<string>(tuple.size()) +
                                    "-tuple in an 1-dimensional Itemset");
}


void Itemset::setValues(const vector<vector<unsigned int> >& bits, const bool value)
{
  if (bits.empty())
    {
      setValue(value);
    }
  else
    {
      const vector<unsigned int>& projectedBits = bits.front();
      for (vector<unsigned int>::const_iterator projectedBitIt = projectedBits.begin(); projectedBitIt != projectedBits.end(); )
	{
	  setValue(*projectedBitIt++, value);
	}
    }
}

unsigned int Itemset::count() const
{
  return bitset.count();
}

vector<vector<vector<unsigned int> > > Itemset::unidimensionalNRectangles(const vector<vector<unsigned int> >::iterator& elementsIt, const vector<vector<unsigned int> >::const_iterator& firstElementsIt, const vector<vector<unsigned int> >::const_iterator& pastTheLastElementsIt) const
{
  if (bitset.none())
    {
      return vector<vector<vector<unsigned int> > >();
    }
  elementsIt->clear();
  for (dynamic_bitset<>::size_type elementId = bitset.find_first(); elementId != dynamic_bitset<>::npos; elementId = bitset.find_next(elementId))
    {
      elementsIt->push_back(elementId);
    }
  return vector<vector<vector<unsigned int> > >(1, vector<vector<unsigned int> >(firstElementsIt, pastTheLastElementsIt));
}

bool Itemset::enough0On(int& threshold, const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
  for (vector<unsigned int>::const_iterator elementIt = elementsIt->begin(); elementIt != elementsIt->end(); )
    {
      if (!bitset.test(*elementIt++) && (--threshold == -1))
	{
	  return true;
	}
    }
  return false;
}

bool Itemset::trueOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
  vector<unsigned int>::const_iterator elementIt = elementsIt->begin();
  for (; elementIt != elementsIt->end() && bitset.test(*elementIt); ++elementIt)
    {
    }
  return elementIt == elementsIt->end();
}

bool Itemset::falseOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
  vector<unsigned int>::const_iterator elementIt = elementsIt->begin();
  for (; elementIt != elementsIt->end() && !bitset.test(*elementIt); ++elementIt)
    {
    }
  return elementIt == elementsIt->end();
}

bool Itemset::trueOnPotentialBits(const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::const_iterator& potentialElementsIt, bool isPotentialElementUsed) const
{
  vector<unsigned int>::const_iterator elementIt = presentElementsIt->begin();
  if (isPotentialElementUsed)
    {
      for (; elementIt != presentElementsIt->end() && bitset.test(*elementIt); ++elementIt)
	{
	}
      if (elementIt != presentElementsIt->end())
	{
	  return false;
	}
    }
  for (elementIt = potentialElementsIt->begin(); elementIt != potentialElementsIt->end() && bitset.test(*elementIt); ++elementIt)
    {
    }
  return elementIt == potentialElementsIt->end();
}

unsigned int Itemset::countOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
  unsigned int total = 0;
  for (vector<unsigned int>::const_iterator elementIt = elementsIt->begin(); elementIt != elementsIt->end(); )
    {
      if (bitset.test(*elementIt++))
	{
	  ++total;
	}
    }
  return total;
}

unsigned int Itemset::countAndCropOn(const vector<vector<unsigned int> >::const_iterator& elementsIt, const vector<vector<bool> >::iterator& coveringElementsIt) const
{
  unsigned int total = 0;
  vector<unsigned int>::const_iterator elementIt = elementsIt->begin();
  for (vector<bool>::iterator coveringElementIt = coveringElementsIt->begin(); coveringElementIt != coveringElementsIt->end(); ++coveringElementIt)
    {
      if (bitset.test(*elementIt++))
	{
	  ++total;
	  *coveringElementIt = true;
	}
    }
  return total;
}

bool Itemset::setPresent(const vector<vector<unsigned int> >::iterator& newAbsentElementsIt, const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::iterator& potentialElementsIt, const bool isPotentialNotUsed) const
{
    if (isPotentialNotUsed)
    {
        for (vector<unsigned int>::iterator potentialElementIt = potentialElementsIt->begin(); potentialElementIt != potentialElementsIt->end(); )
        {
            if (!bitset.test(*potentialElementIt))
            {
                newAbsentElementsIt->push_back(*potentialElementIt);
                potentialElementIt = potentialElementsIt->erase(potentialElementIt);
            }
            else
            {
                ++potentialElementIt;
            }
        }
        return false;
    }
    else
    {
        for (vector<unsigned int>::const_iterator presentElementIt = presentElementsIt->begin(); presentElementIt != presentElementsIt->end(); )
        {
            if (!bitset.test(*presentElementIt++))
            {
                return true;
            }
        }
    }
    return false;
}

bool Itemset::densityLimitViolated() const
{
    // Itemset is dense; it's meant to be potentially full.
    return false;
}

AbstractHyperBitset* Itemset::cloneAsDenserHyperBitset() const
{
    // There's no bitset representation denser than Itemset
    return clone();
}
