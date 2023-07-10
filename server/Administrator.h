// Created by Zyavgarov Rustam
#ifndef SERVER__ADMINISTRATOR_H_
#define SERVER__ADMINISTRATOR_H_
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue>
#include <string>
#include <thread>
using namespace std;
#define PORT    5555
#define BUFLEN  512

/* That class is about connecting the clients.
 * It has nothing to do with the idea of commends, just getting them and passing to Controller
 * Administrator also creates the line of clients commands to do
 */

class Administrator {
  static int sock;
  static queue<pair<int, string>>
      responses; // ordered list of responses to clients. first is socket, second is the response itself
  static fd_set active_set;
 public:
  static int prepare ();
  static int run ();
  static bool get_from_client (int fd, char buf[512]);
  static bool send_to_client (int fd, char buf[512]);
  static void disconnect (int fd);
  static void answer ();
  static void add_response (int fd, const string &result);
};

#endif //SERVER__ADMINISTRATOR_H_
