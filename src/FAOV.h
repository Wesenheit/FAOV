#include <math.h>
#include <immintrin.h>
#define NMAX 20000


typedef union __attribute__ ((aligned (sizeof(__m256d))))
{
    __m256d simd_vector;
    double array[4];
} Vector;
/*
Most important data structure for the program. Union that represents four doubles.
*/

double AOV(Vector* magnitude, Vector* time, int n, int bins,double freq,double mean);
double period(double *time, double *magnitude, int n,int bins,double t_max);
double period_time_domain(double *time, double *magnitude, int n,int bins,double t_min,double t_max,int num_points);