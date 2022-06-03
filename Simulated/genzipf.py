import os
import numpy as np
import random
import math
import gc
from tkinter import _flatten
from scipy.special import zeta

import scipy.stats as stats
s=1.5
N=1000000
k=1
p=0
Flist=[]
ze=zeta(s)
for t in range(100):
    k=1
    while N/ze/k**(s+1)>1:
        #print(k)
        p+=1/ze/k**s
        #Flist.append([k,int(N*(stats.poisson.pmf(k,mu=100)/k))])
        if int(N/ze/k**(s+1))>2**30:
            num=math.ceil(N/ze/k**(s+1)/2**30)
            for i in range(num):
                Flist.append([k,int(N/ze/k**(s+1)/num)])
        else:
            Flist.append([k,int(N/ze/k**(s+1))])
        k=k+1
Flist=np.array(Flist)
N=sum(Flist[:,0]*Flist[:,1])
D=sum(Flist[:,1])
print(N)
print(D)


def sample(Fi,sample_rate,num_m,name):
    fs=[]
    machine=[[] for i in range(num_m)]
    idd=0
    level=0
    index=0
    for [key,value] in Fi:
        #print(key,value)
        fi=np.random.binomial(key,sample_rate,size=value)
        fi=fi[fi>0]
        for item in fi:
            for it in range(item):
                machine[random.randint(0,num_m-1)].append(str(idd))
            idd+=1
            if idd%10000000==0:
                for m in range(num_m):
                    print("write",num_m)
                    if level==0:
                        with open(name+"-"+str(m)+"-"+str(index)+".txt",'w') as f:
                            f.write("\n".join(machine[m]))
                            f.write("\n")
                    else:
                        with open(name+"-"+str(m)+"-"+str(index)+".txt",'w') as f:
                            f.write("\n".join(machine[m]))
                            f.write("\n")
                index+=1
                for i in range(len(machine)):
                    del machine[0]
                del machine
                gc.collect()
                machine=[[] for i in range(num_m)]
                level+=1
        del fi
        gc.collect()
    for m in range(num_m):
        if level==0:
            with open(name+"-"+str(m)+"-"+str(index)+".txt",'w') as f:
                f.write("\n".join(machine[m]))
                f.write("\n")
        else:
            with open(name+"-"+str(m)+"-"+str(index)+".txt",'w') as f:
                f.write("\n".join(machine[m]))
                f.write("\n")
    index+=1
    return idd

fi=sample(Flist,0.01,128,"zipf/sample-0.01-zipf-1.5-100bw-"+str(N)+"-"+str(D))

