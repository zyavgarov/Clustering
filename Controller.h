#ifndef INTERFACE4__CONTROLLER_H_
#define INTERFACE4__CONTROLLER_H_
#include "Field.h"
#include <iostream>

class Controller {
 private:
  class Cluster {
   public:
   private:
    const vector<Point> point_;
    vector<vector<bool>> edges;
  };
  
  string log_file_name;
  ofstream logger;
  bool write_log;
 
 public:
  Field *field_;
  explicit Controller (bool write_log);
  ~Controller ();
  int get_help (int id, string &file_name);
  int generate_cloud (int id, double center_x, double center_y, double disp_x, double disp_y, int c_length);
  void log (const string &s);
  int fprintf (const string &file_name) const;
  void histogram (int pieces, vector<int> &x_distr, vector<int> &y_distr) const;
  vector<Cluster> dbscan (double d, int k);
  vector<vector<int>> make_distance_matrix ();
};

#endif //INTERFACE4__CONTROLLER_H_
