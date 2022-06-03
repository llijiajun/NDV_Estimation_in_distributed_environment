### Simulated Experiment
* Ubuntu
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
for i in {0...1024}
do
	cat sample-0.01-poi-0.01-poi-50-1000ww-$N-$D-$i-* >> sample-0.01-poi-0.01-poi-50-1000ww-$N-$D-$i.txt
	rm sample-0.01-poi-0.01-poi-50-1000ww-$N-$D-$i-*
done
cd ../..
```

#### Compile

c++
```
g++ BaseMain.cpp -o BaseNDV.o -O3 -std=c++11
g++ OurMain.cpp -o OurNDV.o -O3 -std=c++11
```

#### Evaluate

1024 denotes the number of files, 512 denotes the number of simulated machine, 16 is the HyperLogLog's parameter, b.

```
./BaseNDV.o dataset/poi/sample-0.01-poi-0.01-poi-50-1000ww-$N-$D- 1024 512
./OurNDV.o dataset/poi/sample-0.01-poi-0.01-poi-50-1000ww-$N-$D- 1024 512 16
```
