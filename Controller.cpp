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
    working_ = true;
    thread manage_th (manager);
    manage_th.detach ();
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
        return "Cloud created";
    } else if (main == "HELP") {
        thread help_writer (send_help, fd);
        help_writer.join ();
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
