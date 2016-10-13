// Copyright 2007-2014 Loïc (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "HyperBitset.h"
#include "SparseItemset.h"
#include "Itemset.h"
#include "EmptyItemset.h"

#include <iostream>

HyperBitset::HyperBitset()
    : hyperPlans(),
      sharedLastDimensionBitset(nullptr)
{
}

HyperBitset::HyperBitset(const HyperBitset& otherHyperBitset)
    : AbstractHyperBitset(),
      hyperPlans(),
      sharedLastDimensionBitset(otherHyperBitset.sharedLastDimensionBitset)
{
  copy(otherHyperBitset);
}

HyperBitset::HyperBitset(const vector<unsigned int>& nbOfElementsInDi,
                         const bool value,
                         AbstractHyperBitset* lastDimensionBitset)
    : hyperPlans(),
      sharedLastDimensionBitset(lastDimensionBitset)
{
    unsigned int n = nbOfElementsInDi.front();
    hyperPlans.reserve(n);
    if (nbOfElementsInDi.size() != 2)
    {
        vector<unsigned int> nbOfElementsInDj(nbOfElementsInDi.begin() + 1, nbOfElementsInDi.end());
        for (unsigned int i = 0; i < n; ++i)
        {
            hyperPlans.push_back(new HyperBitset(nbOfElementsInDj, value, lastDimensionBitset));
        }
    }
    else
    {
        for (unsigned int i = 0; i < n; ++i)
        {
            if (lastDimensionBitset) {
                hyperPlans.push_back(lastDimensionBitset);
            } else if (value) {
                hyperPlans.push_back(new Itemset(nbOfElementsInDi.back(), value));
            } else {
                hyperPlans.push_back(new EmptyItemset(nbOfElementsInDi.back()));
            }
        }
    }
}

HyperBitset::~HyperBitset()
{
  for (vector<AbstractHyperBitset*>::iterator hyperPlanIt = hyperPlans.begin(); hyperPlanIt != hyperPlans.end(); )
    {
        if (*hyperPlanIt != sharedLastDimensionBitset)
            delete *hyperPlanIt;

        hyperPlanIt++;
    }
}

HyperBitset* HyperBitset::clone() const
{
  return new HyperBitset(*this);
}

HyperBitset& HyperBitset::operator=(const HyperBitset& otherHyperBitset)
{
  copy(otherHyperBitset);
  return *this;
}

void HyperBitset::copy(const HyperBitset& otherHyperBitset)
{
  hyperPlans.reserve(otherHyperBitset.hyperPlans.size());
  for (vector<AbstractHyperBitset*>::const_iterator hyperPlanIt = otherHyperBitset.hyperPlans.begin(); hyperPlanIt != otherHyperBitset.hyperPlans.end(); )
    {
      hyperPlans.push_back((*hyperPlanIt++)->clone());
    }
}

bool HyperBitset::includedIn(AbstractHyperBitset* otherAbstractHyperBitset) const
{
  HyperBitset* otherHyperBitset = dynamic_cast<HyperBitset*>(otherAbstractHyperBitset);
  vector<AbstractHyperBitset*>::const_iterator otherHyperPlanIt = otherHyperBitset->hyperPlans.begin();
  vector<AbstractHyperBitset*>::const_iterator hyperPlanIt = hyperPlans.begin();
  for (; hyperPlanIt != hyperPlans.end() && (*hyperPlanIt)->includedIn(*otherHyperPlanIt++); ++hyperPlanIt)
    {
    }
  return hyperPlanIt == hyperPlans.end();
}

HyperBitset& HyperBitset::bitwiseAndEqual(AbstractHyperBitset* otherAbstractHyperBitset)
{
  vector<AbstractHyperBitset*>::const_iterator otherHyperPlanIt = dynamic_cast<HyperBitset*>(otherAbstractHyperBitset)->hyperPlans.begin();
  for (vector<AbstractHyperBitset*>::const_iterator hyperPlanIt = hyperPlans.begin(); hyperPlanIt != hyperPlans.end(); )
    {
      (*hyperPlanIt++)->bitwiseAndEqual(*otherHyperPlanIt++);
    }
  return *this;
}

vector<AbstractHyperBitset*>& HyperBitset::getHyperPlans()
{
  return hyperPlans;
}

unsigned int HyperBitset::nbOfDimensions() const
{
  return hyperPlans.back()->nbOfDimensions() + 1;
}

unsigned int HyperBitset::size() const
{
  return hyperPlans.size() * hyperPlans.back()->size();
}

void HyperBitset::setValue(const bool value)
{
  for (vector<AbstractHyperBitset*>::const_iterator hyperPlanIt = hyperPlans.begin(); hyperPlanIt != hyperPlans.end(); )
    {
      (*hyperPlanIt++)->setValue(value);
    }
}

void HyperBitset::setValue(const std::vector< unsigned int >& tuple, const bool value)
{
    setValue(tuple.begin(), tuple.end(), value);
}

void HyperBitset::recreateToFit(unsigned int hyperplaneIndex)
{
    while (hyperPlans[hyperplaneIndex]->densityLimitViolated()) {
        AbstractHyperBitset *newHyperPlane = hyperPlans[hyperplaneIndex]->cloneAsDenserHyperBitset();

        if (hyperPlans[hyperplaneIndex] != sharedLastDimensionBitset)
            delete hyperPlans[hyperplaneIndex];

        hyperPlans[hyperplaneIndex] = newHyperPlane;
    }
}

void HyperBitset::setValue(vector<unsigned int>::const_iterator begin,
                           vector<unsigned int>::const_iterator end,
                           bool value)
{
    if (value)
        recreateToFit(*begin);

    hyperPlans[*begin]->setValue(begin + 1, end, value);
}


void HyperBitset::setValues(const vector<vector<unsigned int> >& bits, const bool value)
{
    if (bits.empty())
    {
        setValue(value);
    }
    else
    {
        const vector<vector<unsigned int> > subBits(bits.begin() + 1, bits.end());
        const vector<unsigned int>& projectedBits = bits.front();
        for (vector<unsigned int>::const_iterator projectedBitIt = projectedBits.begin(); projectedBitIt != projectedBits.end(); )
        {
            if (value)
                recreateToFit(*projectedBitIt);

            hyperPlans[*projectedBitIt++]->setValues(subBits, value);
        }
    }
}

unsigned int HyperBitset::count() const
{
  unsigned int total = 0;
  for (vector<AbstractHyperBitset*>::const_iterator hyperPlanIt = hyperPlans.begin(); hyperPlanIt != hyperPlans.end(); )
    {
      total += (*hyperPlanIt++)->count();
    }
  return total;
}

vector<vector<vector<unsigned int> > > HyperBitset::unidimensionalNRectangles(const vector<vector<unsigned int> >::iterator& elementsIt, const vector<vector<unsigned int> >::const_iterator& firstElementsIt, const vector<vector<unsigned int> >::const_iterator& pastTheLastElementsIt) const
{
  const vector<vector<unsigned int> >::iterator nextElementsIt = elementsIt + 1;
  vector<vector<vector<unsigned int> > > unidimensionalNRectanglesVector;
  unsigned int hyperPlanId = 0;
  for (vector<AbstractHyperBitset*>::const_iterator hyperPlanIt = hyperPlans.begin(); hyperPlanIt != hyperPlans.end(); )
    {
      elementsIt->clear();
      elementsIt->push_back(hyperPlanId++);
      vector<vector<vector<unsigned int> > > unidimensionalNRectanglesInHyperPlan = (*hyperPlanIt++)->unidimensionalNRectangles(nextElementsIt, firstElementsIt, pastTheLastElementsIt);
      unidimensionalNRectanglesVector.insert(unidimensionalNRectanglesVector.end(), unidimensionalNRectanglesInHyperPlan.begin(), unidimensionalNRectanglesInHyperPlan.end());
    }
  return unidimensionalNRectanglesVector;
}

bool HyperBitset::enough0On(int& threshold, const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
  const vector<vector<unsigned int> >::const_iterator nextElementsIt = elementsIt + 1;
  for (vector<unsigned int>::const_iterator elementIt = elementsIt->begin(); elementIt != elementsIt->end() && !hyperPlans[*elementIt]->enough0On(threshold, nextElementsIt); ++elementIt)
    {
    }
  return threshold == -1;
}

bool HyperBitset::trueOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
  const vector<vector<unsigned int> >::const_iterator nextElementsIt = elementsIt + 1;
  vector<unsigned int>::const_iterator elementIt = elementsIt->begin();
  for (; elementIt != elementsIt->end() && hyperPlans[*elementIt]->trueOn(nextElementsIt); ++elementIt)
    {
    }
  return elementIt == elementsIt->end();
}

bool HyperBitset::falseOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
  const vector<vector<unsigned int> >::const_iterator nextElementsIt = elementsIt + 1;
  vector<unsigned int>::const_iterator elementIt = elementsIt->begin();
  for (; elementIt != elementsIt->end() && hyperPlans[*elementIt]->falseOn(nextElementsIt); ++elementIt)
    {
    }
  return elementIt == elementsIt->end();
}

bool HyperBitset::trueOnPotentialBits(const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::const_iterator& potentialElementsIt, bool isPotentialElementUsed) const
{
  const vector<vector<unsigned int> >::const_iterator nextPresentElementsIt = presentElementsIt + 1;
  const vector<vector<unsigned int> >::const_iterator nextPotentialElementsIt = potentialElementsIt + 1;
  vector<unsigned int>::const_iterator elementIt = presentElementsIt->begin();
  for (; elementIt != presentElementsIt->end() && hyperPlans[*elementIt]->trueOnPotentialBits(nextPresentElementsIt, nextPotentialElementsIt, isPotentialElementUsed); ++elementIt)
    {
    }
  if (elementIt != presentElementsIt->end())
    {
      return false;
    }
  for (elementIt = potentialElementsIt->begin(); elementIt != potentialElementsIt->end() && hyperPlans[*elementIt]->trueOnPotentialBits(nextPresentElementsIt, nextPotentialElementsIt, true); ++elementIt)
    {
    }
  return elementIt == potentialElementsIt->end();
}

unsigned int HyperBitset::countOn(const vector<vector<unsigned int> >::const_iterator& elementsIt) const
{
  unsigned int total = 0;
  const vector<vector<unsigned int> >::const_iterator nextElementsIt = elementsIt + 1;
  for (vector<unsigned int>::const_iterator elementIt = elementsIt->begin(); elementIt != elementsIt->end(); )
    {
      total += hyperPlans[*elementIt++]->countOn(nextElementsIt);
    }
  return total;
}

unsigned int HyperBitset::countAndCropOn(const vector<vector<unsigned int> >::const_iterator& elementsIt, const vector<vector<bool> >::iterator& coveringElementsIt) const
{
  unsigned int total = 0;
  const vector<vector<unsigned int> >::const_iterator nextElementsIt = elementsIt + 1;
  const vector<vector<bool> >::iterator nextCoveringElementsIt = coveringElementsIt + 1;
  vector<unsigned int>::const_iterator elementIt = elementsIt->begin();
  for (vector<bool>::iterator coveringElementIt = coveringElementsIt->begin(); coveringElementIt != coveringElementsIt->end(); ++coveringElementIt)
    {
      unsigned int partialCount = hyperPlans[*elementIt++]->countAndCropOn(nextElementsIt, nextCoveringElementsIt);
      if (partialCount != 0)
	{
	  total += partialCount;
	  *coveringElementIt = true;
	}
    }
  return total;
}

bool HyperBitset::setPresent(const vector<vector<unsigned int> >::iterator& newAbsentElementsIt, const vector<vector<unsigned int> >::const_iterator& presentElementsIt, const vector<vector<unsigned int> >::iterator& potentialElementsIt, const bool isPotentialNotUsed) const
{
  const vector<vector<unsigned int> >::iterator nextNewAbsentElementsIt = newAbsentElementsIt + 1;
  const vector<vector<unsigned int> >::const_iterator nextPresentElementsIt = presentElementsIt + 1;
  const vector<vector<unsigned int> >::iterator nextPotentialElementsIt = potentialElementsIt + 1;
  for (vector<unsigned int>::const_iterator presentElementIt = presentElementsIt->begin(); presentElementIt != presentElementsIt->end(); )
    {
      if (hyperPlans[*presentElementIt++]->setPresent(nextNewAbsentElementsIt, nextPresentElementsIt, nextPotentialElementsIt, isPotentialNotUsed))
	{
	  return true;
	}
    }
  if (isPotentialNotUsed)
    {
      for (vector<unsigned int>::iterator potentialElementIt = potentialElementsIt->begin(); potentialElementIt != potentialElementsIt->end(); )
	{
	  if (hyperPlans[*potentialElementIt]->setPresent(nextNewAbsentElementsIt, nextPresentElementsIt, nextPotentialElementsIt, false))
	    {
	      newAbsentElementsIt->push_back(*potentialElementIt);
	      potentialElementIt = potentialElementsIt->erase(potentialElementIt);
	    }
	  else
	    {
	      ++potentialElementIt;
	    }
	}
    }
  return false;
}

bool HyperBitset::densityLimitViolated() const
{
    return false;
}

AbstractHyperBitset* HyperBitset::cloneAsDenserHyperBitset() const
{
    return clone();
}
