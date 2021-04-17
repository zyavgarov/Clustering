#include "../Field.h"

int Field::hieararchical_algorithm () {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!readonly ()) {
        return -1;
    }
    searches_.emplace_back (this);
    searches_.back ().hierarchical_algorithm ();
    return 0;
}

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
