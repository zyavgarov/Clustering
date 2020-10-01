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
    Cloud cloud (c_length, disp_x, disp_y);
    cloud.shift (center_x, center_y);
    if (field_ == nullptr) {
        field_ = new Field ();
        log ("Field initialized");
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
        for (int j = 0; j < field_->cloud ()[i].length (); ++j) {
            if (started) {
                if (field_->cloud ()[i].point ()[j].x () > max) {
                    max = field_->cloud ()[i].point ()[j].x ();
                } else if (field_->cloud ()[i].point ()[j].x () < min) {
                    min = field_->cloud ()[i].point ()[j].x ();
                }
            } else {
                min = max = field_->cloud ()[i].point ()[j].x ();
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
        for (int j = 0; j < field_->cloud ()[i].length (); ++j) {
            for (int k = 0; k < pieces; ++k) {
                if (min + piece_len * (k + 1) > field_->cloud ()[i].point ()[j].x ()) {
                    ++x_distr[k];
                    break;
                }
            }
        }
    }
    //same for y
    started = false;
    for (int i = 0; i < field_->length (); ++i) {
        for (int j = 0; j < field_->cloud ()[i].length (); ++j) {
            if (started) {
                if (field_->cloud ()[i].point ()[j].y () > max) {
                    max = field_->cloud ()[i].point ()[j].y ();
                } else if (field_->cloud ()[i].point ()[j].y () < min) {
                    min = field_->cloud ()[i].point ()[j].y ();
                }
            } else {
                min = max = field_->cloud ()[i].point ()[j].y ();
                started = true;
            }
        }
    }
    y_distr = line;
    for (int i = 0; i < field_->length (); ++i) {
        for (int j = 0; j < field_->cloud ()[i].length (); ++j) {
            for (int k = 0; k < pieces; ++k) {
                if (min + piece_len * (k + 1) > field_->cloud ()[i].point ()[j].x ()) {
                    ++y_distr[k];
                    break;
                }
            }
        }
    }
}

vector<Cluster> Controller::dbscan (double d, int k) {
    // The first step is making matrix of distances
    vector<vector<int>> distance_matrix;
    distance_matrix = make_distance_matrix();
    
}

vector<vector<int>> Controller::make_distance_matrix () {
    // to be done
}

