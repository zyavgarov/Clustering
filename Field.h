#ifndef INTERFACE4__FIELD_H_
#define INTERFACE4__FIELD_H_

#include "Buffer.h"
class Field {
 public:
  friend class Buffer;
  Field ();
  Field (int f_length, int c_length);
  ~Field ();
  int add (const Cloud &addition);
  int fprintf (ofstream &out) const;
  int length () const;
  const vector<Cloud> & cloud ();
  Buffer buf;
 private:
  Field &operator= (Field const &f);
  Field (Field const &f);
  vector<Cloud> cloud_;
};

#endif //INTERFACE4__FIELD_H_
