// Copyright 2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef TUPLE_POINT_SPARSE_TUBE_H
#define TUPLE_POINT_SPARSE_TUBE_H

#include <cmath>
#include <algorithm>

#include "TuplePointTube.h"

class TuplePointSparseTube final : public TuplePointTube
{
 public:
  TuplePointSparseTube();

  vector<pair<double, double>> getDenseRepresentation(const unsigned int nbOfHyperplanes) const;
  const bool setTuplePoints(const vector<vector<unsigned int>>::const_iterator dimensionIt, const pair<double, double>& point, const unsigned int sizeThreshold, const unsigned int lastDimensionCardinality);
  void sortTubesAndGetMinCoordinates(double& minX, double& minY);
  void translate(const double deltaX, const double deltaY);
  void setSlopeSums(SlopeSums& slopeSums) const;
  void increaseSlopeSums(const vector<unsigned int>& dimension, SlopeSums& slopeSums) const;
  void decreaseSlopeSums(const vector<unsigned int>& dimension, SlopeSums& slopeSums) const;
 protected:
  vector<pair<unsigned int, pair<double, double>>> tube;

  TuplePointSparseTube* clone() const;
};

#endif /*TUPLE_POINT_SPARSE_TUBE_H*/
