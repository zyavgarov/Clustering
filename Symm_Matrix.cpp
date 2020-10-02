#include "Symm_Matrix.h"

Symm_Matrix::Symm_Matrix (int order) {
    for (int i = 0; i < order; ++i) {
        lines.emplace_back (order - i);
    }
}

Symm_Matrix::Line Symm_Matrix::operator[] (int index) {
    int i = 0;
    vector<double *> res;
    for (; i < index; ++i) {
        res.push_back (&lines[i][index]);
    }
    for (; i < lines.size (); ++i) {
        res.push_back (&lines[index][i]);
    }
    return Line (res);
}

Symm_Matrix::Line::Line (vector<double *> vec) : line (vec) {
}

double &Symm_Matrix::Line::operator[] (int index) {
    return *line[index];
}
