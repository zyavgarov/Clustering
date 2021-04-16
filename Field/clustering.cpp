#include "../Field.h"

int Field::k_means_cores (int clusters_number, int cores_number) {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!readonly ()) {
        return -1;
    }
    searches_.emplace_back (this);
    searches_.back ().k_means_cores (clusters_number, cores_number);
    return 0;
}

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
