// Copyright 2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MinGroupCoverLeverage.h"

vector<vector<float>> MinGroupCoverLeverage::thresholds;
vector<vector<float>> MinGroupCoverLeverage::weights;

MinGroupCoverLeverage::MinGroupCoverLeverage(const unsigned int numeratorGroupIdParam, const unsigned int denominatorGroupIdParam, const float threshold): GroupMeasure(), numeratorGroupId(numeratorGroupIdParam), denominatorGroupId(denominatorGroupIdParam)
{
  isSomeMeasureMonotone = true;
  isSomeMeasureAntiMonotone = true;
  if (numeratorGroupId >= thresholds.size())
    {
      thresholds.resize(numeratorGroupId + 1);
      weights.resize(numeratorGroupId + 1);
    }
  if (denominatorGroupId >= thresholds[numeratorGroupId].size())
    {
      thresholds[numeratorGroupId].resize(denominatorGroupId + 1);
      weights[numeratorGroupId].resize(denominatorGroupId + 1);
    }
  thresholds[numeratorGroupId][denominatorGroupId] = threshold;
  const unsigned int denominatorGroupSize = maxCoverOfGroup(denominatorGroupId);
  weights[numeratorGroupId][denominatorGroupId] = static_cast<float>(maxCoverOfGroup(numeratorGroupId)) / (denominatorGroupSize * denominatorGroupSize);
}

MinGroupCoverLeverage* MinGroupCoverLeverage::clone() const
{
  return new MinGroupCoverLeverage(*this);
}

const bool MinGroupCoverLeverage::violationAfterMinCoversIncreased() const
{
#ifdef DEBUG
  if (optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId])
    {
      cout << thresholds[numeratorGroupId][denominatorGroupId] << "-minimal cover leverage between group " << numeratorGroupId << " and group " << denominatorGroupId << " cannot be satisfied -> Prune!" << endl;
    }
#endif
  return optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId];
}

const bool MinGroupCoverLeverage::violationAfterMaxCoversDecreased() const
{
#ifdef DEBUG
  if (optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId])
    {
      cout << thresholds[numeratorGroupId][denominatorGroupId] << "-minimal cover leverage between group " << numeratorGroupId << " and group " << denominatorGroupId << " cannot be satisfied -> Prune!" << endl;
    }
#endif
  return optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId];
}

const float MinGroupCoverLeverage::optimisticValue() const
{
  const unsigned int maxCoverOfNumeratorGroup = maxCoverOfGroup(numeratorGroupId);
  const unsigned int minCoverOfDenominatorGroup = minCoverOfGroup(denominatorGroupId);
  if (maxCoverOfNumeratorGroup == 0)
    {
      return -numeric_limits<float>::infinity();
    }
  return static_cast<float>(maxCoverOfNumeratorGroup) / minCoverOfDenominatorGroup - weights[numeratorGroupId][denominatorGroupId] * minCoverOfDenominatorGroup;
}
