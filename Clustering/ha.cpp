//
// Created by pierre on 17/04/2021.
//

#include "ha.h"

int ha::err () {
    return 0;
}

ha::ha () {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!Field::readonly ()) {
        err_ = -1;
        return;
    }
    auto * s = new Field;
    Field::searches_.emplace_back (s);
    hierarchical_algorithm ();
    err_ = 0;
}

void ha::hierarchical_algorithm () {
    vector<TreeNode<const Point *> *> tree_nodes;
    tree_nodes.reserve (Point::quantity ());
    ofstream tree ("gnuplot/ha/ha_tree.txt");
    for (int p = 0; p < Point::quantity (); ++p) {
        tree_nodes.push_back (new TreeNode<const Point *> (Point::get_by_id (p + 1)));
    }
    while (1 < tree_nodes.size ()) {
        int a, b;
        ha_get_closest_nodes (a, b, tree_nodes);
        Point old_1 (tree_nodes[a]->value ()->x (), tree_nodes[a]->value ()->y (), 0);
        Point old_2 (tree_nodes[b]->value ()->x (), tree_nodes[b]->value ()->y (), 0);
        ha_merge_nodes (a, b, tree_nodes);
        ha_fprintf (tree_nodes, Point::quantity () - tree_nodes.size (), old_1, old_2, tree);
    }
    Field::searches_.back().tree_root_ = tree_nodes[0];
}

void ha::ha_get_closest_nodes (int &a, int &b, const vector<TreeNode<const Point *> *> &tree_node) {
    // searches for closest nodes writes its numbers to a and b
    double min_dist = Field::dist ()[0][1];
    int min_a = 0;
    int min_b = 1;
    for (a = 0; a < tree_node.size (); ++a) {
        for (b = a + 1; b < tree_node.size (); ++b) {
            double distance = Point::dist (tree_node[a]->value (), tree_node[b]->value ());
            if (distance < min_dist) {
                min_dist = distance;
                min_a = a;
                min_b = b;
            }
        }
    }
    a = min_a;
    b = min_b;
}

void ha::ha_merge_nodes (int a, int b, vector<TreeNode<const Point *> *> &tree_nodes) {
    
    /* there are that steps:
     * get center of new node
     * connect old centers to new node
     * delete old pointers from vector tree_node
     * add new node to that vector
     */
    
    int i_1, i_2;
    TreeNode<const Point *> *&first = tree_nodes[a];
    TreeNode<const Point *> *&second = tree_nodes[b];
    Point *new_center = ha_get_new_node_center (first, second);
    auto *new_node = new TreeNode<const Point *> (new_center);
    int min_i = min (a, b);
    int max_i = max (a, b);
    tree_nodes.erase (tree_nodes.begin () + max_i);
    tree_nodes.erase (tree_nodes.begin () + min_i);
    tree_nodes.push_back (new_node);
}

TreeNode<const Point *> *&ha::ha_get_node_by_coords (int a,
                                                                 vector<TreeNode<const Point *> *> &tree_node,
                                                                 int &i, double x, double y) {
    for (i = 0; i < tree_node.size (); ++i) {
        if (tree_node[i]->value ()->x () == x && tree_node[i]->value ()->y () == y) {
            return tree_node[i];
        }
    }
    // just to escape warning, it's impossible branch
    return tree_node[0];
}

Point *ha::ha_get_new_node_center (TreeNode<const Point *> *&first, TreeNode<const Point *> *&second) {
    double sum_x = 0;
    double sum_y = 0;
    int points = 0;
    ha_get_node_sum (first, sum_x, sum_y, points);
    ha_get_node_sum (second, sum_x, sum_y, points);
    return new Point (sum_x / points, sum_y / points, 0);
}

void ha::ha_get_node_sum (TreeNode<const Point *> *&node, double &sum_x, double &sum_y, int &points) {
    if (node->first_child () != nullptr) {
        ha_get_node_sum (node->first_child_, sum_x, sum_y, points);
    }
    if (node->brother () != nullptr) {
        ha_get_node_sum (node->brother_, sum_x, sum_y, points);
    }
    sum_x += node->value ()->x ();
    sum_y += node->value ()->y ();
    points++;
}

void ha::ha_fprintf (const vector<TreeNode<const Point *> *> &tree_nodes,
                                 int iteration,
                                 const Point &old_1,
                                 const Point &old_2,
                                 ofstream &tree) {
    if (tree_nodes.size () < 3) {
        double a;
    }
    ofstream out ("gnuplot/ha/ha" + to_string (iteration) + ".txt");
    for (auto tree_node : tree_nodes) {
        out << tree_node->value ()->x () << " " << tree_node->value ()->y () << " " << 0 << endl;
    }
    out << old_1.x () << " " << old_1.y () << " " << 1 << endl;
    out << old_2.x () << " " << old_2.y () << " " << 1 << endl;
    tree << old_1.x () << " " << old_1.y () << endl;
    tree << tree_nodes.back ()->value ()->x () << " " << tree_nodes.back ()->value ()->y () << endl << endl;
    tree << old_2.x () << " " << old_2.y () << endl;
    tree << tree_nodes.back ()->value ()->x () << " " << tree_nodes.back ()->value ()->y () << endl << endl;
}

void ha::ha_fprintf_tree (const TreeNode<const Point *> *tree_node, ofstream &out) {
    auto *pointer = tree_node->first_child ();
    while (pointer != nullptr) {
        out << tree_node->value ()->x () << " " << tree_node->value ()->y () << endl;
        out << pointer->value ()->x () << " " << pointer->value ()->y () << endl;
    }
    pointer = tree_node->first_child ();
    while (pointer != nullptr) {
        ha_fprintf_tree (pointer, out);
        pointer = pointer->brother ();
    }
}
