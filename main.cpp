#include "Interface.h"

int main () {
    auto *ss = new Sender (true);
    Interface I;
    Interface::run ();
    delete ss;
}
/* to be done:
 * - async between getting info from user and from server
 */

// There is an idea that getting and sending messages should be separated to differnet threads