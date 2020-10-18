#include "Controller.h"

Controller::Controller (bool write_log) : write_log (write_log), field_ (nullptr) {
    if (write_log) {
        log_file_name = "log_ctrl.txt";
        logger = ofstream (log_file_name);
        if (!logger.is_open ()) {
            cout << "Error while writing logs" << endl;
            this->write_log = false;
        }
    }
}

Controller::~Controller () {
    delete field_;
    if (logger.is_open ()) {
        logger.close ();
    }
}

int Controller::get_help (int id, string &file_name) {
    file_name = "help.txt";
    log (to_string (id) + ": help.txt");
    return 0;
}

int Controller::generate_cloud (int id,
                                double center_x = 0,
                                double center_y = 0,
                                double disp_x = 1,
                                double disp_y = 1,
                                int c_length = 1000) {
    // generates cloud with center on the field with dispersions by x and y
    // -1 if readonly
    auto *cloud = new Cloud(c_length, disp_x, disp_y);
    cloud->shift (center_x, center_y);
    if (field_ == nullptr) {
        field_ = new Field ();
        log ("Field initialized");
    }
    if (field_->readonly ()) {
        return -1;
    }
    field_->add (cloud);
    log (to_string (id) + ": generated cloud (" + to_string (center_x) + ", " + to_string (center_y)
             + ") with dispersion (" + to_string (disp_x) + ";" + to_string (disp_y) + ")");
    return 0;
}

void Controller::log (const string &s) {
    if (write_log) {
        logger << s << endl;
    }
}

int Controller::fprintf (const string &file_name) const {
    // prints field to file
    // -1 - file not found
    ofstream out (file_name);
    field_->fprintf (out);
    out.close ();
    return 0;
}

void Controller::histogram (int pieces, vector<int> &x_distr, vector<int> &y_distr) const {
    // makes a vector histogram of distribution of dots on the field
    bool started = false;
    double max;
    double min;
    //Working over x
    //searching for range of distribution
    x_distr.clear ();
    y_distr.clear ();
    for (int i = 0; i < field_->length (); ++i) {
        for (int j = 0; j < field_->cloud ()[i]->length (); ++j) {
            if (started) {
                if (field_->cloud ()[i]->point ()[j]->x () > max) {
                    max = field_->cloud ()[i]->point ()[j]->x ();
                } else if (field_->cloud ()[i]->point ()[j]->x () < min) {
                    min = field_->cloud ()[i]->point ()[j]->x ();
                }
            } else {
                min = max = field_->cloud ()[i]->point ()[j]->x ();
                started = true;
            }
        }
    }
    //distribution itself
    double piece_len = (max - min) / pieces;
    vector<int> line (pieces);
    for (int i = 0; i < pieces; ++i) {
        line[i] = 0;
    }
    x_distr = line;
    for (int i = 0; i < field_->length (); ++i) {
        for (int j = 0; j < field_->cloud ()[i]->length (); ++j) {
            for (int k = 0; k < pieces; ++k) {
                if (min + piece_len * (k + 1) > field_->cloud ()[i]->point ()[j]->x ()) {
                    ++x_distr[k];
                    break;
                }
            }
        }
    }
    //same for y
    started = false;
    for (int i = 0; i < field_->length (); ++i) {
        for (int j = 0; j < field_->cloud ()[i]->length (); ++j) {
            if (started) {
                if (field_->cloud ()[i]->point ()[j]->y () > max) {
                    max = field_->cloud ()[i]->point ()[j]->y ();
                } else if (field_->cloud ()[i]->point ()[j]->y () < min) {
                    min = field_->cloud ()[i]->point ()[j]->y ();
                }
            } else {
                min = max = field_->cloud ()[i]->point ()[j]->y ();
                started = true;
            }
        }
    }
    y_distr = line;
    for (int i = 0; i < field_->length (); ++i) {
        for (int j = 0; j < field_->cloud ()[i]->length (); ++j) {
            for (int k = 0; k < pieces; ++k) {
                if (min + piece_len * (k + 1) > field_->cloud ()[i]->point ()[j]->x ()) {
                    ++y_distr[k];
                    break;
                }
            }
        }
    }
}

int Controller::buffer_add_cloud (int id) const {
    // adds cloud with id to buffer
    // returns -1 if field is readonly
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.add (field_->cloud ()[id]);
    return 0;
}

int Controller::buffer_unload () const {
    // unloads buffer to field
    // returns -1 if field is readonly
    if (field_->readonly ()) {
        return -1;
    }
    field_->add (&field_->buf.cloud);
    return 0;
}

int Controller::matrix () const {
    // sets field to readonly, creats matrix of distances
    // if there is no field returns -1
    if (field_ == nullptr) {
        return -1;
    }
    field_->create_dist_matrix ();
    return 0;
}

int Controller::dbscan (int k) const { // 1) what if field is not readonly 2) incidence matrix already created
    // realises dbscan-clustering with k-core points and d-incidence
    field_->db_clustering(k);
    return 0;
}

int Controller::wave () { // 1) what if field is not readonly 2) incidence matrix already created
    // realises wave-clustering algorithm
    field_->wave_clustering();
    log ("Started wave-clustering");
    return 0;
}

const vector<Cluster_Search> &Controller::info_cluster_search () const {
    return field_->searches ();
}

int Controller::buffer_shift (double x, double y) const {
    //shifts cloud in buffer to vector (x, y)
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.shift (x, y);
    return 0;
}

int Controller::buffer_zoom (double k) const {
    //zooms cloud in buffer to coefficient k
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.zoom (k);
    return 0;
}

int Controller::buffer_mirror () const {
    //zooms cloud in buffer to coefficient k
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.mirror ();
    return 0;
}

int Controller::buffer_rotate (double angle) const {
    //rotates cloud in buffer to angle
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.rotate (angle);
    return 0;
}

int Controller::incidence_matrix (double delta) const {
    //creates incidence matrix in field
    field_->create_edges_matrix(delta);
    return 0;
}
