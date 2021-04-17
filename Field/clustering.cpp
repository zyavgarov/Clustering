#include "../Field.h"

int Field::forel () {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!readonly ()) {
        return -1;
    }
    searches_.emplace_back (this);
    searches_.back ().forel ();
    return 0;
}
