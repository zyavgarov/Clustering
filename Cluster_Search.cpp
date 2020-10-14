#include "Cluster_Search.h"
#include "Field.h"

Cluster_Search::Cluster::Cluster (vector<Point> vec) : points (vec) {
}

Cluster_Search::Cluster::Cluster (const vector<int> &vec) {
    vector<Point> a;
    a.reserve (vec.size ());
    for (int i : vec) {
        a.push_back (Point::get_by_id (i));
    }
}

const vector<vector<bool>> &Cluster_Search::edges () const {
    return edges_;
}

void Cluster_Search::add (const Cluster_Search::Cluster &addition) {
    clusters.push_back (addition);
}

Cluster_Search::Cluster_Search (Field *field, double delta, int k) : field_ (field), delta (delta), k (k) {
    
    create_edges_matrix ();
}

void Cluster_Search::create_edges_matrix () {
    // matrix of incidences
    if (edges_.empty ()) {
        vector<vector<bool>> incidence (Point::quantity (), vector<bool> (Point::quantity (), false));
        for (int i = 0; i < Point::quantity (); ++i) {
            for (int j = i + 1; j < Point::quantity (); ++j) {
                incidence[j][i] = incidence[i][j] = (field_->dist ()[i][j] < delta);
            }
        }
        edges_ = incidence;
    }
}

void Cluster_Search::wave_clustering () {
    vector<bool> burnt (Point::quantity (), false);
    int burnt_num = 0; // number of true in burnt vector
    // that cycle checks if point in marked cluster and if not creates new one
    for (int m = 1; m <= Point::quantity (); ++m) {
        if (burnt[m]) {
            continue;
        }
        vector<int> curr_wave = {m};
        vector<int> new_wave = curr_wave; // points, connected to smth from curr_wave
        vector<int> curr_cluster; // points from current cluster
        // that cycle searches for neighbours of points in curr_wave
        while (!new_wave.empty ()) {
            for (int i = 0; i < curr_wave.size (); ++i) {
                for (int j = 0; j < Point::quantity (); ++j) {
                    if (i != j && edges ()[i][j] && !burnt[j]) {
                        new_wave.push_back (j);
                    }
                }
            }
            curr_cluster.insert (curr_cluster.end (), curr_wave.begin (), curr_wave.end ());
            curr_wave = new_wave;
            burnt_num += new_wave.size ();
            for (auto &&i: new_wave) {
                burnt[i] = true;
            }
        }
        add (Cluster_Search::Cluster (curr_cluster));
    }
}

vector<int> Cluster_Search::db_sorting () const {
    // counting neighbours for all the points
    vector<int> type (Point::quantity (), 0);
    for (int i = 0; i < Point::quantity (); ++i) {
        vector<int> neighbours;
        for (int j = 0; j < Point::quantity (); ++j) {
            if (edges ()[i][j]) {
                neighbours.push_back (j);
            }
        }
        if (neighbours.size () >= k) {
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

void Cluster_Search::db_clustering (const vector<int> &state) {
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
                for (int i = 0; i < curr_added.size (); ++i) {
                    for (int j = 0; j < Point::quantity (); ++j) {
                        if (!burnt[j] && i != j && edges ()[i][j]) {
                            new_added.push_back (j);
                            burnt[j] = true;
                        }
                    }
                }
                curr_cluster.insert (curr_cluster.end (), new_added.begin (), new_added.end ());
                curr_added = new_added;
            }
            clusters.emplace_back (curr_cluster);
        } else if (state[l] == 0) {
            clusters.emplace_back (vector<int> (1, l));
            burnt[l] = true;
        }
        /* The problems are:
         * - bruteforcing of Points sometimes can start from 0. This is not allowed
         * - curr_cluster vector in some algorithms is removed before going to the next point
         * - I still need to finish the functions about clustering
         */
    }
}

Cluster_Search Cluster_Search::wave () {
    create_edges_matrix();
    wave_clustering();
    return *this;
}

Cluster_Search Cluster_Search::dbscan () {
    create_edges_matrix();
    db_clustering(db_sorting());
    return *this;
}
