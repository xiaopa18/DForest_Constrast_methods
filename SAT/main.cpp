#include<iostream>
#include<cstring>
#include<algorithm>
#include<fstream>
#include<string>
#include<time.h>
#include<vector>
#include"./sat.hpp"
#include"./monitoring.hpp"
#include<chrono>
using namespace std;
using namespace std::chrono;
double compDists;

vector<vector<float>> read(string file_name)
{
    ifstream inf(file_name,ios::in);
    if(!inf.is_open())
    {
        cout<<"open "<<file_name<<" failed"<<endl;
        exit(-1);
    }
    vector<vector<float>> res;
    string tmp;
    while(getline(inf,tmp))
    {
        vector<float> vc;
        string x="";
        for(char ch:tmp)
        {
            if(ch==',')
            {
                if(x!="") vc.push_back(stod(x));
                x="";
            }else x.push_back(ch);
        }
        if(x!="") vc.push_back(stod(x));
        res.push_back(vc);
    }
    return res;
}

int main(int argc, char **argv)
{
    string dataid=string(argv[1]);
    string queryid=string(argv[2]);
    int querycount=atoi(argv[3]);
    int knncount=atoi(argv[4+querycount]);
    vector<vector<float>> dataset=read("../../data_set/"+dataid+"/"+dataid+"_afterpca.csv");
    vector<vector<float>> queryset=read("../../data_set/"+dataid+"/"+dataid+"_"+queryid+"_afterpca.csv");
    printf("read dataset queryset over\n");
    auto tst=steady_clock::now(),ted=steady_clock::now();
    int current_pid=GetCurrentPid();
    auto mem_use=-GetMemoryUsage(current_pid);
    tst=steady_clock::now();
    SAT sat(dataset);
	ted=steady_clock::now();
    mem_use+=GetMemoryUsage(current_pid);
    double buildEnd=duration_cast<microseconds>(ted - tst).count()/1000.0;
    printf("SAT build over\n");
    ofstream ouf("./record/"+dataid+"_rangequery.csv",ios::app);
    if(!ouf.is_open())
    {
        cout<<"open ./record/" + dataid + "_rangequery.csv" + "failed\n";
        exit(-1);
    }
    ouf.setf(ios::fixed);
    //vector<float> q(dataset[0].size(),0);
    for(int i=0;i<querycount;i++)
    {
        compDists=0;
        double r=stod(string(argv[4+i]));
        double tim=0;
        double sum=0;
        for(int i=0;i<queryset.size();i++)
        {
            //for(int j=0;j<queryset[j].size();j++) q[j]=queryset[i][j];
            vector<PDI> res;
            tst=steady_clock::now();
            sat.rangequery(Objvector(queryset[i]),r,&res);
            ted=steady_clock::now();
            sum+=res.size();
            tim+=duration_cast<microseconds>(ted - tst).count()/1000.0;
        }
        ouf<<dataid<<","<<queryid<<",SAT,"<<"null"<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<r<<","<<sum/queryset.size()<<","
                <<compDists/queryset.size()<<","<<tim/queryset.size()<<"ms"<<endl;
        cout<<dataid<<","<<queryid<<",SAT,"<<"null"<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<r<<","<<sum/queryset.size()<<","
                <<compDists/queryset.size()<<","<<tim/queryset.size()<<"ms"<<endl;
    }
    ouf.close();

    ouf.open("./record/"+dataid+"_knn.csv",ios::app);
    if(!ouf.is_open())
    {
        cout<<"open ./record/" + dataid + "_knn.csv" + "failed\n";
        exit(-1);
    }
    ouf.setf(ios::fixed);
    for(int i=0;i<knncount;i++)
    {
        int k=stoi(argv[5+querycount+i]);
        compDists = 0;
        double rad=0;
        double tim=0;
        for(int i=0;i<queryset.size();i++)
        {
            tst=steady_clock::now();
            rad+=sat.knn(Objvector(queryset[i]),k);
            ted=steady_clock::now();
            tim+=duration_cast<microseconds>(ted - tst).count()/1000.0;
        }
        ouf<<dataid<<","<<queryid<<",SAT,"<<"null"<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<k<<","<<rad/queryset.size()<<","
                <<compDists/queryset.size()<<","<<tim/queryset.size()<<"ms"<<endl;
        cout<<dataid<<","<<queryid<<",SAT,"<<"null"<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<k<<","<<rad/queryset.size()<<","
                <<compDists/queryset.size()<<","<<tim/queryset.size()<<"ms"<<endl;
    }
    ouf.close();
}
