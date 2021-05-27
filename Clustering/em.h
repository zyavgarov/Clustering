#ifndef INTERFACE4__EM_H_
#define INTERFACE4__EM_H_
#include "../Field.h"
#define EPS 0.01

class em {
  int err_;
  int clusters_number;
 public:
  explicit em (int clusters_number);
  int err () const;
  double N (const Point *a, vector<double> m, vector<double> Sgm);
  void em_fprintf (int iteration,
                   vector<vector<double>> sgm,
                   vector<vector<double>> m,
                   vector<vector<double>> r) const;
};

#endif //INTERFACE4__EM_H_
