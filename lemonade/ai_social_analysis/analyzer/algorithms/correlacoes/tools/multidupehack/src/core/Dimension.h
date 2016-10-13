// Copyright 2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef DIMENSION_H_
#define DIMENSION_H_

class Dimension
{
public:
  Dimension(const unsigned int id, const unsigned int cardinality, const bool isSymmetric);

  const bool operator<(const Dimension& otherDimension) const;

  const unsigned int getId() const;
  const unsigned int getCardinality() const;
  const bool symmetric() const;
  void decrementCardinality();

protected:
  unsigned int id;
  unsigned int cardinality;
  bool isSymmetric;
};

#endif /*LABELED_DIMENSION_H_*/
