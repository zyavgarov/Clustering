#include "Interface.h"

Interface::Interface (int id, bool write_log, Controller *c)
    : id (id), write_log (write_log), cc (c) {
    if (write_log) {
        log_file_name = "log.txt";
        logger = ofstream (log_file_name);
        if (!logger.is_open ()) {
            cout << "Error while writing logs" << endl;
            this->write_log = false;
        }
    }
}

Interface::~Interface () {
    if (logger.is_open ()) {
        logger.close ();
    }
}

int Interface::run () {
    // the function asks commands from console until gets the command exit
    cout << "Type HELP to see the list of supported commands." << endl;
    string cur_command; // command with parameters
    getline (cin, cur_command);
    while (manager (cur_command) != -1) {
        getline (cin, cur_command);
    }
    return 0;
}

void Interface::log (const string &s) {
    // writes s to log file
    if (write_log) {
        logger << s << endl;
    }
}

void Interface::show (const string &s) {
    // writes s to console and to log file
    cout << s << endl;
    log ("< " + s);
}

int Interface::run (ifstream &inp) {
    /* runs the program from filestream
     * -1 - file not found
     */
    if (!inp.is_open ()) {
        show ("File not found");
        return -1;
    }
    string cur_command_str;
    getline (inp, cur_command_str);
    while (manager (cur_command_str) != -1 && !inp.eof ()) {
        getline (inp, cur_command_str);
    }
    return 0;
}

int Interface::manager (string cur_command) {
    /* gets command for controller and sends it;
     * returns -1 for exit;
     */
    for (auto &c: cur_command) c = toupper (c); // string to upper case
    log ("> " + cur_command);
    stringstream ss (cur_command);
    string main;
    ss >> main;
    if (main == "HELP") {
        // output of manual
        string manual;
        int err = cc->get_help (id, manual);
        if (err != 0) {
            show ("Mistake while getting data");
            log ("$ " + to_string (err));
        } else {
            ifstream help (manual);
            if (!help.is_open ()) {
                show ("Manual file not found");
                log ("$ " + manual);
            } else {
                string cur_line;
                log ("* file print:");
                while (getline (help, cur_line)) {
                    printf ("%s\n", cur_line.c_str ());
                    log ("< " + cur_line);
                }
                help.close ();
            }
        }
    } else if (main == "GC") {
        // generation of cloud(s)
        double x, y, disp_x, disp_y;
        int dots;
        ss >> x >> y >> disp_x >> disp_y >> dots;
        cc->generate_cloud (id, x, y, disp_x, disp_y, dots);
        show ("Cloud created");
    } else if (main == "LOG") {
        string b;
        ss >> b;
        if (b == "1") {
            write_log = true;
            show ("Writing logs started");
        } else if (b == "0") {
            write_log = false;
            show ("Writing logs stopped");
        } else if (write_log) {
            show ("Logs are being written");
        } else {
            show ("Logs are not being written");
        }
    } else if (main == "PRINTF") {
        string file_name;
        ss >> file_name;
        int err = cc->fprintf (file_name);
        if (err == -1) {
            show ("Mistake while opening file");
            log ("$ " + file_name);
        }
    } else if (main == "HISTOGRAM") {
        int pieces;
        ss >> pieces;
        vector<int> x, y;
        cc->histogram (pieces, x, y);
        ofstream out ("histogram_x.txt");
        for (int i = 0; i < pieces; ++i) {
            out << x[i] << endl;
        }
        out.close ();
        out.open ("histogram_y.txt");
        for (int i = 0; i < pieces; ++i) {
            out << y[i] << endl;
        }
        out.close ();
        show ("Histogram saved to histogram_x.txt and histogram_y.txt");
    } else if (main == "EXIT") {
        show("Finishing...");
        return -1;
    } else if (main == "BUFFER") {
        char l;
        ss >> l;
        if (l == 'u'){
            cc->buffer_unload();
            show("Buffer loaded to field");
        } else if (l == 'c'){
            int cloud_id;
            ss >> cloud_id;
            cc->buffer_add_cloud(cloud_id);
            show("Added cloud " + to_string(cloud_id) + " to buffer");
        }
    } else {
        // something went wrong
        show ("Unknown command. Check your input and try again.");
        show ("Type HELP to see the list of supported commands");
    }
    return 0;
}