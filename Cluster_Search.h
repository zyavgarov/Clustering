#ifndef INTERFACE4__CLUSTER_SEARCH_H_
#define INTERFACE4__CLUSTER_SEARCH_H_
#include "Field.h"

class Cluster_Search {
 public: // initiate the search
  class Cluster {
   public:
    explicit Cluster (vector<Point> vec);
    explicit Cluster(const vector<int>& vec);
    vector<Point> points;
  };
  
  explicit Cluster_Search (double delta, int k = 0);
  Cluster_Search () = default;
  const vector<vector<bool>> &edges () const;
  static const vector<vector<double>> &dist ();
  double delta;// max distance between points considered to have an edge
  int k;// 0 if not dbscan; in dbscan - density of cluster
  vector<Cluster> clusters;
  void add (const Cluster &addition);
  static void create_dist_matrix ();
  void create_edges_matrix ();
 private:
  vector<vector<bool>> edges_; //if points with id i and j are connected edge_matrix[i][j] == true
  static vector<vector<double>> dist_; // matrix of distances
};

#endif //INTERFACE4__CLUSTER_SEARCH_H_
