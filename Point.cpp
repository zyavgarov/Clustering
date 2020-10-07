#include "Point.h"

Point::Point () : id_ (0), note (0), x_ (0), y_ (0) {
}

Point::Point (double x, double y, int id) : x_ (x), y_ (y), note (0) {
    if (id != -1) {
        id_ = 0;
    } else {
        id_ = ++quantity_;
    }
}

Point::Point (Point const &c) : x_ (c.x ()), y_ (c.y ()), id_ (id ()), note (0) {
}

double Point::x () const {
    return x_;
}

double Point::y () const {
    return y_;
}

int Point::fprintf (ofstream &out) const {
    //prints all the clouds to file out
    out << x () << " " << y () << endl;
    return 0;
}

int Point::id () const {
    return id_;
}

void Point::shift (const double &x, const double &y) {
    x_ += x;
    y_ += y;
}

void Point::shift (const Point &vector) {
    x_ += vector.x ();
    y_ += vector.y ();
}

Point Point::operator+ (const Point &a) const {
    if (id () != 0 && a.id () != 0) {
        return Point (x () + a.x (), y () + a.y ());
    } else if (id () != 0 && a.id () == 0) {
        return Point (x () + a.x (), y () + a.y (), id ());
    } else if (a.id () != 0 && id () == 0) {
        return Point (x () + a.x (), y () + a.y (), a.id ());
    } else {
        return Point (x () + a.x (), y () + a.y (), 0);
    }
}

Point Point::operator* (const double &a) const {
    if (id () == 0) {
        return Point (x () * a, y () * a, 0);
    } else {
        return Point (x () * a, y () * a);
    }
}

Point Point::operator/ (const double &a) const {
    if (id () == 0) {
        return Point (x () / a, this->y () / a, 0);
    } else {
        return Point (x () / a, this->y () / a);
    }
}

Point &Point::get_by_id (int id) {
    return *id_pointers[id];
}

void Point::reset_quantity () {
    quantity_ = 0;
}

double Point::dist (const Point &A, const Point &B) {
    return sqrt (pow (A.x () - B.x (), 2) + pow (A.y () - B.y (), 2));
}

#ifndef POINT_STATIC_INIT
#define POINT_STATIC_INIT
//int Point::quantity_ = 0;

int Point::quantity () {
    return quantity_;
}

vector<Point *> Point::id_pointers;
#endif //POINT_STATIC_INIT