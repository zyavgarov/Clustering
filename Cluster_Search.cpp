#include "Cluster_Search.h"

Cluster_Search::Cluster::Cluster (vector<Point> vec) : points (vec) {
}

Cluster_Search::Cluster::Cluster (const vector<int> &vec) {
    vector<Point> a;
    a.reserve (vec.size ());
    for (int i : vec) {
        a.push_back (Point::get_by_id (i));
    }
}

const vector<vector<bool>> &Cluster_Search::edges () const {
    return edges_;
}

const vector<vector<double>> &Cluster_Search::dist () {
    return dist_;
}

void Cluster_Search::add (const Cluster_Search::Cluster &addition) {
    clusters.push_back (addition);
}

void Cluster_Search::create_dist_matrix () {
    if (dist_.empty ()) {
        vector<vector<double>> dist_M (Point::quantity);
        // making matrix of distances
        for (int i = 0; i < Point::quantity; ++i) {
            dist_M[i] = vector<double> (Point::quantity);
            for (int j = 0; j < Point::quantity; ++j) {
                dist_M[i].push_back (-1);
            }
        }
        for (int i = 0; i < Point::quantity; ++i) {
            for (int j = i + 1; j < Point::quantity; ++j) {
                dist_M[j][i] = dist_M[i][j] = Point::dist (Point::get_by_id (i), Point::get_by_id (j));
            }
        }
        dist_ = dist_M;
    }
}

Cluster_Search::Cluster_Search (double delta, int k) : delta (delta), k (k) {
    create_dist_matrix ();
    create_edges_matrix ();
}

void Cluster_Search::create_edges_matrix () {
    // matrix of incidences
    if (edges_.empty ()) {
        vector<vector<bool>> incidence (Point::quantity, vector<bool> (Point::quantity, false));
        for (int i = 0; i < Point::quantity; ++i) {
            for (int j = i + 1; j < Point::quantity; ++j) {
                incidence[j][i] = incidence[i][j] = (dist ()[i][j] < delta);
            }
        }
        edges_ = incidence;
    }
}
