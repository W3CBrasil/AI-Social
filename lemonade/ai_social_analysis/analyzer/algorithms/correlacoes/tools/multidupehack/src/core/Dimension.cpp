// Copyright 2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "Dimension.h"

Dimension::Dimension(const unsigned int idParam, const unsigned int cardinalityParam, const bool isSymmetricParam): id(idParam), cardinality(cardinalityParam), isSymmetric(isSymmetricParam)
{
}

const bool Dimension::operator<(const Dimension& otherDimension) const
{
  return cardinality < otherDimension.cardinality || (cardinality == otherDimension.cardinality && isSymmetric);
}

const unsigned int Dimension::getId() const
{
  return id;
}

const unsigned int Dimension::getCardinality() const
{
  return cardinality;
}

const bool Dimension::symmetric() const
{
  return isSymmetric;
}

void Dimension::decrementCardinality()
{
  --cardinality;
}
