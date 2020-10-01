#ifndef INTERFACE4__POINT_H_
#define INTERFACE4__POINT_H_
#include <fstream>
#include <vector>
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
  int note; // for dbscan
  int neighbours; // for dbscan, quantity of edges from that point
  static Point & get_by_id(int id);
  static void reset_quantity();
  static int quantity();
 private:
  static int quantity_;
  static vector<Point*> id_pointers;
  double x_, y_; //coords
  int id_;
  int fprintf (ofstream &out) const;
};

#endif //INTERFACE4__POINT_H_
