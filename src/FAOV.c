#include <math.h>
#include <immintrin.h>
#include "FAOV.h"
#include <stdio.h>


double AOV(Vector* time, Vector* magnitude, int n, int bins,double freq,double mean)
{
    int num_of_vec=n/4;
    Vector phase[num_of_vec];
    Vector freq_vec;
    Vector support;
    for (int i=0;i<4;i++)
    {
        freq_vec.array[i]=freq;
        support.array[i]=(double) bins;
    }
    for (uint i=0;i<num_of_vec;i++)
    {
        phase[i].simd_vector=_mm256_sub_pd(_mm256_mul_pd(time[i].simd_vector,freq_vec.simd_vector),_mm256_floor_pd(_mm256_mul_pd(time[i].simd_vector,freq_vec.simd_vector)));
    }
    double sum_square[bins];
    double sum_[bins];
    int num_obs[bins];
    for (int i=0;i<bins;i++)
    {
        sum_square[i]=0;
        sum_[i]=0;
        num_obs[i]=0;
    }
    Vector square;
    Vector ide;
    int id;
    for (uint i=0;i<num_of_vec;i++)
    {
        square.simd_vector=_mm256_mul_pd(magnitude[i].simd_vector,magnitude[i].simd_vector);
        ide.simd_vector=_mm256_floor_pd(_mm256_mul_pd(support.simd_vector,phase[i].simd_vector));
        for (int j=0;j<4;j++)
        {
           id=(int) ide.array[j];
           sum_square[id]+=square.array[j];
           sum_[id]+=magnitude[i].array[j];
           num_obs[id]+=1;
        }
    }
    double var=0;
    double temp=0;
    for (int i=0;i<bins;i++)
    {
        if (num_obs[i] == 0) continue;
        sum_[i]/=(double)num_obs[i];
        var += num_obs[i]*(sum_[i]-mean)*(sum_[i]-mean);
        temp += sum_square[i]-num_obs[i]*sum_[i]*sum_[i];
        //printf("%d %f %f\n",num_obs[i],sum_[i],sum_square[i]);
    }
    double pow=(var*(double) (n-bins))/(((double) bins-1)*temp);
    return pow;
}

double period(double *time, double *magnitude, int n,int bins,double t_max)
{
    int num=n/4;
    Vector mag[num];
    Vector t[num];
    Vector mean_vect;
    for (int i=0;i<4;i++)
    {
        mean_vect.array[i]=0;
    }
    for (int i=0;i<num;i++)
    {
        for (int j=0;j<4;j++)
        {
            mag[i].array[j]=magnitude[4*i+j];
            t[i].array[j]=time[4*i+j]-time[0];
        }
        mean_vect.simd_vector=_mm256_add_pd(mean_vect.simd_vector,mag[i].simd_vector);
    }
    double mean=0;
    for (int i=0;i<4;i++)
    {
        mean+=mean_vect.array[i]/(double)(num*4);
    }
    double f_max,f_min;
    if (t_max<0)
    {
        f_min=1/(time[num*4-1]-time[0]);
    }
    else
    {
        f_min=1/t_max;
    }
    f_max=20;
    double df=0.1*f_min;
    int n_f=(f_max-f_min)/df;
    double p=0;
    double p_o,f_best;
    f_best=f_min;
    for (int i=0;i<n_f;i++)
    {
        p_o=AOV(t,mag,num*4,bins,f_min+i*df,mean);
        if (p_o>p)
        {
            p=p_o;
            f_best=f_min+i*df;
        }
    }
    return 1/f_best;
}

double period_time_domain(double *time, double *magnitude, int n,int bins,double t_min,double t_max,int num_points)
{
    int num=n/4;
    Vector mag[num];
    Vector t[num];
    Vector mean_vect;
    for (int i=0;i<4;i++)
    {
        mean_vect.array[i]=0;
    }
    for (int i=0;i<num;i++)
    {
        for (int j=0;j<4;j++)
        {
            mag[i].array[j]=magnitude[4*i+j];
            t[i].array[j]=time[4*i+j]-time[0];
        }
        mean_vect.simd_vector=_mm256_add_pd(mean_vect.simd_vector,mag[i].simd_vector);
    }
    double mean=0;
    for (int i=0;i<4;i++)
    {
        mean+=mean_vect.array[i]/(double)(num*4);
    }
    if (t_max<0)
    {
        t_max=(time[num*4-1]-time[0]);
    }

    double p=0;
    double p_o,t_best;
    t_best=t_min;
    double per;
    double dt=(t_max-t_min)/ (double) num_points;
    for (int i=0;i<num_points;i++)
    {
        per=t_min+i*dt;
        p_o=AOV(t,mag,num*4,bins,1/per,mean);
        if (p_o>p)
        {
            p=p_o;
            t_best=per;
        }
    }
    return t_best;
}