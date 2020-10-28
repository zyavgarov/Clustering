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
  int wave_clustering ();
  int db_clustering (int k);
  vector<int> s_tree ();
 private:
  Field &operator= (Field const &f);
  Field (Field const &f);
  vector<Cloud *> cloud_;
  vector<vector<double>> dist_; // matrix of distances
  vector<Cluster_Search> searches_;
  bool readonly_{};
  int add_closest_point (TreeNode<int> &node, vector<bool> &added);
  void get_closest (TreeNode<int> &current,
                    vector<bool> &added,
                    double &min_dist,
                    TreeNode<int> *&tree_node,
                    TreeNode<int> *&out_node);
  vector<int> create_histogram (TreeNode<int> &root, int pieces);
  void picking_histogram (TreeNode<int> *node, double min_dist, double max_dist, vector<int> &histogram);
  void get_tree_range (TreeNode<int> *&node, double &min_dist, double &max_dist);
  void put_value_to_histogram (vector<int> &histogram, double min_dist, double max_dist, double distance);
  void fprintf_tree (const string& file_name, TreeNode<int> *root);
  void fprintf_node (TreeNode<int> *node, ofstream *out);
  static void fprintf_incidence_graph (const Cluster_Search &search, int id);
};

#endif //INTERFACE4__FIELD_H_
