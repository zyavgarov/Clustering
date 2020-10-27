#include "Field.h"

Field::Field () : readonly_ (false) {
}

Field &Field::operator= (Field const &f) {
    if (this != &f) {
        for (auto &c: cloud_) {
            delete c;
        }
        cloud_.clear ();
        for (int i = 0; i < f.length (); ++i) {
            cloud_.push_back (f.cloud_[i]);
        }
    }
    return *this;
}

Field::Field (Field const &f) {
    for (auto &c: cloud_) {
        delete c;
    }
    cloud_.clear ();
    for (int i = 0; i < f.length (); ++i) {
        cloud_[i] = f.cloud_[i];
    }
}

Field::~Field () = default;

int Field::add (Cloud *addition) {
    // The function adds cloud to the field
    // returns -1 if readonly
    if (readonly ()) {
        return -1;
    }
    cloud_.push_back (addition);
    return 0;
}

int Field::fprintf (ofstream &out) const {
    //prints all the clouds to file out
    for (int i = 0; i < length (); ++i) {
        cloud_[i]->fprintf (out);
    }
    return 0;
}

int Field::length () const {
    // returns the number of clouds in the field
    return cloud_.size ();
}

const vector<Cloud *> &Field::cloud () {
    return cloud_;
}

const vector<vector<double>> &Field::dist () const {
    return dist_;
}

bool Field::readonly () const {
    return readonly_;
}

void Field::create_dist_matrix () {
    readonly_ = true;
    dist_.clear ();
    dist_ = vector<vector<double>> (Point::quantity (), vector<double> (Point::quantity ()));
    for (int i = 0; i < Point::quantity (); ++i) {
        for (int j = i; j < Point::quantity (); ++j) {
            dist_[i][j] = dist_[j][i] = Point::dist (Point::get_by_id (i + 1), Point::get_by_id (j + 1));
        }
    }
}

const vector<Cluster_Search> &Field::searches () const {
    return searches_;
}

void Field::create_edges_matrix (double delta) {
    searches_.push_back (Cluster_Search (this, delta));
}

int Field::wave_clustering () {
    searches_.back ().wave ();
    return 0;
}

int Field::db_clustering (int k) {
    searches_.back ().dbscan (k);
    return 0;
}

vector<int> Field::s_tree () {
//searching minimum in dist matrix
    int min_i = 0;
    int min_j = 0;
    double min_dist_all = dist ()[0][0];
    vector<bool> added (Point::quantity (), false);
    for (int i = 0; i < Point::quantity (); ++i) {
        for (int j = i + 1; j < Point::quantity (); ++j) {
            if (dist ()[i][j] < min_dist_all) {
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
    return histogram;
}

int Field::add_closest_point (TreeNode<int> &node, vector<bool> &added) {
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

void Field::get_closest (TreeNode<int> &current,
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
        if (dist ()[current.value ()][i] < min_dist && !added[i]) {
            min_dist = dist ()[current.value ()][i];
            tree_node = &current;
            delete out_node;
            out_node = new TreeNode<int> (i);
        }
    }
}

vector<int> Field::create_histogram (TreeNode<int> &root, int pieces) {
    // creates a histogram of distances in minimal spanning tree made previously
    // root is a root of tree. pieces is a pieces of histogram
    
    // searching the max and min distance in tree
    double max_dist = dist ()[root.value ()][root.first_child ()->value ()];
    double min_dist = max_dist;
    TreeNode<int> *root_pointer = &root;
    get_tree_range (root_pointer, max_dist, min_dist);
    
    // picking up the distances in tree and filling the vector of histogram
    vector<int> histogram (pieces, 0);
    picking_histogram (&root, min_dist, max_dist, histogram);
    return histogram;
}

void Field::get_tree_range (TreeNode<int> *&node, double &max_dist, double &min_dist) {
    if (node->first_child () != nullptr) {
        double dist_to_child = dist ()[node->value ()][node->first_child ()->value ()];
        if (dist_to_child > max_dist) {
            max_dist = dist_to_child;
        } else if (dist_to_child < min_dist) {
            min_dist = dist_to_child;
        }
        get_tree_range (node->first_child_, max_dist, min_dist);
    }
    if (node->brother () != nullptr) {
        double dist_to_brother = dist ()[node->value ()][node->first_child ()->value ()];
        if (dist_to_brother > max_dist) {
            max_dist = dist_to_brother;
        } else if (dist_to_brother < min_dist) {
            min_dist = dist_to_brother;
        }
        get_tree_range (node->brother_, max_dist, min_dist);
    }
}

void Field::picking_histogram (TreeNode<int> *node,
                               double max_dist,
                               double min_dist,
                               vector<int> &histogram) {
    if (node->first_child () != nullptr) {
        put_value_to_histogram (histogram,
                                min_dist,
                                max_dist,
                                dist ()[node->value ()][node->first_child ()->value ()]);
    }
    if (node->brother () != nullptr) {
        put_value_to_histogram (histogram,
                                min_dist,
                                max_dist,
                                dist ()[node->value ()][node->first_child ()->value ()]);
    }
}

void Field::put_value_to_histogram (vector<int> &histogram,
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
