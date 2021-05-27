#include "Buffer.h"

void Buffer::add (Cloud *c) {
    //adds cloud to buffer
    for (int i = 0; i < c->length (); ++i) {
        points.emplace_back (*c->point ()[i], 0);
    }
}

void Buffer::add (Point *p) {
    // adds point to buffer
    points.emplace_back (*p);
}

int Buffer::length () const {
    // returns number of points in buffer
    // return cloud.length ();
    return points.size ();
}

const vector<Point> &Buffer::unload () const {
    // returns all the points from buffer
    // return cloud.point ();
    return points;
}

void Buffer::shift (double shift_x, double shift_y) {
    //shifts the cloud to shift_x up and shift_y right
    int len = length ();
    for (int i = 0; i < len; ++i) {
        points[i].shift (shift_x, shift_y);
    }
}

void Buffer::rotate (double angle) {
    // rotates the cloud around the center counter-clockwise for a given angle
    double center_x = 0;
    double center_y = 0;
    for (auto &point : points) {
        center_x += point.x ();
        center_y += point.y ();
    }
    center_x /= length ();
    center_y /= length ();
    double shift_x = -center_x;
    double shift_y = -center_y;
    shift (shift_x, shift_y);
    for (int i = 0; i < length (); ++i) {
        double old_x = points[i].x ();
        double old_y = points[i].y ();
        points[i] = Point (old_x * cos (angle) - old_y * sin (angle),
                           old_x * sin (angle) + old_y * cos (angle),
                           0);
    }
    shift (-shift_x, -shift_y);
}

void Buffer::mirror () {
    //makes a reflection about center and Oy
    double center_x = 0;
    for (const auto &p: points) {
        center_x += p.x ();
    }
    center_x /= length ();
    double shift_x = -center_x;
    shift (shift_x, 0);
    for (int i = 0; i < length (); ++i) {
        points[i].shift (-points[i].x () * 2, 0);
    }
    shift (-shift_x, 0);
    
}

void Buffer::zoom (double k) {
    // distance from the center to point increases in k times
    // the direction of vectors from the center to each point doesn't change
    double center_x = 0;
    double center_y = 0;
    for (const auto &p: points) {
        center_x += p.x ();
        center_y += p.y ();
    }
    center_x /= length ();
    center_y /= length ();
    double shift_x = -center_x;
    double shift_y = -center_y;
    shift (shift_x, shift_y);
    for (int i = 0; i < length (); i++) {
        Point temp (points[i].x () * k, points[i].y () * k, 0);
        points[i] = Point (points[i].x () * k, points[i].y () * k, 0);
    }
    shift (-shift_x, -shift_y);
}

void Buffer::erase () {
    points.clear ();
}
