#include "Field.h"

Field::Field () = default;

Field::Field (int f_length, int c_length) {
    for (int i = 0; i < f_length; ++i) {
        cloud_.emplace_back (c_length);
    }
}

Field &Field::operator= (Field const &f) {
    if (this != &f) {
        cloud_.clear ();
        for (int i = 0; i < f.length (); ++i) {
            cloud_.push_back (f.cloud_[i]);
        }
    }
    return *this;
}

Field::Field (Field const &f) {
    for (int i = 0; i < f.length (); ++i) {
        cloud_[i] = f.cloud_[i];
    }
}

Field::~Field () = default;

int Field::add (const Cloud &addition) {
    // The function adds cloud to the field
    cloud_.push_back (addition);
    return 0;
}

int Field::fprintf (ofstream &out) const {
    //prints all the clouds to file out
    for (int i = 0; i < length (); ++i) {
        cloud_[i].fprintf (out);
    }
    return 0;
}

int Field::length () const {
    // returns the number of clouds in the field
    return cloud_.size ();
}

const vector<Cloud> &Field::cloud () {
    return cloud_;
}