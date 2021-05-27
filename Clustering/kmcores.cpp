// Created by Zyavgarov Rustam

#include "kmcores.h"

int kmcores::err () {
    return err_;
}

kmcores::kmcores (int clusters_number, int cores_number)
    : clusters_number (clusters_number), cores_number (cores_number) {
    
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!Field::readonly ()) {
        err_ = -1;
        return;
    }
    // auto * f =  new Field;
    // Field::searches_.emplace_back (f);
    k_means_cores ();
    err_ = 0;
}

void kmcores::k_means_cores () const {
    /* realises k-means withcores clustering
     * clusters_number shows how many clusters we'll get
     * cores_number shows how many cores each cluster contains
     */
    vector<int> nearest_cluster (Point::quantity (), 0);
    // shows which cluster's core is the nearest to that point.
    // values are from 0 to clusters_number-1
    vector<int> nearest_core (Point::quantity (), 0);
    // shows which core in cluster is the closest.
    // Values are from 0 to cores_number-1
    vector<vector<Point>> cores (clusters_number, vector<Point> (cores_number)); // Cores of clusters itself
    // filling core vector by random points
    for (int i = 0; i < cores.size (); ++i) {
        for (int j = 0; j < cores[i].size (); ++j) {
            const Point *t = Point::get_by_id (i * cores.size () + j + 1);
            cores[i][j] = Point (t->x (), t->y (), 0);
        }
    }
    // iterations of k-means
    int iteration = 0;
    bool changed = true;
    while (changed) {
        // searching the closest core point for each field point
        changed = false;
        for (int i = 0; i < Point::quantity (); ++i) {
            for (int j = 0; j < cores.size (); ++j) {
                for (int l = 0; l < cores[j].size (); ++l) {
                    double a = Point::dist (Point::get_by_id (i + 1), &cores[j][l]);
                    double b = Point::dist (Point::get_by_id (i + 1), &cores[nearest_cluster[i]][nearest_core[i]]);
                    if (a < b) {
                        changed = true;
                        nearest_cluster[i] = j;
                        nearest_core[i] = l;
                    }
                }
            }
        }
        // recomputing cores
        // vector<vector<int>> cluster_length_ (clusters_number, vector<int> (cores_number, 0));
        for (auto &cluster : cores) {
            for (auto &core : cluster) {
                core = Point ();
            }
        }
        // here we just collect points we need before the action
        // the first step is running kmeans for each cluster
        for (int i = 0; i < cores.size (); i++) {
            auto &cluster = cores[i];
            // searching which points current cluster contains
            vector<int> curr_points;
            // throws double free or corruption if number of clusters is not equal to number of cores.
            // maybe i'm messing up that number while counting length of vectors
            // another hypotheses says that something wrong with Point::shift function
            for (int p = 0; p < Point::quantity (); ++p) {
                if (nearest_cluster[p] == i) {
                    curr_points.push_back (p);
                }
            }
            for (int j = 0; j < cluster.size (); ++j) {
                const Point *t = Point::get_by_id (curr_points[j] + 1);
                cluster[j] = Point (t->x (), t->y (), 0);
            }
            bool changed_in = true;
            int iteration_in = 0;
            while (changed_in) {
                changed_in = false;
                for (int curr_point : curr_points) {
                    for (int j = 0; j < cluster.size (); ++j) {
                        double a = Point::dist (Point::get_by_id (curr_point + 1), &cluster[j]);
                        double b = Point::dist (Point::get_by_id (curr_point + 1), &cluster[nearest_core[curr_point]]);
                        if (a < b) {
                            changed_in = true;
                            nearest_core[curr_point] = j;
                        }
                    }
                }
                // recomputing cores
                vector<int> subcluster_length (cores_number, 0);
                for (auto &l : cluster) {
                    l = Point ();
                }
                for (int curr_point : curr_points) {
                    cluster[nearest_core[curr_point]].shift (*Point::get_by_id (curr_point + 1));
                    subcluster_length[nearest_core[curr_point]]++;
                }
                for (int l = 0; l < cluster.size (); ++l) {
                    cluster[l] = Point (cluster[l].x () / subcluster_length[l],
                                        cluster[l].y () / subcluster_length[l],
                                        0);
                }
                iteration_in++;
            }
        }
        kmeans_core_fprintf (nearest_cluster, cores, iteration);
        iteration++;
    }
    //Field::searches_.back().clusters.clear ();
    vector<vector<int>> clusters_to_set (clusters_number);
    for (int i = 0; i < nearest_cluster.size (); ++i) {
        clusters_to_set[nearest_cluster[i]].push_back (i);
    }
    Cluster_Search cs (clusters_to_set);
    Field::add_search (cs);
    // for (int i = 0; i < clusters_number; ++i) {
//         Field::searches_.back().clusters.emplace_back (clusters_to_set[i]);
    // }
}

void kmcores::kmeans_core_fprintf (const vector<int> &nearest_cluster,
                                   const vector<vector<Point>> &cores,
                                   int iteration) {
    ofstream out ("gnuplot/kmcores/km" + to_string (iteration) + ".txt");
    ofstream final ("gnuplot/kmcores/km_final.txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        out << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << " " << nearest_cluster[i]
            << endl;
        final << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << " " << nearest_cluster[i]
              << endl;
    }
    for (auto &cluster : cores) {
        for (auto &core:cluster) {
            out << core.x () << " " << core.y () << " " << -1 << endl;
            final << core.x () << " " << core.y () << " " << -1 << endl;
        }
    }
}
