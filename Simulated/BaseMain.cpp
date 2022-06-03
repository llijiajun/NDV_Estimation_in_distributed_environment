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
        int file_k,k;
        sscanf(argv[2],"%d",&file_k);
        sscanf(argv[3],"%d",&k);
        cout<<"filepath, Number Of Files, Number Of Machines"<<endl;
        cout<<file<<","<<file_k<<","<<k<<endl;

	vector<string> filelist;
//	string file="dataset/poi/sample-0.01-poi-50-5kww-499999881800-10208520650-";
//	string file="dataset/poi/sample-0.01-poi-100-5bww-49999970055-505102835-";
	cout<<"baseline of "<<file<<", file number is: "<<file_k<<", machine number is: "<<k<<endl;

	for(int i=0;i<file_k;i++){
		string filename=file+to_string(i)+".txt";
		filelist.push_back(filename);
	}
	cout<<"begin"<<endl;
	BaseMaster serverbase(filelist,k,0.01);
	cout<<"GEE | Chao | Shlosser| CL1 | d | f1 "<<endl;
	cout<<serverbase.GEE(0.01)<<","<<serverbase.Chao()<<","<<serverbase.Shlosser(0.01)<< ","<<serverbase.CL1()<<","<<serverbase.d<<","<<serverbase.samplefi[1]<<endl;
}
