#include <iostream>
#include <fstream>
#include "Interface.h"

int main () {
    Controller cc;
    Interface ii(0, true, &cc);
    ifstream s("run.txt");
    ii.run(s);
}

/* The problems are:
 * - bruteforcing of Points sometimes can start from 0. This is not allowed
 * - possible solution can be starting id from 0
 * - curr_cluster vector in some algorithms is removed before going to the next point
 * - sigsegv in dbscan and wave
 */