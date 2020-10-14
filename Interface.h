#ifndef INTERFACE4__INTERFACE_H_
#define INTERFACE4__INTERFACE_H_
#include <sstream>
#include "Controller.h"

class Interface {
 public:
  int id;
  bool write_log;
  Interface (int id, bool write_log, Controller *c);
  ~Interface ();
  int run ();
  void log (const string &s);
  void show (const string &s);
  int run (ifstream &inp);
 private:
  int manager (string cur_command);
  Controller *cc;
  ofstream logger;
  string log_file_name;
  /* HOW DOES THE INTERFACE WRITE LOG
   *
   * @ - registration data
   * > - user input
   * < - program output
   * $ - data from server
   * * - notes
   */
};

#endif //INTERFACE4__INTERFACE_H_
