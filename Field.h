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
  int k_means_cores (int clusters_number, int cores_number);
  int hieararchical_algorithm ();
  int forel ();
  int delaunay ();
  static int add (const vector<Point> &addition);
  void histogram (int pieces, vector<int> &x_distr, vector<int> &y_distr) const;
  static vector<Cluster_Search> searches_;
 private:
  Field &operator= (Field const &f);
  Field (Field const &f);
  static vector<Cloud *> cloud_;
  static vector<vector<double>> dist_; // matrix of distances
  static bool readonly_;
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
