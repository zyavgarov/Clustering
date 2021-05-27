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

Cluster_Search::Cluster_Search (double delta, int k) : delta (delta), k (k) {
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
                incidence[j][i] = incidence[i][j] = (Field::dist ()[i][j] < delta);
            }
        }
        edges_ = incidence;
    }
}

Cluster_Search::Cluster_Search (const Cluster_Search &cs)
    : delta (cs.delta), k (cs.k), clusters (cs.clusters), edges_ (cs.edges_) {
}

Cluster_Search &Cluster_Search::operator= (const Cluster_Search &cs) {
    if (this != &cs) {
        delta = cs.delta;
        k = cs.k;
        clusters = cs.clusters;
        edges_ = cs.edges_;
    }
    return *this;
}

Cluster_Search::Cluster_Search (vector<vector<bool>> edges) {
    edges_ = edges;
}

Cluster_Search::Cluster_Search (const vector<vector<int>> &points_by_clusters) {
    for (auto &cluster : points_by_clusters) {
        clusters.emplace_back (cluster);
    }
}

Cluster_Search::Cluster_Search (TreeNode<const Point *> *tree_nodes) {
    tree_root_ = tree_nodes;
}
