#include "Cloud.h"

Cloud::Cloud () = default;

Cloud::Cloud (int length) {
    //creates the cloud with Gauss distribution
    //from length points with center in (0,0)
    //and radius 0.5
    for (int i = 0; i < length; ++i) {
        double x, y;
        x = y = 0;
        for (int j = 0; j < M; ++j) {
            x += (double) rand () / RAND_MAX - 0.5;
        }
        x /= M;
        for (int j = 0; j < M; ++j) {
            y += (double) rand () / RAND_MAX - 0.5;
        }
        y /= M;
        point_.emplace_back (x, y);
    }
}

Cloud::Cloud (int length, double disp_x, double disp_y) {
    //creates the cloud with Gauss distribution
    //from length points with center in (0,0)
    //and radius 0.5
    //disp_x and disp_y - coefficients of Gauss distribution
    for (int i = 0; i < length; ++i) {
        double x, y;
        x = y = 0;
        for (int j = 0; j < M; ++j) {
            x += (double) rand () / RAND_MAX - 0.5;
        }
        x /= M;
        x *= disp_x;
        for (int j = 0; j < M; ++j) {
            y += (double) rand () / RAND_MAX - 0.5;
        }
        y /= M;
        y *= disp_y;
        point_.emplace_back (x, y);
    }
}

Cloud &Cloud::operator= (Cloud const &c) {
    //operator =
    if (this != &c) {
        point_.clear ();
        for (int i = 0; i < c.length (); ++i) {
            point_.push_back (c.point_[i]);
        }
    }
    return *this;
}

Cloud::Cloud (Cloud const &c) {
    //Copy constructor
    for (int i = 0; i < c.length (); ++i) {
        point_.push_back (c.point_[i]);
    }
}

Cloud::~Cloud () = default;

int Cloud::length () const {
    // returns the number of points in cloud
    return point_.size ();
}

void Cloud::shift (double shift_x, double shift_y) {
    //shifts the cloud to shift_x up and shift_y right
    int len = length ();
    for (int i = 0; i < len; ++i) {
        point_[i].shift (shift_x, shift_y);
    }
}

void Cloud::rotate (double angle) {
    // rotates the cloud around the center counter-clockwise for a given angle
    double center_x = 0;
    double center_y = 0;
    for (const auto &p: point_) {
        center_x += p.x ();
        center_y += p.y ();
    }
    center_x /= length ();
    center_y /= length ();
    double shift_x = -center_x;
    double shift_y = -center_y;
    shift (shift_x, shift_y);
    for (int i = 0; i < length (); ++i) {
        double old_x = point_[i].x ();
        double old_y = point_[i].y ();
        Point a (old_x * cos (angle) - old_y * sin (angle), old_x * sin (angle) + old_y * cos (angle), 0);
        point_[i] = a;
    }
    shift (-shift_x, -shift_y);
}

void Cloud::mirror () {
    //makes a reflection about center and Oy
    double center_x = 0;
    for (const auto &p: point_) {
        center_x += p.x ();
    }
    center_x /= length ();
    double shift_x = -center_x;
    shift (shift_x, 0);
    for (int i = 0; i < length (); ++i) {
        point_[i].shift (-point_[i].x () * 2, 0);
    }
    shift (-shift_x, 0);
}

void Cloud::zoom (double k) {
    // distance from the center to point increases in k times
    // the direction of vectors from the center to each point doesn't change
    double center_x = 0;
    double center_y = 0;
    for (const auto &p: point_) {
        center_x += p.x ();
        center_y += p.y ();
    }
    center_x /= length ();
    center_y /= length ();
    double shift_x = -center_x;
    double shift_y = -center_y;
    shift (shift_x, shift_y);
    for (int i = 0; i < length (); i++) {
        Point temp (point_[i].x () * k, point_[i].y () * k, 0);
        point_[i] = temp;
    }
    shift (-shift_x, -shift_y);
}

int Cloud::fprintf (ofstream out) const {
    //prints all the clouds to file out
    for (int i = 0; i < length (); ++i) {
        point_[i].fprintf (out);
    }
    return 0;
}

vector<Point> &Cloud::point () {
    return point_;
}
