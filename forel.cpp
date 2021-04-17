#include "forel.h"

void forel::forel_clustering () {
    // realises the forel algorithm
    vector<TreeNode<Point> *> center_nodes;
    double R = 0.03;
    for (int p = 0; p < Point::quantity (); ++p) {
        auto *temp = new TreeNode<Point> (Point (Point::get_by_id (p + 1)->x (), Point::get_by_id (p + 1)->y (), 0));
        center_nodes.push_back (temp);
    }
    int print_number = 0;
    while (center_nodes.size () > 1) {
        vector<bool> clustered (center_nodes.size (), false);
        vector<TreeNode<Point> *> new_center_nodes;
        for (int p = 0; p < center_nodes.size (); ++p) {
            // If the point is clustered we pass over it
            // if not - we make it a new center
            if (clustered[p]) {
                continue;
            }
            Point new_center (center_nodes[p]->value ().x (), center_nodes[p]->value ().y (), 0);
            vector<bool> in_circle (center_nodes.size (), false);
            // the vector shows if the point is close enough to center
            for (int i = 0; i < center_nodes.size (); ++i) {
                Point p_i (center_nodes[i]->value ().x (), center_nodes[i]->value ().y (), 0);
                double distance = Point::dist (&new_center, &p_i);
                if (distance < R) {
                    in_circle[i] = true;
                }
            }
            bool changed = true;
            while (changed) {
                changed = false;
                
                // recomputing center
                new_center = Point ();
                int node_size = 0;
                for (int i = 0; i < center_nodes.size (); ++i) {
                    if (in_circle[i] && !clustered[i]) {
                        new_center = new_center + center_nodes[i]->value ();
                        node_size++;
                    }
                }
                new_center = new_center / node_size;
                
                // updating circles
                for (int i = 0; i < center_nodes.size (); ++i) {
                    Point p_i (center_nodes[i]->value ().x (), center_nodes[i]->value ().y (), 0);
                    double distance = Point::dist (&new_center, &p_i);
                    if (distance < R) {
                        if (!in_circle[i]) {
                            changed = true;
                            in_circle[i] = true;
                        }
                    } else {
                        if (in_circle[i]) {
                            changed = true;
                            in_circle[i] = false;
                        }
                    }
                }
                frl_fprintf (print_number, center_nodes, new_center_nodes, clustered, in_circle, R, new_center);
                print_number++;
            }
            auto *node = new TreeNode<Point> (new_center);
            for (int i = 0; i < in_circle.size (); ++i) {
                if (in_circle[i] && !clustered[i]) {
                    node->add_child (center_nodes[i]); // the problem is here. I cant find the moment where i put brothers to the ring
                    clustered[i] = true;
                }
            }
            new_center_nodes.push_back (node);
        }
        center_nodes = new_center_nodes;
        R *= 2;
    }
}

void forel::frl_fprintf (int print_num,
                         vector<TreeNode<Point> *> center_nodes, vector<TreeNode<Point> *> new_centers_node,
                         vector<bool> clustered,
                         vector<bool> in_circle,
                         double R,
                         const Point &center) {
    ofstream out ("gnuplot/forel/frl" + to_string (print_num) + ".txt");
    ofstream circle ("gnuplot/forel/circle" + to_string (print_num) + ".txt");
    for (int i = 0; i < new_centers_node.size (); ++i) {
        circle << new_centers_node[i]->value ().x () << " " << new_centers_node[i]->value ().y () << " " << R << endl;
        auto *pointer = new_centers_node[i]->first_child ();
        while (pointer != nullptr) {
            out << pointer->value ().x () << " " << pointer->value ().y () << " " << i << endl;
            pointer = pointer->brother ();
        }
    }
    // current circle print
    for (int i = 0; i < in_circle.size (); ++i) {
        if (in_circle[i]) {
            out << center_nodes[i]->value ().x () << " " << center_nodes[i]->value ().y () << " -1" << endl;
        }
    }
    circle << center.x () << " " << center.y () << " " << R << endl;
    //printing unclustered points
    for (int i = 0; i < center_nodes.size (); ++i) {
        if (!clustered[i] && !in_circle[i]) {
            out << center_nodes[i]->value ().x () << " " << center_nodes[i]->value ().y () << " " << "-2" << endl;
        }
    }
}

int forel::err () const {
    return err_;
}

forel::forel () {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!Field::readonly ()) {
        err_ = -1;
        return;
    }
    auto *f = new Field;
    Field::searches_.emplace_back (f);
    forel_clustering ();
    err_ = 0;
}
