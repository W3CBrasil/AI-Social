// Copyright 2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "TuplePointSparseTube.h"

TuplePointSparseTube::TuplePointSparseTube() : tube()
{
}

TuplePointSparseTube* TuplePointSparseTube::clone() const
{
  return new TuplePointSparseTube(*this);
}

vector<pair<double, double>> TuplePointSparseTube::getDenseRepresentation(const unsigned int nbOfHyperplanes) const
{
  vector<pair<double, double>> denseRepresentation(nbOfHyperplanes);
  for (const pair<unsigned int, pair<double, double>>& entry : tube)
    {
      denseRepresentation[entry.first] = entry.second;
    }
  return denseRepresentation;
}

const bool TuplePointSparseTube::setTuplePoints(const vector<vector<unsigned int>>::const_iterator dimensionIt, const pair<double, double>& point, const unsigned int sizeThreshold, const unsigned int lastDimensionCardinality)
{
  for (const unsigned int hyperplaneId : *dimensionIt)
    {
      tube.push_back(pair<unsigned int, pair<double, double>>(hyperplaneId, point));
    }
  return tube.size() >= sizeThreshold;
}

void TuplePointSparseTube::sortTubesAndGetMinCoordinates(double& minX, double& minY)
{
  tube.resize(tube.size());
  sort(tube.begin(), tube.end());  
  for (const pair<unsigned int, pair<double, double>>& entry : tube)
    {
      if (entry.second.first < minX)
	{
	  minX = entry.second.first;
	}
      if (entry.second.second < minY)
	{
	  minY = entry.second.second;
	}
    }
}

void TuplePointSparseTube::translate(const double deltaX, const double deltaY)
{
  for (pair<unsigned int, pair<double, double>>& entry : tube)
    {
      entry.second.first += deltaX;
      entry.second.second += deltaY;
    }
}

void TuplePointSparseTube::setSlopeSums(SlopeSums& slopeSums) const
{
  slopeSums.nbOfPoints += tube.size();
  for (const pair<unsigned int, pair<double, double>>& entry : tube)
    {
      slopeSums.sumX += entry.second.first;
      slopeSums.sumXSquared += entry.second.first * entry.second.first;
      slopeSums.sumY += entry.second.second;
      slopeSums.sumXY += entry.second.first * entry.second.second;
    }
}

void TuplePointSparseTube::increaseSlopeSums(const vector<unsigned int>& dimension, SlopeSums& slopeSums) const
{
  const vector<pair<unsigned int, pair<double, double>>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, pair<double, double>>>::const_iterator tubeBegin = tube.begin();
  for (const unsigned int element : dimension)
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, pair<double, double>>(element, pair<double, double>(-numeric_limits<double>::infinity(), -numeric_limits<double>::infinity())));
      if (tubeBegin != tubeEnd && tubeBegin->first == element)
	{
	  ++slopeSums.nbOfPoints;
	  slopeSums.sumX += tubeBegin->second.first;
	  slopeSums.sumXSquared += tubeBegin->second.first * tubeBegin->second.first;
	  slopeSums.sumY += tubeBegin->second.second;
	  slopeSums.sumXY += tubeBegin->second.first * tubeBegin->second.second;
	}
    }
}

void TuplePointSparseTube::decreaseSlopeSums(const vector<unsigned int>& dimension, SlopeSums& slopeSums) const
{
  const vector<pair<unsigned int, pair<double, double>>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, pair<double, double>>>::const_iterator tubeBegin = tube.begin();
  for (const unsigned int element : dimension)
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, pair<unsigned int, pair<double, double>>(element, pair<double, double>(-numeric_limits<double>::infinity(), -numeric_limits<double>::infinity())));
      if (tubeBegin != tubeEnd && tubeBegin->first == element)
	{
	  --slopeSums.nbOfPoints;
	  slopeSums.sumX -= tubeBegin->second.first;
	  slopeSums.sumXSquared -= tubeBegin->second.first * tubeBegin->second.first;
	  slopeSums.sumY -= tubeBegin->second.second;
	  slopeSums.sumXY -= tubeBegin->second.first * tubeBegin->second.second;
	}
    }
}
