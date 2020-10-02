//
// Created by pierre on 01.10.2020.
//

#ifndef INTERFACE4__CLUSTER_SEARCH_H_
#define INTERFACE4__CLUSTER_SEARCH_H_
#include "Field.h"

class Cluster_Search {
 public: // initiate the search
  int delta;// max distance between points considered to have an edge
  int cluster_len; // number of clusters after search
  vector<vector<bool>> edges; //if points with id i and j are connected edge_matrix[i][j] == true
  static vector<vector<double>> dist; // matrix of distances
 private:
  class Cluster {
   public:
    vector<Point> points;
   private:
  };
};

#endif //INTERFACE4__CLUSTER_SEARCH_H_
