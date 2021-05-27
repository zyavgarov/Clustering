#ifndef INTERFACE4__DELAUNAY_H_
#define INTERFACE4__DELAUNAY_H_
#include "../Field.h"

class delaunay {
  struct Edge {
    const Point *a, *b;
    
    Edge (const Point *a, const Point *b) : a (a), b (b) {
    };
  };
  
  int err_;
  static void delaunay_fprintf (vector<vector<bool>> &edge, int iteration);
 public:
  delaunay ();
  int err () const;
  void delaunay_base_run (vector<Edge> &baselines,
                          vector<vector<bool>> &edge,
                          vector<bool> &points_done,
                          int iteration);
  static double delaunay_angle_to_edge (Edge &edge, int num);
};

#endif //INTERFACE4__DELAUNAY_H_
