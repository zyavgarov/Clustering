#include "dbscan.h"

int dbscan::err () const {
    return err_;
}

dbscan::dbscan (int search_id, int density) : search_id (search_id), density (density) {
    /* Errors
     * -1 field is not in readonly mode
     * -3 There is no such Cluster_Search
     */
    if (!Field::readonly ()) {
        err_ = -1;
        return;
    }
    if (search_id >= Field::searches ().size ()) {
        err_ = -3;
        return;
    }
    db_clustering (db_sorting ());
    err_ = 0;
}

vector<int> dbscan::db_sorting () const {
    // counts number of neighbours for all the points
    vector<int> type (Point::quantity (), 0);
    for (int i = 0; i < Point::quantity (); ++i) {
        vector<int> neighbours;
        for (int j = 0; j < Point::quantity (); ++j) {
            if (Field::searches ()[search_id].edges ()[i][j]) {
                neighbours.push_back (j);
            }
        }
        if (neighbours.size () >= density) {
            type[i] = 2;
            for (int neighbour : neighbours) {
                if (type[neighbour] != 2) {
                    type[neighbour] = 1;
                }
            }
        }
    }
    return type;
}

void dbscan::db_clustering (const vector<int> &state) const {
    // picks up core points and finds its cluster
    // saves info about the states of points to file
    // result of clustering saved to file too
    vector<bool> burnt (Point::quantity (), false);
    for (int l = 0; l < state.size (); ++l) {
        if (burnt[l]) {
            continue;
        }
        vector<int> curr_cluster;
        if (state[l] == 2) {
            vector<int> curr_added;
            curr_added.push_back (l);
            vector<int> new_added;
            while (!curr_added.empty ()) {
                for (int i : curr_added) {
                    for (int j = 0; j < Point::quantity (); ++j) {
                        if (!burnt[j] && i != j && Field::searches ()[search_id].edges ()[i][j]) {
                            new_added.push_back (j);
                            burnt[j] = true;
                        }
                    }
                }
                curr_cluster.insert (curr_cluster.end (), new_added.begin (), new_added.end ());
                curr_added = new_added;
                new_added.clear ();
            }
            Field::searches_[search_id].clusters.emplace_back (curr_cluster);
        } else if (state[l] == 0) {
            // Doesn't seem to be true.
            // That point can be connected to core point which has id number more than ours
            Field::searches_[search_id].clusters.emplace_back (vector<int> (1, l));
            burnt[l] = true;
        }
    }
    dbscan_fprintf_type (state);
}

void dbscan::dbscan_fprintf_type (const vector<int> &state) {
    ofstream core ("gnuplot/dbscan/core.txt");
    ofstream peripherical ("gnuplot/dbscan/peripherical.txt");
    for (int i = 0; i < state.size (); ++i) {
        if (state[i] == 2) {
            core << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
        } else if (state[i] == 1) {
            peripherical << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
        }
    }
}