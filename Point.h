//
// Created by pierre on 30.09.2020.
//

#ifndef INTERFACE4__POINT_H_
#define INTERFACE4__POINT_H_
#include <fstream>
using namespace std;

class Point {
  friend class Cloud;
 
 public:
  Point ();
  Point (Point const &c);
  Point (double x, double y);
  double x () const;
  double y () const;
  int fprintf (ofstream *out) const;
  int id () const;
 private:
  double x_, y_; //coords
  int id_;
};

#endif //INTERFACE4__POINT_H_
