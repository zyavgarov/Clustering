// Created by Zyavgarov Rustam
#include "Sender.h"
int Sender::sock;
bool Sender::write_log;
ofstream Sender::logger;
bool Sender::working_ = true;

Sender::Sender (bool wl) {
    sock = -1;
    write_log = wl;
    if (wl) {
        logger = ofstream ("log.txt");
        if (!logger.is_open ()) {
            cout << "Error while writing logs" << endl; // should be returned to Interface
            write_log = false;
        }
    }
}

int Sender::connect_server () {
    /* Error codes:
     * -1 - Unknown host
     * -2 - Socket was not created
     * -3 - connect_server failure
     */
    // connection to server
    // that part needs deeper analysis for mistakes
    int err;
    struct sockaddr_in server_addr{};
    struct hostent *hostinfo;
    
    // Getting info about server by its DNS address
    hostinfo = gethostbyname (SERVER_NAME);
    if (hostinfo == nullptr) {
        return -1;
    }
    
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons (SERVER_PORT);
    server_addr.sin_addr = *(struct in_addr *) hostinfo->h_addr;
    
    // Creating TCP socket
    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror ("Client: socket was not created");
        return -2;
    }
    
    // Connect to the server
    err = connect (sock, (struct sockaddr *) &server_addr, sizeof (server_addr));
    if (err < 0) {
        perror ("Client:  connect_server failure");
        return -3;
    }
    log ("Connected");
    cout << "Connection is ready" << endl;
    return 0;
}

void Sender::log (const string &s) {
    if (write_log) {
        logger << s << endl;
    }
}

Sender::~Sender () {

}

int Sender::disconnect_server () {
    // Closing the socket
    close (sock);
    exit (EXIT_SUCCESS);
}

int Sender::send_to_server () {
    // Sends string s to the server
    /* Error codes
     */
    while (working ()) {
        string s;
        int nbytes;
        char buf[BUFLEN];
        cout << "Send to server > ";
        getline (cin, s);
        stringstream str;
        str << s;
        /*
        if (fgets (buf, BUFLEN, stdin) == nullptr) {
            printf ("error\n");
        }*/
        buf[strlen (buf) - 1] = 0;
        strcpy (buf, s.c_str ());
        nbytes = write (sock, buf, strlen (buf) + 1);
        if (nbytes < 0) {
            perror ("write");
            break;
        }
        log ("sent: " + string (buf));
        if (strstr (buf, "stop")) return -1;
    }
    return 0;
}

void Sender::get_from_server () {
    /* gets server answers*/
    /* Reserved error codes:
     * -1 - error
     */
    while (working ()) {
        int nbytes;
        char buf[BUFLEN]; // error code as char
        nbytes = read (sock, buf, BUFLEN);
        if (nbytes < 0) {
            perror ("read");
            break;
        } else if (nbytes == 0) {
            cerr << "Empty message" << endl;
        } else {
            //there should be things if read success
            manage_answer (string (buf));
        }
    }
}

void Sender::run () {
    // Exchanging data
    thread sender (send_to_server);
    thread getter (get_from_server);
    sender.join ();
    getter.join ();
}

void Sender::manage_answer (const string &answer) {
    log ("got: " + answer);
    cout << answer << endl; // to be done smth better and more valuable
}

bool Sender::working () {
    return working_;
}
