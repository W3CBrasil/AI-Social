// Copyright 2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "TuplePointDenseTube.h"

TuplePointDenseTube::TuplePointDenseTube(const vector<pair<double, double>>& tubeParam) : tube(tubeParam)
{
}

TuplePointDenseTube* TuplePointDenseTube::clone() const
{
  return new TuplePointDenseTube(*this);
}

const bool TuplePointDenseTube::setTuplePoints(const vector<vector<unsigned int>>::const_iterator dimensionIt, const pair<double, double>& point, const unsigned int sizeThreshold, const unsigned int lastDimensionCardinality)
{
  for (const unsigned int hyperplaneId : *dimensionIt)
    {
      tube[hyperplaneId] = point;
    }
  return false;
}

void TuplePointDenseTube::sortTubesAndGetMinCoordinates(double& minX, double& minY)
{
  for (const pair<double, double>& point : tube)
    {
      if (point.first < minX)
	{
	  minX = point.first;
	}
      if (point.second < minY)
	{
	  minY = point.second;
	}
    }
}

void TuplePointDenseTube::translate(const double deltaX, const double deltaY)
{
  for (pair<double, double>& point : tube)
    {
      point.first += deltaX;
      point.second += deltaY;
    }
}

void TuplePointDenseTube::setSlopeSums(SlopeSums& slopeSums) const
{
  for (const pair<double, double>& point : tube)
    {
      if (!std::isnan(point.first))
	{
	  ++slopeSums.nbOfPoints;
	  slopeSums.sumX += point.first;
	  slopeSums.sumXSquared += point.first * point.first;
	  slopeSums.sumY += point.second;
	  slopeSums.sumXY += point.first * point.second;
	}
    }
}

void TuplePointDenseTube::increaseSlopeSums(const vector<unsigned int>& dimension, SlopeSums& slopeSums) const
{
  for (const unsigned int element : dimension)
    {
      const pair<double, double>& point = tube[element];
      if (!std::isnan(point.first))
	{
	  ++slopeSums.nbOfPoints;
	  slopeSums.sumX += point.first;
	  slopeSums.sumXSquared += point.first * point.first;
	  slopeSums.sumY += point.second;
	  slopeSums.sumXY += point.first * point.second;
	}
    }
}

void TuplePointDenseTube::decreaseSlopeSums(const vector<unsigned int>& dimension, SlopeSums& slopeSums) const
{
  for (const unsigned int element : dimension)
    {
      const pair<double, double>& point = tube[element];
      if (!std::isnan(point.first))
	{
	  --slopeSums.nbOfPoints;
	  slopeSums.sumX -= point.first;
	  slopeSums.sumXSquared -= point.first * point.first;
	  slopeSums.sumY -= point.second;
	  slopeSums.sumXY -= point.first * point.second;
	}
    }
}
