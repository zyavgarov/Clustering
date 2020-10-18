#include "Interface.h"

int main () {
    Controller cc;
    Interface ii (0, true, &cc);
    ifstream s ("run.txt");
    ii.run (s);
}

/* The problems are:
 * - curr_cluster vector in some algorithms is removed before going to the next point
 * - check permissions for editing and clustering
 * - understand how vector of Cluster_Search is kept in memory
 * - INFOCLUSTERSEARCH (or infocs) should be useful for particular searches and clusters;
 * - spanning tree & s-tree_histo
 * - hdbscan
 * - Symmetric matrices
 * - k-means
 * - Interface::manager should be simplified and separated to little functions
 * - using c++11 random lib to avoid warnings
 * - stop creating binary matrix without distance matrix
 */