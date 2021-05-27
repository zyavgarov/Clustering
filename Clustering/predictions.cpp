#include "predictions.h"
Point predictions::new_point;
int predictions::err_;

double predictions::f (const Point &p) {
    return cos (p.x ()) * sin (p.y ());
}

double predictions::predict (double x, double y) {
    // the idea:
    // I take the square and increase its size until there will be 20 points inside
    if (!Field::readonly ()) {
        err_ = -1;
        return 0;
    }
    int length = Point::quantity ();
    double ball_size = BALL_START_SIZE;
    vector<bool> appropriate_points (Point::quantity (), true);
    do {
        length = Point::quantity ();
        appropriate_points = vector<bool> (Point::quantity (), true);
        for (int i = 0; i < Point::quantity (); ++i) {
            if (appropriate_points[i] && (abs (Point::get_by_id (i + 1)->x () - x) > ball_size
                || abs (Point::get_by_id (i + 1)->y () - y) > ball_size)) {
                appropriate_points[i] = false;
                length--;
            }
        }
        ball_size *= BALL_GROWTH_COEFFICIENT;
        if (ball_size > 0.5) {
            err_ = -3;
            return 0;
        };
    } while (length < 20);
    // we have found close points and can run delaunay triangulation on them
    vector<int> points;
    for (int i = 0; i < appropriate_points.size (); ++i) {
        if (appropriate_points[i]) {
            points.push_back (i);
        }
    }
    new_point = Point (x, y, 0);
    points.push_back (-1);
    vector<vector<bool>> edges = p_delaunay (points);
    // the idea now is to find out how exactly edges works
    // and using this knowledge find the neighbours of our point.
    // This immediately leads us to the understanding of prediction
    
    // edges connects points by their numbers
    
    vector<int> neighbours; // contains numbers of points which are connected to new point
    for (int i = 1; i < edges.size (); ++i) {
        if (edges[0][i]) {
            neighbours.push_back (i);
        }
    }
    vector<double> distances; // distance from new point to its i-th neighbour
    distances.reserve (neighbours.size ());
    for (int neighbour : neighbours) {
        distances.push_back (Point::dist (get_point_by_id (0), get_point_by_id (neighbour + 1)));
    }
    double result = 0;
    for (int i = 0; i < neighbours.size (); ++i) {
        result = result + f (*get_point_by_id (neighbours[i] + 1)) * distances[i];
    }
    double sum_distances = 0;
    for (double distance : distances) {
        sum_distances += distance;
    }
    return result / sum_distances;
}

vector<vector<bool>> predictions::p_delaunay (const vector<int> &points) {
// the worst algorithm can be done. it's here only because I have a deadline

// Sorting all points by x coordinate
    vector<int> points_sorted /*= points*/;
    points_sorted.reserve (points.size ());
    for (int i = 0; i < points.size (); ++i) {
        points_sorted.push_back (i);
    }
    for (int i = 0; i < points_sorted.size (); ++i) {
        for (int j = 0; j < points_sorted.size (); ++j) {
            if (get_point_by_id (points_sorted[j] + 1)->x () > get_point_by_id (points_sorted[i] + 1)->x ()) {
                int temp = points_sorted[i];
                points_sorted[i] = points_sorted[j];
                points_sorted[j] = temp;
            }
        }
    }
    map<int, int> get_sorted_by_id;
    for (int i = 0; i < points_sorted.size (); ++i) {
        get_sorted_by_id[points[points_sorted[i]]] = i;
    }
    vector<vector<bool>> edge (points.size (), vector<bool> (points.size (), false));
    // creating the first edge
    // getting the first edge is to find the line
    // which satisfies condition: all the field points are on one side
    // we already have sorted points, so it's enough to check the most left and find pair for it
    // it means, we search the most vertical vector here
    int min_i = 1;
    double min_angle = 90;
    for (int i = 1; i < points_sorted.size (); ++i) {
        // it's good idea to draw that points to understand what does temp here
        auto *temp = new Point (get_point_by_id (points[points_sorted[0]] + 1)->x (),
                                get_point_by_id (points[points_sorted[i]] + 1)->y (),
                                0);
        Edge temp_edge (temp, get_point_by_id (points[points_sorted[i]] + 1));
        double curr_angle = abs (p_delaunay_angle_to_edge (temp_edge, points[points_sorted[0]]));
        if (min_angle > curr_angle) {
            min_angle = curr_angle;
            min_i = i;
        }
        delete temp;
    }
    vector<Edge> baselines;
    if (get_point_by_id (points[points_sorted[0]] + 1)->y () >
        get_point_by_id (points[points_sorted[min_i]] + 1)->y ()) {
        baselines.emplace_back (get_point_by_id (points[points_sorted[min_i]] + 1),
                                get_point_by_id (points[points_sorted[0]] + 1));
    } else {
        baselines.emplace_back (get_point_by_id (points[points_sorted[0]] + 1),
                                get_point_by_id (points[points_sorted[min_i]] + 1));
    }
    edge[points_sorted[0]][points_sorted[min_i]] = edge[points_sorted[min_i]][points_sorted[0]] = true;
    vector<bool> points_done (points.size (), false);
    int iteration = 0;
    p_delaunay_base_run (baselines, edge, points_done, iteration, get_sorted_by_id);
    return edge;
}

void predictions::p_delaunay_base_run (vector<Edge> &baselines,
                                       vector<vector<bool>> &edge,
                                       vector<bool> &points_done,
                                       int iteration,
                                       map<int, int> &get_sorted_by_id) {
    // The function has no mechanics for cases when there is no satisfying points
    
    int max_i;
    double max_angle;
    bool started = false;
    max_angle = 0;
    for (int i = 0; i < points_done.size (); ++i) {
        //checking if the point is on the right side from edge
        const Point *m_point = get_point_by_id (i + 1);
        Point vec_1 (baselines[0].b->x () - m_point->x (), baselines[0].b->y () - m_point->y (), 0);
        Point vec_2 (baselines[0].a->x () - m_point->x (), baselines[0].a->y () - m_point->y (), 0);
        if (!((vec_1.x () == 0 && vec_1.y () == 0) || (vec_2.x () == 0 && vec_2.y () == 0))) {
            if (vec_1.x () * vec_2.y () > vec_2.x () * vec_1.y ()) {
                double curr_angle = p_delaunay_angle_to_edge (baselines[0], i);
                if (max_angle < curr_angle) {
                    started = true;
                    max_angle = curr_angle;
                    max_i = i;
                }
            }
        }
    }
    if (started) {
        if (!edge[get_sorted_by_id[baselines[0].a->id () - 1]][max_i]) {
            edge[get_sorted_by_id[baselines[0].a->id () - 1]][max_i] = edge[max_i][get_sorted_by_id[
                baselines[0].a->id () - 1]] = true;
            baselines.emplace_back (baselines[0].a, get_point_by_id (max_i + 1));
        }
        if (!edge[get_sorted_by_id[baselines[0].b->id () - 1]][max_i]) {
            edge[get_sorted_by_id[baselines[0].b->id () - 1]][max_i] = edge[max_i][get_sorted_by_id[
                baselines[0].b->id () - 1]] = true;
            baselines.emplace_back (get_point_by_id (max_i + 1), baselines[0].b);
        }
    }
    baselines.erase (baselines.cbegin ());
    iteration++;
    if (!baselines.empty ()) {
        p_delaunay_base_run (baselines, edge, points_done, iteration, get_sorted_by_id);
    }
}

double predictions::p_delaunay_angle_to_edge (Edge &edge, int num) {
    const Point *m_point = get_point_by_id (num + 1);
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

const Point *predictions::get_point_by_id (int id) {
    if (id == 0) {
        return &new_point;
    } else {
        return Point::get_by_id (id);
    }
}

int predictions::err () {
    return err_;
}
