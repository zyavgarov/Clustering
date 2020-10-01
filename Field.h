#ifndef INTERFACE4__FIELD_H_
#define INTERFACE4__FIELD_H_

#include "Cloud.h"

class Field {
 public:
  Field ();
  Field (int f_length, int c_length);
  ~Field ();
  int add (const Cloud &addition);
  int fprintf (ofstream &out) const;
  int length () const;
  const vector<Cloud> & cloud ();
  int readable ();
 private:
  Field &operator= (Field const &f);
  Field (Field const &f);
  bool readable_;
  vector<Cloud> cloud_;
};

#endif //INTERFACE4__FIELD_H_
