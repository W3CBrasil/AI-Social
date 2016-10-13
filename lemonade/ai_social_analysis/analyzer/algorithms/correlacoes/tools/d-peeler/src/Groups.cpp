// Copyright 2012,2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "Groups.h"

#include <limits>

vector<vector<dynamic_bitset<> > > Groups::groups;
vector<vector<float> > Groups::minRatios;
vector<unsigned int> Groups::maxSizes;

Groups::Groups(): sizeOfInterPresent(), sizeOfInterPresentAndPotential()
{
}

Groups::Groups(const Groups& otherGroups): sizeOfInterPresent(otherGroups.sizeOfInterPresent), sizeOfInterPresentAndPotential(otherGroups.sizeOfInterPresentAndPotential)
{
}

Groups::Groups(const vector<string>& groupFileNames, const char* groupItemSeparator, const char* groupDimensionItemsSeparator, const vector<map<const string, const unsigned int> >& labels2Ids, const vector<unsigned int>& dimensionOrder, const vector<unsigned int>& groupMinSizes, const vector<unsigned int>& groupMaxSizes, const char* groupMinRatiosFileName) throw(UsageException, DataFormatException, NoFileException): sizeOfInterPresent(), sizeOfInterPresentAndPotential()
{
  // Parsing the groups
  const char_separator<char> itemSeparator(groupItemSeparator);
  const char_separator<char> dimensionItemsSeparator(groupDimensionItemsSeparator);
  const unsigned int n = labels2Ids.size();
  const unsigned int nbOfGroups = groupFileNames.size();
  sizeOfInterPresent.resize(nbOfGroups);
  sizeOfInterPresentAndPotential.reserve(nbOfGroups);
  groups.resize(nbOfGroups);
  vector<vector<dynamic_bitset<> > >::iterator groupIt = groups.begin();
  for (vector<string>::const_iterator fileNameIt = groupFileNames.begin(); fileNameIt != groupFileNames.end(); ++fileNameIt)
    {
      ifstream file(fileNameIt->c_str());
      if (file.fail())
	{
	  throw NoFileException(fileNameIt->c_str());
	}
      groupIt->resize(n);
      unsigned int dimensionId = 0;
      for (vector<map<const string, const unsigned int> >::const_iterator labels2IdsIt = labels2Ids.begin(); labels2IdsIt != labels2Ids.end(); ++labels2IdsIt)
	{
	  (*groupIt)[dimensionOrder[dimensionId++]].resize(labels2IdsIt->size());
	}
      unsigned int lineNb = 0;
      while (!file.eof())
	{
	  ++lineNb;
	  string dataString;
	  getline(file, dataString);
#ifdef VERBOSE_PARSER
	  cout << *fileNameIt << ':' << lineNb << ": " << dataString << endl;
#endif
	  tokenizer<char_separator<char> > dimensionItems(dataString, dimensionItemsSeparator);
	  tokenizer<char_separator<char> >::const_iterator dimensionItemsIt = dimensionItems.begin();
	  if (dimensionItemsIt != dimensionItems.end())
	    {
	      try
		{
		  dimensionId = lexical_cast<unsigned int>(*dimensionItemsIt);
		  if (dimensionId < n)
		    {
		      if (++dimensionItemsIt == dimensionItems.end())
			{
			  throw DataFormatException(fileNameIt->c_str(), lineNb, "no items!");
			}
		      dynamic_bitset<>& view = (*groupIt)[dimensionOrder[dimensionId]];
		      const map<const string, const unsigned int>& labels2IdsView = labels2Ids[dimensionId];
		      tokenizer<char_separator<char> > items(*dimensionItemsIt, itemSeparator);
		      for (tokenizer<char_separator<char> >::const_iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
			{
			  const map<const string, const unsigned int>::const_iterator labels2IdsViewIt = labels2IdsView.find(*itemIt);
			  if (labels2IdsViewIt == labels2IdsView.end())
			    {
			      cerr << "Warning: ignoring " << *itemIt << " at line " << lineNb << " of " << *fileNameIt << " because it is absent from dimension " << dimensionId << " of the input data" << endl;
			    }
			  else if (labels2IdsViewIt->second != std::numeric_limits<unsigned int>::max())
			    {
			      view.set(labels2IdsViewIt->second);
			    }
			}
		    }
		  else
		    {
		      throw DataFormatException(fileNameIt->c_str(), lineNb, ("dimension " + lexical_cast<string>(dimensionId) + " does not exist! (at most " + lexical_cast<string>(n) + " expected)").c_str());
		    }
		}
	      catch (bad_lexical_cast &)
		{
		  throw DataFormatException(fileNameIt->c_str(), lineNb, ("the dimension should be an integer between 0 and " + lexical_cast<string>(n - 1) + '!').c_str());
		}
	      if (++dimensionItemsIt != dimensionItems.end())
		{
		  throw DataFormatException(fileNameIt->c_str(), lineNb, "more than a pair (dimension, items)!");
		}
	    }
	}
      file.close();
      unsigned int size = 0;
      for (vector<dynamic_bitset<> >::const_iterator dimensionIt = groupIt->begin(); dimensionIt != groupIt->end(); ++dimensionIt)
	{
	  size += dimensionIt->count();
	}
      sizeOfInterPresentAndPotential.push_back(size);
      ++groupIt;
    }
  // Minimal ratios and sizes (on the diagonal)
  if (string(groupMinRatiosFileName).empty())
    {
      vector<unsigned int>::const_iterator sizeIt = sizeOfInterPresentAndPotential.begin();
      minRatios.resize(nbOfGroups);
      vector<vector<float> >::iterator minRatiosIt = minRatios.begin();
      for (unsigned int groupId = 0; groupId != nbOfGroups; ++groupId)
	{
	  minRatiosIt->resize(nbOfGroups);
	  (*minRatiosIt++)[groupId] = *sizeIt++;
	}
    }
  else
    {
      ifstream minRatiosFile(groupMinRatiosFileName);
      if (minRatiosFile.fail())
	{
	  throw NoFileException(groupMinRatiosFileName);
	}
      unsigned int lineNb = 0;
      minRatios.resize(nbOfGroups);
      vector<vector<float> >::iterator minRatiosIt = minRatios.begin();
      for (; minRatiosIt != minRatios.end() && !minRatiosFile.eof(); ++minRatiosIt)
	{
	  minRatiosIt->reserve(nbOfGroups);
	  ++lineNb;
	  string minRatiosString;
	  getline(minRatiosFile, minRatiosString);
	  stringstream ss(minRatiosString);
	  float minRatio;
	  while (ss >> minRatio)
	    {
	      minRatiosIt->push_back(minRatio);
	    }
	  if (minRatiosIt->size() != nbOfGroups)
	    {
	      throw DataFormatException(groupMinRatiosFileName, lineNb, (lexical_cast<string>(minRatiosIt->size()) + " ratios! (" + lexical_cast<string>(nbOfGroups) + " expected)").c_str());
	    }
	}
      minRatiosFile.close();
      if (minRatiosIt != minRatios.end())
	{
	  throw DataFormatException(groupMinRatiosFileName, lineNb, (lexical_cast<string>(nbOfGroups) + " groups expected!").c_str());
	}
    }
  // Min sizes specified with the gs option
  if (!groupMinSizes.empty())
    {
      if (groupMinSizes.size() != nbOfGroups)
	{
	  throw UsageException(("gs option should provide " + lexical_cast<string>(nbOfGroups) + " sizes!").c_str());
	}
      vector<unsigned int>::const_iterator minSizeIt = groupMinSizes.begin();
      vector<vector<float> >::iterator minRatiosIt = minRatios.begin();
      for (unsigned int groupId = 0; groupId != nbOfGroups; ++groupId)
	{
	  (*minRatiosIt++)[groupId] = *minSizeIt++;
	}
    }
  // Max sizes specified with the gS option
  if (groupMaxSizes.empty())
    {
      maxSizes = sizeOfInterPresentAndPotential;
    }
  else
    {
      if (groupMaxSizes.size() != nbOfGroups)
	{
	  throw UsageException(("gS option should provide " + lexical_cast<string>(nbOfGroups) + " sizes!").c_str());
	}
      maxSizes = groupMaxSizes;
    }
}

Groups::~Groups() throw()
{
}

void Groups::setPresent(const int dimensionId, const unsigned int itemId)
{
  vector<unsigned int>::iterator sizeOfInterPresentIt = sizeOfInterPresent.begin();
  for (vector<vector<dynamic_bitset<> > >::const_iterator groupIt = groups.begin(); groupIt != groups.end(); ++groupIt)
    {
      const dynamic_bitset<>& groupView = (*groupIt)[dimensionId];
      if (groupView[itemId])
	{
	  ++(*sizeOfInterPresentIt);
	}
      ++sizeOfInterPresentIt;
    }
}

void Groups::setPresent(const int dimensionId, const vector<unsigned int>& itemIds)
{
  vector<unsigned int>::iterator sizeOfInterPresentIt = sizeOfInterPresent.begin();
  for (vector<vector<dynamic_bitset<> > >::const_iterator groupIt = groups.begin(); groupIt != groups.end(); ++groupIt)
    {
      const dynamic_bitset<>& groupView = (*groupIt)[dimensionId];
      for (vector<unsigned int>::const_iterator itemIdIt = itemIds.begin(); itemIdIt != itemIds.end(); ++itemIdIt)
	{
	  if (groupView[*itemIdIt])
	    {
	      ++(*sizeOfInterPresentIt);
	    }
	}
      ++sizeOfInterPresentIt;
    }
}

void Groups::setAbsent(const int dimensionId, const unsigned int itemId)
{
  vector<unsigned int>::iterator sizeOfInterPresentAndPotentialIt = sizeOfInterPresentAndPotential.begin();
  for (vector<vector<dynamic_bitset<> > >::const_iterator groupIt = groups.begin(); groupIt != groups.end(); ++groupIt)
    {
      const dynamic_bitset<>& groupView = (*groupIt)[dimensionId];
      if (groupView[itemId])
	{
	  --(*sizeOfInterPresentAndPotentialIt);
	}
      ++sizeOfInterPresentAndPotentialIt;
    }
}

void Groups::setAbsent(const vector<vector<unsigned int> >& itemIds)
{
  vector<unsigned int>::iterator sizeOfInterPresentAndPotentialIt = sizeOfInterPresentAndPotential.begin();
  for (vector<vector<dynamic_bitset<> > >::const_iterator groupIt = groups.begin(); groupIt != groups.end(); ++groupIt)
    {
      vector<dynamic_bitset<> >::const_iterator dimensionIt = groupIt->begin();
      for (vector<vector<unsigned int> >::const_iterator itemIdsIt = itemIds.begin(); itemIdsIt != itemIds.end(); ++itemIdsIt)
	{
	  for (vector<unsigned int>::const_iterator itemIdIt = itemIdsIt->begin(); itemIdIt != itemIdsIt->end(); ++itemIdIt)
	    {
	      if ((*dimensionIt)[*itemIdIt])
		{
		  --(*sizeOfInterPresentAndPotentialIt);
		}
	    }
	  ++dimensionIt;
	}
      ++sizeOfInterPresentAndPotentialIt;
    }
}

const bool Groups::constraintsSatisfied() const
{
  const unsigned int nbOfGroups = sizeOfInterPresent.size();
  vector<unsigned int>::const_iterator sizeOfInterPresentAndPotentialIt = sizeOfInterPresentAndPotential.begin();
  vector<vector<float> >::const_iterator minRatiosIt = minRatios.begin();
  vector<unsigned int>::const_iterator maxSizeIt = maxSizes.begin();
  vector<unsigned int>::const_iterator sizeOfInterPresentIt = sizeOfInterPresent.begin();
  for (unsigned int i = 0; i != nbOfGroups; ++i)
    {
      if (*sizeOfInterPresentIt++ > *maxSizeIt++)
        {
#ifdef DEBUG_EXTRACTION
	  cout << "The maximal size constraint is not satisfied in group " << i << " -> Pruned!" << endl;
#endif
	  return false;
        }
      vector<unsigned int>::const_iterator sizeOfInterPresentForRatiosIt = sizeOfInterPresent.begin();
      vector<float>::const_iterator minRatioIt = minRatiosIt->begin();
      for (unsigned int j = 0; j != nbOfGroups; ++j)
	{
	  if (i == j)
	    {
	      if (*sizeOfInterPresentAndPotentialIt < *minRatioIt++)
		{
#ifdef DEBUG_EXTRACTION
		  cout << "The minimal size constraint is not satisfied in group " << i << " -> Pruned!" << endl;
#endif
		  return false;
		}
	    }
	  else
	    {
	      if (static_cast<float>(*sizeOfInterPresentAndPotentialIt) / static_cast<float>(*sizeOfInterPresentForRatiosIt) < *minRatioIt++)
		{
#ifdef DEBUG_EXTRACTION
		  cout << "The minimal ratio between the groups " << i << " and " << j << " cannot be reached -> Pruned!" << endl;
#endif
		  return false;
		}
	    }
	  ++sizeOfInterPresentForRatiosIt;
	}
      ++sizeOfInterPresentAndPotentialIt;
      ++minRatiosIt;
    }
  return true;  
}
