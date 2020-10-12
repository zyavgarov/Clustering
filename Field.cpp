#include "Field.h"

Field::Field () = default;

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

const vector<vector<double>> &Field::dist () const {
    return dist_;
}

bool Field::readonly () const {
    return readonly_;
}

void Field::create_dist_matrix () {
    dist_.clear ();
    dist_ = vector<vector<double>>(Point::quantity(), vector<double>(Point::quantity()));
    for (int i = 0; i < Point::quantity (); ++i) {
        for (int j = 0; j < Point::quantity (); ++j) {
            dist_[i][j] = dist_[j][i] = Point::dist (Point::get_by_id (i), Point::get_by_id (j));
        }
    }
}
