#ifndef INTERFACE4__FIELD_H_
#define INTERFACE4__FIELD_H_
#include "Buffer.h"
#include "Cluster_Search.h"
#include "TreeNode.h"

class Field {
 public:
  friend class Buffer;
  
  Field ();
  ~Field ();
  int add (Cloud *addition);
  int fprintf (ofstream &out) const;
  int length () const;
  Buffer buf;
  const vector<vector<double>> &dist () const;
  const vector<Cloud *> &cloud ();
  bool readonly () const;
  void create_dist_matrix ();
  const vector<Cluster_Search> &searches () const;
  void create_edges_matrix (double delta);
  int wave_clustering (int search_id);
  int db_clustering (int search_id, int k);
  int k_means (int clusters_number);
  vector<int> s_tree ();
  int k_means_cores (int clusters_number, int cores_number);
 private:
  Field &operator= (Field const &f);
  Field (Field const &f);
  vector<Cloud *> cloud_;
  vector<vector<double>> dist_; // matrix of distances
  vector<Cluster_Search> searches_;
  bool readonly_{};
  int stree_add_closest_point (TreeNode<int> &node, vector<bool> &added);
  void stree_get_closest (TreeNode<int> &current,
                          vector<bool> &added,
                          double &min_dist,
                          TreeNode<int> *&tree_node,
                          TreeNode<int> *&out_node);
  vector<int> stree_create_histogram (TreeNode<int> &root, int pieces);
  void stree_picking_histogram (TreeNode<int> *node, double min_dist, double max_dist, vector<int> &histogram);
  void stree_get_tree_range (TreeNode<int> *&node, double &min_dist, double &max_dist);
  void stree_put_value_to_histogram (vector<int> &histogram, double min_dist, double max_dist, double distance);
  void stree_fprintf_tree (const string& file_name, TreeNode<int> *root);
  void stree_fprintf_node (TreeNode<int> *node, ofstream *out);
  static void fprintf_incidence_graph (const Cluster_Search &search, int id);
};

#endif //INTERFACE4__FIELD_H_
