// Copyright 2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef GROUP_MEASURE_H
#define GROUP_MEASURE_H

#include "Measure.h"
#include "GroupCovers.h"

class GroupMeasure : public Measure
{
 public:
  GroupMeasure();
  GroupMeasure(const GroupMeasure& otherGroupMeasure);
  GroupMeasure(GroupMeasure&& otherGroupMeasure);
  virtual ~GroupMeasure();

  GroupMeasure& operator=(const GroupMeasure& otherGroupMeasure);
  GroupMeasure& operator=(GroupMeasure&& otherGroupMeasure);

  static void initGroups(const vector<string>& groupFileNames, const char* groupElementSeparator, const char* groupDimensionElementsSeparator, const vector<unsigned int>& cardinalities, const vector<unordered_map<string, unsigned int>>& labels2Ids, const vector<unsigned int>& dimensionOrder);
  static void allMeasuresSet(); /* must be called after the construction of all group measures in (SkyPattern)Tree::initMeasures */
  static unsigned int minCoverOfGroup(const unsigned int groupId);
  static unsigned int maxCoverOfGroup(const unsigned int groupId);

  const bool violationAfterAdding(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent);
  const bool violationAfterRemoving(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent);

 protected:
  static vector<GroupMeasure*> firstMeasures;
  static vector<GroupCovers*> groupCovers;
  static bool isSomeMeasureMonotone;
  static bool isSomeMeasureAntiMonotone;

  virtual const bool violationAfterMinCoversIncreased() const;
  virtual const bool violationAfterMaxCoversDecreased() const;
};

#endif /*GROUP_MEASURE_H*/
