#ifndef INTERFACE4__CONTROLLER_H_
#define INTERFACE4__CONTROLLER_H_
#include "Field.h"
#include "Cluster_Search.h"
#include <iostream>

class Controller {
 private:
  string log_file_name;
  ofstream logger;
  bool write_log;
  void log (const string &s);
 public:
  Field *field_;
  vector<Cluster_Search> searches;
  explicit Controller (bool write_log = true);
  ~Controller ();
  int get_help (int id, string &file_name);
  int generate_cloud (int id, double center_x, double center_y, double disp_x, double disp_y, int c_length);
  int fprintf (const string &file_name) const;
  void histogram (int pieces, vector<int> &x_distr, vector<int> &y_distr) const;
  Cluster_Search dbscan (double d, int k) const;
  int buffer_add_cloud(int id) const;
  int buffer_unload() const;
  int matrix() const;
  Cluster_Search wave (double d);
};

#endif //INTERFACE4__CONTROLLER_H_
