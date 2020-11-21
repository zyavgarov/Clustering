#ifndef INTERFACE4__CLUSTER_SEARCH_H_
#define INTERFACE4__CLUSTER_SEARCH_H_
#include "Cloud.h"

class Field;

class Cluster_Search {
 public:
  class Cluster {
   public:
    explicit Cluster (vector<const Point *> vec);
    explicit Cluster (const vector<int> &vec);
    vector<const Point *> points; //It's never read, but really works to save the points
  };
  
  explicit Cluster_Search (Field *field, double delta = 0, int k = 0);
  Cluster_Search (Cluster_Search const &cs);
  Cluster_Search &operator= (Cluster_Search const &cs);
  const vector<vector<bool>> &edges () const;
  double delta; // max distance between points considered to have an edge
  int k; // 0 if not dbscan; in dbscan - density of cluster
  vector<Cluster> clusters;
  void add (const Cluster &addition);
  void create_edges_matrix ();
  vector<int> db_sorting (int density);
  void db_clustering (const vector<int> &state);
  void wave_clustering ();
  Cluster_Search wave ();
  Cluster_Search dbscan (int density);
  Cluster_Search k_means (int clusters_number);
  Cluster_Search em (int clusters_number);
  Cluster_Search k_means_cores (int clusters_number, int cores_number);
 private:
  Field *field_;
  vector<vector<bool>> edges_;
  static void dbscan_fprintf_type (const vector<int> &state);
  void kmeans_fprintf (vector<int> &nearest_cluster, vector<Point> &cores, int iteration);
  static void kmeans_core_fprintf (const vector<int> &nearest_cluster, const vector<vector<Point>> &cores, int iteration);
};

#endif //INTERFACE4__CLUSTER_SEARCH_H_
