from libcpp.vector cimport vector
from libcpp.string cimport string

import numpy as np
cimport numpy as np
ctypedef np.npy_uint32 UINT32_t
ctypedef np.npy_uint8 UINT8_t


cdef extern from "hyperloglog.hpp" namespace "sketch":
    cdef cppclass HyperLogLog:
        HyperLogLog() except +
        HyperLogLog(int ) except +
        void add(char *,int)
        void merge(HyperLogLog)
        double estimate()
        vector[UINT8_t]& Array()
        void update(vector[UINT8_t]&)

cdef extern from "hyperloglog.hpp" namespace "sketch":
    cdef cppclass CountSketch:
        CountSketch() except +
        CountSketch(double,double ) except +
        void addString(string,int)
        double estimatel2()
        vector[vector[int]] Array()
        void update(vector[vector[int]]&)

cdef extern from "hyperloglog.hpp" namespace "sketch":
    cdef cppclass Data2Sketch:
        Data2Sketch() except +
        Data2Sketch(UINT8_t) except +
        Data2Sketch(UINT8_t, UINT8_t) except +
        void ScanString(vector[string]&)
        vector[UINT8_t]& NDVArray()
        vector[UINT8_t]& fiArray(int)

cdef extern from "hyperloglog.hpp" namespace "sketch":
    cdef cppclass Data2SketchPlus:
        Data2SketchPlus() except +
        Data2SketchPlus(int) except +
        Data2SketchPlus(int, int, float,float) except +
        void ScanString(vector[string]&)
        vector[UINT8_t]& NDVArray()
        vector[UINT8_t]& fiArray(int)
        vector[vector[int]]& CSArray()
