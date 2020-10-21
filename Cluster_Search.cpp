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
}

Cluster_Search Cluster_Search::wave () {
    wave_clustering ();
    return *this;
}

Cluster_Search Cluster_Search::dbscan (int density) {
    db_clustering (db_sorting (density));
    return *this;
}

Cluster_Search Cluster_Search::s_tree () {
    //searching minimum in dist matrix
    int min_i = 0;
    int min_j = 0;
    int min_dist_all = field_->dist ()[0][0];
    vector<bool> added (Point::quantity (), false);
    for (int i = 0; i < Point::quantity (); ++i) {
        for (int j = i + 1; j < Point::quantity (); ++j) {
            if (field_->dist ()[i][j] < min_dist_all) {
                min_i = i;
                min_j = j;
            }
        }
    }
    TreeNode<int> root (min_i);
    root.add_child (min_j);
    added[min_i] = added[min_j] = true;
    int closest;
    do {
        closest = add_closest_point (root, added);
    } while (closest >= 0);
    vector<int> histogram = create_histogram (root, 30);
    return Cluster_Search (nullptr, 0);
}

int Cluster_Search::add_closest_point (TreeNode<int> &node, vector<bool> &added) {
    // searches for closest point to tree and adds it
    // returns number of the point
    TreeNode<int> *curr_min; // point which has closest neighbour out of tree
    TreeNode<int> *cursor = &node;
    TreeNode<int> *out_node = nullptr;
    double dist;
    get_closest (node, added, dist, cursor, out_node);
    if (out_node != nullptr) {
        cursor->add_child (out_node);
        return out_node->value ();
    } else {
        return -1;
    }
}

vector<int> Cluster_Search::create_histogram (TreeNode<int> root, int pieces) {
    // creates a histogram of distances in minimal spanning tree made previously
    // root is a root of tree. pieces is a pieces of histogram
    
    // searching the max and min distance in tree
    double max_dist = field_->dist ()[root.value ()][root.first_child ()->value ()];
    double min_dist = max_dist;
    TreeNode<int> *root_pointer = &root;
    get_tree_range (root_pointer, max_dist, min_dist);
    
    // picking up the distances in tree and filling the vector of histogram
    vector<int> histogram (pieces, 0);
    picking_histogram (&root, min_dist, max_dist, histogram);
    return histogram;
}

void Cluster_Search::get_closest (TreeNode<int> &current,
                                  vector<bool> &added,
                                  double &min_dist,
                                  TreeNode<int> *&tree_node,
                                  TreeNode<int> *&out_node) {
    // function searches the closest point to the branch current in tree
    // min dist is the distance to closest point to tree
    // added[i] is true if point i in tree
    // tree_node and out node are ends of smallest distance
    if (current.first_child () != nullptr) {
        get_closest (*current.first_child_, added, min_dist, tree_node, out_node);
    }
    if (current.brother () != nullptr) {
        get_closest (*current.brother_, added, min_dist, tree_node, out_node);
    }
    for (int i = 0; i < Point::quantity (); ++i) {
        if (field_->dist ()[current.value ()][i] < min_dist && !added[i]) {
            min_dist = field_->dist ()[current.value ()][i];
            tree_node = &current;
            delete out_node;
            out_node = new TreeNode<int> (i);
        }
    }
}

void Cluster_Search::get_tree_range (TreeNode<int> *&node, double &max_dist, double &min_dist) {
    if (node->first_child () != nullptr) {
        double dist_to_child = field_->dist ()[node->value ()][node->first_child ()->value ()];
        if (dist_to_child > max_dist) {
            max_dist = dist_to_child;
        } else if (dist_to_child < min_dist) {
            min_dist = dist_to_child;
        }
        get_tree_range (node->first_child_, max_dist, min_dist);
    }
    if (node->brother () != nullptr) {
        double dist_to_brother = field_->dist ()[node->value ()][node->first_child ()->value ()];
        if (dist_to_brother > max_dist) {
            max_dist = dist_to_brother;
        } else if (dist_to_brother < min_dist) {
            min_dist = dist_to_brother;
        }
        get_tree_range (node->brother_, max_dist, min_dist);
    }
}

void Cluster_Search::picking_histogram (TreeNode<int> *node,
                                        double max_dist,
                                        double min_dist,
                                        vector<int> &histogram) {
    if (node->first_child () != nullptr) {
        put_value_to_histogram (histogram,
                                min_dist,
                                max_dist,
                                field_->dist ()[node->value ()][node->first_child ()->value ()]);
    }
    if (node->brother () != nullptr) {
        put_value_to_histogram (histogram,
                                min_dist,
                                max_dist,
                                field_->dist ()[node->value ()][node->first_child ()->value ()]);
    }
}

void Cluster_Search::put_value_to_histogram (vector<int> &histogram,
                                             double min_dist,
                                             double max_dist,
                                             double distance) {
    // searches the place of number in histogram and when found increments that row in histogram
    double step = (max_dist - min_dist) / (histogram.size ());
    int i = 0;
    while (distance > min_dist + step * i) {
        i++;
    }
    histogram[i]++;
}
