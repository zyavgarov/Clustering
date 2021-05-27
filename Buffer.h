#ifndef INTERFACE4__BUFFER_H_
#define INTERFACE4__BUFFER_H_
//#include "Controller.h"
#include "Cloud.h"

/* This is class Buffer
 * The idea how it works: when we add the point into B
 * its copy with id=0 is created in vector points
 * when we unload buffer the point is created on the field with its new id
 */
class Buffer {
 public:
  void add (Cloud *c);
  void add (Point *p);
  const vector<Point> &unload () const;
  void erase ();
  int length () const;
  void shift (double shift_x, double shift_y);
  void rotate (double angle);
  void mirror ();
  void zoom (double k);
  Cloud cloud;
 private:
  vector<Point> points;
};

#endif //INTERFACE4__BUFFER_H_
