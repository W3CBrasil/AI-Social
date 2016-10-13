// Copyright 2007-2014 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include <boost/lexical_cast.hpp>
#include <exception>

#include "SparseItemset.h"
#include "Itemset.h"

using std::unordered_set;

double SparseItemset::densityLimit;

SparseItemset::SparseItemset(unsigned int bitsetSize) :
    sparseBitset(),
    bitsetSize(bitsetSize)
{
}

SparseItemset::SparseItemset(const SparseItemset& otherItemset) :
    AbstractHyperBitset(),
    sparseBitset(),
    bitsetSize(otherItemset.bitsetSize)
{
    copy(otherItemset);
}

SparseItemset::~SparseItemset()
{
}

SparseItemset& SparseItemset::operator=(const SparseItemset& otherItemset)
{
    copy(otherItemset);
    return *this;
}

bool SparseItemset::isPresent(unsigned int elementId) const
{
    return sparseBitset.count(elementId) > 0;
}

SparseItemset* SparseItemset::clone() const
{
    return new SparseItemset(*this);
}

void SparseItemset::copy(const SparseItemset& otherItemset)
{
    sparseBitset = otherItemset.sparseBitset;
}

SparseItemset& SparseItemset::bitwiseAndEqual(AbstractHyperBitset* otherAbstractHyperBitset)
{
    SparseItemset *other = dynamic_cast<SparseItemset*>(otherAbstractHyperBitset);

    if (other) {
        auto i = std::begin(sparseBitset);

        while (i != sparseBitset.end()) {
            if (!other->sparseBitset.count(*i)) {
                i = sparseBitset.erase(i);
            } else {
                i++;
            }
        }
    }

    return *this;
}

bool SparseItemset::includedIn(AbstractHyperBitset* otherAbstractHyperBitset) const
{
    SparseItemset *other = dynamic_cast<SparseItemset*>(otherAbstractHyperBitset);

    if (other) {
        for (unsigned i : sparseBitset)
            if (other->sparseBitset.count(i) == 0)
                return false;
        return true;
    }

    return false;
}

unsigned int SparseItemset::nbOfDimensions() const
{
    return 1;
}

unsigned int SparseItemset::size() const
{
    return bitsetSize;
}

void SparseItemset::setValue(const bool value)
{
    if (!value)
        sparseBitset.clear();
    // TODO check what to do if value is true according to the calls to this method
}

void SparseItemset::setValue(const unsigned int& elementId, const bool value)
{
    if (value)
        sparseBitset.insert(elementId);
    else
        sparseBitset.erase(elementId);
}

void SparseItemset::setValue(vector<unsigned int>::const_iterator begin,
        vector<unsigned int>::const_iterator end,
        bool value)
{
    if (begin + 1 == end)
        setValue(*begin, value);
    else
        throw std::invalid_argument(string("Setting a n-tuple"
                    "in an 1-dimensional SparseItemset"));
}

void SparseItemset::setValue(const std::vector< unsigned int >& tuple, const bool value)
{
    if (tuple.size() == 1)
        setValue(tuple.front(), value);
    else
        throw std::invalid_argument(string("Setting a ") +
                lexical_cast<string>(tuple.size()) +
                "-tuple in an 1-dimensional SparseItemset");
}

void SparseItemset::setValues(const vector<vector<unsigned int> >& bits, const bool value)
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

unsigned int SparseItemset::count() const
{
    return sparseBitset.size();
}

vector<vector<vector<unsigned int> > >
SparseItemset::unidimensionalNRectangles(const vector<vector<unsigned int> >::iterator& elementsIt,
        const vector<vector<unsigned int> >::const_iterator& firstElementsIt,
        const vector<vector<unsigned int> >::const_iterator& pastTheLastElementsIt) const
{
    if (sparseBitset.empty()) {
        return vector<vector<vector<unsigned int> > >();
    }
    elementsIt->clear();
    for (auto elementId : sparseBitset) {
        elementsIt->push_back(elementId);
    }
    return vector<vector<vector<unsigned int> > >(1,
            vector<vector<unsigned int> >(firstElementsIt, pastTheLastElementsIt));
}

bool SparseItemset::enough0On(int& threshold, const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
    for (vector<unsigned int>::const_iterator elementIt = elementsIt->begin(); elementIt != elementsIt->end(); )
    {
        if (!sparseBitset.count(*elementIt++) && (--threshold == -1))
        {
            return true;
        }
    }
    return false;
}

bool SparseItemset::trueOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
    vector<unsigned int>::const_iterator elementIt = elementsIt->begin();
    for (; elementIt != elementsIt->end() && sparseBitset.count(*elementIt); ++elementIt)
    {
    }
    return elementIt == elementsIt->end();
}

bool SparseItemset::falseOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
    vector<unsigned int>::const_iterator elementIt = elementsIt->begin();
    for (; elementIt != elementsIt->end() && !sparseBitset.count(*elementIt); ++elementIt)
    {
    }
    return elementIt == elementsIt->end();
}

bool SparseItemset::trueOnPotentialBits(const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::const_iterator& potentialElementsIt, bool isPotentialElementUsed) const
{
    vector<unsigned int>::const_iterator elementIt = presentElementsIt->begin();
    if (isPotentialElementUsed)
    {
        for (; elementIt != presentElementsIt->end() && sparseBitset.count(*elementIt); ++elementIt)
        {
        }
        if (elementIt != presentElementsIt->end())
        {
            return false;
        }
    }
    for (elementIt = potentialElementsIt->begin();
            elementIt != potentialElementsIt->end() &&
            sparseBitset.count(*elementIt);
            ++elementIt)
    {
    }
    return elementIt == potentialElementsIt->end();
}

unsigned int SparseItemset::countOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
    unsigned int total = 0;
    for (vector<unsigned int>::const_iterator elementIt = elementsIt->begin(); elementIt != elementsIt->end(); )
    {
        if (sparseBitset.count(*elementIt++))
        {
            ++total;
        }
    }
    return total;
}

unsigned int SparseItemset::countAndCropOn(const vector<vector<unsigned int> >::const_iterator& elementsIt, const vector<vector<bool> >::iterator& coveringElementsIt) const
{
    unsigned int total = 0;
    vector<unsigned int>::const_iterator elementIt = elementsIt->begin();
    for (vector<bool>::iterator coveringElementIt = coveringElementsIt->begin(); coveringElementIt != coveringElementsIt->end(); ++coveringElementIt)
    {
        if (sparseBitset.count(*elementIt++))
        {
            ++total;
            *coveringElementIt = true;
        }
    }
    return total;
}

bool SparseItemset::setPresent(const vector<vector<unsigned int> >::iterator& newAbsentElementsIt, const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::iterator& potentialElementsIt, const bool isPotentialNotUsed) const
{
    if (isPotentialNotUsed)
    {
        for (vector<unsigned int>::iterator potentialElementIt = potentialElementsIt->begin(); potentialElementIt != potentialElementsIt->end(); )
        {
            if (!sparseBitset.count(*potentialElementIt))
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
            if (!sparseBitset.count(*presentElementIt++))
            {
                return true;
            }
        }
    }

    return false;
}

bool SparseItemset::densityLimitViolated() const
{
    return count() > size()*densityLimit;
}

AbstractHyperBitset* SparseItemset::cloneAsDenserHyperBitset() const
{
    Itemset* itemset = new Itemset(bitsetSize);

    for (const unsigned i : sparseBitset)
        itemset->setValue(i, true);

    return itemset;
}

void SparseItemset::setDensityLimit(double limit)
{
    SparseItemset::densityLimit = limit;
}
