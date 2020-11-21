#include "Interface.h"

int main () {
    Controller cc;
    Interface ii (0, true, &cc);
    ifstream s ("run.txt");
    ii.run (s);
}

/* The problems are:
 * - INFOCLUSTERSEARCH (or infocs) should be useful for particular searches and clusters;
 * - hdbscan
 * - Symmetric matrices
 * - Interface::manager should be simplified and separated to little functions
 * - using c++11 random lib to avoid warnings
 * - stop creating binary matrix without distance matrix
 * - mechanics which moves interface to console administrating if there is no EXIT line in script file
 * - EM-clustering
 * - k-means with multiple cores
 * - error in file should throw user to manual input
 * - kmeans should be separated to little functions
 */