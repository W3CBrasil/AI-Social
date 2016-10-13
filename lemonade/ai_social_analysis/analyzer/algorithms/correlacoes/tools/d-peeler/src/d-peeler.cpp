// Copyright 2007-2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include <sstream>
#include <boost/program_options.hpp>
#include "sysexits.h"

#include "Tree.h"
#include "SparseItemset.h"

using namespace boost::program_options;
using namespace std;

template<typename T> vector<T> getVectorFromString(const string& str)
{
  vector<T> tokens;
  T token;
  stringstream ss(str);
  while (ss >> token)
    {
      tokens.push_back(token);
    }
  return tokens;
}

int main(int argc, char* argv[])
{
  string optionFileName;
  Tree::Flags flags = static_cast<Tree::Flags>(0);

  string dontCareSetFileName;
  vector<float> deltaVector;
  string deltaVectorString;
  float minArea = 0;
  float maxArea = -1;
  vector<float> areaCoefficients;
  float minimizationRatio = 0;
  double sparseItemsetDensityLimit = 0.1;
  vector<unsigned int> cliqueDimensions;
  string cliqueDimensionsString;
  vector<unsigned int> minSizes;
  string minSizesString;
  vector<unsigned int> maxSizes;
  string maxSizesString;
  string areaCoefficientsString;
  string groupFileNamesString;
  string dataFileName;
  string outputFileName;
  string inputItemSeparator = ",";
  string inputDimensionSeparator = " ";
  vector<string> groupFileNames;
  string groupItemSeparator;
  string groupDimensionItemsSeparator;
  vector<unsigned int> groupMinSizes;
  string groupMinSizesString;
  vector<unsigned int> groupMaxSizes;
  string groupMaxSizesString;
  string groupMinRatiosFileName;
  string outputItemSeparator = ",";
  string outputDimensionSeparator = " ";
  string patternSizeSeparator = " : ";
  string sizeSeparator = " ";
  string sizeAreaSeparator = " : ";
  string emptySet = "ø";
  // Parsing the command line and the option file
  try
    {
      options_description generic("Generic options");
      generic.add_options()
	("help,h", "produce help message")
	("version,V", "display version information and exit")
	("opt", value<string>(&optionFileName), "set the option file name (by default [data-file].opt if present)");
      options_description config("Configuration (on the command line or in the option file)");
      config.add_options()
	("minimize,m", "output minimization of the whole dataset instead of the list of closed n-sets")
	("dcs", value<string>(&dontCareSetFileName), "combined with --minimize, set \"don't care\" file name (by default [data-file].dcs if present)")
	("delta,d", value<string>(&deltaVectorString), "set delta values for each dimension (by default 0 for every dimension)")
	("sizes,s", value<string>(&minSizesString), "set minimal sizes in each dimension of any computed closed n-set (by default 0 for every dimension)")
	("Sizes,S", value<string>(&maxSizesString), "set maximal sizes in each attribute of any computed closed n-set (unconstrained by default)")
	("area,a", value<float>(&minArea), "set minimal area of any computed closed n-set (by default 0)")
	("Area,A", value<float>(&maxArea), "set maximal area of any computed closed n-set (unconstrained by default)")
	("ac", value<string>(&areaCoefficientsString), "set coefficients to dimensions when calculating the area of a pattern (by default 1 for every dimension)")
	("mr", value<float>(&minimizationRatio), "set minimization ratio (by default 0, best)")
	("clique,c", value<string>(&cliqueDimensionsString), "set dimensions on which cliques are searched (CAUTION: these dimensions MUST form a graph in the data-set)")
	("density", value<double>(&sparseItemsetDensityLimit), "set the maximum density a sparse itemset should reach before being made dense")
	("iis", value<string>(&inputItemSeparator), string("set any character separating two items in input data (by default \"" + inputItemSeparator + "\")").c_str())
	("ids", value<string>(&inputDimensionSeparator), string("set any character separating two dimensions in input data (by default \"" + inputDimensionSeparator + "\")").c_str())
	("groups,g", value<string>(&groupFileNamesString), "set the names of the files describing every group (none by default)")
	("gs", value<string>(&groupMinSizesString), "set minimal number of elements in each group (all elements by default)")
	("gS", value<string>(&groupMaxSizesString), "set maximal number of elements in each group (unconstrained by default)")
	("gr", value<string>(&groupMinRatiosFileName), "set file name specifying minimal ratios between the number of elements in each group (by default 0 for every pair of group)")
	("gis", value<string>(&groupItemSeparator), string("set any character separating two items in a group (by default same as --iis)").c_str())
	("gds", value<string>(&groupDimensionItemsSeparator), string("set any character separating the dimension from its items in a group (by default same as --ids)").c_str())
	("out,o", value<string>(&outputFileName), "set output file name (by default [data-file].out)")
	("ois", value<string>(&outputItemSeparator), string("set string separating two items in output data (by default \"" + outputItemSeparator + "\")").c_str())
	("ods", value<string>(&outputDimensionSeparator), string("set string separating two dimensions in output data (by default \"" + outputDimensionSeparator + "\")").c_str())
	("empty", value<string>(&emptySet), string("set string specifying an empty set in output data (by default \"" + emptySet + "\")").c_str())
	("ps", "print sizes in output data")
	("css", value<string>(&patternSizeSeparator), string("set string separating patterns from sizes in output data (by default \"" + patternSizeSeparator + "\")").c_str())
	("ss", value<string>(&sizeSeparator), string("set string separating sizes of the different dimensions in output data (by default \"" + sizeSeparator + "\")").c_str())
	("pa", "print areas in output data")
	("sas", value<string>(&sizeAreaSeparator), string("set string separating sizes from areas in output data (by default \"" + sizeAreaSeparator + "\")").c_str())
        ("reduction,r", "output a lossless (w.r.t. the result and size constraints) reduction of the data set.")
        ("no-reduction,nr", "don't reduce (preprocess) the dataset before pattern mining.");
      options_description hidden("Hidden options");
      hidden.add_options()
	("data-file", value<string>(&dataFileName), "set input data file");
      positional_options_description p;
      p.add("data-file", -1);
      options_description options;
      options.add(generic).add(config).add(hidden);
      variables_map vm;
      store(command_line_parser(argc, argv).options(options).positional(p).run(), vm);
      notify(vm);
      if (vm.count("help"))
	{
	  cout << "Usage: d-peeler [options] data-file" << endl << generic << config << endl;
	  return EX_OK;
	}
      if (vm.count("version"))
	{
	  cout << "d-peeler version 2.10" << endl;
	  return EX_OK;
	}
      if (!vm.count("data-file"))
	{
	  cerr << "Usage: d-peeler [options] data-file" << endl << generic << config << endl;
	  return EX_USAGE;
	}
      ifstream optionFile;
      if (vm.count("opt"))
	{
	  optionFile.open(optionFileName.c_str());
	  if (optionFile.fail())
	    {
	      throw NoFileException(optionFileName.c_str());
	    }
	  optionFile.close();
	}
      else
	{
	  optionFileName = dataFileName + ".opt";
	}
      optionFile.open(optionFileName.c_str());
      store(parse_config_file(optionFile, config), vm);
      notify(vm);
      optionFile.close();
      if (!vm.count("dcs"))
	{
	  dontCareSetFileName = dataFileName + ".dcs";
	  ifstream dontCareSet(dontCareSetFileName.c_str());
	  if (dontCareSet.fail())
	    {
	      dontCareSetFileName = "";
	    }
	  dontCareSet.close();
	}
      if (vm.count("minimize"))
	{
            flags = static_cast<Tree::Flags>(flags | Tree::IS_MINIMIZATION);
	}
      if (vm.count("delta"))
	{
	  deltaVector = getVectorFromString<float>(deltaVectorString);
	}
      if (vm.count("sizes"))
      	{
      	  minSizes = getVectorFromString<unsigned int>(minSizesString);
      	}
      if (vm.count("Sizes"))
	{
	  maxSizes = getVectorFromString<unsigned int>(maxSizesString);
	}
      if (vm.count("ac"))
      	{
      	  areaCoefficients = getVectorFromString<float>(areaCoefficientsString);
      	}
      if (vm.count("groups"))
	{
	  // groupFileNames = getVectorFromString<string>(areaCoefficientsString);
	  char_separator<char> fileSeparator(" ");
	  tokenizer<char_separator<char> > tokens(groupFileNamesString, fileSeparator);
	  for (tokenizer<char_separator<char> >::const_iterator groupFileNameIt = tokens.begin(); groupFileNameIt != tokens.end(); ++groupFileNameIt)
	    {
	      groupFileNames.push_back(*groupFileNameIt);
	    }
	}
      if (vm.count("gs"))
	{
	  groupMinSizes = getVectorFromString<unsigned int>(groupMinSizesString);
	}
      if (vm.count("gS"))
	{
	  groupMaxSizes = getVectorFromString<unsigned int>(groupMaxSizesString);
	}
      if (!vm.count("gis"))
	{
	  groupItemSeparator = inputItemSeparator;
	}
      if (!vm.count("gds"))
	{
	  groupDimensionItemsSeparator = inputDimensionSeparator;
	}
      if (vm.count("clique"))
	{
	  cliqueDimensions = getVectorFromString<unsigned int>(cliqueDimensionsString);
	}
      if (!vm.count("out"))
	{
	  outputFileName = dataFileName + ".out";
	}
      if (vm.count("ps"))
      	{
      	  flags = static_cast<Tree::Flags>(flags | Tree::IS_SIZE_PRINTED);
      	}
      if (vm.count("pa"))
      	{
          flags = static_cast<Tree::Flags>(flags | Tree::IS_AREA_PRINTED);
      	}
      if (vm.count("no-reduction"))
      {
          flags = static_cast<Tree::Flags>(flags | Tree::NO_DATASET_REDUCTION);
      }
      if (vm.count("reduction"))
      {
          flags = static_cast<Tree::Flags>(flags | Tree::DATASET_REDUCTION_ONLY);
      }
      if (vm.count("density"))
      {
          SparseItemset::setDensityLimit(sparseItemsetDensityLimit);
      }
    }
  catch (NoFileException e)
    {
      cerr << e.what() << endl;
      return EX_IOERR;
    }
  catch (unknown_option e)
    {
      cerr << "Unknown option!" << endl;
      return EX_USAGE;
    }
  try
    {
      Tree root(dataFileName.c_str(), dontCareSetFileName.c_str(), deltaVector, minSizes, maxSizes, minArea, maxArea, areaCoefficients, minimizationRatio, groupFileNames, groupMinSizes, groupMaxSizes, groupMinRatiosFileName.c_str(), cliqueDimensions, inputItemSeparator.c_str(), inputDimensionSeparator.c_str(), groupItemSeparator.c_str(), groupDimensionItemsSeparator.c_str(), outputFileName.c_str(), outputItemSeparator.c_str(), outputDimensionSeparator.c_str(), patternSizeSeparator.c_str(), sizeSeparator.c_str(), sizeAreaSeparator.c_str(), emptySet.c_str(), flags);
    }
  catch (NoFileException e)
    {
      cerr << e.what() << endl;
      return EX_IOERR;
    }
  catch (UsageException e)
    {
      cerr << e.what() << endl;
      return EX_USAGE;
    }
  catch (DataFormatException e)
    {
      cerr << e.what() << endl;
      return EX_DATAERR;
    }
  return EX_OK;
}
