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

class FiSketch{
public:
	int order;// 8
	HyperLogLog NDV;// size 40
	vector<HyperLogLog> fi;//size 24
	FiSketch(int _order,uint8_t b=20){
		NDV=HyperLogLog(b);
		order=_order;
		fi=vector<HyperLogLog>(_order);
		for(int i=0;i<order;i++){
			fi[i]=HyperLogLog(b);
		}
	}
	void ScanDict(map<string,int> &dictdata){
		map<string,int>::iterator iter;
    	for(iter=dictdata.begin();iter != dictdata.end(); iter++){
    		if(iter->second<=order)
        		fi[iter->second-1].add(iter->first.c_str(), iter->first.size());
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
				if(count<=order)
        			fi[count-1].add(orderstring[curr].c_str(), orderstring[curr].size());
				count=1;
				curr=i;
			}
		}
	}
	int sizeofSketch(){
		long size=0;
		size+=sizeof(order);
		size+=NDV.sizeofSketch();
		for(int i=0;i<fi.size();i++){
			size+=(long)fi[i].sizeofSketch();
		}
		return size;
	}
};

class computeFi{
public:
	int k;
	int aim;
	vector<int> arr;
	vector<vector<int> > ans;
	computeFi(int _k,int _aim){
		k=_k;
		aim=_aim;
		arr=vector<int>(k);
		search(0,k,aim);
	}
	void search(int x,int n,int res){
		if(x==n-1){
			arr[n-1]=res;
			vector<int> a(arr);
			ans.push_back(a);
			return;
		}
		for(int i=0;i<res+1;i++){
			arr[x]=i;
			search(x+1,n,res-i);
		}
	}
};

class Master{
public:
	int k_machine;
	vector<FiSketch> SketchList;
	map<int,long> samplefi;
	map<int,long> samplesamplefi;
	map<string,int> sampleMap;
	map<string,int> finaldict;
	HyperLogLog NDV;
	long d;
	uint8_t b;
	long n;
	vector<CountSketch> CSList;
	CountSketch CS;
	unsigned long long ll2;
	Master(vector<string> & filename,int k_,double SampleRate=0.01,uint8_t _b=20){
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
		n=0;
		for(iter=filename.begin();iter!=filename.end();iter++){
			ifstream fin(*iter,ios::binary);
			vector<string>data1;
			while(getline(fin,line)){
				data1.push_back(line);
				if(drand()<SampleRate){
					if(sampleMap.find(line)==sampleMap.end())
						sampleMap[line]=1;
					else
						sampleMap[line]+=1;
				}
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
				CSList[m_k].addString(mapiter->first,mapiter->second);
				n+=mapiter->second;
				if(finaldict.find(mapiter->first)!=finaldict.end())
					finaldict[mapiter->first]+=mapiter->second;
				else
					finaldict[mapiter->first]=mapiter->second;
			}
			m_k+=1;
			FiSketch Fi1(2,b);
			Fi1.ScanString(data1);
			SketchList.push_back(Fi1);
			SketchSize+=Fi1.sizeofSketch();
		}
		counter(finaldict,samplefi);
		d=0;
		map<int,long>::iterator countiter;
		for(countiter=samplefi.begin();countiter!=samplefi.end();countiter++){
			d+=countiter->second;
		}
		counter(finaldict,samplesamplefi);
		NDV=HyperLogLog(b);
		for(int i=0;i<SketchList.size();i++){
			NDV.merge(SketchList[i].NDV);
		}
		for(int i=0;i<m_k;i++){
			CS.merge(CSList[i]);
		}
		cout<<"dict size: "<<TransmitSize<<" HyperLogLog Size: "<<SketchSize<<" CountSketch Size: "<<CS.sizeofSketch()<<endl;
	}
	double estiFi(int fi){
		computeFi cFi(k_machine,fi);
		int iter;
		double result=0.0;
		for(iter=0;iter<cFi.ans.size();iter++){
			result+=DPE_compute(cFi.ans[iter]);
		}
		return result>0?result:0;
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
	double DPE_compute(vector<int>& aimList){
		if(SketchList.size()!=aimList.size()){
			cout<<"dimension error"<<endl;
		}
		HyperLogLog ZeroSketch(b);
		vector<int> IntersetID;
		for(int i=0;i<k_machine;i++){
			if(aimList[i]==0){
				ZeroSketch.merge(SketchList[i].NDV);
			}else{
				IntersetID.push_back(i);
			}
		}
		int nzk=IntersetID.size();
		int m=1<<nzk;
		double ans=0;
		vector<int> sign=vector<int>(m-1);
		vector<HyperLogLog> UnionList;
		for(int i=0;i<m-1;i++){
			UnionList.push_back(HyperLogLog(b));
		}
		for(int i=0;i<nzk;i++){
			UnionList[(1<<i)-1].merge(SketchList[IntersetID[i]].fi[aimList[IntersetID[i]]-1]);
		}
		for(int stat=1;stat<m;stat++){
			int lowbit=stat&-stat;
			HyperLogLog temp(b);
			if(stat==lowbit){
				sign[stat-1]=1;
				continue;
			}
			temp.merge(UnionList[(stat^lowbit)-1]);
			temp.merge(UnionList[lowbit-1]);
			UnionList[stat-1]=temp;
			sign[stat-1]=sign[(stat^lowbit)-1]^1;
		}
		for(int stat=0;stat<m-1;stat++){
			UnionList[stat].merge(ZeroSketch);
			if(sign[stat]&1){
				ans+=UnionList[stat].estimate();
			}else{
				ans-=UnionList[stat].estimate();
			}
		}
		ans-=ZeroSketch.estimate();
		return ans;
	}
	double EstiChao(){
		double f1=estiFi(1);
		return NDV.estimate()+f1*f1/(2.0*(NDV.estimate()-f1));
	}
	double EstiGEE(double q){
		return NDV.estimate()+(double)(sqrt(1.0/q)-1.0)*estiFi(1);
	}
	double EstiShl(){
		double f1=estiFi(1);
		int sampled=0;
		map<int,long>::iterator iter;
		for(iter=samplesamplefi.begin();iter!=samplesamplefi.end();iter++){
			sampled+=iter->second;
		}
		return NDV.estimate()+f1*(NDV.estimate()-sampled)/samplesamplefi[1];
	}
	double EstiCL1(){
		double f1=estiFi(1);
		double l2=CS.estimatel2();
		double dd=NDV.estimate();
		double tmp=(double)dd*(l2-n);//
		tmp=(double)tmp/(1.0-(double)f1/n)/((double)n*n-n-1);
		double esti=(dd+f1*(tmp>0?tmp:0))/(1.0-(double)f1/n);
		return esti;
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

#endif
