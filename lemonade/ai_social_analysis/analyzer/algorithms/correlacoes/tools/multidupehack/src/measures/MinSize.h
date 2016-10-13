// Copyright 2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef MIN_SIZE_H_
#define MIN_SIZE_H_

#include "Measure.h"

class MinSize final : public Measure
{
 public:
  MinSize(const unsigned int dimensionId, const unsigned int minSize, const unsigned int threshold);
  MinSize* clone() const;

  const bool monotone() const;
  const bool violationAfterRemoving(const unsigned int dimensionIdOfElementsSetAbsent, const vector<unsigned int>& elementsSetAbsent);
  const float optimisticValue() const;

 protected:
  unsigned int dimensionId;
  unsigned int minSize;

  static vector<unsigned int> thresholds;
};

#endif /*MIN_SIZE_H_*/
