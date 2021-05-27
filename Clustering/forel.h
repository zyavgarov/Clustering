#ifndef INTERFACE4__FOREL_H_
#define INTERFACE4__FOREL_H_
#include "../Field.h"

class forel {
  int err_;
 public:
  forel ();
  int err () const;
  static void forel_clustering ();
  static void frl_fprintf (int print_num,
                           vector<TreeNode<Point> *> center_nodes,
                           vector<TreeNode<Point> *> new_centers_node,
                           vector<bool> clustered,
                           vector<bool> in_circle,
                           double R,
                           const Point &center);
};

#endif //INTERFACE4__FOREL_H_
