#include "Controller.h"
bool Controller::working_ = false;
queue<pair<int, string>> Controller::actions;

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
    thread manage_th(manager);
    manage_th.detach();
    err = Administrator::run ();
}

void Controller::add_next (int fd, const string &command) {
    // adds command to queue
    actions.push (pair<int, string> (fd, command));
}

string Controller::do_command (const string &command) {
    // figures out what to do with client's command
    stringstream ss (command);
    string main;
    ss >> main;
    for (auto &c: main) c = toupper (c);
    if (main == "END") {
        working_ = false;
    } else {
        return "answer"; //temporary thing just for test
    }
    return "";
}

void Controller::manager () {
    // chooses the next action using infinite loop
    while (working ()) {
        if (!actions.empty ()) {
            string result = do_command (actions.front ().second);
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
