#ifndef INTERFACE4__BUFFER_H_
#define INTERFACE4__BUFFER_H_
//#include "Controller.h"
#include "Cloud.h"

class Buffer {
 public:
  void add (const Cloud &c);
  void add (Point *p);
  const vector<Point *> &unload () const;
  void erase();
  int length () const;
  void shift (double shift_x, double shift_y);
  void rotate (double angle);
  void mirror ();
  void zoom (double k);
  Cloud cloud;
 private:
};

#endif //INTERFACE4__BUFFER_H_
