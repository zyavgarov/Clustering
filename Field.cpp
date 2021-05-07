#include "Field.h"
#include <iostream>
Buffer Field::buf;
vector<Cloud *> Field::cloud_;
vector<vector<double>> Field::dist_; // matrix of distances
vector<Cluster_Search> Field::searches_;
bool Field::readonly_ = false;

Field::Field () = default;

Field &Field::operator= (Field const &f) {
    if (this != &f) {
        for (auto &c: cloud_) {
            delete c;
        }
        cloud_.clear ();
        for (int i = 0; i < Field::length (); ++i) {
            cloud_.push_back (Field::cloud_[i]);
        }
    }
    return *this;
}

Field::Field (Field const &f) {
    for (auto &c: cloud_) {
        delete c;
    }
    cloud_.clear ();
    for (int i = 0; i < Field::length (); ++i) {
        cloud_[i] = Field::cloud_[i];
    }
}

Field::~Field () {
    for (auto &i : cloud_) {
        delete i;
    }
}

int Field::add (Cloud *addition) {
    // The function adds cloud to the field
    // returns -1 if readonly
    if (readonly ()) {
        return -1;
    }
    cloud_.push_back (addition);
    return 0;
}

int Field::add (const vector<Point> &addition) {
    // The function adds vector of points to one cloud to the field together
    // returns -1 if readonly
    if (readonly ()) {
        return -1;
    }
    auto *new_cloud = new Cloud (addition);
    cloud_.push_back (new_cloud);
    return 0;
}

int Field::fprintf (ofstream &out) {
    //prints all the clouds to file out
    for (int i = 0; i < length (); ++i) {
        cloud_[i]->fprintf (out);
    }
    return 0;
}

int Field::length () {
    // returns the number of clouds in the field
    return cloud_.size ();
}

const vector<Cloud *> &Field::cloud () {
    return cloud_;
}

const vector<vector<double>> &Field::dist () {
    return dist_;
}

bool Field::readonly () {
    return readonly_;
}

void Field::create_dist_matrix () {
    readonly_ = true;
    dist_.clear ();
    dist_ = vector<vector<double>> (Point::quantity (), vector<double> (Point::quantity ()));
    for (int i = 0; i < Point::quantity (); ++i) {
        for (int j = i; j < Point::quantity (); ++j) {
            dist_[i][j] = dist_[j][i] = Point::dist (Point::get_by_id (i + 1), Point::get_by_id (j + 1));
        }
    }
}

const vector<Cluster_Search> &Field::searches () {
    return searches_;
}

void Field::create_edges_matrix (double delta) {
    // creating incidence matrix, saving incidence graph to file
    searches_.emplace_back (this, delta);
    fprintf_incidence_graph (searches ().back (), searches ().size () - 1);
}

void Field::fprintf_incidence_graph (const Cluster_Search &search, int id) {
    ofstream out ("gnuplot/incidence/incidence" + to_string (id) + ".txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        for (int j = i + 1; j < Point::quantity (); ++j) {
            if (search.edges ()[i][j]) {
                out << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
                out << Point::get_by_id (j + 1)->x () << " " << Point::get_by_id (j + 1)->y () << endl;
                out << endl;
            }
        }
    }
}
