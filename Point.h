#ifndef INTERFACE4__POINT_H_
#define INTERFACE4__POINT_H_
#include <fstream>
#include <vector>
#include <cmath>
using namespace std;

class Point {
  friend class Cloud;
 
 private:
  double x_, y_; //coords
  int id_;
  int fprintf (ofstream &out) const;
  static int quantity_;
  static vector<Point *> id_pointers; // vector of pointers because vector of references is not supported
 public:
  Point ();
  Point (Point const &c);
  Point (double x, double y, int id = -1);
  Point &operator= (Point const &p);
  double x () const;
  double y () const;
  int id () const;
  void shift (const double &x, const double &y);
  void shift (const Point &vector);
  Point operator+ (const Point &a) const;
  Point operator* (const double &a) const;
  Point operator/ (const double &a) const;
  static const Point * get_by_id (int id);
  static double dist (const Point *A, const Point *B);
  static int quantity ();
};

#endif //INTERFACE4__POINT_H_
