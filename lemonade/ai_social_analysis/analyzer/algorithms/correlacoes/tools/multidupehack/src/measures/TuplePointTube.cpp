// Copyright 2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "TuplePointTube.h"

TuplePointTube::~TuplePointTube()
{
}

void TuplePointTube::increaseSlopeSums(const vector<vector<unsigned int>>& present, const vector<unsigned int>::const_iterator dimensionIdIt, SlopeSums& slopeSums) const
{
  increaseSlopeSums(present[*dimensionIdIt], slopeSums);
}

void TuplePointTube::decreaseSlopeSums(const vector<vector<unsigned int>>& present, const vector<vector<unsigned int>>& potential, const vector<unsigned int>::const_iterator dimensionIdIt, SlopeSums& slopeSums) const
{
  decreaseSlopeSums(present[*dimensionIdIt], slopeSums);
  decreaseSlopeSums(potential[*dimensionIdIt], slopeSums);
}

void TuplePointTube::increaseSlopeSums(const vector<vector<unsigned int>>& present, const vector<unsigned int>& elementsSetPresent, const vector<unsigned int>::const_iterator dimensionIdIt, const vector<unsigned int>::const_iterator presentDimensionIdIt, SlopeSums& slopeSums) const
{
  // dimensionIdIt == presentDimensionIdIt necessarily
  increaseSlopeSums(elementsSetPresent, slopeSums);
}

void TuplePointTube::decreaseSlopeSums(const vector<vector<unsigned int>>& present, const vector<vector<unsigned int>>& potential, const vector<unsigned int>& elementsSetAbsent, const vector<unsigned int>::const_iterator dimensionIdIt, const vector<unsigned int>::const_iterator absentDimensionIdIt, SlopeSums& slopeSums) const
{
  // dimensionIdIt == absentDimensionIdIt necessarily
  decreaseSlopeSums(elementsSetAbsent, slopeSums);
}
