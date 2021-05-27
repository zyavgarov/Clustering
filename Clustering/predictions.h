#ifndef SERVER_CLUSTERING_PREDICTIONS_H_
#define SERVER_CLUSTERING_PREDICTIONS_H_
#include "../Field.h"
#include <map>
#define BALL_START_SIZE 0.00001
#define BALL_GROWTH_COEFFICIENT 1.1

class predictions {
 public:
  static double predict (double x, double y);
  static int err ();
 private:
  struct Edge {
    const Point *a, *b;
    
    Edge (const Point *a, const Point *b) : a (a), b (b) {
    };
  };
  
  static Point new_point;
  static void p_delaunay_base_run (vector<Edge> &baselines,
                                   vector<vector<bool>> &edge,
                                   vector<bool> &points_done,
                                   int iteration,
                                   map<int, int> &get_sorted_by_id);
  static double f (const Point &p);
  static double p_delaunay_angle_to_edge (Edge &edge, int num);
  static vector<vector<bool>> p_delaunay (const vector<int> &points);
  static const Point *get_point_by_id (int id);
  static int err_;
};

#endif //SERVER_CLUSTERING_PREDICTIONS_H_
