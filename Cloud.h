//
// Created by pierre on 30.09.2020.
//

#ifndef INTERFACE4__CLOUD_H_
#define INTERFACE4__CLOUD_H_
#include "Point.h"
#include <vector>
using namespace std;
class Cloud {
 public:
  vector<Point> point_;
  Cloud ();
  explicit Cloud (int length);
  Cloud (int length, double disp_x, double disp_y);
  Cloud &operator= (Cloud const &c);
  Cloud (Cloud const &c);
  ~Cloud ();
  int length () const;
  void shift (double shift_x, double shift_y);
  void rotate (double angle);
  void mirror ();
  void zoom (double k);
  int fprintf (ofstream *out) const;
};


#endif //INTERFACE4__CLOUD_H_
