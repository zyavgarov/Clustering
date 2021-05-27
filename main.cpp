#include "Interface.h"

int main () {
    auto *ss = new Sender (true);
    Interface I;
    Interface::run ();
    delete ss;
}
