#include "Interface.h"

int main () {
    Controller cc;
    Interface ii(0, true, &cc);
    ifstream s("run.txt");
    ii.run(s);
}

/* The problems are:
 * - curr_cluster vector in some algorithms is removed before going to the next point
 * - searching the ways to output the clustering
 * - INFOCLUSTERSEARCH
 * - spanning tree & s-tree_histo
 * - hdbscan
 * - Symmetric matrixes
 * - k-means
 */