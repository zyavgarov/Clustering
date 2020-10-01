#ifndef INTERFACE4__BUFFER_H_
#define INTERFACE4__BUFFER_H_
#include "Controller.h"

class Buffer {
 public:
  explicit Buffer (Controller *cc);
  ~Buffer ();
  void add (const Cloud &c) const;
  void add (const Point &p) const;
  void add (const Buffer &b) const;
  void unload () const;
  int length () const;
  Cloud *cloud;
 private:
  Controller *cc;
};

#endif //INTERFACE4__BUFFER_H_
