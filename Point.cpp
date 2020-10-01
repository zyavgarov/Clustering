#include "Point.h"

Point::Point () : id_ (0) {
    x_ = 0;
    y_ = 0;
}

Point::Point (double x, double y, int id) : x_ (x), y_ (y) {
    static int quantity = 0; // maximum id; number of points
    if (id != -1) {
        id_ = 0;
    } else {
        id_ = ++quantity;
    }
}

Point::Point (Point const &c) : x_ (c.x ()), y_ (c.y ()), id_ (id ()) {
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

Point operator+ (const Point &a, const Point &b) {
    if (a.id () != 0 && b.id () != 0) {
        return Point (a.x () + b.x (), a.y () + b.y ());
    } else if (a.id () != 0 && b.id () == 0) {
        return Point (a.x () + b.x (), a.y () + b.y (), a.id ());
    } else if (b.id () != 0 && a.id () == 0) {
        return Point (a.x () + b.x (), a.y () + b.y (), b.id ());
    } else {
        return Point (a.x () + b.x (), a.y () + b.y (), 0);
    }
}