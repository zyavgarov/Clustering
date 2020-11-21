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
    if (delta != 0) {
        // a little defence for kmeans
        create_edges_matrix ();
    }
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
    // counts number of neighbours for all the points
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
                        if (!burnt[j] && i != j && edges ()[i][j]) {
                            new_added.push_back (j);
                            burnt[j] = true;
                        }
                    }
                }
                curr_cluster.insert (curr_cluster.end (), new_added.begin (), new_added.end ());
                curr_added = new_added;
                new_added.clear ();
            }
            clusters.emplace_back (curr_cluster);
        } else if (state[l] == 0) {
            // Doesn't seem to be true.
            // That point can be connected to core point which has id number more than ours
            clusters.emplace_back (vector<int> (1, l));
            burnt[l] = true;
        }
    }
    dbscan_fprintf_type (state);
    //fprintf_clusters();
}

Cluster_Search Cluster_Search::wave () {
    wave_clustering ();
    return *this;
}

Cluster_Search Cluster_Search::dbscan (int density) {
    db_clustering (db_sorting (density));
    return *this;
}

Cluster_Search::Cluster_Search (const Cluster_Search &cs)
    : delta (cs.delta), k (cs.k), field_ (cs.field_), clusters (cs.clusters), edges_ (cs.edges_) {
}

Cluster_Search &Cluster_Search::operator= (const Cluster_Search &cs) {
    if (this != &cs) {
        delta = cs.delta;
        k = cs.k;
        field_ = cs.field_;
        clusters = cs.clusters;
        edges_ = cs.edges_;
    }
    return *this;
}

void Cluster_Search::dbscan_fprintf_type (const vector<int> &state) {
    ofstream core ("core.txt");
    ofstream peripherical ("peripherical.txt");
    for (int i = 0; i < state.size (); ++i) {
        if (state[i] == 2) {
            core << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
        } else if (state[i] == 1) {
            peripherical << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
        }
    }
}

Cluster_Search Cluster_Search::k_means (int clusters_number) {
    bool changed = true;
    vector<Point> cores (clusters_number); // Pointers to core points
    vector<int> nearest_cluster (Point::quantity (), 0); // shows which element of cores is the nearest to that point
    // filling core vector by random points
    for (int i = 0; i < cores.size (); ++i) {
        const Point *t = Point::get_by_id (i + 1);
        cores[i] = Point (t->x (), t->y (), 0);
    }
    int iteration = 0;
    while (changed || iteration == 1) {
        // searching the closest point for each field point
        changed = false;
        for (int i = 0; i < Point::quantity (); ++i) {
            for (int j = 0; j < cores.size (); ++j) {
                double a = Point::dist (Point::get_by_id (i + 1), &cores[j]);
                double b = Point::dist (Point::get_by_id (i + 1), &cores[nearest_cluster[i]]);
                if (a < b) {
                    changed = true;
                    nearest_cluster[i] = j;
                }
            }
        }
        // recomputing cores
        vector<int> cluster_length (clusters_number, 0);
        for (auto &core : cores) {
            core = Point ();
        }
        for (int i = 0; i < Point::quantity (); ++i) {
            cores[nearest_cluster[i]].shift (*Point::get_by_id (i + 1));
            cluster_length[nearest_cluster[i]]++;
        }
        for (int i = 0; i < cores.size (); ++i) {
            cores[i] = Point (cores[i].x () / cluster_length[i],
                              cores[i].y () / cluster_length[i],
                              0);
        }
        kmeans_fprintf (nearest_cluster, cores, iteration);
        iteration++;
    }
    clusters.clear ();
    vector<vector<int>> clusters_to_set (clusters_number);
    for (int i = 0; i < nearest_cluster.size (); ++i) {
        clusters_to_set[nearest_cluster[i]].push_back (i);
    }
    for (int i = 0; i < clusters_number; ++i) {
        clusters.emplace_back (clusters_to_set[i]);
    }
    return *this;
}

void Cluster_Search::kmeans_fprintf (vector<int> &nearest_cluster, vector<Point> &cores, int iteration) {
    ofstream out ("kmeans/km" + to_string (iteration) + ".txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        out << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << " " << nearest_cluster[i]
            << endl;
    }
    for (auto &core : cores) {
        out << core.x () << " " << core.y () << " " << -1
            << endl;
    }
}

Cluster_Search Cluster_Search::em (int clusters_number) {
    /* realises EM-clustering algorithm
     */
    vector<vector<double>> probabilities;
    // probabilities[i][j] = 0.6 means that it's 60% chance
    // for point with id i to be contained in cluster №j
    return Cluster_Search (nullptr);
}

Cluster_Search Cluster_Search::k_means_cores (int clusters_number, int cores_number) {
    /* realises k-means withcores clustering
     * clusters_number shows how many clusters we'll get
     * cores_number shows how many cores each cluster contains
     */
    vector<int> nearest_cluster (Point::quantity (), 0);
    // shows which cluster's core is the nearest to that point.
    // values are from 0 to clusters_number-1
    vector<int> nearest_core (Point::quantity (), 0);
    // shows which core in cluster is the closest.
    // Values are from 0 to cores_number-1
    vector<vector<Point>> cores (clusters_number, vector<Point> (cores_number)); // Cores of clusters itself
    // filling core vector by random points
    for (int i = 0; i < cores.size (); ++i) {
        for (int j = 0; j < cores[i].size (); ++j) {
            const Point *t = Point::get_by_id (i * cores.size () + j + 1);
            cores[i][j] = Point (t->x (), t->y (), 0);
        }
    }
    // iterations of k-means
    int iteration = 0;
    bool changed = true;
    while (changed) {
        // searching the closest core point for each field point
        changed = false;
        for (int i = 0; i < Point::quantity (); ++i) {
            for (int j = 0; j < cores.size (); ++j) {
                for (int l = 0; l < cores[j].size (); ++l) {
                    double a = Point::dist (Point::get_by_id (i + 1), &cores[j][l]);
                    double b = Point::dist (Point::get_by_id (i + 1), &cores[nearest_cluster[i]][nearest_core[i]]);
                    if (a < b) {
                        changed = true;
                        nearest_cluster[i] = j;
                        nearest_core[i] = l;
                    }
                }
            }
        }
        // recomputing cores
        // vector<vector<int>> cluster_length_ (clusters_number, vector<int> (cores_number, 0));
        for (auto &cluster : cores) {
            for (auto &core : cluster) {
                core = Point ();
            }
        }
        // here we just collect points we need before the action
        // the first step is running kmeans for each cluster
        for (int i = 0; i < cores.size (); i++) {
            auto &cluster = cores[i];
            // searching which points current cluster contains
            vector<int> curr_points;
            // throws double free or corruption if number of clusters is not equal to number of cores.
            // maybe i'm messing up that number while counting length of vectors
            // another hypotheses says that something wrong with Point::shift function
            for (int p = 0; p < Point::quantity (); ++p) {
                if (nearest_cluster[p] == i) {
                    curr_points.push_back (p);
                }
            }
            for (int j = 0; j < cluster.size (); ++j) {
                const Point *t = Point::get_by_id (curr_points[j] + 1);
                cluster[j] = Point (t->x (), t->y (), 0);
            }
            bool changed_in = true;
            int iteration_in = 0;
            while (changed_in) {
                changed_in = false;
                for (int curr_point : curr_points) {
                    for (int j = 0; j < cluster.size (); ++j) {
                        double a = Point::dist (Point::get_by_id (curr_point + 1), &cluster[j]);
                        double b = Point::dist (Point::get_by_id (curr_point + 1), &cluster[nearest_core[curr_point]]);
                        if (a < b) {
                            changed_in = true;
                            nearest_core[curr_point] = j;
                        }
                    }
                }
                // recomputing cores
                vector<int> subcluster_length (cores_number, 0);
                for (auto &l : cluster) {
                    l = Point ();
                }
                for (int curr_point : curr_points) {
                    cluster[nearest_core[curr_point]].shift (*Point::get_by_id (curr_point + 1));
                    subcluster_length[nearest_core[curr_point]]++;
                }
                for (int l = 0; l < cluster.size (); ++l) {
                    cluster[l] = Point (cluster[l].x () / subcluster_length[l],
                                        cluster[l].y () / subcluster_length[l],
                                        0);
                }
                iteration_in++;
            }
        }
        kmeans_core_fprintf (nearest_cluster, cores, iteration);
        iteration++;
    }
    clusters.clear ();
    vector<vector<int>> clusters_to_set (clusters_number);
    for (int i = 0; i < nearest_cluster.size (); ++i) {
        clusters_to_set[nearest_cluster[i]].push_back (i);
    }
    for (int i = 0; i < clusters_number; ++i) {
        clusters.emplace_back (clusters_to_set[i]);
    }
    return *this;
}

void Cluster_Search::kmeans_core_fprintf (const vector<int> &nearest_cluster,
                                          const vector<vector<Point>> &cores,
                                          int iteration) {
    ofstream out ("kmeans/km" + to_string (iteration) + ".txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        out << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << " " << nearest_cluster[i]
            << endl;
    }
    for (auto &cluster : cores) {
        for (auto &core:cluster) {
            out << core.x () << " " << core.y () << " " << -1
                << endl;
        }
    }
}
