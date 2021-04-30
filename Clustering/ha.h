#ifndef INTERFACE4__HA_H_
#define INTERFACE4__HA_H_
#include "../Field.h"
class ha {
  int err_;
 public:
  int err ();
  ha();
  void hierarchical_algorithm ();
  void ha_get_closest_nodes (int &a, int &b, const vector<TreeNode<const Point *> *> &tree_node);
  void ha_merge_nodes (int a, int b, vector<TreeNode<const Point *> *> &tree_nodes);
  TreeNode<const Point *> *&ha_get_node_by_coords (int a,
                                                   vector<TreeNode<const Point *> *> &tree_node,
                                                   int &i,
                                                   double x,
                                                   double y);
  Point *ha_get_new_node_center (TreeNode<const Point *> *&first, TreeNode<const Point *> *&second);
  void ha_get_node_sum (TreeNode<const Point *> *&node, double &sum_x, double &sum_y, int &points);
  void ha_fprintf (const vector<TreeNode<const Point *> *> &tree_nodes,
                   int iteration,
                   const Point &old_1,
                   const Point &old_2,
                   ofstream &tree);
  void ha_fprintf_tree (const TreeNode<const Point *> *tree_node, ofstream &out);
};

#endif //INTERFACE4__HA_H_
