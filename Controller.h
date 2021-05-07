#ifndef SERVER__CONTROLLER_H_
#define SERVER__CONTROLLER_H_
#include "Administrator.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include "Field.h"
class Controller {
  static queue<pair<int, string>> actions; // ordered list of commands which should be done
  static bool working_;
  static ofstream logger;
  static void log (const string &s);
  static Field *field_;
 public:
  Controller ();
  static void add_next (int fd, const string& command);
  static string do_command (const string &command, int fd);
  static void manager ();
  static bool working ();
  static int generate_cloud (double center_x, double center_y, double disp_x, double disp_y, int cloud_size);
  static void show (const string &s);
  static void send_help (int fd);
  static int buffer_unload ();
  static int buffer_add_cloud (int id);
  static int buffer_shift (double x, double y);
  static int buffer_zoom (double k);
  static int buffer_mirror ();
  static int buffer_rotate (double angle);
  static int buffer_erase ();
};

#endif //SERVER__CONTROLLER_H_
