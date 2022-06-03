#ifndef DPE_H
#define DPE_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "hyperloglog.hpp"
#include <chrono>
#include <map>
#include <cmath>
#include "rand.h"

using namespace std;
using namespace sketch;

class F1Sketch{
public:
	HyperLogLog NDV;// size 40
	HyperLogLog f1;//size 24
	F1Sketch(uint8_t b=16){
		NDV=HyperLogLog(b);
		f1=HyperLogLog(b);
	}
	void ScanDict(map<string,int> &dictdata){
		map<string,int>::iterator iter;
    	for(iter=dictdata.begin();iter != dictdata.end(); iter++){
    		if(iter->second==1)
        		f1.add(iter->first.c_str(), iter->first.size());
        	NDV.add(iter->first.c_str(), iter->first.size());
        }
	}
	void ScanString(vector<string> &orderstring){
		int count=1;
		int n_string=orderstring.size();
		int curr=0;
		for(int i=1;i<n_string;i++){
			if(! orderstring[curr].compare(orderstring[i])){
				count+=1;
			}else{
				NDV.add(orderstring[curr].c_str(), orderstring[curr].size());
				if(count<=1)
        			f1.add(orderstring[curr].c_str(), orderstring[curr].size());
				count=1;
				curr=i;
			}
		}
	}
	int sizeofSketch(){
		long size=0;
		size+=(long)NDV.sizeofSketch();
		size+=(long)f1.sizeofSketch();
		return size;
	}
};

class F1Interval{
public:
	int machine_k;
	vector<vector<HyperLogLog>> NDV;// size 40
	vector<HyperLogLog> f1;//size 24
	uint8_t b;
	F1Interval(uint8_t _b=16){
		NDV=vector<vector<HyperLogLog>>();
		vector<HyperLogLog> temp=vector<HyperLogLog>();
		NDV.push_back(temp);
		f1=vector<HyperLogLog>();
		b=_b;
		machine_k=0;
	}
	void add(F1Sketch& newF1){
		machine_k+=1;
		int m=machine_k;
		int level=1;
		HyperLogLog temp(b);
		temp.merge(newF1.f1);
		f1.push_back(temp);
		HyperLogLog temp2(b);
		temp2.merge(newF1.NDV);
		NDV[0].push_back(temp2);
		for(level=1;level<NDV.size();level++){
			if(m==(1<<level)*NDV[level].size()){
				int last=NDV[level].size()-1;
				int last2=NDV[level-1].size()-1;
				NDV[level][last].merge(NDV[level-1][last2-1]);
				NDV[level][last].merge(NDV[level-1][last2]);
			}
			if(m==(1<<level)*NDV[level].size()+1){
				HyperLogLog tmp(b);
				NDV[level].push_back(tmp);
			}
		}
		if(NDV[NDV.size()-1].size()==3){
			vector<HyperLogLog> tmp=vector<HyperLogLog>();
			NDV.push_back(tmp);
			HyperLogLog tmp2(b);
			int last=NDV.size()-1;
			NDV[last].push_back(tmp2);
			NDV[last][0].merge(NDV[last-1][0]);
			NDV[last][0].merge(NDV[last-1][1]);
			HyperLogLog tmp3(b);
			NDV[last].push_back(tmp3);
		}
	}
	void add(HyperLogLog& newNDV,HyperLogLog & newf1){
		machine_k+=1;
		int m=machine_k;
		int level=1;
		HyperLogLog temp(b);
		temp.merge(newf1);
		f1.push_back(temp);
		HyperLogLog temp2(b);
		temp2.merge(newNDV);
		NDV[0].push_back(temp2);
		for(level=1;level<NDV.size();level++){
			if(m==(1<<level)*NDV[level].size()){
				int last=NDV[level].size()-1;
				int last2=NDV[level-1].size()-1;
				NDV[level][last].merge(NDV[level-1][last2-1]);
				NDV[level][last].merge(NDV[level-1][last2]);
			}
			if(m==(1<<level)*NDV[level].size()+1){
				HyperLogLog tmp(b);
				NDV[level].push_back(tmp);
			}
		}
		if(NDV[NDV.size()-1].size()==3){
			vector<HyperLogLog> tmp=vector<HyperLogLog>();
			NDV.push_back(tmp);
			HyperLogLog tmp2(b);
			int last=NDV.size()-1;
			NDV[last].push_back(tmp2);
			NDV[last][0].merge(NDV[last-1][0]);
			NDV[last][0].merge(NDV[last-1][1]);
			HyperLogLog tmp3(b);
			NDV[last].push_back(tmp3);
		}
	}
	void add(std::vector<uint8_t>& newNDV,std::vector<uint8_t>& newf1){
		machine_k+=1;
		int m=machine_k;
		int level=1;
		HyperLogLog temp(b);
		temp.update(newf1);
		f1.push_back(temp);
		HyperLogLog temp2(b);
		temp2.update(newNDV);
		NDV[0].push_back(temp2);
		for(level=1;level<NDV.size();level++){
			if(m==(1<<level)*NDV[level].size()){
				int last=NDV[level].size()-1;
				int last2=NDV[level-1].size()-1;
				NDV[level][last].merge(NDV[level-1][last2-1]);
				NDV[level][last].merge(NDV[level-1][last2]);
			}
			if(m==(1<<level)*NDV[level].size()+1){
				HyperLogLog tmp(b);
				NDV[level].push_back(tmp);
			}
		}
		if(NDV[NDV.size()-1].size()==3){
			vector<HyperLogLog> tmp=vector<HyperLogLog>();
			NDV.push_back(tmp);
			HyperLogLog tmp2(b);
			int last=NDV.size()-1;
			NDV[last].push_back(tmp2);
			NDV[last][0].merge(NDV[last-1][0]);
			NDV[last][0].merge(NDV[last-1][1]);
			HyperLogLog tmp3(b);
			NDV[last].push_back(tmp3);
		}
	}
	void merge(F1Interval & other){// limits big merge small
		for(int i=0;i<other.machine_k;i++){
			this->add(other.f1[i],other.NDV[0][i]);
		}
	}
	double querySingle(int index){
		if(machine_k==0)
			return 0.0;
		int level=NDV.size()-1;
		HyperLogLog result(b);
		int base=0;
		if(index< 1<<level){
			level-=1;
			while(level>=0){
				if(index< (base+1)*(1<<level)){
					result.merge(NDV[level][base+1]);
					base=base*2;
				}else{
					result.merge(NDV[level][base]);
					base=(base+1)*2;
				}
				level-=1;
			}
			//right
			int machine=NDV[0].size();
			level=0;
			while(1<<level<=NDV[0].size()-(1<<(NDV.size()-1))){
				if(machine%2==0 && machine/2!=1){
					machine=machine/2;
				}else{
					result.merge(NDV[level][machine-1]);
					machine=(machine-1)/2;
				}
				level+=1;
			}
		}else{
			result.merge(NDV[NDV.size()-1][0]);
			int machine=NDV[0].size();
			level=0;
			while( 1<<level <= NDV[0].size()- (1<<(NDV.size()-1)) ){
				if(machine%2==0 && machine/2!=1){
					machine=machine/2;
				}else{
					if(index>machine*(1<<level)-(1<<level)-1 && index<=machine*(1<<level)-1){
						int tempindex=index;
						for(int newlevel=0;newlevel<level;newlevel++){
							result.merge(NDV[newlevel][tempindex^1]);
							tempindex=tempindex/2;
						}
					}else{
						result.merge(NDV[level][machine-1]);
					}
					machine=(machine-1)/2;
				}
				level+=1;
			}
		}
		double esti=-result.estimate();
		result.merge(f1[index]);
		esti+=result.estimate();
		return esti;
	}
	double queryf1(){
		double result=0.0;
		for(int i=0;i<machine_k;i++){
			result+=querySingle(i);
		}
		return result;
	}
	double queryf1test(){
		double result=0.0;
		for(int i=0;i<machine_k;i++){
			result+=querySingle(i);
			cout<<"index "<<i<<": "<<result;
		}
		return result;
	}
	double queryf1test2(){
		double result=0.0;
		for(int i=0;i<machine_k;i++){
			HyperLogLog temp(b);
			for(int j=0;j<machine_k;j++){
				if(j!=i){
					temp.merge(NDV[0][j]);
				}
			}
			result-=f1[i].estimate();
			temp.merge(f1[i]);
			result+=temp.estimate();
			cout<<"index "<<i<<": "<<result;
		}
		return result;
	}
	double queryNDV(){
		double result=0.0;
		HyperLogLog temp(b);
		int level=0;
		int m=machine_k;
		temp.merge(NDV[NDV.size()-1][0]);
		while( (1<<level) <= machine_k-( 1<<(NDV.size()-1) )){
			if(m%2==0 && m/2!=1){
				m=m/2;
			}else{
				temp.merge(NDV[level][m-1]);
				m=(m-1)/2;
			}
			level+=1;
		}
		return temp.estimate();
	}
	double originNDV(){
		double result=0.0;
		HyperLogLog temp(b);
		for(int i=0;i<NDV[0].size();i++){
			temp.merge(NDV[0][i]);
		}
		return temp.estimate();
	}
	double estimate(){
		double result=0.0;
		for(int i=0;i<machine_k;i++){
			HyperLogLog temp(b);
			for(int j=0;j<machine_k;j++){
				if(j!=i){
					temp.merge(NDV[0][j]);
				}
			}
			result-=temp.estimate();
			temp.merge(f1[i]);
			result+=temp.estimate();
		}
		//cout<<"step: "<<i<<" : "<<temp.estimate()<<endl;
		return result;
	}
};

class BaseMaster{
public:
	int k_machine;
	map<int,long> samplefi;
	long d;
	uint8_t b;
	long n;
	unsigned long long ll2;
	BaseMaster(vector<string> & filename,int k_,double SampleRate=0.01,uint8_t _b=16){
		map<string,int> finaldict;
		k_machine=k_;
		b=_b;
		vector<string>::iterator iter;
		long TransmitSize=0;
		string line;
		int m_k=0;
		int read_one_time=filename.size()/k_;
		n=0;
		for(iter=filename.begin();iter!=filename.end();){
			vector<string>data1;
			for(int i=0;i<read_one_time;i++){
				ifstream fin(*iter,ios::binary);
				while(getline(fin,line)){
					data1.push_back(line);
				}
				iter++;
			}
			sort(data1.begin(),data1.end());
			int count=1;
			size_t n_string=data1.size();
			size_t curr=0;
			map<string,int> dict;
			for(size_t i=1;i<n_string;i++){
				if(! data1[curr].compare(data1[i])){
					count+=1;
				}else{
					dict[data1[curr]]=count;
					count=1;
					curr=i;
				}
			}
			std::map<string,int>::iterator mapiter;
			for(mapiter=dict.begin();mapiter!=dict.end();mapiter++){
				TransmitSize+=sizeof(char)*(mapiter->first.length()+1)+sizeof(mapiter->second);
				n+=mapiter->second;
				if(finaldict.find(mapiter->first)!=finaldict.end())
					finaldict[mapiter->first]+=mapiter->second;
				else
					finaldict[mapiter->first]=mapiter->second;
			}
			m_k+=1;
		}

		counter(finaldict,samplefi);
		d=0;
		map<int,long>::iterator countiter;
		for(countiter=samplefi.begin();countiter!=samplefi.end();countiter++){
			d+=countiter->second;
		}
		cout<<"dict size: "<<TransmitSize<<endl;
	}
	void counter(map<string,int> & dict,map<int,long> &fi){
		map<string,int>::iterator mapiter;
		for(mapiter=dict.begin();mapiter!=dict.end();mapiter++){
			if(fi.find(mapiter->second)!=fi.end()){
				fi[mapiter->second]+=1;
			}else{
				fi[mapiter->second]=1;
			}
		}
	}
	double Fi(int i){
		return samplefi[i];
	}
	double Chao(){
		return d+(double)samplefi[1]*(samplefi[1]-1)/(2*(samplefi[2]+1));
	}
	double GEE(double q){
		return d+(double)(sqrt(1.0/q)-1)*samplefi[1];
	}
	double CL1(){
		double n=0;
		long l2=0;
		double dd=(double)d;
		map<int,long>::iterator iter;
		for(iter=samplefi.begin();iter!=samplefi.end();iter++){
			n+=iter->first*iter->second;
			l2+=iter->first*iter->first*iter->second;
		}
		this->ll2=l2;
		double tmp=(double)d*(l2-n);
		tmp=(double)tmp/(1.0-(double)samplefi[1]/n)/((double)n*n-n-1);
		return (d+samplefi[1]*( tmp>0?tmp:0))/(1.0-(double)samplefi[1]/n);
	}
	double Shlosser(double q){
		double Ef=0.0;
		double Exf=0.0;
		map<int,long>::iterator iter;
		for(iter=samplefi.begin();iter!=samplefi.end();iter++){
			Ef+=pow(1-q,(double)iter->first)*(double)iter->second;
			Exf+=q*(double)iter->first*pow(1-q,(double)iter->first-1)*(double)iter->second;
		}
		return d+(double)samplefi[1]*Ef/Exf;
	}
};

class Master{
public:
	int k_machine;
	F1Interval f1sketch;
	map<int,long> samplefi;
	map<int,long> samplesamplefi;
	map<string,int> sampleMap;
	map<string,int> finaldict;
	long d;
	uint8_t b;
	long n;
	vector<CountSketch> CSList;
	CountSketch CS;
	unsigned long long ll2;
	Master(vector<string> & filename,int k_,double SampleRate=0.01,uint8_t _b=16){
		k_machine=k_;
		b=_b;
		vector<string>::iterator iter;
		long TransmitSize=0;
		long SketchSize=0;
		string line;
		CS=CountSketch(0.01,0.1);
		for(int i=0;i<k_machine;i++){
			CountSketch count(0.01,0.1);
			CSList.push_back(count);
		}
		int m_k=0;
		int read_one_time=filename.size()/k_;
		n=0;
		f1sketch=F1Interval(b);
		for(iter=filename.begin();iter!=filename.end();){
			vector<string>data1;
			for(int i=0;i<read_one_time;i++){
				ifstream fin(*iter,ios::binary);
				while(getline(fin,line)){
					data1.push_back(line);
					if(drand()<SampleRate){
						if(sampleMap.find(line)==sampleMap.end())
							sampleMap[line]=1;
						else
							sampleMap[line]+=1;
					}
					n+=1;
				}
				iter++;
			}
			cout<<"begin sort"<<endl;
			sort(data1.begin(),data1.end());
			cout<<"end sort"<<endl;
			CSList[m_k].addStringArray(data1);
			//for(int i=0;i<data1.size();i++)
			//	CSList[m_k].addString(data1[i]);
			m_k+=1;
			F1Sketch Fi1(b);
			Fi1.ScanString(data1);
			f1sketch.add(Fi1);
			SketchSize+=Fi1.sizeofSketch();
		}

		counter(finaldict,samplefi);
		d=0;
		map<int,long>::iterator countiter;
		for(countiter=samplefi.begin();countiter!=samplefi.end();countiter++){
			d+=countiter->second;
		}
		counter(sampleMap,samplesamplefi);
		for(int i=0;i<m_k;i++){
			CS.merge(CSList[i]);
		}
		cout<<" HyperLogLog Size: "<<SketchSize<<" CountSketch Size: "<<CS.sizeofSketch()<<endl;
	}
	void counter(map<string,int> & dict,map<int,long> &fi){
		map<string,int>::iterator mapiter;
		for(mapiter=dict.begin();mapiter!=dict.end();mapiter++){
			if(fi.find(mapiter->second)!=fi.end()){
				fi[mapiter->second]+=1;
			}else{
				fi[mapiter->second]=1;
			}
		}
	}
	double Fi(int i){
		return samplefi[i];
	}
	double EstiChao(){
		double f1=f1sketch.queryf1();
		double ndv=f1sketch.queryNDV();
		return ndv+f1*f1/(2.0*(ndv-f1));
	}
	double EstiGEE(double q){
		double f1=f1sketch.queryf1();
		double ndv=f1sketch.queryNDV();
		return ndv+(double)(sqrt(1.0/q)-1.0)*f1;
	}
	double EstiShl(){
		double f1=f1sketch.queryf1();
		double ndv=f1sketch.queryNDV();
		int sampled=0;
		map<int,long>::iterator iter;
		for(iter=samplesamplefi.begin();iter!=samplesamplefi.end();iter++){
			sampled+=iter->second;
		}
		return d+f1*(ndv-sampled)/samplesamplefi[1];
	}
	double EstiCL1(){
		double f1=f1sketch.queryf1();
		double l2=CS.estimatel2();
		double ndv=f1sketch.queryNDV();
		double tmp=(double)ndv*(l2-n);
		tmp=(double)tmp/(1.0-(double)f1/n)/((double)n*n-n-1);
		double esti=(ndv+f1*(tmp>0?tmp:0))/(1.0-(double)f1/n);
		return esti;
	}
};

#endif
