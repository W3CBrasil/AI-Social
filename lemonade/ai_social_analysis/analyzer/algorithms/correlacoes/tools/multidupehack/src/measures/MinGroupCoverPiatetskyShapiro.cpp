// Copyright 2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MinGroupCoverPiatetskyShapiro.h"

vector<vector<float>> MinGroupCoverPiatetskyShapiro::thresholds;
vector<vector<float>> MinGroupCoverPiatetskyShapiro::weights;

MinGroupCoverPiatetskyShapiro::MinGroupCoverPiatetskyShapiro(const unsigned int numeratorGroupIdParam, const unsigned int denominatorGroupIdParam, const float threshold): GroupMeasure(), numeratorGroupId(numeratorGroupIdParam), denominatorGroupId(denominatorGroupIdParam)
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
  const float denominatorGroupSize = maxCoverOfGroup(denominatorGroupId);
  thresholds[numeratorGroupId][denominatorGroupId] = denominatorGroupSize * threshold;
  weights[numeratorGroupId][denominatorGroupId] = static_cast<float>(maxCoverOfGroup(numeratorGroupId)) / denominatorGroupSize;
}

MinGroupCoverPiatetskyShapiro* MinGroupCoverPiatetskyShapiro::clone() const
{
  return new MinGroupCoverPiatetskyShapiro(*this);
}

const bool MinGroupCoverPiatetskyShapiro::violationAfterMinCoversIncreased() const
{
#ifdef DEBUG
  if (optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId])
    {
      cout << thresholds[numeratorGroupId][denominatorGroupId] << "-minimal Piatetsky-Shapiro's measure between group " << numeratorGroupId << " and group " << denominatorGroupId << " cannot be satisfied -> Prune!" << endl;
    }
#endif
  return optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId];
}

const bool MinGroupCoverPiatetskyShapiro::violationAfterMaxCoversDecreased() const
{
#ifdef DEBUG
  if (optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId])
    {
      cout << thresholds[numeratorGroupId][denominatorGroupId] << "-minimal Piatetsky-Shapiro's measure between group " << numeratorGroupId << " and group " << denominatorGroupId << " cannot be satisfied -> Prune!" << endl;
    }
#endif
  return optimisticValue() < thresholds[numeratorGroupId][denominatorGroupId];
}

const float MinGroupCoverPiatetskyShapiro::optimisticValue() const
{
  return static_cast<float>(maxCoverOfGroup(numeratorGroupId)) - weights[numeratorGroupId][denominatorGroupId] * minCoverOfGroup(denominatorGroupId);
}
