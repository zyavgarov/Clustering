#ifndef INTERFACE4__CLUSTER_SEARCH_H_
#define INTERFACE4__CLUSTER_SEARCH_H_
#include "Cloud.h"
class Field;
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
  double delta;// max distance between points considered to have an edge
  int k;// 0 if not dbscan; in dbscan - density of cluster
  vector<Cluster> clusters;
  void add (const Cluster &addition);
  void create_edges_matrix ();
  vector<int> db_sorting () const;
  void db_clustering (const vector<int> &state);
  void wave_clustering ();
 private:
  Field *field_;
  vector<vector<bool>> edges_; //if points with id i and j are connected edge_matrix[i][j] == true
};

#endif //INTERFACE4__CLUSTER_SEARCH_H_
