#include "Controller.h"
bool Controller::working_ = false;
queue<pair<int, string>> Controller::actions;
Field *Controller::field_;
ofstream Controller::logger;

Controller::Controller () {
    int err = Administrator::prepare ();
    if (err == -1) {
        perror ("Server: cannot create socket");
        exit (EXIT_FAILURE);
    } else if (err == -2) {
        perror ("Server: cannot bind socket");
        exit (EXIT_FAILURE);
    } else if (err == -3) {
        perror ("Server: listen queue failure");
        exit (EXIT_FAILURE);
    }
    logger = ofstream ("log.txt");
    working_ = true;
    thread manage_th (manager);
    manage_th.detach ();
    log ("connection opened");
    err = Administrator::run ();
}

void Controller::add_next (int fd, const string &command) {
    // adds command to queue
    actions.push (pair<int, string> (fd, command));
}

string Controller::do_command (const string &command, int fd) {
    // figures out what to do with client's command
    stringstream ss (command);
    string main;
    ss >> main;
    for (auto &c: main) c = toupper (c);
    if (main == "END") {
        working_ = false;
        // there should be something more accurate
    } else if (main == "GC") {
        // generation of cloud(s)
        double x, y, disp_x, disp_y;
        int dots = 0;
        ss >> x >> y >> disp_x >> disp_y >> dots;
        generate_cloud (x, y, disp_x, disp_y, dots);
        log ("cloud created" + to_string (x) + " " + to_string (y) + " " + to_string (disp_x) + " " + to_string (disp_y)
                 + ": " + to_string (dots) + " points", fd);
        return "Cloud created";
    } else if (main == "HELP") {
        thread help_writer (send_help, fd);
        help_writer.join ();
    } else if (main == "BUFFER") {
        string operation;
        ss >> operation;
        for (auto &c: operation) c = toupper (c);
        if (operation == "UNLOAD") {
            if (Controller::buffer_unload () == -1) {
                log ("buffer not loaded: readonly", fd);
                return "Field in readonly mode";
            } else {
                log ("buffer unloaded", fd);
                return "Buffer unloaded to the field";
            }
        } else if (operation == "ADD") {
            int cloud_id;
            ss >> cloud_id;
            if (Controller::buffer_add_cloud (cloud_id) == -1) {
                log ("cloud not added: readonly", fd);
                return "Field in readonly mode";
            } else {
                log ("cloud added to buffer", fd);
                return "Cloud №" + to_string (cloud_id) + " added to buffer";
            }
        } else if (operation == "ROTATE") {
            double angle;
            ss >> angle;
            if (Controller::buffer_rotate (angle) == -1) {
                log ("buffer not rotated: readonly", fd);
                return "Field in readonly mode";
            } else {
                return "Buffer rotated";
            }
        } else if (operation == "ZOOM") {
            double k;
            ss >> k;
            if (Controller::buffer_zoom (k) == -1) {
                log ("buffer not scaled: readonly", fd);
                return "Field in readonly mode";
            } else {
                log ("buffer zoomed", fd);
                return "Buffer zoomed";
            }
        } else if (operation == "SHIFT") {
            double x, y;
            ss >> x >> y;
            if (Controller::buffer_shift (x, y) == -1) {
                log ("buffer not shifted: readonly", fd);
                return "Field in readonly mode";
            } else {
                log ("buffer shifted", fd);
                return "Buffer shifted";
            }
        } else if (operation == "MIRROR") {
            if (Controller::buffer_mirror () == -1) {
                log ("buffer not reflected: readonly", fd);
                return "Field in readonly mode";
            } else {
                log ("buffer reflectedy", fd);
                return "Buffer reflected";
            }
        } else if (operation == "ERASE") {
            if (Controller::buffer_erase () == -1) {
                log ("buffer not erased: readonly", fd);
                return "Field in readonly mode";
            } else {
                log ("buffer cleaned", fd);
                return "Buffer cleaned";
            }
        } else {
            // something went wrong
            log ("wrong operation", fd);
            return "Unknown buffer operation. Check your input and try again. Type HELP to see the list of supported commands"; // possibly there will be wrap and two packages of sending
        }
    } else if (main == "MATRIX") {
        if (Controller::matrix () == -1) {
            log ("matrix not done: no field", fd);
            return "Field not found";
        } else {
            log ("matrix done", fd);
            return "Field is readonly now. You can analyze it. Type HELP to see what you can do"; //needed wrap
        }
    } else if (main == "WAVE") {
        int search_id;
        ss >> search_id;
        int err = Controller::wave (search_id);
        if (err == 0) {
            log ("wave clustering done", fd);
            return "Field is clustered";
        } else if (err == -1) {
            log ("wave clustering failed: readonly", fd);
            return "Check field state. Type MATRIX to set it to readonly";
        } else if (err == -2) {
            log ("wave clustering failed: no field", fd);
            return "Field doesn't exist. Type GC to create the clouds";
        } else if (err == -3) {
            log ("wave clustering failed: wrong id", fd);
            return "No search with such id. Type INFOCS to see accessible searches";
        }
    } else if (main == "DBSCAN") {
        int k;
        int search_id;
        ss >> search_id >> k;
        show ("Clustering...");
        int err = Controller::dbscan (search_id, k);
        if (err == 0) {
            log ("dbscan clustering done", fd);
            return "Field is clustered";
        } else if (err == -1) {
            log ("wave clustering failed: readonly", fd);
            return "Check field state. Type MATRIX to set it to readonly";
        } else if (err == -2) {
            log ("wave clustering failed: no field", fd);
            return "Field doesn't exist. Type GC to create the clouds";
        } else if (err == -3) {
            log ("wave clustering failed: wrong id", fd);
            return "No search with such id. Type INFOCS to see accessible searches";
        }
    } else if (main == "BINARY") {
        //creating matrix of incidences
        double delta;
        ss >> delta;
        int err = Controller::incidence_matrix (delta);
        if (err == 0) {
            log ("binary matrix created", fd);
            return "Incidence matrix is created. Now you can run clustering";
        } else if (err == -2) {
            log ("binary matrix creating failed: no field", fd);
            return "Field not found";
        }
    } else if (main == "KMEANS") {
        int k;
        ss >> k;
        show ("Clustering...");
        int err = Controller::k_means (k);
        if (err == 0) {
            log ("kmeans clustering done", fd);
            return "Field is clustered";
        } else if (err == -1) {
            log ("kmeans clustering failed: readonly", fd);
            return "Check field state. Type MATRIX to set it to readonly";
        } else if (err == -2) {
            log ("kmeans clustering failed: no field", fd);
            return "Field doesn't exist. Type GC to create the clouds";
        }
    } else if (main == "KMCORES") {
        // K-means with cores
        int clusters_number = 0;
        int cores_number = 0;
        ss >> clusters_number >> cores_number;
        show ("Clustering...");
        int err = Controller::k_means_cores (clusters_number, cores_number);
        if (err == 0) {
            log ("kmcores clustering done", fd);
            return "Field is clustered";
        } else if (err == -1) {
            log ("kmcores clustering failed: readonly", fd);
            return "Check field state. Type MATRIX to set it to readonly";
        } else if (err == -2) {
            log ("kmcores clustering failed: no field", fd);
            return "Field doesn't exist. Type GC to create the clouds";
        }
    } else if (main == "EM") {
        int k;
        ss >> k;
        show ("Clustering...");
        int err = Controller::em (k);
        if (err == 0) {
            log ("em clustering done", fd);
            return "Field is clustered";
        } else if (err == -1) {
            log ("em clustering failed: readonly", fd);
            return "Check field state. Type MATRIX to set it to readonly";
        } else if (err == -2) {
            log ("em clustering failed: no field", fd);
            return "Field doesn't exist. Type GC to create the clouds";
        }
    } else if (main == "HA") {
        show ("Clustering...");
        int err = Controller::hierarchical_algorithm ();
        if (err == 0) {
            log ("ha clustering done", fd);
            return "Field is clustered";
        } else if (err == -1) {
            log ("ha clustering failed: readonly", fd);
            return "Check field state. Type MATRIX to set it to readonly";
        } else if (err == -2) {
            log ("ha clustering failed: no field", fd);
            return "Field doesn't exist. Type GC to create the clouds";
        }
    } else if (main == "FOREL") {
        show ("Clustering...");
        int err = Controller::forel ();
        if (err == 0) {
            log ("forel clustering done", fd);
            return "Field is clustered";
        } else if (err == -1) {
            log ("forel clustering failed: readonly", fd);
            return "Check field state. Type MATRIX to set it to readonly";
        } else if (err == -2) {
            log ("forel clustering failed: no field", fd);
            return "Field doesn't exist. Type GC to create the clouds";
        }
    } else if (main == "DELAUNAY") {
        show ("Clustering...");
        int err = Controller::delaunay ();
        if (err == 0) {
            log ("delaunay triangulation done", fd);
            return "Field is triangulated";
        } else if (err == -1) {
            log ("delaunay triangulation failed: readonly", fd);
            return "Check field state. Type MATRIX to set it to readonly";
        } else if (err == -2) {
            log ("delaunay triangulation failed: no field", fd);
            return "Field doesn't exist. Type GC to create the clouds";
        } else if (err == -3) {
            log ("delaunay triangulation failed: a few points", fd);
            return "Not enough points for triangulation";
        }
    } else if (main == "PREDICT") {
        show ("Predicting...");
        int err;
        double x, y;
        ss >> x >> y;
        double result = Controller::predict (err, x, y);
        if (err == 0) {
            log ("predicted f(" + to_string (x) + "," + to_string (y) + ") to be " + to_string (result), fd);
            return "Predicted: " + to_string (result);
        } else if (err == -1) {
            log ("prediction failed: readonly", fd);
            return "Check field state. Type MATRIX to set it to readonly";
        } else if (err == -2) {
            log ("prediction failed: no field", fd);
            return "Field doesn't exist. Type GC to create the clouds";
        } else if (err == -3) {
            log ("prediction failed: a few points", fd);
            return "Not enough points around for good prediction";
        }
    } else {
        // something went wrong
        log ("wrong operation", fd);
        return "Unknown command. Check your input and try again. Type HELP to see the list of supported commands ("
            + main + ")"; // needed wrap the line
    }
    return "";
}

void Controller::manager () {
    // chooses the next action using infinite loop
    while (working ()) {
        if (!actions.empty ()) {
            string result = do_command (actions.front ().second, actions.front ().first);
            Administrator::add_response (actions.front ().first, result);
            actions.pop ();
        }
        sleep (1); //letting other processes to be correctly finished
        // there should be something to exit the infinite loop
    }
}

bool Controller::working () {
    return working_;
}

int Controller::generate_cloud (double center_x = 0,
                                double center_y = 0,
                                double disp_x = 1,
                                double disp_y = 1,
                                int cloud_size = 1000) {
    // generates cloud with center on the field with dispersions by x and y
    // -1 if readonly
    if (field_ == nullptr) {
        field_ = new Field ();
        log ("Field initialized");
    }
    if (Field::readonly ()) {
        return -1;
    }
    auto *cloud = new Cloud (cloud_size, disp_x, disp_y);
    cloud->shift (center_x, center_y);
    Field::add (cloud);
    return 0;
}

void Controller::log (const string &s) {
    logger << s << endl;
}

void Controller::show (const string &s) {
    // writes s to console and to log file
    cout << s << endl;
    log ("< " + s);
}

void Controller::send_help (int fd) {
    // sends to socket fd help.txt
    ifstream help ("help.txt");
    if (!help.is_open ()) {/*
        show ("Manual file not found");
        log ("$ " + manual);*/
    } else {
        string cur_line;
        // log ("* file print:");
        while (getline (help, cur_line)) {
            /*
            printf ("%s\n", cur_line.c_str ());
            log ("< " + cur_line);*/
            Administrator::add_response (fd, cur_line);
        }
        help.close ();
    }
}

int Controller::matrix () {
    // sets field to readonly, creats matrix of distances
    // if there is no field returns -1
    if (field_ == nullptr) {
        return -1;
    }
    Field::create_dist_matrix ();
    return 0;
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