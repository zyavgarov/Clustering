#include "Cluster_Search.h"
#include "Field.h"

Cluster_Search::Cluster::Cluster (vector<const Point *> vec) : points (vec) {
}

Cluster_Search::Cluster::Cluster (const vector<int> &vec) {
    vector<const Point *> a;
    a.reserve (vec.size ());
    for (int i : vec) {
        a.push_back (Point::get_by_id (i + 1));
    }
    points = a;
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
    // that cycle checks if point in marked cluster and if not creates new one
    for (int m = 0; m < Point::quantity (); ++m) {
        if (burnt[m]) {
            continue;
        }
        vector<int> curr_wave = {m};
        burnt[m] = true;
        vector<int> next_wave; // points, connected to smth from curr_wave
        vector<int> curr_cluster; // points from current cluster
        // that cycle searches for neighbours of points in curr_wave
        while (!curr_wave.empty ()) {
            for (int i : curr_wave) {
                for (int j = m + 1; j < Point::quantity (); ++j) {
                    if (edges ()[i][j] && !burnt[j]) {
                        next_wave.push_back (j);
                    }
                }
            }
            curr_cluster.insert (curr_cluster.end (), curr_wave.begin (), curr_wave.end ());
            curr_wave = next_wave;
            for (auto &&i: next_wave) {
                burnt[i] = true;
            }
            next_wave.clear ();
        }
        add (Cluster_Search::Cluster (curr_cluster));
    }
}

vector<int> Cluster_Search::db_sorting (int density) {
    // counting neighbours for all the points
    k = density;
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
                for (int i : curr_added) {
                    for (int j = 0; j < Point::quantity (); ++j) {
                        if (!burnt[j] && i != j && edges ()[i][j]) {
                            new_added.push_back (j);
                            burnt[j] = true;
                        }
                    }
                }
                curr_cluster.insert (curr_cluster.end (), new_added.begin (), new_added.end ());
                curr_added = new_added;
                new_added.clear();
            }
            clusters.emplace_back (curr_cluster);
        } else if (state[l] == 0) {
            // Doesn't seem to be true.
            // That point can be connected to core point which has id number more than ours
            clusters.emplace_back (vector<int> (1, l));
            burnt[l] = true;
        }
    }
}

Cluster_Search Cluster_Search::wave () {
    wave_clustering ();
    return *this;
}

Cluster_Search Cluster_Search::dbscan (int density) {
    db_clustering (db_sorting (density));
    return *this;
}
