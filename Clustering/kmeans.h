#ifndef INTERFACE4__KMEANS_H_
#define INTERFACE4__KMEANS_H_
#include "../Field.h"

class kmeans {
  int clusters_number;
  int err_;
  static void kmeans_fprintf (vector<int> &nearest_cluster, vector<Point> &cores, int iteration);
 public:
  explicit kmeans (int clusters_number);
  int err () const;
  void k_means ();
};

#endif //INTERFACE4__KMEANS_H_
