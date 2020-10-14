#include <iostream>
#include <fstream>
#include "Interface.h"

int main () {
    Controller cc;
    Interface ii(0, true, &cc);
    ifstream s("run.txt");
    ii.run(s);
}
