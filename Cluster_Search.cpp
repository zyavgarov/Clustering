#include "Cluster_Search.h"
#include "Field.h"

Cluster_Search::Cluster::Cluster (vector<const Point *> vec) : points (vec) {
}

Cluster_Search::Cluster::Cluster (const vector<int> &vec) {
    vector<const Point *> a;
    a.reserve (vec.size ());
    for (int i : vec) {
        a.push_back (Point::get_by_id (i + 1));
    }
    points = a;
}

const vector<vector<bool>> &Cluster_Search::edges () const {
    return edges_;
}

void Cluster_Search::add (const Cluster_Search::Cluster &addition) {
    clusters.push_back (addition);
}

Cluster_Search::Cluster_Search (Field *field, double delta, int k) : field_ (field), delta (delta), k (k) {
    if (delta != 0) {
        // a little defence for kmeans
        create_edges_matrix ();
    }
}

void Cluster_Search::create_edges_matrix () {
    // matrix of incidences
    if (edges_.empty ()) {
        vector<vector<bool>> incidence (Point::quantity (), vector<bool> (Point::quantity (), false));
        for (int i = 0; i < Point::quantity (); ++i) {
            for (int j = i + 1; j < Point::quantity (); ++j) {
                incidence[j][i] = incidence[i][j] = (field_->dist ()[i][j] < delta);
            }
        }
        edges_ = incidence;
    }
}

void Cluster_Search::wave_clustering () {
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
                    if (edges ()[i][j] && !burnt[j]) {
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
        add (Cluster_Search::Cluster (curr_cluster));
    }
}

vector<int> Cluster_Search::db_sorting (int density) {
    // counts number of neighbours for all the points
    k = density;
    vector<int> type (Point::quantity (), 0);
    for (int i = 0; i < Point::quantity (); ++i) {
        vector<int> neighbours;
        for (int j = 0; j < Point::quantity (); ++j) {
            if (edges ()[i][j]) {
                neighbours.push_back (j);
            }
        }
        if (neighbours.size () >= k) {
            type[i] = 2;
            for (int neighbour : neighbours) {
                if (type[neighbour] != 2) {
                    type[neighbour] = 1;
                }
            }
        }
    }
    return type;
}

void Cluster_Search::db_clustering (const vector<int> &state) {
    // picks up core points and finds its cluster
    // saves info about the states of points to file
    // result of clustering saved to file too
    vector<bool> burnt (Point::quantity (), false);
    for (int l = 0; l < state.size (); ++l) {
        if (burnt[l]) {
            continue;
        }
        vector<int> curr_cluster;
        if (state[l] == 2) {
            vector<int> curr_added;
            curr_added.push_back (l);
            vector<int> new_added;
            while (!curr_added.empty ()) {
                for (int i : curr_added) {
                    for (int j = 0; j < Point::quantity (); ++j) {
                        if (!burnt[j] && i != j && edges ()[i][j]) {
                            new_added.push_back (j);
                            burnt[j] = true;
                        }
                    }
                }
                curr_cluster.insert (curr_cluster.end (), new_added.begin (), new_added.end ());
                curr_added = new_added;
                new_added.clear ();
            }
            clusters.emplace_back (curr_cluster);
        } else if (state[l] == 0) {
            // Doesn't seem to be true.
            // That point can be connected to core point which has id number more than ours
            clusters.emplace_back (vector<int> (1, l));
            burnt[l] = true;
        }
    }
    dbscan_fprintf_type (state);
    //fprintf_clusters();
}

Cluster_Search Cluster_Search::wave () {
    wave_clustering ();
    return *this;
}

Cluster_Search Cluster_Search::dbscan (int density) {
    db_clustering (db_sorting (density));
    return *this;
}

Cluster_Search::Cluster_Search (const Cluster_Search &cs)
    : delta (cs.delta), k (cs.k), field_ (cs.field_), clusters (cs.clusters), edges_ (cs.edges_) {
}

Cluster_Search &Cluster_Search::operator= (const Cluster_Search &cs) {
    if (this != &cs) {
        delta = cs.delta;
        k = cs.k;
        field_ = cs.field_;
        clusters = cs.clusters;
        edges_ = cs.edges_;
    }
    return *this;
}

void Cluster_Search::dbscan_fprintf_type (const vector<int> &state) {
    ofstream core ("core.txt");
    ofstream peripherical ("peripherical.txt");
    for (int i = 0; i < state.size (); ++i) {
        if (state[i] == 2) {
            core << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
        } else if (state[i] == 1) {
            peripherical << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << endl;
        }
    }
}

Cluster_Search Cluster_Search::k_means (int clusters_number) {
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
    clusters.clear ();
    vector<vector<int>> clusters_to_set (clusters_number);
    for (int i = 0; i < nearest_cluster.size (); ++i) {
        clusters_to_set[nearest_cluster[i]].push_back (i);
    }
    for (int i = 0; i < clusters_number; ++i) {
        clusters.emplace_back (clusters_to_set[i]);
    }
    return *this;
}

void Cluster_Search::kmeans_fprintf (vector<int> &nearest_cluster, vector<Point> &cores, int iteration) {
    ofstream out ("kmeans/km" + to_string (iteration) + ".txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        out << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << " " << nearest_cluster[i]
            << endl;
    }
    for (auto &core : cores) {
        out << core.x () << " " << core.y () << " " << -1
            << endl;
    }
}

Cluster_Search Cluster_Search::k_means_cores (int clusters_number, int cores_number) {
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
    clusters.clear ();
    vector<vector<int>> clusters_to_set (clusters_number);
    for (int i = 0; i < nearest_cluster.size (); ++i) {
        clusters_to_set[nearest_cluster[i]].push_back (i);
    }
    for (int i = 0; i < clusters_number; ++i) {
        clusters.emplace_back (clusters_to_set[i]);
    }
    return *this;
}

void Cluster_Search::kmeans_core_fprintf (const vector<int> &nearest_cluster,
                                          const vector<vector<Point>> &cores,
                                          int iteration) {
    ofstream out ("kmeans/km" + to_string (iteration) + ".txt");
    for (int i = 0; i < Point::quantity (); ++i) {
        out << Point::get_by_id (i + 1)->x () << " " << Point::get_by_id (i + 1)->y () << " " << nearest_cluster[i]
            << endl;
    }
    for (auto &cluster : cores) {
        for (auto &core:cluster) {
            out << core.x () << " " << core.y () << " " << -1 << endl;
        }
    }
}

Cluster_Search Cluster_Search::em (int clusters_number) {
    
    /* realises EM-clustering algorithm
     */
    
    bool changed = true;
    vector<Point> means (clusters_number); // centers of Gauss distributions
    for (auto &mean : means) {
        mean = Point ((double) (rand ()) / RAND_MAX - 0.5, (double) (rand ()) / RAND_MAX - 0.5, 0);
    }
    
    vector<vector<double>>
        posteriors (Point::quantity (), vector<double> (clusters_number, (double) 1 / clusters_number));
    // point with id i+1 has probability posteriors[i][j] that it is contained to cluster number j
    // e.g. P(c | x_i)
    
    vector<vector<double>> another_probabilty_vector (Point::quantity (),
                                                      vector<double> (clusters_number,
                                                                      (double) 1 / Point::quantity ()));
    // P(x_i | c)
    
    
    vector<vector<vector<double>>>
        covariants (clusters_number, vector<vector<double>> (2, vector<double> (2, 0)));
    // covariances matrices, the first number shows to which cluster it's related, the others are for points
    for (auto &cov_matrix : covariants) {
        for (int i = 0; i < 2; ++i) {
            cov_matrix[i][i] = 1;
        }
    }
    
    vector<double> priors (clusters_number);
    // vector of P(c)
    for (int i = 0; i < priors.size (); ++i) {
        priors[i] = em_get_prior (i, posteriors);
    }
    
    int iteration = 0;
    while (changed && iteration < 20) {
        em_fprintf (posteriors, iteration, covariants, means);
        changed = false;
        
        // updating P(x_i | c)
        for (int p = 0; p < another_probabilty_vector.size (); ++p) {
            for (int c = 0; c < another_probabilty_vector[c].size (); ++c) {
                another_probabilty_vector[p][c] = em_get_probability_cluster_similarity (p, c, means, covariants[c]);
            }
        }
        
        // recompute posteriors
        for (int p = 0; p < Point::quantity (); ++p) {
            for (int i = 0; i < posteriors[p].size (); ++i) {
                if (isnan (em_get_posterior (i, p, means, priors, covariants, another_probabilty_vector))) {
                    double r = 0;
                }
                double t = em_get_posterior (i, p, means, priors, covariants, another_probabilty_vector);
                if (abs (t - posteriors[p][i]) > EM_LIMITATION) {
                    changed = true;
                }
                posteriors[p][i] = t;
            }
        }
        
        // searching weights
        vector<vector<double>> weights (Point::quantity (), vector<double> (clusters_number, 0));
        /*for (int p = 0; p < weights.size (); ++p) {
            for (int i = 0; i < weights[p].size (); ++i) {
                double sum = 0;
                for (int j = 0; j < Point::quantity (); ++j) {
                    sum += posteriors[p][j];
                }
                weights[p][i] = posteriors[p][i] / sum;
            }
        }*/
        for (int c = 0; c < clusters_number; ++c) {
            for (int i = 0; i < Point::quantity (); ++i) {
                double sum = 0;
                for (int j = 0; j < Point::quantity (); ++j) {
                    sum += posteriors[j][c];
                }
                weights[i][c] = posteriors[i][c] / sum;
            }
        }
        
        //recomputing means
        for (int i = 0; i < clusters_number; ++i) {
            double sum_x = 0;
            double sum_y = 0;
            for (int p = 0; p < Point::quantity (); ++p) {
                sum_x += Point::get_by_id (p + 1)->x () * weights[p][i];
                sum_y += Point::get_by_id (p + 1)->y () * weights[p][i];
            }
            means[i] = Point (sum_x, sum_y, 0);
        }
        
        // recomputing covariance matrix
        for (int c = 0; c < covariants.size (); ++c) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    double sum = 0;
                    for (int p = 0; p < Point::quantity (); ++p) {
                        sum += weights[p][c] * pow (Point::get_by_id (p + 1)->x () - means[c].x (), 1 - i)
                            * pow (Point::get_by_id (p + 1)->x () - means[c].x (), 1 - j)
                            * pow (Point::get_by_id (p + 1)->y () - means[c].y (), i)
                            * pow (Point::get_by_id (p + 1)->y () - means[c].y (), j);
                    }
                    covariants[c][i][j] = sum;
                }
            }
        }
        iteration++;
    }
    clusters.clear ();
    vector<vector<int>> clusters_to_set (clusters_number);
    for (int p = 0; p < Point::quantity (); ++p) {
        int max_i = 0;
        for (int i = 1; i < clusters_number; ++i) {
            if (posteriors[p][i] > posteriors[p][max_i]) {
                max_i = i;
            }
        }
        clusters_to_set[max_i].push_back (p);
    }
    for (int i = 0; i < clusters_number; ++i) {
        clusters.emplace_back (clusters_to_set[i]);
    }
    return *this;
}

double Cluster_Search::em_get_probability_cluster_similarity (int point,
                                                              int cluster,
                                                              const vector<Point> &means,
                                                              const vector<vector<double>> &matrix) {
    /* function computes the probability that some point in cluster is that specific point from parameters
     * In other words we return P(x_i | c)
     */
    auto *p = Point::get_by_id (point + 1);
    // using formula of inverted matrix 2*2
    vector<vector<double>> inverted_matrix (2, vector<double> (2));
    double det_m = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            inverted_matrix[i][j] = matrix[1 - i][1 - j] / det_m;
        }
    }
    // we have x^T * S^(-1) * x. We are doing that step-by-step
    // let x^T * S^(-1) = (r_x, r_y), so:
    /*double r_x = inverted_matrix[0][0] * (p->x () - means[cluster].x ())
        + inverted_matrix[1][0] * (p->y () - means[cluster].y ());
    double r_y = inverted_matrix[0][1] * (p->x () - means[cluster].x ())
        + inverted_matrix[1][1] * (p->y () - means[cluster].y ());
    // searching q = r * x
    double q = r_x * (p->x () - means[cluster].x ()) + r_y * (p->y () - means[cluster].y ());*/
    double q = 0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            q += (Point::get_by_id (point + 1)->x () - means[cluster].x ()) * inverted_matrix[i][j]
                * (Point::get_by_id (point + 1)->y () - means[cluster].y ());
        }
    }
    return 1 / sqrt (2 * M_PI * det_m) * exp (-0.5 * q);
}

double Cluster_Search::em_get_prior (int cluster, const vector<vector<double>> &posteriors) {
    /*the function returns the probability of being random point in cluster
     * e.g. P(c)
     */
    double sum = 0;
    for (int i = 0; i < Point::quantity (); ++i) {
        sum += posteriors[i][cluster]; // check if correct
    }
    return sum / Point::quantity ();
}

double Cluster_Search::em_get_posterior (int cluster,
                                         int point,
                                         const vector<Point> &means,
                                         const vector<double> &priors,
                                         const vector<vector<vector<double>>> &covariants,
                                         const vector<vector<double>> &another_vector_of_probabilities) {
    /* the function returns the probability of being selected point in cluster
     * e.g. P(c | x_i)
     */
    
    // computing denominator of fraction
    double denominator_of_fraction = 0;
    for (int c = 0; c < means.size (); ++c) {
        denominator_of_fraction += another_vector_of_probabilities[point][c] * priors[c];
    }
    return another_vector_of_probabilities[point][cluster] * priors[cluster] / denominator_of_fraction;
}

void Cluster_Search::em_fprintf (vector<vector<double>> &posteriors,
                                 int iteration,
                                 vector<vector<vector<double>>> &cov_matrix, vector<Point> &means) {
    ofstream out ("em/em" + to_string (iteration) + ".txt");
    vector<vector<int>> clusters (means.size ());
    for (int p = 0; p < Point::quantity (); ++p) {
        int max_i = 0;
        for (int i = 1; i < posteriors[0].size (); ++i) {
            if (posteriors[p][i] > posteriors[p][max_i]) {
                max_i = i;
            }
        }
        clusters[max_i].push_back (p);
        out << Point::get_by_id (p + 1)->x () << " " << Point::get_by_id (p + 1)->y () << " " << max_i << endl;
    }
    vector<vector<double>> ellipses (means.size (), vector<double> (3));
    // here's e_1, e_2 and angle
    double l_1, l_2, angle;
    double min_dist;
    double max_dist;
    for (int i = 0; i < clusters.size (); ++i) {
        if (clusters[i].empty ()) {
            continue;
        }
        min_dist = Point::dist (Point::get_by_id (clusters[i][0] + 1), Point::get_by_id (clusters[i][1] + 1));
        int min_p = 0;
        int min_q = 0;
        max_dist = Point::dist (Point::get_by_id (clusters[i][0] + 1), Point::get_by_id (clusters[i][1] + 1));
        //possible sigegv while cluster has only one element
        int max_p = 0;
        int max_q = 0;
        for (int p = 0; p < clusters[i].size (); ++p) {
            for (int q = p + 1; q < clusters[i].size (); ++q) {
                double distance = Point::dist (Point::get_by_id (clusters[i][p] + 1),
                                               Point::get_by_id (clusters[i][q] + 1));
                if (distance == 0) {
                    double t = 0;
                }
                if (distance < min_dist) {
                    min_p = p;
                    min_q = q;
                    min_dist = distance;
                } else if (distance > max_dist) {
                    max_p = p;
                    max_q = q;
                    max_dist = distance;
                }
            }
        }
    }
    for (auto &mean : means) {
        out << mean.x () << " " << mean.y () << " -1" << endl;
    }
    // printing ellipses
    ofstream ellipse ("em/ellipse" + to_string (iteration) + ".txt");
    int i = 0;
    for (auto &c : cov_matrix) {
        //searching eigenvectors
        double a = pow (c[0][0] - c[1][1], 2);
        double b = 4 * c[0][1] * c[1][0];
        double discriminant = sqrt (pow (c[0][0] - c[1][1], 2) + 4 * c[0][1] * c[1][0]);
        double e_1 = 0.5 * (c[1][1] + c[0][0] + discriminant);
        double e_2 = 0.5 * (c[1][1] + c[0][0] - discriminant);
        //searching the angle
        double angle;
        if (pow (c[0][1], 2) + pow (c[0][0] - e_1, 2) == 0) {
            angle = 0;
        } else if (c[0][0] >= e_1) {
            angle = acos (-c[0][1] / sqrt (pow (c[0][1], 2) + pow (c[0][0] - e_1, 2))) * 180 / M_PI;
        } else {
            angle = acos (c[0][1] / sqrt (pow (c[0][1], 2) + pow (c[0][0] - e_1, 2))) * 180 / M_PI;
        }
        double e_1_n = e_1 / sqrt (pow (e_1, 2) + (e_2, 2)) * max_dist;
        double e_2_n = e_2 / sqrt (pow (e_1, 2) + (e_2, 2)) * min_dist;
        ellipse << means[i].x () << " " << means[i].y () << " " << e_1_n << " " << e_2 << " " << angle << endl;
        i++;
    }
}

Cluster_Search Cluster_Search::hierarchical_algorithm () {
    int tree_number = Point::quantity ();
    vector<TreeNode<const Point *> *> tree_nodes;
    tree_nodes.reserve (Point::quantity ());
    for (int p = 0; p < Point::quantity (); ++p) {
        tree_nodes.push_back (new TreeNode<const Point *> (Point::get_by_id (p + 1)));
    }
    while (1 < tree_nodes.size ()) {
        int a, b;
        ha_get_closest_nodes (a, b, tree_nodes);
        Point old_1 (tree_nodes[a]->value ()->x (), tree_nodes[a]->value ()->y (), 0);
        Point old_2 (tree_nodes[b]->value ()->x (), tree_nodes[b]->value ()->y (), 0);
        ha_merge_nodes (a, b, tree_nodes);
        ha_fprintf (tree_nodes, Point::quantity () - tree_nodes.size (), old_1, old_2);
    }
    tree_root_ = tree_nodes[0];
    return *this;
}

void Cluster_Search::ha_get_closest_nodes (int &a, int &b, const vector<TreeNode<const Point *> *> &tree_node) {
    // searches for closest nodes writes its numbers to a and b
    double min_dist = field_->dist ()[0][1];
    int min_a = 0;
    int min_b = 0;
    for (a = 0; a < tree_node.size (); ++a) {
        for (b = a + 1; b < tree_node.size (); ++b) {
            double distance = Point::dist (tree_node[a]->value (), tree_node[b]->value ());
            if (distance < min_dist) {
                min_dist = distance;
                min_a = a;
                min_b = b;
            }
        }
    }
    a = min_a;
    b = min_b;
}

void Cluster_Search::ha_merge_nodes (int a, int b, vector<TreeNode<const Point *> *> &tree_nodes) {
    
    /* there are that steps:
     * get center of new node
     * connect old centers to new node
     * delete old pointers from vector tree_node
     * add new node to that vector
     */
    
    int i_1, i_2;
    TreeNode<const Point *> *&first = tree_nodes[a];
    TreeNode<const Point *> *&second = tree_nodes[b];
    Point *new_center = ha_get_new_node_center (first, second);
    auto *new_node = new TreeNode<const Point *> (new_center);
    /* int min_i = min (i_1, i_2);
    int max_i = max (i_1, i_2); */
    int min_i = min (a, b);
    int max_i = max (a, b);
    tree_nodes.erase (tree_nodes.begin () + max_i);
    tree_nodes.erase (tree_nodes.begin () + min_i);
    tree_nodes.push_back (new_node);
}

TreeNode<const Point *> *&Cluster_Search::ha_get_node_by_coords (int a,
                                                                 vector<TreeNode<const Point *> *> &tree_node,
                                                                 int &i, double x, double y) {
    for (i = 0; i < tree_node.size (); ++i) {
        if (tree_node[i]->value ()->x () == x && tree_node[i]->value ()->y () == y) {
            return tree_node[i];
        }
    }
    // just to escape warning, it's impossible branch
    return tree_node[0];
}

Point *Cluster_Search::ha_get_new_node_center (TreeNode<const Point *> *&first, TreeNode<const Point *> *&second) {
    double sum_x = 0;
    double sum_y = 0;
    int points = 0;
    ha_get_node_sum (first, sum_x, sum_y, points);
    ha_get_node_sum (second, sum_x, sum_y, points);
    return new Point (sum_x / points, sum_y / points, 0);
}

void Cluster_Search::ha_get_node_sum (TreeNode<const Point *> *&node, double &sum_x, double &sum_y, int &points) {
    if (node->first_child () != nullptr) {
        ha_get_node_sum (node->first_child_, sum_x, sum_y, points);
    }
    if (node->brother () != nullptr) {
        ha_get_node_sum (node->brother_, sum_x, sum_y, points);
    }
    sum_x += node->value ()->x ();
    sum_y += node->value ()->y ();
    points++;
}

void Cluster_Search::ha_fprintf (const vector<TreeNode<const Point *> *> &tree_nodes,
                                 int iteration,
                                 Point old_1,
                                 Point old_2) {
    ofstream out ("ha/ha" + to_string (iteration) + ".txt");
    for (int i = 0; i < tree_nodes.size (); ++i) {
        out << tree_nodes[i]->value ()->x () << " " << tree_nodes[i]->value ()->y () << " " << 0 << endl;
    }
    out << old_1.x () << " " << old_1.y () << " " << 1 << endl;
    out << old_2.x () << " " << old_2.y () << " " << 1 << endl;
}

const TreeNode<const Point *> *Cluster_Search::tree_root () {
    return tree_root_;
}
