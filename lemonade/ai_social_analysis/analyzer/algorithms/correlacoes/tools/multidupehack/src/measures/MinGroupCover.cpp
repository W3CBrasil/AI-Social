// Copyright 2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "MinGroupCover.h"

vector<unsigned int> MinGroupCover::thresholds;

MinGroupCover::MinGroupCover(const unsigned int groupIdParam, const unsigned int threshold): GroupMeasure(), groupId(groupIdParam)
{
  isSomeMeasureMonotone = true;
  if (groupId >= thresholds.size())
    {
      thresholds.resize(groupId + 1);
    }
  thresholds[groupId] = threshold;
}

MinGroupCover* MinGroupCover::clone() const
{
  return new MinGroupCover(*this);
}

const bool MinGroupCover::monotone() const
{
  return true;
}

const bool MinGroupCover::violationAfterMaxCoversDecreased() const
{
#ifdef DEBUG
  if (maxCoverOfGroup(groupId) < thresholds[groupId])
    {
      cout << thresholds[groupId] << "-minimal size constraint on group " << groupId << " cannot be satisfied -> Prune!" << endl;
    }
#endif
  return maxCoverOfGroup(groupId) < thresholds[groupId];
}

const float MinGroupCover::optimisticValue() const
{
  return maxCoverOfGroup(groupId);
}
