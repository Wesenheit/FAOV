#define PY_SSIZE_T_CLEAN 
#include "Python.h"
#include "structmember.h"
#include "FAOV.h"
#include <numpy/arrayobject.h>



PyObject* Power(PyObject* self, PyObject* args)
{
    /*
    Python wrapper 
    */
    PyArrayObject* t_arr;
    PyArrayObject* mag_arr;
    double t_max;
    int bins; 
    if (!PyArg_ParseTuple(args, "O!O!id",&PyArray_Type, &t_arr,&PyArray_Type,&mag_arr,&bins,&t_max)) {
        return NULL; // Process object,object, integer, double
    }
    double* time;
    double* magnitude;
    int num_dim = PyArray_NDIM(t_arr);
    if (num_dim > 1)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    npy_intp len1 = PyArray_DIMS(t_arr)[0];
    npy_intp len2 = PyArray_DIMS(mag_arr)[0];
    npy_intp s1 = PyArray_STRIDE(t_arr,0);
    npy_intp s2 = PyArray_STRIDE(mag_arr,0);
    npy_intp type = PyArray_TYPE(t_arr);
    if (type != 12)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    if (len1!=len2)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    time = malloc(len1*sizeof(double));
    magnitude = malloc(len2*sizeof(double));
    
    for (npy_intp i=0;i<len1;i++)
    {
        time[i] = * (double *) PyArray_GETPTR1(t_arr,i);
        magnitude[i] = * (double *) PyArray_GETPTR1(mag_arr,i);
    }

    int num = len1/4;
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
    if (t_max < 0)
    {
        f_min = 1/(time[num*4-1]-time[0]);
    }
    else
    {
        f_min=1/t_max;
    }
    f_max = 20;
    double df = 0.1*f_min;
    int n_f = (f_max-f_min)/df;
    double *p_arr;
    double *f_arr;
    p_arr = malloc(sizeof(double)*n_f);
    f_arr = malloc(sizeof(double)*n_f);
    for (int i = 0;i < n_f;i++)
    {
        f_arr[i] = f_min + df* ((double) i);
        p_arr[i] = AOV(t,mag,num*4,bins,f_arr[i],mean);
    }
    npy_intp dims[1];
    dims[0] = n_f;
    PyObject* F_ARR = PyArray_SimpleNewFromData(1,dims,NPY_DOUBLE,f_arr);
    PyObject* P_ARR = PyArray_SimpleNewFromData(1,dims,NPY_DOUBLE,p_arr);
    PyObject *rslt = PyTuple_New(2);
    PyTuple_SetItem(rslt, 0, F_ARR);
    PyTuple_SetItem(rslt, 1, P_ARR);
    free(time);
    free(magnitude);
    return rslt;
}


PyObject* Period_py(PyObject* self, PyObject* args)
{
    PyArrayObject* t_arr;
    PyArrayObject* mag_arr;
    double t_max;
    int bins; 
    if (!PyArg_ParseTuple(args, "O!O!id",&PyArray_Type, &t_arr,&PyArray_Type,&mag_arr,&bins,&t_max)) {
        return NULL;
    }
    double* time;
    double* mag;
    int num_dim=PyArray_NDIM(t_arr);
    if (num_dim>1)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    npy_intp len1 = PyArray_DIMS(t_arr)[0];
    npy_intp len2 = PyArray_DIMS(mag_arr)[0];
    npy_intp s1 = PyArray_STRIDE(t_arr,0);
    npy_intp s2 = PyArray_STRIDE(mag_arr,0);
    npy_intp type = PyArray_TYPE(t_arr);
    if (type != 12)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    if (len1!=len2)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    time = malloc(len1*sizeof(double));
    mag = malloc(len2*sizeof(double));
    
    for (npy_intp i=0;i<len1;i++)
    {
        time[i]=* (double *) PyArray_GETPTR1(t_arr,i);
        mag[i]=* (double *) PyArray_GETPTR1(mag_arr,i);
    }
    double p=period(time,mag,len1,bins,t_max);
    free(time);
    free(mag);
    return PyFloat_FromDouble(p);
}

static PyMethodDef FAOVMethods[] = {
    {"Period",  Period_py, METH_VARARGS,
     "Find period using AOV."},
    {"Power", Power,METH_VARARGS,
     "Return freq and power arrays"},
    {NULL, NULL, 0, NULL}        
};



static struct PyModuleDef FAOV_module = {
    PyModuleDef_HEAD_INIT,
    "FOAV",   
    NULL, 
    -1,    
    
    FAOVMethods
};


PyMODINIT_FUNC
PyInit_FAOV(void)

{   import_array();
    return PyModule_Create(&FAOV_module);
}
