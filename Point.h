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
 public:
  static vector<Point *> id_pointers;
  static int quantity;// better make it static
  Point ();
  Point (Point const &c);
  Point (double x, double y, int id = -1);
  double x () const;
  double y () const;
  int id () const;
  void shift (const double &x, const double &y);
  void shift (const Point &vector);
  Point operator+ (const Point &a) const;
  Point operator* (const double &a) const;
  Point operator/ (const double &a) const;
  int note; // for dbscan
  int neighbours; // for dbscan, quantity of edges from that point
  static Point &get_by_id (int id);
  static void reset_quantity ();
  static double dist (const Point &A, const Point &B);
};

#endif //INTERFACE4__POINT_H_
