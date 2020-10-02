#ifndef INTERFACE4__SYMM_MATRIX_H_
#define INTERFACE4__SYMM_MATRIX_H_

#include <vector>
using namespace std;

/* My idea is to keep the matrix like that:
 * . . . .
 * . . .
 * . .
 * .
 */

class Symm_Matrix {
 public:
  class Line {
   public:
    explicit Line(vector<double*> vec);
    double &operator[] (int index);
    vector<double *> line;
  };
  
  Symm_Matrix (int order);
  Line operator[] (int index);
 private:
  vector<vector<double>> lines;
};

#endif //INTERFACE4__SYMM_MATRIX_H_