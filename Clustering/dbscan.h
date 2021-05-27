#ifndef INTERFACE4__DBSCAN_H_
#define INTERFACE4__DBSCAN_H_
#include "../Field.h"
// Created by Zyavgarov Rustam

class dbscan {
  int search_id;
  int density;
  int err_;
  vector<int> db_sorting () const;
  void db_clustering (const vector<int> &state) const;
  static void dbscan_fprintf_type (const vector<int> &state);
  dbscan (dbscan const &);
 public:
  int err () const;
  dbscan (int search_id, int density);
};

#endif //INTERFACE4__DBSCAN_H_
