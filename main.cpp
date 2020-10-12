#include <iostream>
#include "Interface.h"

int main () {
    Controller cc;
    Interface ii(0, true, &cc);
    ii.run();
}
