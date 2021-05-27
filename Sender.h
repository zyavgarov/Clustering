#ifndef CLIENT__SENDER_H_
#define CLIENT__SENDER_H_
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sstream>
#include <thread>
#define  SERVER_PORT     5555
#define  SERVER_NAME    "127.0.0.1"
#define  BUFLEN          512
using namespace std;

class Sender {
 private:
  static ofstream logger;
  static bool write_log;
  static int sock;
  static bool working_;
 public:
  explicit Sender (bool wl);
  ~Sender ();
  static void log (const string &s);
  static int connect_server ();
  static int disconnect_server ();
  static int send_to_server ();
  static void run ();
  static void get_from_server ();
  static void manage_answer (const string &answer);
  static bool working ();
};

#endif //CLIENT__SENDER_H_
