#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include <omp.h>

// Load the multidimensional array template
#include "arrayff.hxx"

// Load the template functions to draw the hot and cold spots
#include "draw.hxx"
#define NUM_THREAD 6

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
    int id, start_seq, end_seq;
    int npi_length = floor(npixy / NUM_THREAD);
    int share_nconverged;

    fix_boundaries2(h);
    dump_array<float, 2>(h, "plate0.fit");
    //int num_threads = omp_get_num_procs();
    omp_set_num_threads(NUM_THREAD);
    cout << "set the number of threads: " << NUM_THREAD << endl;

    double start_time = omp_get_wtime();

    do {
        share_nconverged = 0;

#pragma omp parallel shared(share_nconverged) private(id, start_seq, end_seq, nconverged) 
        {
            id = omp_get_thread_num();
            if (id == 0) start_seq = 1;
            else start_seq = id * npi_length;
            
            if (id == NUM_THREAD - 1) end_seq = npixy - 1;
            else end_seq = (id + 1) * npi_length;

            //for (int y = 1; y < npixy - 1; ++y) {
            for (int y = start_seq; y < end_seq; ++y) {
                for (int x = 1; x < npixx - 1; ++x) {
                    g(y, x) = 0.25 * (h(y, x - 1) + h(y, x + 1) + h(y - 1, x) + h(y + 1, x));
                }
            }
#pragma omp barrier

#pragma omp single 
            {fix_boundaries2(g); }
            
            nconverged = 0;

            if (id == NUM_THREAD - 1) end_seq = npixy;
            
            //for (int y = 0; y < npixy; ++y) {
            for (int y = (id * npi_length); y < end_seq; ++y) {
                for (int x = 0; x < npixx; ++x) {
                    float dhg = std::fabs(g(y, x) - h(y, x));
                    if (dhg < tol) ++nconverged;
                    h(y, x) = g(y, x);
                }
            }
#pragma omp atomic 
            share_nconverged += nconverged;

        }

        ++iter;

    //} while (nconverged < nrequired && iter < ITMAX);
    } while (share_nconverged < nrequired && iter < ITMAX);

    double duration = omp_get_wtime() - start_time;

    dump_array<float, 2>(h, "plate1.fit");
    cout << "Duration is: " << duration << endl;
    cout << "Required " << iter << " iterations" << endl;


    //int i; double x, pi, sum = 0.0;
    //step = 1.0 / (double)num_steps;
    //omp_set_num_threads(NUM_THREADS);

    //#pragma omp parallel for private(x) reduction(+:sum)
    //for (i = 0; i < num_steps; i++) {
    //    x = (i + 0.5) * step;
    //    sum = sum + 4.0 / (1.0 + x * x);
    //    printf("loop %d: sum: %f\n", i, sum);
    //    //cout << "loop " << i << endl;
    //}
    //printf("sum is: %f", sum);
    //pi = step * sum;



  
}
