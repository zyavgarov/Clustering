#include "em.h"

em::em (int clusters_number) : clusters_number (clusters_number) {
    /* Errors
     * -1 field is not in readonly mode
     */
    if (!Field::readonly ()) {
        err_ = -1;
        return;
    }
    //auto *temp_field = new Field ();
    //Field::searches_.emplace_back (temp_field);
    vector<vector<double>> sigma;
    vector<vector<double>> mu;
    vector<vector<double>> r;
    vector<double> pi;
    vector<double> v0;
    // filling mu sigma and pi vectors
    for (int i = 0; i < clusters_number; ++i) {
        vector<vector<double>> v22;
        vector<double> v4;
        vector<double> v2;
        v4.reserve (4);
        for (int j = 0; j < 4; j++) {
            v4.push_back (0);
        }
        v2.reserve (2);
        for (int j = 0; j < 2; j++) {
            v2.push_back (0);
        };
        sigma.push_back (v4);
        mu.push_back (v2);
        // mu looks like to be randomly set
        mu[i][0] = Point::get_by_id (i + 1)->x ();
        mu[i][1] = Point::get_by_id (i + 1)->y ();
        pi.push_back ((double) 1 / clusters_number);
        v0.push_back (0);
        //possibly here should be created all the clusters
    }
    for (int i = 0; i < Point::quantity (); ++i) {
        r.push_back (v0);
    }
    for (int i = 0; i < clusters_number; ++i) {
        vector<vector<double>> a (2, vector<double> (2, 0));
        for (int j = 0; j < Point::quantity (); ++j) {
            a[0][0] += (Point::get_by_id (j + 1)->x () - mu[i][0]) * (Point::get_by_id (j + 1)->x () - mu[i][0]);
            a[0][1] += (Point::get_by_id (j + 1)->x () - mu[i][0]) * (Point::get_by_id (j + 1)->y () - mu[i][1]);
            a[1][0] += (Point::get_by_id (j + 1)->y () - mu[i][1]) * (Point::get_by_id (j + 1)->x () - mu[i][0]);
            a[1][1] += (Point::get_by_id (j + 1)->y () - mu[i][1]) * (Point::get_by_id (j + 1)->y () - mu[i][1]);
        }
        sigma[i][0] = a[0][0] / Point::quantity ();
        sigma[i][1] = a[0][1] / Point::quantity ();
        sigma[i][2] = a[1][0] / Point::quantity ();
        sigma[i][3] = a[1][1] / Point::quantity ();
    }
    bool sw_em;
    int iteration = 0;
    do {
        iteration++;
        sw_em = true;
        
        // E step
        // I have no idea what all that stuff do
        double s = 0;
        vector<double> sum;
        for (int i = 0; i < Point::quantity (); ++i) {
            s = 0;
            for (int c = 0; c < clusters_number; ++c) {
                s += pi[c] * N (Point::get_by_id (i + 1), mu[c], sigma[c]);
            }
            sum.push_back (s);
        }
        for (int i = 0; i < Point::quantity (); ++i) {
            for (int c = 0; c < clusters_number; ++c) {
                if (((r[i][c] - pi[c] * N (Point::get_by_id (i + 1), mu[c], sigma[c]) / sum[i]) > EPS)
                    || ((r[i][c] - pi[c] * N (Point::get_by_id (i + 1), mu[c], sigma[c]) / sum[i]) < -EPS)) {
                    sw_em = false;
                }
                r[i][c] = pi[c] * N (Point::get_by_id (i + 1), mu[c], sigma[c]) / sum[i];
            }
        }
        
        // M step
        double m_c;
        for (int c = 0; c < clusters_number; ++c) {
            m_c = 0;
            mu[c][0] = 0;
            mu[c][1] = 0;
            for (int i = 0; i < Point::quantity (); ++i) {
                m_c += r[i][c];
            }
            for (int i = 0; i < Point::quantity (); ++i) {
                mu[c][0] += Point::get_by_id (i + 1)->x () * r[i][c] / m_c;
                mu[c][1] += Point::get_by_id (i + 1)->y () * r[i][c] / m_c;
            }
            vector<vector<double>> a (2, vector<double> (2, 0));
            for (int i = 0; i < Point::quantity (); ++i) {
                a[0][0] += r[i][c] * (Point::get_by_id (i + 1)->x () - mu[c][0])
                    * (Point::get_by_id (i + 1)->x () - mu[c][0]);
                a[0][1] += r[i][c] * (Point::get_by_id (i + 1)->x () - mu[c][0])
                    * (Point::get_by_id (i + 1)->y () - mu[c][1]);
                a[1][0] += r[i][c] * (Point::get_by_id (i + 1)->y () - mu[c][1])
                    * (Point::get_by_id (i + 1)->x () - mu[c][0]);
                a[1][1] += r[i][c] * (Point::get_by_id (i + 1)->y () - mu[c][1])
                    * (Point::get_by_id (i + 1)->y () - mu[c][1]);
            }
            sigma[c][0] = a[0][0] / m_c;
            sigma[c][1] = a[0][1] / m_c;
            sigma[c][2] = a[1][0] / m_c;
            sigma[c][3] = a[1][1] / m_c;
            pi[c] = m_c / Point::quantity ();
        }
        em_fprintf (iteration, sigma, mu, r);
    } while (!sw_em);
    vector<vector<int>> clusters_to_set (clusters_number, vector<int> (Point::quantity ()));
    for (int p = 0; p < Point::quantity (); ++p) {
        int ind = 0;
        for (int c = 0; c < clusters_number; ++c) {
            if (r[p][c] > r[p][ind]) {
                ind = c;
            }
        }
        clusters_to_set[ind][p];
    }
    Cluster_Search cs (clusters_to_set);
    Field::add_search (cs);
    err_ = 0;
}

int em::err () const {
    return err_;
}

double em::N (const Point *a, vector<double> m, vector<double> Sgm) {
    double det = Sgm[0] * Sgm[3] - Sgm[1] * Sgm[2];
    vector<vector<double>> b_s (2, vector<double> (2));
    b_s[0][0] = Sgm[3] / det;
    b_s[0][1] = -Sgm[1] / det;
    b_s[1][0] = -Sgm[2] / det;
    b_s[1][1] = Sgm[0] / det;
    if (det < 0) { det = -det; }
    return exp (-(b_s[0][0] * (a->x () - m[0]) * (a->x () - m[0])
        + (b_s[1][0] + b_s[0][1]) * (a->x () - m[0]) * (a->y () - m[1])
        + b_s[1][1] * (a->y () - m[1]) * (a->y () - m[1])) / 2) / (sqrt (2 * M_PI * det));
}

void em::em_fprintf (int iteration,
                     vector<vector<double> > sgm,
                     vector<vector<double> > m,
                     vector<vector<double> > r) const {
    ofstream out ("gnuplot/em/em" + to_string (iteration) + ".txt");
    ofstream final ("gnuplot/em/em_fin.txt");
    for (int p = 0; p < Point::quantity (); ++p) {
        int ind = 0;
        for (int c = 0; c < clusters_number; ++c) {
            if (r[p][c] > r[p][ind]) {
                ind = c;
            }
        }
        out << Point::get_by_id (p + 1)->x () << " " << Point::get_by_id (p + 1)->y () << " " << ind << endl;
        final << Point::get_by_id (p + 1)->x () << " " << Point::get_by_id (p + 1)->y () << " " << ind << endl;
    }
    ofstream ellipsis ("gnuplot/em/ellipse" + to_string (iteration) + ".txt");
    for (int c = 0; c < clusters_number; ++c) {
        double discr, angle; // discriminant
        vector<double> lambda;
        vector<double> v;
        discr = (sgm[c][0] + sgm[c][3]) * (sgm[c][0] + sgm[c][3]) - 4 * (sgm[c][0] * sgm[c][3] - sgm[c][1] * sgm[c][2]);
        if (discr >= 0) {
            lambda.push_back ((sgm[c][0] + sgm[c][3] + sqrt (discr)) / 2);
            lambda.push_back ((sgm[c][0] + sgm[c][3] - sqrt (discr)) / 2);
        }
        v.push_back (sgm[c][0] - lambda[0]);
        v.push_back (sgm[c][1]);
        if (v[1] < 0) {
            v[0] *= -1;
            v[1] *= -1;
        }
        angle = acos (v[0] / sqrt (v[0] * v[0] + v[1] * v[1]));
        angle *= 180;
        angle /= M_PI;
        ellipsis << m[c][0] << " " << m[c][1] << " " << 100 * lambda[0] << " " << 100 * lambda[1] << " " << angle
                 << endl;
    }
}