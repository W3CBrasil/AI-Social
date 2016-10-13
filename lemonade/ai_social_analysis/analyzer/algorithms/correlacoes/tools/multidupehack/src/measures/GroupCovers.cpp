// Copyright 2012,2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "GroupCovers.h"

vector<vector<dynamic_bitset<>>> GroupCovers::groups;

void printCovers(ostream& out, const vector<unsigned int>& covers)
{
  out << '(';
  bool isFirst = true;
  for (const unsigned int cover : covers)
    {
      if (isFirst)
	{
	  isFirst = false;
	}
      else
	{
	  out << ',';
	}
      out << cover;
    }
  out << ')';
}

GroupCovers::GroupCovers(const vector<string>& groupFileNames, const char* groupElementSeparator, const char* groupDimensionElementsSeparator, const vector<unsigned int>& cardinalities, const vector<unordered_map<string, unsigned int>>& labels2Ids, const vector<unsigned int>& dimensionOrder): minCovers(), maxCovers()
{
  const char_separator<char> elementSeparator(groupElementSeparator);
  const char_separator<char> dimensionElementsSeparator(groupDimensionElementsSeparator);
  const unsigned int n = cardinalities.size();
  minCovers.resize(groupFileNames.size());
  maxCovers.reserve(groupFileNames.size());
  groups.resize(groupFileNames.size());
  vector<vector<dynamic_bitset<>>>::iterator groupIt = groups.begin();
  for (const string& fileName : groupFileNames)
    {
      ifstream file(fileName.c_str());
      if (!file)
	{
	  throw NoFileException(fileName.c_str());
	}
      groupIt->reserve(n);
      for (const unsigned int cardinality : cardinalities)
	{
	  groupIt->push_back(dynamic_bitset<>(cardinality));
	}
      unsigned int lineNb = 0;
      while (!file.eof())
	{
	  ++lineNb;
	  string dataString;
	  getline(file, dataString);
#ifdef VERBOSE_PARSER
	  cout << fileName << ':' << lineNb << ": " << dataString << endl;
#endif
	  tokenizer<char_separator<char>> dimensionElements(dataString, dimensionElementsSeparator);
	  tokenizer<char_separator<char>>::const_iterator dimensionElementsIt = dimensionElements.begin();
	  if (dimensionElementsIt != dimensionElements.end())
	    {
	      try
		{
		  const unsigned int dimensionId = lexical_cast<unsigned int>(*dimensionElementsIt);
		  if (dimensionId >= n)
		    {
		      throw DataFormatException(fileName.c_str(), lineNb, ("dimension " + lexical_cast<string>(dimensionId) + " does not exist! (at most " + lexical_cast<string>(n - 1) + " expected)").c_str());
		    }
		  if (++dimensionElementsIt != dimensionElements.end())
		    {
		      const unsigned int internalDimensionId = dimensionOrder[dimensionId];
		      dynamic_bitset<>& view = (*groupIt)[internalDimensionId];
		      const unordered_map<string, unsigned int>& labels2IdsView = labels2Ids[internalDimensionId];
		      tokenizer<char_separator<char>> elements(*dimensionElementsIt, elementSeparator);
		      for (const string& element : elements)
			{
			  const unordered_map<string, unsigned int>::const_iterator labels2IdsViewIt = labels2IdsView.find(element);
			  if (labels2IdsViewIt == labels2IdsView.end())
			    {
			      cerr << "Warning: ignoring " << element << " at line " << lineNb << " of " << fileName << " because it is absent from dimension " << dimensionId << " of the input data" << endl;
			    }
			  else
			    {
			      if (labels2IdsViewIt->second != numeric_limits<unsigned int>::max())
				{
				  view.set(labels2IdsViewIt->second);
				}
			    }
			}
		    }
		}
	      catch (bad_lexical_cast &)
		{
		  throw DataFormatException(fileName.c_str(), lineNb, ("the dimension should be an integer between 0 and " + lexical_cast<string>(n - 1) + '!').c_str());
		}
	      if (++dimensionElementsIt != dimensionElements.end())
		{
		  throw DataFormatException(fileName.c_str(), lineNb, "more than a pair (dimension, elements)!");
		}
	    }
	}
      file.close();
      unsigned int size = 0;
      for (const dynamic_bitset<>& dimension : *groupIt)
	{
	  size += dimension.count();
	}
      maxCovers.push_back(size);
      ++groupIt;
    }
}

ostream& operator<<(ostream& out, const GroupCovers& groupCovers)
{
  printCovers(out, groupCovers.minCovers);
  out << ',';
  printCovers(out, groupCovers.maxCovers);
  return out;
}

unsigned int GroupCovers::minCoverOfGroup(const unsigned int groupId) const
{
  return minCovers[groupId];
}

unsigned int GroupCovers::maxCoverOfGroup(const unsigned int groupId) const
{
  return maxCovers[groupId];
}

void GroupCovers::clearMinCovers()
{
  minCovers.clear();
}

void GroupCovers::clearMaxCovers()
{
  maxCovers.clear();
}

void GroupCovers::add(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent)
{
  vector<unsigned int>::iterator minCoverIt = minCovers.begin();
  for (const vector<dynamic_bitset<>>& group : groups)
    {
      const dynamic_bitset<>& groupView = group[dimensionIdOfElementsSetPresent];
      for (const unsigned int element : elementsSetPresent)
	{
	  if (groupView[element])
	    {
	      ++(*minCoverIt);
	    }
	}
      ++minCoverIt;
    }
}

void GroupCovers::remove(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent)
{
  vector<unsigned int>::iterator maxCoverIt = maxCovers.begin();
  for (const vector<dynamic_bitset<>>& group : groups)
    {
      const dynamic_bitset<>& groupView = group[dimensionIdOfElementsSetAbsent];
      for (const unsigned int element : elementsSetAbsent)
	{
	  if (groupView[element])
	    {
	      --(*maxCoverIt);
	    }
	}
      ++maxCoverIt;
    }
}
