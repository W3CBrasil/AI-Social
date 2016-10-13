// Copyright 2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MinGroupCoverYulesY.h"

vector<vector<float>> MinGroupCoverYulesY::thresholds;
vector<unsigned int> MinGroupCoverYulesY::nbOfElementsInGroups;

MinGroupCoverYulesY::MinGroupCoverYulesY(const unsigned int numeratorGroupIdParam, const unsigned int denominatorGroupIdParam, const float threshold): GroupMeasure(), numeratorGroupId(numeratorGroupIdParam), denominatorGroupId(denominatorGroupIdParam)
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

MinGroupCoverYulesY* MinGroupCoverYulesY::clone() const
{
  return new MinGroupCoverYulesY(*this);
}

const bool MinGroupCoverYulesY::violationAfterMinCoversIncreased() const
{
#ifdef DEBUG
  if (optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId])
    {
      cout << thresholds[numeratorGroupId][denominatorGroupId] << "-minimal cover Yule's Y between group " << numeratorGroupId << " and group " << denominatorGroupId << " cannot be satisfied -> Prune!" << endl;
    }
#endif
  return optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId];
}

const bool MinGroupCoverYulesY::violationAfterMaxCoversDecreased() const
{
#ifdef DEBUG
  if (optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId])
    {
      cout << thresholds[numeratorGroupId][denominatorGroupId] << "-minimal cover Yule's Y between group " << numeratorGroupId << " and group " << denominatorGroupId << " cannot be satisfied -> Prune!" << endl;
    }
#endif
  return optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId];
}

const float MinGroupCoverYulesY::optimisticValue() const
{
  const unsigned int maxCoverOfNumeratorGroup = maxCoverOfGroup(numeratorGroupId);
  if (maxCoverOfNumeratorGroup == 0)
    {
      return -1;
    }
  const float tmp = sqrt(minCoverOfGroup(denominatorGroupId) * (nbOfElementsInGroups[numeratorGroupId] - maxCoverOfNumeratorGroup));
  const unsigned int nbOfElementsInBothGroups = nbOfElementsInGroups[numeratorGroupId] + nbOfElementsInGroups[denominatorGroupId];
  const unsigned int minCoverOfNumeratorGroup = minCoverOfGroup(numeratorGroupId);
  const float denominator = sqrt(minCoverOfNumeratorGroup * (nbOfElementsInBothGroups - maxCoverOfNumeratorGroup)) + tmp;
  if (denominator == 0)
    {
      return 1;
    }
  return (sqrt(maxCoverOfNumeratorGroup * (nbOfElementsInBothGroups - minCoverOfNumeratorGroup)) - tmp) / denominator;
}
