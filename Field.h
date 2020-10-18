#ifndef INTERFACE4__FIELD_H_
#define INTERFACE4__FIELD_H_
#include "Buffer.h"
#include "Cluster_Search.h"

class Field {
 public:
  friend class Buffer;
  
  Field ();
  ~Field ();
  int add (Cloud *addition);
  int fprintf (ofstream &out) const;
  int length () const;
  Buffer buf;
  const vector<vector<double>> &dist () const;
  const vector<Cloud *> & cloud ();
  bool readonly () const;
  void create_dist_matrix ();
  const vector<Cluster_Search> &searches () const;
  void create_edges_matrix(double delta);
  int wave_clustering();
  int db_clustering (int k);
 private:
  Field &operator= (Field const &f);
  Field (Field const &f);
  vector<Cloud *> cloud_;
  vector<vector<double>> dist_; // matrix of distances
  vector<Cluster_Search> searches_;
  bool readonly_{};
};

#endif //INTERFACE4__FIELD_H_
