#include "stree.h"

stree::stree () {
    //searching minimum in dist matrix
    int min_i = 0;
    int min_j = 0;
    double min_dist_all = Field::dist ()[0][1];
    vector<bool> added (Point::quantity (), false);
    for (int i = 0; i < Point::quantity (); ++i) {
        for (int j = i + 1; j < Point::quantity (); ++j) {
            if (Field::dist ()[i][j] < min_dist_all) {
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
        closest = stree_add_closest_point (root, added);
    } while (closest >= 0);
    stree_fprintf_tree ("gnuplot/stree/tree.txt", &root);
    tree_ =  stree_create_histogram (root, 30);
}

int stree::stree_add_closest_point (TreeNode<int> &node, vector<bool> &added) {
    // searches for closest point to tree and adds it
    // returns number of the point
    // point which has closest neighbour out of tree
    TreeNode<int> *cursor = &node;
    TreeNode<int> *out_node = nullptr;
    double dist = -1;
    stree_get_closest (node, added, dist, cursor, out_node);
    if (out_node != nullptr) {
        cursor->add_child (out_node);
        added[out_node->value ()] = true;
        return out_node->value ();
    } else {
        return -1;
    }
}

void stree::stree_get_closest (TreeNode<int> &current,
                               vector<bool> &added,
                               double &min_dist,
                               TreeNode<int> *&tree_node,
                               TreeNode<int> *&out_node) {
    // function searches the closest point to the branch current in tree
    // min dist is the distance to closest point to tree
    // added[i] is true if point i in tree
    // tree_node and out node are ends of smallest distance
    // if found, function changes min_dist, tree_node (to show where new edge should start) \
    // and out_node (showing the end of a new edge)
    // dist named -1 means that every distance is less than given
    if (current.first_child () != nullptr) {
        stree_get_closest (*current.first_child_, added, min_dist, tree_node, out_node);
    }
    if (current.brother () != nullptr) {
        stree_get_closest (*current.brother_, added, min_dist, tree_node, out_node);
    }
    for (int i = 0; i < Point::quantity (); ++i) {
        double a = Field::dist ()[current.value ()][i];
        bool b = (min_dist == -1) || (a < min_dist);
        if (b && !added[i] && (i != current.value ())) {
            min_dist = Field::dist ()[current.value ()][i];
            tree_node = &current;
            delete out_node;
            out_node = new TreeNode<int> (i);
        }
    }
}

vector<int> stree::stree_create_histogram (TreeNode<int> &root, int pieces) {
    // creates a histogram of distances in minimal spanning tree made previously
    // root is a root of tree. pieces is a pieces of histogram
    
    // searching the max and min distance in tree
    double max_dist = Field::dist ()[root.value ()][root.first_child ()->value ()];
    double min_dist = max_dist;
    TreeNode<int> *root_pointer = &root;
    stree_get_tree_range (root_pointer, min_dist, max_dist);
    
    // picking up the distances in tree and filling the vector of histogram
    vector<int> histogram (pieces, 0);
    stree_picking_histogram (&root, min_dist, max_dist, histogram);
    return histogram;
}

void stree::stree_get_tree_range (TreeNode<int> *&node, double &min_dist, double &max_dist) {
    if (node->first_child () != nullptr) {
        double dist_to_child = Field::dist ()[node->value ()][node->first_child ()->value ()];
        if (dist_to_child > max_dist) {
            max_dist = dist_to_child;
        } else if (dist_to_child < min_dist) {
            min_dist = dist_to_child;
        }
        stree_get_tree_range (node->first_child_, min_dist, max_dist);
    }
    if (node->brother () != nullptr) {
        double dist_to_brother = Field::dist ()[node->value ()][node->brother ()->value ()];
        if (dist_to_brother > max_dist) {
            max_dist = dist_to_brother;
        } else if (dist_to_brother < min_dist) {
            min_dist = dist_to_brother;
        }
        stree_get_tree_range (node->brother_, min_dist, max_dist);
    }
}

void stree::stree_picking_histogram (TreeNode<int> *node, double min_dist, double max_dist, vector<int> &histogram) {
    if (node->brother () != nullptr) {
        stree_picking_histogram (node->brother_, min_dist, max_dist, histogram);
        stree_put_value_to_histogram (histogram,
                                      min_dist,
                                      max_dist,
                                      Field::dist ()[node->value ()][node->brother ()->value ()]);
    }
    if (node->first_child () != nullptr) {
        stree_picking_histogram (node->first_child_, min_dist, max_dist, histogram);
        stree_put_value_to_histogram (histogram,
                                      min_dist,
                                      max_dist,
                                      Field::dist ()[node->value ()][node->first_child ()->value ()]);
    }
    
}

void stree::stree_put_value_to_histogram (vector<int> &histogram,
                                          double min_dist,
                                          double max_dist,
                                          double distance) {
    // searches the place of number in histogram and when found increments that row in histogram
    double step = (max_dist - min_dist) / (histogram.size ());
    int i = 0;
    while (distance > min_dist + step * (i + 1)) {
        i++;
    }
    histogram[i]++;
}

void stree::stree_fprintf_tree (const string &file_name, TreeNode<int> *root) {
    ofstream out (file_name);
    stree_fprintf_node (root, &out);
}

void stree::stree_fprintf_node (TreeNode<int> *node, ofstream *out) {
    if (node->brother () != nullptr) {
        stree_fprintf_node (node->brother_, out);
    }
    if (node->first_child () != nullptr) {
        stree_fprintf_node (node->first_child_, out);
    }
    TreeNode<int> *cursor = node->first_child_;
    while (cursor != nullptr) {
        *out << Point::get_by_id (node->value () + 1)->x () << " " << Point::get_by_id (node->value () + 1)->y ()
             << endl;
        *out << Point::get_by_id (cursor->value () + 1)->x () << " " << Point::get_by_id (cursor->value () + 1)->y ()
             << endl;
        *out << endl;
        cursor = cursor->brother_;
    }
}

vector<int> stree::tree () {
    return tree_;
}
