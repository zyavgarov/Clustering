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
    if (field_ == nullptr) {
        field_ = new Field ();
        log ("Field initialized");
    }
    if (Field::readonly ()) {
        return -1;
    }
    auto *cloud = new Cloud (c_length, disp_x, disp_y);
    cloud->shift (center_x, center_y);
    Field::add (cloud);
    log (to_string (id) + ": generated cloud (" + to_string (center_x) + ", " + to_string (center_y)
             + ") with dispersion (" + to_string (disp_x) + ";" + to_string (disp_y) + ")");
    return 0;
}

void Controller::log (const string &s) {
    if (write_log) {
        logger << s << endl;
    }
}

int Controller::fprintf (const string &file_name) {
    // prints field to file
    // -1 - file not found
    ofstream out (file_name);
    Field::fprintf (out);
    out.close ();
    return 0;
}

void Controller::histogram (int pieces, vector<int> &x_distr, vector<int> &y_distr) const {
    // makes a vector histogram of distribution of dots on the field
    class histogram x(pieces, x_distr, y_distr);
}

int Controller::matrix () const {
    // sets field to readonly, creats matrix of distances
    // if there is no field returns -1
    if (field_ == nullptr) {
        return -1;
    }
    Field::create_dist_matrix ();
    return 0;
}

const vector<Cluster_Search> &Controller::info_cluster_search () const {
    return Field::searches ();
}

int Controller::incidence_matrix (double delta) {
    /* creates incidence matrix in field
     * Errors:
     * -2 - Field not found
     */
    if (field_ == nullptr) {
        log ("Field not found");
        return -2;
    }
    field_->create_edges_matrix (delta);
    return 0;
}
