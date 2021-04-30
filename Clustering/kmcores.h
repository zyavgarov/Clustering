#ifndef INTERFACE4__KMCORES_H_
#define INTERFACE4__KMCORES_H_
#include "../Field.h"
class kmcores {
  int err_;
  int clusters_number;
  int cores_number;
 public:
  kmcores (int clusters_number, int cores_number);
  int err ();
  void k_means_cores ();
  static void kmeans_core_fprintf (const vector<int> &nearest_cluster, const vector<vector<Point>> &cores, int iteration);
};

#endif //INTERFACE4__KMCORES_H_
