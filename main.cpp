#include "Interface.h"

int main () {
    Controller cc;
    Interface ii (0, true, &cc);
    ifstream s ("run.txt");
    ii.run (s);
}

/* The problems are:
 * - curr_cluster vector in some algorithms is removed before going to the next point
 * - check permissions for readonly
 * - understand how vector of Cluster_Search is kept in memory
 * - searching the ways to output the clustering
 * - INFOCLUSTERSEARCH
 * - spanning tree & s-tree_histo
 * - hdbscan
 * - Symmetric matrices
 * - k-means
 * - BINARY command
 * - Interface::manager should be simplified and separated to little functions
 */