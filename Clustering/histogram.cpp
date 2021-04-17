#include "histogram.h"

histogram::histogram (int pieces, vector<int> &x_distr, vector<int> &y_distr) {
    // makes a vector histogram of distribution of dots on the field
    bool started = false;
    double max;
    double min;
    //Working over x
    //searching for range of distribution
    x_distr.clear ();
    y_distr.clear ();
    for (int i = 0; i < Field::length (); ++i) {
        for (int j = 0; j < Field::cloud ()[i]->length (); ++j) {
            if (started) {
                if (Field::cloud ()[i]->point ()[j]->x () > max) {
                    max = Field::cloud ()[i]->point ()[j]->x ();
                } else if (Field::cloud ()[i]->point ()[j]->x () < min) {
                    min = Field::cloud ()[i]->point ()[j]->x ();
                }
            } else {
                min = max = Field::cloud ()[i]->point ()[j]->x ();
                started = true;
            }
        }
    }
    //distribution itself
    double piece_len = (max - min) / pieces;
    vector<int> line (pieces);
    for (int i = 0; i < pieces; ++i) {
        line[i] = 0;
    }
    x_distr = line;
    for (int i = 0; i < Field::length (); ++i) {
        for (int j = 0; j < Field::cloud ()[i]->length (); ++j) {
            for (int k = 0; k < pieces; ++k) {
                if (min + piece_len * (k + 1) > Field::cloud ()[i]->point ()[j]->x ()) {
                    ++x_distr[k];
                    break;
                }
            }
        }
    }
    //same for y
    started = false;
    for (int i = 0; i < Field::length (); ++i) {
        for (int j = 0; j < Field::cloud ()[i]->length (); ++j) {
            if (started) {
                if (Field::cloud ()[i]->point ()[j]->y () > max) {
                    max = Field::cloud ()[i]->point ()[j]->y ();
                } else if (Field::cloud ()[i]->point ()[j]->y () < min) {
                    min = Field::cloud ()[i]->point ()[j]->y ();
                }
            } else {
                min = max = Field::cloud ()[i]->point ()[j]->y ();
                started = true;
            }
        }
    }
    y_distr = line;
    for (int i = 0; i < Field::length (); ++i) {
        for (int j = 0; j < Field::cloud ()[i]->length (); ++j) {
            for (int k = 0; k < pieces; ++k) {
                if (min + piece_len * (k + 1) > Field::cloud ()[i]->point ()[j]->x ()) {
                    ++y_distr[k];
                    break;
                }
            }
        }
    }
}
