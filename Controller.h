#ifndef INTERFACE4__CONTROLLER_H_
#define INTERFACE4__CONTROLLER_H_
#include "Field.h"
#include "Cluster_Search.h"
#include "Clustering/dbscan.h"
#include "Clustering/wave.h"
#include "Clustering/stree.h"
#include "Clustering/kmeans.h"
#include "Clustering/kmcores.h"
#include "Clustering/em.h"
#include "Clustering/ha.h"
#include "Clustering/forel.h"
#include "Clustering/delaunay.h"
#include "Clustering/histogram.h"
#include <iostream>

class Controller {
 private:
  string log_file_name;
  ofstream logger;
  bool write_log;
  void log (const string &s);
 public:
  Field *field_;
  explicit Controller (bool write_log = true);
  ~Controller ();
  int get_help (int id, string &file_name);
  int generate_cloud (int id, double center_x, double center_y, double disp_x, double disp_y, int c_length);
  int fprintf (const string &file_name) ;
  void histogram (int pieces, vector<int> &x_distr, vector<int> &y_distr) const;
  int buffer_add_cloud (int id) const;
  int buffer_unload () const;
  int buffer_shift (double x, double y) const;
  int buffer_zoom (double k) const;
  int buffer_mirror () const;
  int buffer_rotate (double angle) const;
  int matrix () const;
  int incidence_matrix (double delta);
  int wave (int search_id);
  int dbscan (int search_id, int k);
  int k_means (int clusters_number);
  int em (int clusters_number);
  const vector<Cluster_Search> &info_cluster_search () const;
  static vector<int> s_tree () ;
  int k_means_cores (int clusters_number, int cores_number);
  int hierarchical_algorithm ();
  int forel ();
  int delaunay ();
  int buffer_erase () const;
};

#endif //INTERFACE4__CONTROLLER_H_
