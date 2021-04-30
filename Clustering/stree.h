#ifndef INTERFACE4__STREE_H_
#define INTERFACE4__STREE_H_
#include "../Field.h"

class stree {
  vector<int> tree_;
 public:
  vector<int> tree ();
  void stree_fprintf_node (TreeNode<int> *node, ofstream *out);
  static void stree_put_value_to_histogram (vector<int> &histogram, double min_dist, double max_dist, double distance);
  void stree_picking_histogram (TreeNode<int> *node, double min_dist, double max_dist, vector<int> &histogram);
  void stree_get_tree_range (TreeNode<int> *&node, double &min_dist, double &max_dist);
  vector<int> stree_create_histogram (TreeNode<int> &root, int pieces);
  void stree_get_closest (TreeNode<int> &current,
                          vector<bool> &added,
                          double &min_dist,
                          TreeNode<int> *&tree_node,
                          TreeNode<int> *&out_node);
  int stree_add_closest_point (TreeNode<int> &node, vector<bool> &added);
  void stree_fprintf_tree (const string &file_name, TreeNode<int> *root);
  stree();
};

#endif //INTERFACE4__STREE_H_
