// Created by Zyavgarov Rustam
#include "Interface.h"
int Interface::id_ = time (nullptr) * random ();

Interface::Interface () = default;

int Interface::id () {
    return id_;
}

int Interface::run () {
    // the function asks commands from console until gets the command exit
    /*cout << "Type HELP to see the list of supported commands." << endl;
    string cur_command; // command with parameters
    getline (cin, cur_command);
    while (manager (cur_command) != -1) {
        getline (cin, cur_command);
    }*/
    // I suggest here to run Sender loop
    int err = Sender::connect_server (); // needed better error catch
    if (err == -1) {
        cout << "Unknown host " << SERVER_NAME << endl;
    }
    Sender::run ();
    Sender::disconnect_server ();
    return 0;
}

void Interface::log (const string &s) {
    // writes s to log file
    Sender::log (s);
}

void Interface::show (const string &s) {
    // writes s to console and to log file
    cout << s << endl;
    log ("< " + s);
}
