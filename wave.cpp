#include "wave.h"

wave::wave (int search_id) : search_id (search_id) {
    // if not readonly state returns -1
    // if readonly - clustering
    /* Errors
     * -1 field is not in readonly mode
     * -3 There is no such Cluster_Search
     */
    if (!Field::readonly ()) {
        err_ = -1;
        return;
    }
    if (search_id >= Field::searches ().size ()) {
        err_ = -3;
        return;
    }
    vector<bool> burnt (Point::quantity (), false);
    // that cycle checks if point in marked cluster and if not creates new one
    for (int m = 0; m < Point::quantity (); ++m) {
        if (burnt[m]) {
            continue;
        }
        vector<int> curr_wave = {m};
        burnt[m] = true;
        vector<int> next_wave; // points, connected to smth from curr_wave
        vector<int> curr_cluster; // points from current cluster
        // that cycle searches for neighbours of points in curr_wave
        while (!curr_wave.empty ()) {
            for (int i : curr_wave) {
                for (int j = m + 1; j < Point::quantity (); ++j) {
                    if (Field::searches ()[search_id].edges ()[i][j] && !burnt[j]) {
                        next_wave.push_back (j);
                    }
                }
            }
            curr_cluster.insert (curr_cluster.end (), curr_wave.begin (), curr_wave.end ());
            curr_wave = next_wave;
            for (auto &&i: next_wave) {
                burnt[i] = true;
            }
            next_wave.clear ();
        }
        Field::searches_[search_id].add (Cluster_Search::Cluster (curr_cluster));
    }
    ofstream output("gnuplot/wave/wave.txt");
    Field::fprintf (output);
    err_ = 0;
}

int wave::err () const {
    return err_;
}
