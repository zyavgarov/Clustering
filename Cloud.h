#ifndef INTERFACE4__CLOUD_H_
#define INTERFACE4__CLOUD_H_
#include "Point.h"
#include <cmath>
const int M = 100;

class Cloud {
  friend class Field;
  friend class Controller;
  friend class Buffer;
 public:
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
  const vector<Point> & point () const;
 private:
  int fprintf (ofstream &out) const;
  vector<Point> point_;
};

#endif //INTERFACE4__CLOUD_H_
