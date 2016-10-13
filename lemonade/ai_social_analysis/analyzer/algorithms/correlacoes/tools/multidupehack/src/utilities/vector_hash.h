// Copyright 2013,2014 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of multidupehack.

// multidupehack is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// multidupehack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with multidupehack; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef VECTOR_HASH_H_
#define VECTOR_HASH_H_

#include <vector>
#include <boost/functional/hash.hpp>

template<typename T>
struct vector_hash
{
  size_t operator()(const std::vector<T>& v) const
  {
    return boost::hash_range(v.begin(), v.end());
  }
};

#endif /*VECTOR_HASH_H_*/
