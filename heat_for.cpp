#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include <omp.h>

// Load the multidimensional array template
#include "arrayff.hxx"

// Load the template functions to draw the hot and cold spots
#include "draw.hxx"
#define NUM_THREAD 4

using namespace std;


int main(int argc, char* argv[]) 
{

    const float tol = 0.00001;
    const int npix = atoi(argv[1]);
    const int npixx = npix;
    const int npixy = npix;
    const int ntotal = npixx * npixy;
 
    Array<float, 2> h(npixy, npixy), g(npixy, npixx);

    const int nrequired = npixx * npixy;
    const int ITMAX = 1000000;

    int iter = 0;
    int nconverged = 0;

    fix_boundaries2(h);
    dump_array<float, 2>(h, "plate0.fit");
    //int num_threads = omp_get_num_procs();
    omp_set_num_threads(NUM_THREAD);
    cout << "set the number of threads: " << NUM_THREAD << endl;

    double start_time = omp_get_wtime();

    do {
#pragma omp parallel for
        for (int y = 1; y < npixy-1; ++y) {
            for (int x = 1; x < npixx-1; ++x) {
                g(y, x) = 0.25 * (h(y, x-1) + h(y, x+1) + h(y-1, x) + h(y+1,x));
            }
        }

        fix_boundaries2(g);

        nconverged = 0;
#pragma omp parallel for reduction(+:nconverged)
        for (int y = 0; y < npixy; ++y) {
            for (int x = 0; x < npixx; ++x) {
                float dhg = std::fabs(g(y, x) - h(y, x));
                if (dhg < tol) ++nconverged;
                h(y, x) = g(y, x);
            }
        }
        ++iter;

    } while (nconverged < nrequired && iter < ITMAX);

    double duration = omp_get_wtime() - start_time;

    dump_array<float, 2>(h, "plate1.fit");
    cout << "Duration is: " << duration << endl;
    cout << "Required " << iter << " iterations" << endl;

  
}
