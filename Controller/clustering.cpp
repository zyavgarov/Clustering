#include "../Controller.h"

int Controller::dbscan (int search_id, int k) { // 1) what if field is not readonly 2) incidence matrix already created
    /* realises dbscan-clustering with k-core points and d-incidence
     * Errors:
     * -1 - Field is not in readonly mode
     * -2 - Field doesn't exist
     * -3 - No search with such search_id
    */
    if (field_ == nullptr) {
        log ("Field doesn't exist");
        return -2;
    }
    class dbscan x (search_id, k);
    int err = x.err ();
    if (err == 0) {
        log ("Field is clustered");
        return err;
    } else if (err == -1) {
        log ("Field is not in readonly state");
        return err;
    } else if (err == -3) {
        log ("No search with such id");
        return err;
    }
    return 0;
}

int Controller::wave (int search_id) {
    /* realises wave-clustering algorithm
     * Errors:
     * -1 - Field is not in readonly mode
     * -2 - Field doesn't exist
     * -3 - No search with such search_id
     */
    if (field_ == nullptr) {
        log ("Field doesn't exist");
        return -2;
    }
    class wave x (search_id);
    int err = x.err ();
    if (err == 0) {
        log ("Field is clustered");
        return err;
    } else if (err == -1) {
        log ("Field is not in readonly state");
        return err;
    } else if (err == -3) {
        log ("No search with such id");
        return err;
    }
    return 0;
}

vector<int> Controller::s_tree () {
    stree x;
    return x.tree ();
}

int Controller::k_means (int clusters_number) {
    /* realises k-means clustering
     * Errors:
     * -1 - Field is not in readonly mode
     * -2 - Field doesn't exist
     */
    if (field_ == nullptr) {
        log ("Field doesn't exist");
        return -2;
    }
    kmeans x (clusters_number);
    int err = x.err ();
    if (err == 0) {
        log ("Field is clustered");
        return err;
    } else if (err == -1) {
        log ("Field is not in readonly state");
        return err;
    }
    return 0;
}

int Controller::k_means_cores (int clusters_number, int cores_number) {
    /* realises k-means clustering
     * Errors:
     * -1 - Field is not in readonly mode
     * -2 - Field doesn't exist
     */
    if (field_ == nullptr) {
        log ("Field doesn't exist");
        return -2;
    }
    kmcores x (clusters_number, cores_number);
    int err = x.err ();
    if (err == 0) {
        log ("Field is clustered");
        return err;
    } else if (err == -1) {
        log ("Field is not in readonly state");
        return err;
    }
    return 0;
}

int Controller::em (int clusters_number) {
    /* realises k-means clustering
     * Errors:
     * -1 - Field is not in readonly mode
     * -2 - Field doesn't exist
     */
    if (field_ == nullptr) {
        log ("Field doesn't exist");
        return -2;
    }
    class em x (clusters_number);
    int err = x.err ();
    if (err == 0) {
        log ("Field is clustered");
        return err;
    } else if (err == -1) {
        log ("Field is not in readonly state");
        return err;
    }
    return 0;
}

int Controller::hierarchical_algorithm () {
    /* realises k-means clustering
     * Errors:
     * -1 - Field is not in readonly mode
     * -2 - Field doesn't exist
     */
    if (field_ == nullptr) {
        log ("Field doesn't exist");
        return -2;
    }
    ha x;
    int err = x.err ();
    if (err == 0) {
        log ("Field is clustered");
        return err;
    } else if (err == -1) {
        log ("Field is not in readonly state");
        return err;
    }
    return 0;
}

int Controller::forel () {
    /* realises k-means clustering
     * Errors:
     * -1 - Field is not in readonly mode
     * -2 - Field doesn't exist
     */
    if (field_ == nullptr) {
        log ("Field doesn't exist");
        return -2;
    }
    class forel x;
    int err = x.err ();
    if (err == 0) {
        log ("Field is clustered");
        return err;
    } else if (err == -1) {
        log ("Field is not in readonly state");
        return err;
    }
    return 0;
}

int Controller::delaunay () {
    /*realises Delaunay triangulation
     * Errors:
     * -1 - Field is not in readonly mode
     * -2 - Field doesn't exist
     * -3 - Not enough points
     */
    if (field_ == nullptr) {
        log ("Field doesn't exist");
        return -2;
    }
    class delaunay x;
    int err = x.err ();
    if (err == 0) {
        log ("Field is clustered");
        return err;
    } else if (err == -1) {
        log ("Field is not in readonly state");
        return err;
    } else if (err == -2) {
        log ("Not enough points");
        return err;
    }
    return 0;
}

double Controller::predict (int &err, double x, double y) {
/*realises Delaunay triangulation
     * Errors:
     * -1 - Field is not in readonly mode
     * -2 - Field doesn't exist
     * -3 - Not enough points
     */
    if (field_ == nullptr) {
        log ("Field doesn't exist");
        return -2;
    }
    class predictions pr;
    double result = predictions::predict (x, y);
    err = predictions::err ();
    if (err == 0) {
        log ("The value is predicted");
        return result;
    } else if (err == -1) {
        log ("Field is not in readonly state");
        return 0;
    } else if (err == -2) {
        log ("Point is not in cluster");
        return 0;
    }
    return result;
}

void Controller::log (const string &s, int fd) {
    log (to_string (fd) + ": " + s);
}
