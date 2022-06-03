### PySpark Experiment
* CentOS
* C++ 11
* GCC 4.8

#### Preparation

Generate sampling data from Poisson Distribution and Zipfian Distribution.

python
```
python genpoi.py
#python genzipf.py
```

linux example
```
cd dataset/poi/
N=   #filled with N of generated data according to file name
D=   #filled with N of generated data according to file name
for i in {0...7}
do
	cat sample-0.01-poi-0.01-poi-50-1000ww-$N-$D-$i-* >> cat sample-0.01-poi-0.01-poi-50-1000ww-$N-$D-$i.txt
	rm sample-0.01-poi-0.01-poi-50-1000ww-$N-$D-$i-*
done
cd ..
```


Before running the PySpark, put the data into HDFS.
```
hdfs dfs -mkdir /user
hdfs dfs -mkdir /user/poi
hdfs dfs -mkdir /user/poi/poi-50-1kww
hdfs dfs -put dataset/poi/poi-50-1kww /user/poi/poi-50-1kww/
```

#### Sketch implemented by Cython

```
cd Hyperloglog-Cython
python setup.py build_ext --inplace
python setup.py install
```

#### Results of experiment are shown in directory, Sampling-based_NDV_PySpark

* baseline.ipynb
	- Sampling-based NDV estimation by Map Reduce with the exact fi
* SampleDataWithOurMethods.ipynb 
	- Sampling-based NDV estimation by our method and Map Reduce with the approximate fi
* SampleDataWithOurMethod+CL1.ipynb
	- Sampling-based NDV estimation(CL1) by our method and Map Reduce with the approximate fi












