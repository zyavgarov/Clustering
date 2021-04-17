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
  static int add (Cloud *addition);
  static int fprintf (ofstream &out);
  static int length ();
  static Buffer buf;
  static const vector<vector<double>> &dist ();
  static const vector<Cloud *> &cloud ();
  static bool readonly ();
  static void create_dist_matrix ();
  static const vector<Cluster_Search> &searches ();
  void create_edges_matrix (double delta);
  static int add (const vector<Point> &addition);
  static vector<Cluster_Search> searches_;
 private:
  Field &operator= (Field const &f);
  Field (Field const &f);
  static vector<Cloud *> cloud_;
  static vector<vector<double>> dist_; // matrix of distances
  static bool readonly_;
  static void fprintf_incidence_graph (const Cluster_Search &search, int id);
  
};

#endif //INTERFACE4__FIELD_H_
