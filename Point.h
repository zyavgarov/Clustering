#ifndef INTERFACE4__POINT_H_
#define INTERFACE4__POINT_H_
#include <fstream>
using namespace std;

/* There can be added some operators for adiition and multiplication to number
 */

class Point {
  friend class Cloud;
 
 public:
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
  int note;
 private:
  double x_, y_; //coords
  int id_;
  int fprintf (ofstream &out) const;
};

#endif //INTERFACE4__POINT_H_
