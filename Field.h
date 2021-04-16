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
  static int fprintf (ofstream &out) ;
  static int length () ;
  static Buffer buf;
  static const vector<vector<double>> &dist () ;
  static const vector<Cloud *> &cloud () ;
  static bool readonly () ;
  static void create_dist_matrix ();
  static const vector<Cluster_Search> &searches () ;
  void create_edges_matrix (double delta);
  int wave_clustering (int search_id);
  int db_clustering (int search_id, int k);
  int k_means (int clusters_number);
  vector<int> s_tree ();
  int k_means_cores (int clusters_number, int cores_number);
  int em (int clusters_number);
  int hieararchical_algorithm ();
  int forel ();
  int delaunay ();
  static int add (const vector<Point> &addition);
  void histogram (int pieces, vector<int> &x_distr, vector<int> &y_distr) const;
 private:
  Field &operator= (Field const &f);
  Field (Field const &f);
  static vector<Cloud *> cloud_;
  static vector<vector<double>> dist_; // matrix of distances
  static vector<Cluster_Search> searches_;
  static bool readonly_;
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
  void stree_fprintf_tree (const string &file_name, TreeNode<int> *root);
  void stree_fprintf_node (TreeNode<int> *node, ofstream *out);
  static void fprintf_incidence_graph (const Cluster_Search &search, int id);
  
  struct Edge {
    const Point *a, *b;
    
    Edge (const Point *a, const Point *b) : a (a), b (b) {
    };
  };
  
  static void delaunay_base_run (vector<Edge> &baselines,
                                 vector<vector<bool>> &edge,
                                 vector<bool> &points_done,
                                 int iteration);
  
  static double delaunay_angle_to_edge (Edge &edge, int num);
  static void delaunay_fprintf (vector<vector<bool>> &edge, int iteration);
};

#endif //INTERFACE4__FIELD_H_
