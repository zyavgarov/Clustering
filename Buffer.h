#ifndef INTERFACE4__BUFFER_H_
#define INTERFACE4__BUFFER_H_
//#include "Controller.h"
#include "Cloud.h"

class Buffer {
 public:
  Cloud cloud;
  Buffer () = default;
  void add (const Cloud &c);
  void add (const Point &p);
  const vector<Point> &unload () const;
  int length () const;
  void shift (double shift_x, double shift_y);
  void rotate (double angle);
  void mirror ();
  void zoom (double k);
 private:
  //Controller *cc;
};

#endif //INTERFACE4__BUFFER_H_
