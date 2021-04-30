#ifndef SERVER__CONTROLLER_H_
#define SERVER__CONTROLLER_H_
#include "Administrator.h"
#include <sstream>
#include <iostream>
class Controller {
  static queue<pair<int, string>> actions; // ordered list of commands which should be done
  static bool working_;
 public:
  Controller ();
  static void add_next (int fd, const string& command);
  static string do_command (const string& command);
  static void manager ();
  static bool working ();
};

#endif //SERVER__CONTROLLER_H_
