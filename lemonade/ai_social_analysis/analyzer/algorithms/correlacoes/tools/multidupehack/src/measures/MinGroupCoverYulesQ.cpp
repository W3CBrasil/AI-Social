// Copyright 2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MinGroupCoverYulesQ.h"

vector<vector<float>> MinGroupCoverYulesQ::thresholds;
vector<unsigned int> MinGroupCoverYulesQ::nbOfElementsInGroups;

MinGroupCoverYulesQ::MinGroupCoverYulesQ(const unsigned int numeratorGroupIdParam, const unsigned int denominatorGroupIdParam, const float threshold): GroupMeasure(), numeratorGroupId(numeratorGroupIdParam), denominatorGroupId(denominatorGroupIdParam)
{
  isSomeMeasureMonotone = true;
  isSomeMeasureAntiMonotone = true;
  if (numeratorGroupId >= thresholds.size())
    {
      thresholds.resize(numeratorGroupId + 1);
    }
  if (denominatorGroupId >= thresholds[numeratorGroupId].size())
    {
      thresholds[numeratorGroupId].resize(denominatorGroupId + 1);
    }
  thresholds[numeratorGroupId][denominatorGroupId] = threshold;
  const unsigned int maxGroupId = max(numeratorGroupId, denominatorGroupId);
  if (maxGroupId >= nbOfElementsInGroups.size())
    {
      nbOfElementsInGroups.resize(maxGroupId + 1);
    }
  nbOfElementsInGroups[numeratorGroupId] = maxCoverOfGroup(numeratorGroupId);
  nbOfElementsInGroups[denominatorGroupId] = maxCoverOfGroup(denominatorGroupId);
}

MinGroupCoverYulesQ* MinGroupCoverYulesQ::clone() const
{
  return new MinGroupCoverYulesQ(*this);
}

const bool MinGroupCoverYulesQ::violationAfterMinCoversIncreased() const
{
#ifdef DEBUG
  if (optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId])
    {
      cout << thresholds[numeratorGroupId][denominatorGroupId] << "-minimal cover Yule's Q between group " << numeratorGroupId << " and group " << denominatorGroupId << " cannot be satisfied -> Prune!" << endl;
    }
#endif
  return optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId];
}

const bool MinGroupCoverYulesQ::violationAfterMaxCoversDecreased() const
{
#ifdef DEBUG
  if (optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId])
    {
      cout << thresholds[numeratorGroupId][denominatorGroupId] << "-minimal cover Yule's Q between group " << numeratorGroupId << " and group " << denominatorGroupId << " cannot be satisfied -> Prune!" << endl;
    }
#endif
  return optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId];
}

const float MinGroupCoverYulesQ::optimisticValue() const
{
  const unsigned int maxCoverOfNumeratorGroup = maxCoverOfGroup(numeratorGroupId);
  if (maxCoverOfNumeratorGroup == 0)
    {
      return -1;
    }
  const unsigned int tmp = minCoverOfGroup(denominatorGroupId) * (nbOfElementsInGroups[numeratorGroupId] - maxCoverOfNumeratorGroup);
  const unsigned int nbOfElementsInBothGroups = nbOfElementsInGroups[numeratorGroupId] + nbOfElementsInGroups[denominatorGroupId];
  const unsigned int minCoverOfNumeratorGroup = minCoverOfGroup(numeratorGroupId);
  const unsigned int denominator = minCoverOfNumeratorGroup * (nbOfElementsInBothGroups - maxCoverOfNumeratorGroup) + tmp;
  if (denominator == 0)
    {
      return 1;
    }
  return static_cast<float>(maxCoverOfNumeratorGroup * (nbOfElementsInBothGroups - minCoverOfNumeratorGroup) - tmp) / denominator;
}
