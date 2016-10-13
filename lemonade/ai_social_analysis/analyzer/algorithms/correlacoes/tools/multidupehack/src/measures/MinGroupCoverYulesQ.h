// Copyright 2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef MIN_GROUP_COVER_YULE_S_Q_H_
#define MIN_GROUP_COVER_YULE_S_Q_H_

#include "GroupMeasure.h"

class MinGroupCoverYulesQ final : public GroupMeasure
{
 public:
  MinGroupCoverYulesQ(const unsigned int numeratorGroupId, const unsigned int denominatorGroupId, const float threshold);
  MinGroupCoverYulesQ* clone() const;

  const bool violationAfterMinCoversIncreased() const;
  const bool violationAfterMaxCoversDecreased() const;
  const float optimisticValue() const;

 protected:
  unsigned int numeratorGroupId;
  unsigned int denominatorGroupId;

  static vector<vector<float>> thresholds;
  static vector<unsigned int> nbOfElementsInGroups;
};

#endif /*MIN_GROUP_COVER_YULE_S_Q_H_*/
