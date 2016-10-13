// Copyright 2013,2014,2015 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "GroupMeasure.h"

vector<GroupMeasure*> GroupMeasure::firstMeasures;
vector<GroupCovers*> GroupMeasure::groupCovers;
bool GroupMeasure::isSomeMeasureMonotone = false;
bool GroupMeasure::isSomeMeasureAntiMonotone = false;

GroupMeasure::GroupMeasure()
{
  if (firstMeasures.empty())
    {
      firstMeasures.push_back(this);
    }
}

GroupMeasure::GroupMeasure(const GroupMeasure& otherGroupMeasure)
{
  if (&otherGroupMeasure == firstMeasures.back())
    {
      firstMeasures.push_back(this);
      groupCovers.push_back(new GroupCovers(*(groupCovers.back())));
    }
}

GroupMeasure::GroupMeasure(GroupMeasure&& otherGroupMeasure)
{
}

GroupMeasure::~GroupMeasure()
{
  if (!firstMeasures.empty() && firstMeasures.back() == this)
    {
      firstMeasures.pop_back();
      if (!groupCovers.empty())
	{
	  delete groupCovers.back();
	  groupCovers.pop_back();
	}
    }
}

GroupMeasure& GroupMeasure::operator=(const GroupMeasure& otherGroupMeasure)
{
  if (&otherGroupMeasure == firstMeasures.back())
    {
      groupCovers.push_back(new GroupCovers(*(groupCovers.back())));
    }
  return *this;  
}

GroupMeasure& GroupMeasure::operator=(GroupMeasure&& otherGroupMeasure)
{
  return *this;  
}

void GroupMeasure::initGroups(const vector<string>& groupFileNames, const char* groupElementSeparator, const char* groupDimensionElementsSeparator, const vector<unsigned int>& cardinalities, const vector<unordered_map<string, unsigned int>>& labels2Ids, const vector<unsigned int>& dimensionOrder)
{
  groupCovers.push_back(new GroupCovers(groupFileNames, groupElementSeparator, groupDimensionElementsSeparator, cardinalities, labels2Ids, dimensionOrder));
}

void GroupMeasure::allMeasuresSet()
{
  // If unnecessary, clear the maximal group covers (in this way, do not copy them at every copy of the group cover measures)
  if (!isSomeMeasureMonotone)
    {
      groupCovers.back()->clearMaxCovers();
    }
  // If unnecessary, clear the minimal group covers (in this way, do not copy them at every copy of the group cover measures)
  if (!isSomeMeasureAntiMonotone)
    {
      groupCovers.back()->clearMinCovers();
    }
}

unsigned int GroupMeasure::minCoverOfGroup(const unsigned int groupId)
{
  return groupCovers.back()->minCoverOfGroup(groupId);
}

unsigned int GroupMeasure::maxCoverOfGroup(const unsigned int groupId)
{
  return groupCovers.back()->maxCoverOfGroup(groupId);
}

const bool GroupMeasure::violationAfterAdding(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent)
{
  if (isSomeMeasureAntiMonotone)
    {
      if (this == firstMeasures.back())
	{
	  groupCovers.back()->add(dimensionIdOfElementsSetPresent, elementsSetPresent);
	}
      return violationAfterMinCoversIncreased();
    }
  return false;
}

const bool GroupMeasure::violationAfterRemoving(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent)
{
  if (isSomeMeasureMonotone)
    {
      if (this == firstMeasures.back())
	{
	  groupCovers.back()->remove(dimensionIdOfElementsSetAbsent, elementsSetAbsent);
	}
      return violationAfterMaxCoversDecreased();
    }
  return false;
}

const bool GroupMeasure::violationAfterMinCoversIncreased() const
{
  return false;
}

const bool GroupMeasure::violationAfterMaxCoversDecreased() const
{
  return false;
}
