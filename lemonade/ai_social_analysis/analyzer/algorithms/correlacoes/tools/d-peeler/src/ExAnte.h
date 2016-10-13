// Copyright 2014 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef EXANTE_H
#define EXANTE_H

#include <vector>
#include <map>
#include <string>

/*
 * Removes tuples from the dataset without changing the results of mining closed patterns.
 *
 * The removed tuples surely can't be in a closed n-set with respect to
 * the minimum sizes per dimension restrictions.
 * \param tuples the tuples from datasets.
 * \param minimumSize a vector containing, for each dimension, the minimum
 *                    number of items from that dimension that need to be
 *                    in a closed n-set so that the size restrictions are
 *                    respected.
 * \param symmetricDimensionsIds the indices of the dimensions that are symmetric. If one
 *                    tuple is removed, all the tuples that can be obtained by permuting
 *                    the symmetric dimensions are also removed. This vector should be
 *                    already sorted.
 * \return A new set of tuples with the same number or less of tuples of the original
 *         data set. The results of mining closed patterns in this new set, subject to
 *         the minimum size constraints, is equal to mining the old data set.
 */
std::vector<std::vector<unsigned int> >
ExAntePreprocess(const std::vector<std::vector<unsigned int> >& tuples,
                 const std::vector<unsigned int>& minimumSize,
                 const std::vector<unsigned int>& symmetricDimensionsIds);

/*
 * Rebuilds identifier maps, removing unused identifiers.
 *
 * After a call to the preprocessing algorithm, some labels may become
 * absent in the dataset, because all the tuples containing them as values
 * were removed. This function reassigns the label identifiers so that
 * these unused labels are ignored. This potentially makes dimensions
 * smaller, which can make for a faster pattern mining.
 *
 * \param tuples The data set of n-dimensional tuples.
 * \param labels2Ids a vector of maps that map, for each dimension, a label
 *                   that represents an item value to its corresponding integer
 *                   identifier. This parameter is modified.
 * \param ids2Labels a vector of maps that map, for each dimension, an integer
 *                   identifier to its corresponding label, which the user used
 *                   to represent items. This parameter is modified.
 * \param cliqueDimensions the identifiers of dimensions in the data set that are
 *                         symmetric (i.e. that form a graph). This vector should
 *                         be sorted.
 */
void
ReassignIdentifiers(std::vector<std::vector<unsigned int> > &tuples,
                    std::vector<std::map<const std::string, const unsigned int>> &labels2Ids,
                    const std::vector<unsigned int> &cliqueDimensions);

#endif
