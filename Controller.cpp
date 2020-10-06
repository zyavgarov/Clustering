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

Cluster_Search Controller::scan (int k, int d) {
    // Scans points for clusters. Points are in one cluster if there is a way between them.
    // Two points has a straight way if and only if the distance between them is less than d
    // A little notice: if we are on
    readonly_ = true;
    if (Cluster_Search::dist ().empty ()) {
        Cluster_Search::create_dist_matrix ();
    }
    // matrix of incidences
    vector<vector<bool>> incidence (Point::quantity, vector<bool> (Point::quantity, false));
    for (int i = 0; i < Point::quantity; ++i) {
        for (int j = i + 1; j < Point::quantity; ++j) {
            incidence[j][i] = incidence[i][j] = (Cluster_Search::dist ()[i][j] < d);
        }
    }
    vector<bool> burnt (Point::quantity, false);
    int burnt_num = 0; // number of true in burnt vector
    Cluster_Search res(d);
    // that cycle checks if point in marked cluster and if not creates new one
    for (int m = 1; m <= Point::quantity; ++m) {
        if (burnt[m]) {
            continue;
        }
        vector<int> curr_wave = {m};
        vector<int> new_wave = curr_wave; // points, connected to smth from curr_wave
        vector<int> curr_cluster; // points from current cluster
        // that cycle searches for neighbours of points in curr_wave
        while (!new_wave.empty ()) {
            for (int i = 0; i < curr_wave.size (); ++i) {
                for (int j = 0; j < Point::quantity; ++j) {
                    if (i != j && incidence[i][j] && !burnt[j]) {
                        new_wave.push_back (j);
                    }
                }
            }
            curr_cluster.insert (curr_cluster.end (), curr_wave.begin (), curr_wave.end ());
            curr_wave = new_wave;
            burnt_num += new_wave.size ();
            for (auto &&i: new_wave) {
                burnt[i] = true;
            }
        }
        res.add(Cluster_Search::Cluster(curr_cluster));
    }
    return res;
}

bool Controller::readable () const {
    return readonly_;
}
