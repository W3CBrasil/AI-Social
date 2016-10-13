// Copyright 2007-2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

// Output
/* OUTPUT turns on the output. This option may be disabled to evaluate the performance of d-peeler independently from the writing performances of the disk. */
#define OUTPUT

// Heuristics
/* SORT_ITEMS turns on the heuristic consisting in independently sorting the items of each dimension to first consider the least dense. Disabling it considerably decreases the performances of d-peeler. */
#define SORT_ITEMS

/* SORT_DIMENSIONS turns on the heuristic consisting in reordering the dimensions to minimize both the space requirement for the data set and its access costs. */
#define SORT_DIMENSIONS

/* ENUMERATION_PROCESS is used to select the procedure aimed at choosing on which dimension should the next item be enumerated: */
/* 0: the chosen dimension is the smallest non-empty one (choice independent from the past computation) */
/* 1: the chosen dimension presents the least dense item (choice independent from the past computation) */
/* 2: the chosen dimension presents the item introducing the more 0 (choice independent from the past computation) */
/* 3: the chosen dimension has the smallest number of remaining potential items */
/* 4: the chosen dimension maximizes the quantity of potential items that may be absent after propagation */
#define ENUMERATION_PROCESS 4

// Log
/* VERBOSE_PARSER turns on the output (on the standard output) of information when the input data file is parsed. */
/* #define VERBOSE_PARSER */

/* DEBUG_EXTRACTION turns on the output (on the standard output) of information during the extraction of the closed n-sets. This option may be enabled by someone who wishes to precisely understand how this extraction is performed on a small data set. */
/* #define DEBUG_EXTRACTION */

/* DEBUG_MINIMIZATION turns on the output (on the standard output) of information during the minimization (post-processing). This option may be enabled by someone who wishes to precisely understand how this minimization is performed on a small data set. *\/ */
/* #define DEBUG_MINIMIZATION */

/* VERBOSE_DIM_CHOICE turns on the output (on the standard output) of information regarding the choice of the next dimension to be enumerated. Combined with DEBUG, this option may be enabled by someone who wishes to precisely understand how this choice is performed on a small data set. */
/* #define VERBOSE_DIM_CHOICE */

/* NB_OF_CLOSED_N_SETS turns on the output (on the standard output) of how many closed n-sets were selected during the extraction phase. */
/* #define NB_OF_CLOSED_N_SETS */

/* NB_OF_NODES turns on the output (on the standard output) of how many patterns were considered during the extraction phase, i.e., the count of valid (but not always maximal) patterns with regard to the applied constraints. This option may be enabled by someone who wishes a measure of the time complexity independent from the hardware performance. */
/* #define NB_OF_NODES */

/* PRUNING_DETAILS turns on the output (on the standard output) of a detailed analysis of how the tree is pruned. This option may be enabled by someone who wants to have a broad view of the tree w.r.t. the enumeration process and the data set. It gives (in this order): */
/* - the binary logarithm of the number of nodes pruned of for non-maximality */
/* - the binary logarithm of the number of nodes pruned for unsatisfied minimal size constrainst */
/* - the binary logarithm of the number of nodes pruned for unsatisfied maximal size constrainst */
/* - the binary logarithm of the number of nodes pruned for unsatisfied minimal area constrainst */
/* - the binary logarithm of the number of nodes pruned for unsatisfied maximal area constrainst */
/* - the binary logarithm of the number of nodes pruned for unreached minimization ratio */
/* #define PRUNING_DETAILS */

/* AVERAGE_NB_OF_ITEMS_ABSENT_BY_PROPAGATION turns on the output (on the standard output) of the the average number of items set absent per propagation. */
/* #define AVERAGE_NB_OF_ITEMS_ABSENT_BY_PROPAGATION */

/* TIME turns on the output (on the standard output) of the running time of d-peeler. */
/* #define TIME */

/* DETAILED_TIME turns on the output (on the standard output) of a more detailed analysis of how the time is spent. It gives (in this order): */
/* - the parsing time */
/* - the pre-processing time */
/* - the mining time */
/* - the post-processing time (if it is a minimization) */
/* - the time spent checking the maximality of the computed patterns */
/* - the time spent propagating the absence of certain items (when another is set present) */
/* #define DETAILED_TIME */

/* GNUPLOT modifies the way NB_OF_CLOSED_N_SETS, NB_OF_NODES, PRUNING_DETAILS, TIME and DETAILED_TIME (in this order) format their outputs. Instead of being human readable, they are directly understandable by the famous gnuplot software. */
/* #define GNUPLOT */

#endif /*PARAMETERS_H_*/
