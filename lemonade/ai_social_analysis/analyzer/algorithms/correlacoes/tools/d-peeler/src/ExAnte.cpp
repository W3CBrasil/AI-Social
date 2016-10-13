// Copyright 2014 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "ExAnte.h"

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <limits>

#include "vector_hash.h"

using std::unordered_map;
using std::vector;
using std::set;
using std::map;
using std::string;
using std::pair;
using std::make_pair;



typedef vector<unsigned int> tuple;

tuple
ProjectTuple(const tuple& t, unsigned int dimension)
{
    tuple projectedTuple(t.size() - 1);

    for (unsigned int d = 0; d < projectedTuple.size(); d++) {
        if (d < dimension)
            projectedTuple[d] = t[d];
        else
            projectedTuple[d] = t[d + 1];
    }

    return projectedTuple;
}

tuple
PermuteTuple(const tuple& t,
             const vector<unsigned int>& permutation,
             const vector<unsigned int>& dimensionsIds)
{
    tuple permutedTuple(t.size());
    unsigned int permutationDimensionId = 0;

    for (unsigned int i = 0; i < t.size(); i++) {
        if (permutationDimensionId < dimensionsIds.size() &&
                dimensionsIds[permutationDimensionId] == i) {
            permutedTuple[i] = t[dimensionsIds[permutationDimensionId]];
            permutationDimensionId++;
        } else {
            permutedTuple[i] = t[i];
        }
    }

    return permutedTuple;
}

bool IsSelfLoop(const vector<unsigned int>& projectedTuple,
                unsigned int projectionDimension,
                const vector<unsigned int>& symmetricDimensionsIds)
{
    if (symmetricDimensionsIds.size() == 0)
        return false;

    unsigned int symmetricDimensionsValue = 0;
    unsigned int symmetricDimensionIndex = 0;

    for (unsigned i = 0; i < projectedTuple.size(); i++) {
        unsigned dimensionId = i + static_cast<int>(i >= projectionDimension);

        if (symmetricDimensionIndex < symmetricDimensionsIds.size() &&
                symmetricDimensionsIds[symmetricDimensionIndex] == dimensionId) {
            if (symmetricDimensionIndex == 0) {
                symmetricDimensionsValue = projectedTuple[i];
            } else if (projectedTuple[i] != symmetricDimensionsValue) {
                return false;
            }

            symmetricDimensionIndex++;
        }
    }

    return true;
}


void
PruneHyperplane(const tuple& projectedTuple,
                unsigned int projectionDimension,
                const vector<unsigned int>& minimumSize,
                const vector<tuple>& tuples,
                const unordered_map<tuple, unsigned int, vector_hash>& tuplesIds,
                set<unsigned int>& removedTuples,
                vector<unordered_map<tuple, set<unsigned int>, vector_hash>>& projectedTuples,
                const vector<unsigned int>& symmetricDimensionsIds,
                const vector<bool>& isDimensionSymmetric)
{
    set<unsigned int>& hyperplaneTuples = projectedTuples[projectionDimension][projectedTuple];

    // Return if the number of elements in this dimension is unconstrained
    // Or if this hyperplane has already been pruned out
    if (minimumSize[projectionDimension] == 0 || hyperplaneTuples.size() == 0)
        return;

    // If this tube only contains self-loops, return, because they're all present.
    if (IsSelfLoop(projectedTuple, projectionDimension, symmetricDimensionsIds))
        return;

    const unsigned n = minimumSize.size();

    // Minimum pattern size constraint considering that if this dimension is symmetric,
    // then there is one more tuple in this tube, which is the self-loop corresponding
    // to the other symmetric dimension. For example, if the current 3d tube is (X 2 3)
    // and dimensions 1 and 2 are symmetric, then the tuple (2 2 3) is implicitly present.
    unsigned int actualMinimumSize = minimumSize[projectionDimension] -
        static_cast<int>(isDimensionSymmetric[projectionDimension]);

    if (hyperplaneTuples.size() < actualMinimumSize) {
        auto tuplesToBeErased = std::move(hyperplaneTuples);

        for (const auto& t : tuplesToBeErased) {
            if (removedTuples.count(t))
                continue;

            // Remove tuple from all tubes
            vector<unsigned int> symmetricDimensionsPermutation(symmetricDimensionsIds.size());

            std::iota(symmetricDimensionsPermutation.begin(),
                    symmetricDimensionsPermutation.end(), 0);

            do {
                tuple permutedTuple = PermuteTuple(tuples[t],
                        symmetricDimensionsPermutation,
                        symmetricDimensionsIds);

                removedTuples.insert(tuplesIds.find(permutedTuple)->second);
            } while (next_permutation(symmetricDimensionsPermutation.begin(),
                        symmetricDimensionsPermutation.end()));

            for (unsigned d = 0; d < n; d++) {
                if (d != projectionDimension) {
                    std::iota(symmetricDimensionsPermutation.begin(),
                            symmetricDimensionsPermutation.end(), 0);

                    do {
                        tuple permutedTuple = PermuteTuple(tuples[t],
                                symmetricDimensionsPermutation,
                                symmetricDimensionsIds);

                        unsigned permutedTupleId = tuplesIds.find(permutedTuple)->second;

                        tuple projectedRemovedTuple = ProjectTuple(permutedTuple, d);
                        projectedTuples[d][projectedRemovedTuple].erase(permutedTupleId);

                        PruneHyperplane(projectedRemovedTuple,
                                d,
                                minimumSize,
                                tuples,
                                tuplesIds,
                                removedTuples,
                                projectedTuples,
                                symmetricDimensionsIds,
                                isDimensionSymmetric);

                    } while (next_permutation(symmetricDimensionsPermutation.begin(),
                                symmetricDimensionsPermutation.end()));
                }
            }
        }
    }
}

namespace
{
    vector<tuple>
        UniqueTuples(const vector<tuple>& tuples)
        {
            vector<tuple> uniqueTuples;
            std::unordered_set<tuple, vector_hash> addedTuples;

            for (const auto& t : tuples) {
                if (addedTuples.count(t) == 0) {
                    addedTuples.insert(t);
                    uniqueTuples.push_back(t);
                }
            }

            return uniqueTuples;
        }
}

    std::vector<std::vector<unsigned int>>
ExAntePreprocess(const std::vector<std::vector<unsigned int >>& datasetTuples,
        const std::vector<unsigned int>& minimumSize,
        const std::vector<unsigned int>& symmetricDimensionsIds)
{
    vector<vector<unsigned int>> tuples = UniqueTuples(datasetTuples);

    // Number of dimensions
    const unsigned n = minimumSize.size();

    // A map from tuple value to tuple index
    unordered_map<tuple, unsigned int, vector_hash> tuplesIds;

    for (unsigned i = 0; i < tuples.size(); i++) {
        tuplesIds[tuples[i]] = i;
    }

    // A vector containing, for each dimension, all `tuples` projected in
    // the (n-1)-dimensional space of all except that dimension along with
    // the index of that tuple in the vector `tuples`
    vector<unordered_map<tuple, set<unsigned int>, vector_hash>> projectedTuples(n);

    // For each tuple t, for each dimension d, project t onto the space of
    // all n dimensions except d and add this projection to projectedTuples
    for (unsigned int t = 0; t < tuples.size(); t++) {
        for (unsigned int d = 0; d < n; d++) {
            tuple projectedTuple = ProjectTuple(tuples[t], d);
            projectedTuples[d][projectedTuple].insert(t);
        }
    }

    vector<bool> isDimensionSymmetric(n);

    for (unsigned d = 0; d < n; d++) {
        if (std::find(symmetricDimensionsIds.begin(),
                    symmetricDimensionsIds.end(),
                    d) != symmetricDimensionsIds.end()) {
            isDimensionSymmetric[d] = true;
        }
    }

    set<unsigned int> removedTuples;

    // Remove unnecessary tuples from the dataset
    for (unsigned int d = 0; d < n; d++) {
        for (const auto& hyperplanes : projectedTuples[d]) {
            PruneHyperplane(hyperplanes.first, d, minimumSize, tuples, tuplesIds,
                    removedTuples, projectedTuples, symmetricDimensionsIds, isDimensionSymmetric);
        }
    }

    vector<vector<unsigned int>> remainingTuples;

    for (unsigned i = 0; i < tuples.size(); i++)
        if (!removedTuples.count(i))
            remainingTuples.push_back(tuples[i]);

    return remainingTuples;
}

    void
ReassignIdentifiers(vector<vector<unsigned int> >& tuples,
        vector<map<const string, const unsigned int> >& labels2Ids,
        const vector<unsigned>& cliqueDimensions)
{
    const unsigned n = labels2Ids.size();

    vector<map<unsigned int, unsigned int>> ids2NewIds(n);

    // For each tuple, for each dimension, assign a new id to the tuple's value in that dimension.
    // Note that absent values in each dimension don't get assigned a new id.
    for (auto &t : tuples) {
        unsigned symmetricDimensionIndex = 0;

        for (unsigned i = 0; i < n; i++) {

            unsigned int mapIndex = i;

            // In every symmetric dimensions, use the first symmetric dimension to store the
            // new identifiers for each label
            if (symmetricDimensionIndex < cliqueDimensions.size() &&
                    cliqueDimensions[symmetricDimensionIndex] == i) {
                mapIndex = cliqueDimensions.front();
                symmetricDimensionIndex++;
            }

            unsigned nextId = ids2NewIds[mapIndex].size();

            if (ids2NewIds[mapIndex].find(t[i]) == ids2NewIds[mapIndex].end()) {
                ids2NewIds[mapIndex][t[i]] = nextId;
            }

            t[i] = ids2NewIds[mapIndex][t[i]];
        }
    }

    // Copy the map for the first symmetric dimension to the others
    for (unsigned i = 1; i < cliqueDimensions.size(); i++)
        ids2NewIds[cliqueDimensions[i]] = ids2NewIds[cliqueDimensions.front()];

    std::remove_reference<decltype(labels2Ids)>::type newLabels2Ids;
    newLabels2Ids.resize(n);

    for (unsigned i = 0; i < n; i++)
        for (auto &label2Id : labels2Ids[i])
            if (ids2NewIds[i].count(label2Id.second))
                newLabels2Ids[i].insert(make_pair(label2Id.first,
                            ids2NewIds[i][label2Id.second]));
            else
                newLabels2Ids[i].insert(make_pair(label2Id.first,
                            std::numeric_limits<unsigned int>::max()));

    labels2Ids = std::move(newLabels2Ids);
}
