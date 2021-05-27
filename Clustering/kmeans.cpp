#include "kmeans.h"

kmeans::kmeans (int clusters_number) : clusters_number (clusters_number) {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!Field::readonly ()) {
        err_ = -1;
        return;
    }
    // auto *a = new Field ();
    // That's temporary object.
    // In the future I'm going to eliminate pointer to Field from the class, but till that time
    // we're supposed to live our lifes with that thing
    // Field::searches_.emplace_back (a);
    // Field::new_search ();
    k_means ();
}

void kmeans::k_means () {
    bool changed = true;
    vector<Point> cores (clusters_number); // Pointers to core points
    vector<int> nearest_cluster (Point::quantity (), 0); // shows which element of cores is the nearest to that point
    // filling core vector by random points
    for (int i = 0; i < cores.size (); ++i) {
        const Point *t = Point::get_by_id (i + 1);
        cores[i] = Point (t->x (), t->y (), 0);
    }
    int iteration = 0;
    while (changed || iteration == 1) {
        // searching the closest point for each field point
        changed = false;
        for (int i = 0; i < Point::quantity (); ++i) {
            for (int j = 0; j < cores.size (); ++j) {
                double a = Point::dist (Point::get_by_id (i + 1), &cores[j]);
                double b = Point::dist (Point::get_by_id (i + 1), &cores[nearest_cluster[i]]);
                if (a < b) {
                    changed = true;
                    nearest_cluster[i] = j;
                }
            }
        }
        // recomputing cores
        vector<int> cluster_length (clusters_number, 0);
        for (auto &core : cores) {
            core = Point ();
        }
        for (int i = 0; i < Point::quantity (); ++i) {
            cores[nearest_cluster[i]].shift (*Point::get_by_id (i + 1));
            cluster_length[nearest_cluster[i]]++;
        }
        for (int i = 0; i < cores.size (); ++i) {
            cores[i] = Point (cores[i].x () / cluster_length[i],
                              cores[i].y () / cluster_length[i],
                              0);
        }
        kmeans_fprintf (nearest_cluster, cores, iteration);
        iteration++;
    }
    //Field::searches_.back ().clusters.clear ();
    vector<vector<int>> clusters_to_set (clusters_number);
    for (int i = 0; i < nearest_cluster.size (); ++i) {
        clusters_to_set[nearest_cluster[i]].push_back (i);
    }
    Cluster_Search cs (clusters_to_set);
    Field::add_search (cs);
    /*
    for (int i = 0; i < clusters_number; ++i) {
        Field::searches_.back ().clusters.emplace_back (clusters_to_set[i]);
    }*/
    err_ = 0;
}

void kmeans::kmeans_fprintf (vector<int> &nearest_cluster, vector<Point> &cores, int iteration) {
    ofstream out ("gnuplot/kmeans/km" + to_string (iteration) + ".txt");
    ofstream final ("gnuplot/kmeans/km_fin.txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        out << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << " " << nearest_cluster[i]
            << endl;
        final << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << " " << nearest_cluster[i]
              << endl;
    }
    for (auto &core : cores) {
        out << core.x () << " " << core.y () << " " << -1 << endl;
        final << core.x () << " " << core.y () << " " << -1 << endl;
    }
}

int kmeans::err () const {
    return err_;
}
