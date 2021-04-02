#include "Field.h"
#include <iostream>

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

Field::~Field () {
    for (auto &i : cloud_) {
        delete i;
    }
};

int Field::add (Cloud *addition) {
    // The function adds cloud to the field
    // returns -1 if readonly
    if (readonly ()) {
        return -1;
    }
    cloud_.push_back (addition);
    return 0;
}

int Field::add (const vector<Point> &addition) {
    // The function adds vector of points to one cloud to the field together
    // returns -1 if readonly
    if (readonly ()) {
        return -1;
    }
    auto *new_cloud = new Cloud (addition);
    cloud_.push_back (new_cloud);
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
    // creating incidence matrix, saving incidence graph to file
    searches_.emplace_back (this, delta);
    fprintf_incidence_graph (searches ().back (), searches ().size () - 1);
}

int Field::wave_clustering (int search_id) {
    // if not readonly state returns -1
    // if readonly - clustering
    /* Errors
     * -1 field is not in readonly mode
     * -3 There is no such Cluster_Search
     */
    if (!readonly ()) {
        return -1;
    }
    if (search_id >= searches ().size ()) {
        return -3;
    }
    searches_[search_id].wave ();
    return 0;
}

int Field::db_clustering (int search_id, int k) {
    /* Errors
     * -1 field is not in readonly mode
     * -3 There is no such Cluster_Search
     */
    if (!readonly ()) {
        return -1;
    }
    if (search_id >= searches ().size ()) {
        return -3;
    }
    searches_[search_id].dbscan (k);
    return 0;
}

vector<int> Field::s_tree () {
    //searching minimum in dist matrix
    int min_i = 0;
    int min_j = 0;
    double min_dist_all = dist ()[0][1];
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
        closest = stree_add_closest_point (root, added);
    } while (closest >= 0);
    stree_fprintf_tree ("gnuplot/stree/tree.txt", &root);
    vector<int> histogram = stree_create_histogram (root, 30);
    return histogram;
}

int Field::stree_add_closest_point (TreeNode<int> &node, vector<bool> &added) {
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

void Field::stree_get_closest (TreeNode<int> &current,
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
        double a = dist ()[current.value ()][i];
        bool b = (min_dist == -1) || (a < min_dist);
        if (b && !added[i] && (i != current.value ())) {
            min_dist = dist ()[current.value ()][i];
            tree_node = &current;
            delete out_node;
            out_node = new TreeNode<int> (i);
        }
    }
}

vector<int> Field::stree_create_histogram (TreeNode<int> &root, int pieces) {
    // creates a histogram of distances in minimal spanning tree made previously
    // root is a root of tree. pieces is a pieces of histogram
    
    // searching the max and min distance in tree
    double max_dist = dist ()[root.value ()][root.first_child ()->value ()];
    double min_dist = max_dist;
    TreeNode<int> *root_pointer = &root;
    stree_get_tree_range (root_pointer, min_dist, max_dist);
    
    // picking up the distances in tree and filling the vector of histogram
    vector<int> histogram (pieces, 0);
    stree_picking_histogram (&root, min_dist, max_dist, histogram);
    return histogram;
}

void Field::stree_get_tree_range (TreeNode<int> *&node, double &min_dist, double &max_dist) {
    if (node->first_child () != nullptr) {
        double dist_to_child = dist ()[node->value ()][node->first_child ()->value ()];
        if (dist_to_child > max_dist) {
            max_dist = dist_to_child;
        } else if (dist_to_child < min_dist) {
            min_dist = dist_to_child;
        }
        stree_get_tree_range (node->first_child_, min_dist, max_dist);
    }
    if (node->brother () != nullptr) {
        double dist_to_brother = dist ()[node->value ()][node->brother ()->value ()];
        if (dist_to_brother > max_dist) {
            max_dist = dist_to_brother;
        } else if (dist_to_brother < min_dist) {
            min_dist = dist_to_brother;
        }
        stree_get_tree_range (node->brother_, min_dist, max_dist);
    }
}

void Field::stree_picking_histogram (TreeNode<int> *node, double min_dist, double max_dist, vector<int> &histogram) {
    if (node->brother () != nullptr) {
        stree_picking_histogram (node->brother_, min_dist, max_dist, histogram);
        stree_put_value_to_histogram (histogram,
                                      min_dist,
                                      max_dist,
                                      dist ()[node->value ()][node->brother ()->value ()]);
    }
    if (node->first_child () != nullptr) {
        stree_picking_histogram (node->first_child_, min_dist, max_dist, histogram);
        stree_put_value_to_histogram (histogram,
                                      min_dist,
                                      max_dist,
                                      dist ()[node->value ()][node->first_child ()->value ()]);
    }
    
}

void Field::stree_put_value_to_histogram (vector<int> &histogram,
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

void Field::stree_fprintf_tree (const string &file_name, TreeNode<int> *root) {
    ofstream out (file_name);
    stree_fprintf_node (root, &out);
}

void Field::stree_fprintf_node (TreeNode<int> *node, ofstream *out) {
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

void Field::fprintf_incidence_graph (const Cluster_Search &search, int id) {
    ofstream out ("gnuplot/incidence/incidence" + to_string (id) + ".txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        for (int j = i + 1; j < Point::quantity (); ++j) {
            if (search.edges ()[i][j]) {
                out << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
                out << Point::get_by_id (j + 1)->x () << " " << Point::get_by_id (j + 1)->y () << endl;
                out << endl;
            }
        }
    }
}

int Field::k_means (int clusters_number) {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!readonly ()) {
        return -1;
    }
    searches_.emplace_back (this);
    searches_.back ().k_means (clusters_number);
    return 0;
}

int Field::k_means_cores (int clusters_number, int cores_number) {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!readonly ()) {
        return -1;
    }
    searches_.emplace_back (this);
    searches_.back ().k_means_cores (clusters_number, cores_number);
    return 0;
}

int Field::em (int clusters_number) {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!readonly ()) {
        return -1;
    }
    searches_.emplace_back (this);
    searches_.back ().em (clusters_number);
    return 0;
}

int Field::hieararchical_algorithm () {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!readonly ()) {
        return -1;
    }
    searches_.emplace_back (this);
    searches_.back ().hierarchical_algorithm ();
    return 0;
}

int Field::forel () {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!readonly ()) {
        return -1;
    }
    searches_.emplace_back (this);
    searches_.back ().forel ();
    return 0;
}

int Field::delaunay () {
    // the worst algorithm can be done. it's here only because I have a deadline
    
    /* Errors
     * -1 field is not in readonly mode
     * -2 not enough points
     */
    
    if (!readonly ()) {
        return -1;
    }
    if (Point::quantity () < 3) {
        return -2;
    }
    // Sorting all points by x coordinate
    vector<int> points_sorted;
    points_sorted.reserve (points_sorted.size ());
    for (int i = 0; i < Point::quantity (); ++i) {
        points_sorted.emplace_back (i);
    }
    for (int i = 0; i < points_sorted.size (); ++i) {
        for (int j = 0; j < points_sorted.size (); ++j) {
            if (Point::get_by_id (points_sorted[j] + 1)->x () > Point::get_by_id (points_sorted[i] + 1)->x ()) {
                int temp = points_sorted[i];
                points_sorted[i] = points_sorted[j];
                points_sorted[j] = temp;
            }
        }
    }
    vector<vector<bool>> edge (Point::quantity (), vector<bool> (Point::quantity (), false));
    // creating the first edge
    // getting the first edge is to find the line
    // which satisfies condition: all the field points are on one side 
    // we already have sorted points, so it's enough to check the most left and find pair for it
    // it means, we search the most vertical vector here
    
    int min_i = 1;
    double min_angle = 90;
    for (int i = 1; i < points_sorted.size (); ++i) {
        // it's good idea to draw that points to understand what does temp here
        auto *temp = new Point (Point::get_by_id (points_sorted[0] + 1)->x (),
                                Point::get_by_id (points_sorted[i] + 1)->y (),
                                0);
        Edge temp_edge (temp, Point::get_by_id (points_sorted[i] + 1));
        double curr_angle = abs (delaunay_angle_to_edge (temp_edge, points_sorted[0]));
        if (min_angle > curr_angle) {
            min_angle = curr_angle;
            min_i = i;
        }
        delete temp;
    }
    
    vector<Edge> baselines;
    if (Point::get_by_id (points_sorted[0] + 1)->y () > Point::get_by_id (points_sorted[min_i] + 1)->y ()) {
        baselines.emplace_back (Point::get_by_id (points_sorted[min_i] + 1), Point::get_by_id (points_sorted[0] + 1));
    } else {
        baselines.emplace_back (Point::get_by_id (points_sorted[0] + 1), Point::get_by_id (points_sorted[min_i] + 1));
    }
    edge[points_sorted[0]][points_sorted[min_i]] = edge[points_sorted[min_i]][points_sorted[0]] = true;
    vector<bool> points_done (Point::quantity (), false);
    int iteration = 0;
    delaunay_base_run (baselines, edge, points_done, iteration);
    return 0;
}

void Field::delaunay_base_run (vector<Edge> &baselines,
                               vector<vector<bool>> &edge,
                               vector<bool> &points_done,
                               int iteration) {
    // The function has no mechanics for cases when there is no satisfying points
    
    int max_i;
    double max_angle;
    bool started = false;
    max_angle = 0;
    for (int i = 0; i < Point::quantity (); ++i) {
        //checking if the point is on the right side from edge
        const Point *m_point = Point::get_by_id (i + 1);
        Point vec_1 (baselines[0].b->x () - m_point->x (), baselines[0].b->y () - m_point->y (), 0);
        Point vec_2 (baselines[0].a->x () - m_point->x (), baselines[0].a->y () - m_point->y (), 0);
        if (!((vec_1.x () == 0 && vec_1.y () == 0) || (vec_2.x () == 0 && vec_2.y () == 0))) {
            if (vec_1.x () * vec_2.y () > vec_2.x () * vec_1.y ()) {
                double curr_angle = delaunay_angle_to_edge (baselines[0], i);
                if (max_angle < curr_angle) {
                    started = true;
                    max_angle = curr_angle;
                    max_i = i;
                }
            }
        }
    }
    if (started) {
        if (!edge[baselines[0].a->id () - 1][max_i]) {
            edge[baselines[0].a->id () - 1][max_i] = edge[max_i][baselines[0].a->id () - 1] = true;
            baselines.emplace_back (baselines[0].a, Point::get_by_id (max_i + 1));
        }
        if (!edge[baselines[0].b->id () - 1][max_i]) {
            edge[baselines[0].b->id () - 1][max_i] = edge[max_i][baselines[0].b->id () - 1] = true;
            baselines.emplace_back (Point::get_by_id (max_i + 1), baselines[0].b);
        }
        
    }
    baselines.erase (baselines.cbegin ());
    delaunay_fprintf (edge, iteration);
    iteration++;
    if (!baselines.empty ()) {
        delaunay_base_run (baselines, edge, points_done, iteration);
    }
}

double Field::delaunay_angle_to_edge (Edge &edge, int num) {
    const Point *m_point = Point::get_by_id (num + 1);
    Point vec_1 (edge.a->x () - m_point->x (), edge.a->y () - m_point->y (), 0);
    Point vec_2 (edge.b->x () - m_point->x (), edge.b->y () - m_point->y (), 0);
    double dist_a = Point::dist (m_point, edge.a);
    double dist_b = Point::dist (m_point, edge.b);
    double sc_product = vec_1.x () * vec_2.x () + vec_1.y () * vec_2.y ();
    double angle = 180 * acos (sc_product / dist_a / dist_b) / M_PI;
    if (isnan (angle)) {
        return 90;
    }
    return angle;
}

void Field::delaunay_fprintf (vector<vector<bool>> &edge, int iteration) {
    // prints algorithm's state
    ofstream edges ("gnuplot/delaunay/dl" + to_string (iteration) + ".txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        for (int j = i + 1; j < Point::quantity (); ++j) {
            if (edge[i][j]) {
                edges << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
                edges << Point::get_by_id (j + 1)->x () << " " << Point::get_by_id (j + 1)->y () << endl << endl;
            }
        }
    }
    ofstream points ("gnuplot/delaunay/pts" + to_string (iteration) + ".txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        points << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
    }
}
