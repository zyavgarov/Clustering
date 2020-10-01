#include "Buffer.h"
Buffer::Buffer (Controller *cc) : cc (cc) {
    cloud = new Cloud ();
}

Buffer::~Buffer () {
    delete cloud;
}

void Buffer::add (const Cloud &c) const {
    for (int i = 0; i < c.length (); ++i) {
        cloud->point_.push_back (c.point()[i]);
    }
}

void Buffer::add (const Point &p) const {
    cloud->point_.push_back (p);
}

void Buffer::add (const Buffer &b) const {
    for (int i = 0; i < b.length (); ++i) {
        cloud->point_.push_back (b.cloud->point_[i]);
    }
}

int Buffer::length () const {
    return cloud->length ();
}

void Buffer::unload () const {
    cc->field_->add (*cloud);
}