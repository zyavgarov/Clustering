#include "Interface.h"

int main () {
    Controller cc;
    Interface ii (0, true, &cc);
    ifstream s ("run.txt");
    ii.run ();
    Point a(3, 4);
    a.shift(1,2);
    //a.x() = 3;
}

/* The problems are:
 * - curr_cluster vector in some algorithms is removed before going to the next point
 * - check permissions for editing and clustering
 * - INFOCLUSTERSEARCH (or infocs) should be useful for particular searches and clusters;
 * - hdbscan
 * - Symmetric matrices
 * - k-means
 * - Interface::manager should be simplified and separated to little functions
 * - using c++11 random lib to avoid warnings
 * - stop creating binary matrix without distance matrix
 * - mechanics which moves interface to console administrating if there is no EXIT line in file
 * - support comments in script
 * - understand noreturn warning
 */