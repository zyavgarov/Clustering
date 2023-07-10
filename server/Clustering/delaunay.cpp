// Created by Zyavgarov Rustam

#include "delaunay.h"

delaunay::delaunay () {
    // the worst algorithm can be done. it's here only because I have a deadline
    
    /* Errors
     * -1 field is not in readonly mode
     * -2 not enough points
     */
    
    if (!Field::readonly ()) {
        err_ = -1;
        return;
    }
    if (Point::quantity () < 3) {
        err_ = -2;
        return;
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
    Field::new_search (edge);
    err_ = 0;
}

void delaunay::delaunay_base_run (vector<Edge> &baselines,
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

double delaunay::delaunay_angle_to_edge (Edge &edge, int num) {
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

void delaunay::delaunay_fprintf (vector<vector<bool>> &edge, int iteration) {
    // prints algorithm's state
    ofstream edges ("gnuplot/delaunay/dl" + to_string (iteration) + ".txt");
    ofstream final_edges ("gnuplot/delaunay/dl_fin.txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        for (int j = i + 1; j < Point::quantity (); ++j) {
            if (edge[i][j]) {
                edges << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
                final_edges << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
                edges << Point::get_by_id (j + 1)->x () << " " << Point::get_by_id (j + 1)->y () << endl << endl;
                final_edges << Point::get_by_id (j + 1)->x () << " " << Point::get_by_id (j + 1)->y () << endl << endl;
            }
        }
    }
    ofstream points ("gnuplot/delaunay/pts" + to_string (iteration) + ".txt");
    ofstream final_points ("gnuplot/delaunay/pt_fin.txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        points << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
        final_points << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
    }
}

int delaunay::err () const {
    return err_;
}
