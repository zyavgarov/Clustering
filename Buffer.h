#ifndef INTERFACE4__BUFFER_H_
#define INTERFACE4__BUFFER_H_
//#include "Controller.h"
#include "Cloud.h"

class Buffer {
 public:
  Buffer () = default;
  void add (const Cloud &c);
  void add (const Point &p);
  void add (const Buffer &b);
  const vector<Point> &unload () const;
  int length () const;
  Cloud cloud;
 private:
  //Controller *cc;
};

#endif //INTERFACE4__BUFFER_H_
