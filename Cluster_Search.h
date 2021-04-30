#ifndef INTERFACE4__CLUSTER_SEARCH_H_
#define INTERFACE4__CLUSTER_SEARCH_H_
#include "Cloud.h"
#include "TreeNode.h"

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
  TreeNode<const Point *> *tree_root_;
 private:
  Field *field_;
  vector<vector<bool>> edges_;
};

#endif //INTERFACE4__CLUSTER_SEARCH_H_
