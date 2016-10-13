// Copyright 2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef MAX_SIZE_H_
#define MAX_SIZE_H_

#include "Measure.h"

class MaxSize final : public Measure
{
 public:
  MaxSize(const unsigned int dimensionId, const unsigned int threshold);
  MaxSize* clone() const;

  const bool violationAfterAdding(const unsigned int dimensionIdOfElementsSetPresent, const vector<unsigned int>& elementsSetPresent);
  const float optimisticValue() const;

 protected:
  unsigned int dimensionId;
  unsigned int maxSize;

  static vector<unsigned int> thresholds;
};

#endif /*MAX_SIZE_H_*/
