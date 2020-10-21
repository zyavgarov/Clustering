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
  
  explicit Cluster_Search (Field *field, double delta, int k = 0);
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
  Cluster_Search s_tree ();
 private:
  Field *field_;
  vector<vector<bool>> edges_; //if points with id i and j are connected edge_matrix[i][j] == true
  int add_closest_point (TreeNode<int> &node, vector<bool> &added);
  vector<int> create_histogram (TreeNode<int> root, int pieces);
  void get_closest (TreeNode<int> &current,
                    vector<bool> &added,
                    double &min_dist,
                    TreeNode<int> *&tree_node,
                    TreeNode<int> *&out_node);
  void get_tree_range (TreeNode<int> *&node, double &max_dist, double &min_dist);
  void picking_histogram (TreeNode<int> *node, double max_dist, double min_dist, vector<int> &histogram);
  static void put_value_to_histogram (vector<int> &histogram, double min_dist, double max_dist, double distance);
};

#endif //INTERFACE4__CLUSTER_SEARCH_H_
