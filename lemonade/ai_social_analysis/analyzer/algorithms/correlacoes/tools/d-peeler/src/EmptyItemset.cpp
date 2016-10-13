// Copyright 2007-2014 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include <boost/lexical_cast.hpp>
#include <exception>

#include "EmptyItemset.h"
#include "SparseItemset.h"
#include "Itemset.h"

EmptyItemset::EmptyItemset(unsigned int bitsetSizeParam) :
    bitsetSize(bitsetSizeParam)
{
}

EmptyItemset& EmptyItemset::operator=(const EmptyItemset& otherItemset)
{
    return *this;
}

bool EmptyItemset::includedIn(AbstractHyperBitset* otherAbstractHyperBitset) const
{
    return true;
}

unsigned int EmptyItemset::nbOfDimensions() const
{
    return 1;
}

unsigned int EmptyItemset::size() const
{
    return bitsetSize;
}

AbstractHyperBitset& EmptyItemset::bitwiseAndEqual(AbstractHyperBitset* otherAbstractHyperBitset)
{
    return *this;
}

void EmptyItemset::setValue(const bool value)
{
    throw std::domain_error("Setting a value in an EmptyItemset.");
}

void EmptyItemset::setValue(const unsigned int& elementId, const bool value)
{
    throw std::domain_error("Setting a value in an EmptyItemset.");
}

void EmptyItemset::setValue(vector<unsigned int>::const_iterator begin,
                       vector<unsigned int>::const_iterator end,
                       bool value)
{
    throw std::domain_error("Setting a value in an EmptyItemset.");
}


void EmptyItemset::setValue(const std::vector< unsigned int >& tuple, const bool value)
{
    throw std::domain_error("Setting a value in an EmptyItemset.");
}


void EmptyItemset::setValues(const vector<vector<unsigned int> >& bits, const bool value)
{
    throw std::domain_error("Setting a value in an EmptyItemset.");
}

unsigned int EmptyItemset::count() const
{
    return 0;
}

vector<vector<vector<unsigned int> > > EmptyItemset::unidimensionalNRectangles(const vector<vector<unsigned int> >::iterator& elementsIt, const vector<vector<unsigned int> >::const_iterator& firstElementsIt, const vector<vector<unsigned int> >::const_iterator& pastTheLastElementsIt) const
{
      return vector<vector<vector<unsigned int> > >();
}

bool EmptyItemset::enough0On(int& threshold, const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
    threshold = max(-1, threshold - static_cast<int>(elementsIt->size()));
    return threshold == -1;
}

bool EmptyItemset::trueOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
    return elementsIt->size() == 0;
}

bool EmptyItemset::falseOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
    return true;
}

bool EmptyItemset::trueOnPotentialBits(const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::const_iterator& potentialElementsIt, bool isPotentialElementUsed) const
{
    return (!isPotentialElementUsed || presentElementsIt->size() == 0) &&
        potentialElementsIt->size() == 0;
}

unsigned int EmptyItemset::countOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
    return 0;
}

unsigned int EmptyItemset::countAndCropOn(const vector<vector<unsigned int> >::const_iterator& elementsIt, const vector<vector<bool> >::iterator& coveringElementsIt) const
{
    return 0;
}

bool EmptyItemset::setPresent(const vector<vector<unsigned int> >::iterator& newAbsentElementsIt,
                              const vector<vector<unsigned int> >::const_iterator& presentElementsIt,
                              const vector<vector<unsigned int> >::iterator& potentialElementsIt,
                              const bool isPotentialNotUsed) const
{
    if (isPotentialNotUsed)
    {
        newAbsentElementsIt->insert(newAbsentElementsIt->end(),
                                    potentialElementsIt->begin(),
                                    potentialElementsIt->end());
        potentialElementsIt->clear();
        return false;
    }
    else
    {
        return presentElementsIt->size() > 0;
    }
}

bool EmptyItemset::densityLimitViolated() const
{
    return true;
}

AbstractHyperBitset* EmptyItemset::clone() const
{
    return new EmptyItemset(*this);
}

AbstractHyperBitset* EmptyItemset::cloneAsDenserHyperBitset() const
{
    return new Itemset(bitsetSize);
}
