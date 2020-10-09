#include "Buffer.h"

void Buffer::add (const Cloud &c) {
    //adds cloud to buffer
    for (int i = 0; i < c.length (); ++i) {
        cloud.point_.push_back (c.point()[i]);
    }
}

void Buffer::add (const Point &p) {
    //adds point to buffer
    cloud.point_.push_back (p);
}

void Buffer::add (const Buffer &b) {
    //adds another buffer to buffer
    for (int i = 0; i < b.length (); ++i) {
        cloud.point_.push_back (b.cloud.point_[i]);
    }
}

int Buffer::length () const {
    // returns number of points in buffer
    return cloud.length ();
}

const vector<Point>& Buffer::unload () const {
    // returns all the points from buffer
    return cloud.point();
}