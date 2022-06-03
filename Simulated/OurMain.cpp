#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include "unique.hpp"

using namespace std;
using namespace sketch;

int main(int argc,char **argv){
	if(argc<4){
		cout<<"lack arguments"<<endl;
		return -1;
	}
	string file=argv[1];
	int file_k,k,m;
	sscanf(argv[2],"%d",&file_k);
	sscanf(argv[3],"%d",&k);
	sscanf(argv[4],"%d",&m);
	cout<<"filepath, Number Of Files, Number Of Machines"<<endl;
	cout<<file<<","<<file_k<<","<<k<<endl;
	vector<string> filelist;
//	string file = "dataset/poi/poi-200-5bww/sample-0.01-poi-200-5bww-49999920685-251262305-";
	cout<<"Our Method of "<<file<<", file number is: "<<file_k<<", machine number is: "<<k<<endl;
	for(int i=0;i<file_k;i++){
		string filename=file+to_string(i)+".txt";
		filelist.push_back(filename);
	}
	cout<<"begin"<<endl;
	Master server(filelist,k,0.01,m);
	cout<<"finish preparing"<<endl;
	auto s_q_f1 = chrono::system_clock::now();
	cout<<"f11 "<<server.f1sketch.queryf1()<<endl;
	auto e_q_f1 =  chrono::system_clock::now();
	auto q_f1 = chrono::duration_cast<std::chrono::seconds>(e_q_f1-s_q_f1);
	cout<<"query f1 time:"<< q_f1.count()<<endl;
	auto s_e_f1 = chrono::system_clock::now();
	cout<<"f1 with merge "<<server.f1sketch.estimate()<<endl;
	auto e_e_f1 = chrono::system_clock::now();
	auto e_f1 = chrono::duration_cast<std::chrono::seconds>(e_e_f1-s_e_f1);
	cout<<"merge f1 time:"<< e_f1.count()<<endl;
	cout<<"EstiGEE | EstiChao | EstiShl | EstiCL1 | d | f1 "<<endl;
	cout<<server.EstiGEE(0.01)<<","<<server.EstiChao()<<","<<server.EstiShl()<<","<<server.EstiCL1()<<","<<server.f1sketch.originNDV()<<","<<server.f1sketch.queryf1()<<endl;
}
