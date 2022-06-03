# cython: language_level=3
# distutils: language = c++

from HyperLogLog cimport HyperLogLog
from HyperLogLog cimport CountSketch
from HyperLogLog cimport Data2Sketch
from HyperLogLog cimport Data2SketchPlus

# Create a Cython extension type which holds a C++ instance
# as an attribute and create a bunch of forwarding methods
# Python extension type.

cdef class Hyperloglog:
    cdef HyperLogLog c_hll

    def __cinit__(self,_b):
        self.c_hll = HyperLogLog(_b)
    def add(self, s):
        self.c_hll.add(s,len(s))
    def update(self,h):
        self.c_hll.update(h)
    def estimate(self):
        return self.c_hll.estimate()
    def Array(self):
        return self.c_hll.Array()

cdef class Countsketch:
    cdef CountSketch cs
    def __cinit__(self,epsilon,gamma):
        self.cs = CountSketch(epsilon,gamma)
    def addString(self,s,f):
        self.cs.addString(s,f)
    def update(self,sketch):
        self.cs.update(sketch)
    def estimatef2(self):
        return self.cs.estimatel2()
    def Array(self):
        return self.cs.Array()

cdef class Data2sketch:
    cdef Data2Sketch d2s 
    
    def __cinit__(self,_b,level=1):
        self.d2s = Data2Sketch(_b,level)
    def ScanString(self,ss):
        self.d2s.ScanString(ss)
    def NDVArray(self):
        return self.d2s.NDVArray()
    def fiArray(self,i):
        return self.d2s.fiArray(i)

cdef class Data2sketchplus:
    cdef Data2SketchPlus d2s
    def __cint__(self,b,level,epsilon,gamma):
        self.d2s = Data2SketchPlus(b,level,epsilon,gamma)
    def ScanString(self,ss):
        self.d2s.ScanString(ss)
    def NDVArray(self):
        return self.d2s.NDVArray()
    def fiArray(self,i):
        return self.d2s.fiArray(i)
    def CSArray(self):
        return self.d2s.CSArray()


