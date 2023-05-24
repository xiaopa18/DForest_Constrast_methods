#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <sys/time.h>
#include <unordered_map>
#include <cmath>
#include <vector>
#include "BBTree.h"
#include <ctime>
#include"./monitoring.hpp"
#include<chrono>
using namespace std;
using namespace std::chrono;
double compDists;
vector<vector<float>> read(string name)
{
    ifstream inf(name,ios::in);
    if(!inf.is_open())
    {
        cout<<"open "<<name<<" failed\n";
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

double dist(vector<float> &a,vector<float> &b)
{
    compDists++;
    double res=0,dx;
    for(int i=0;i<a.size();i++)
    {
        dx=a[i]-b[i];
        res+=dx*dx;
    }
    return sqrt(res);
}

int main(int argc, char **argv)
{
    ofstream ouf("./contrast_experiment_record.csv",ios::app);
    if(!ouf.is_open())
    {
        cout<<"open ./contrast_experiment_record.csv failed\n";
        exit(-1);
    }
    string dataid=string(argv[1]);
    string queryid=string(argv[2]);
    int querycount=atoi(argv[3]);
    auto tst=steady_clock::now(),ted=steady_clock::now();
    vector<vector<float>> dataset=read("../data_set/"+dataid+"/"+dataid+"_afterpca.csv");
    vector<vector<float>> queryset=read("../data_set/"+dataid+"/"+dataid+"_"+queryid+"_afterpca.csv");
    int threads=1,m=dataset[0].size();
    int current_pid=GetCurrentPid();
    auto mem_use=-GetMemoryUsage(current_pid);
    tst=steady_clock::now();
    BBTree bbtree(m, threads);
    for(int i=0;i<dataset.size();i++) bbtree.InsertObject(dataset[i],i);
    ted=steady_clock::now();
    mem_use+=GetMemoryUsage(current_pid);
    double buildEnd=duration_cast<microseconds>(ted - tst).count()/1000.0;
    for(int i=0;i<querycount;i++)
    {
        int sum=0;
        float r=stod(argv[4+i]);
        double tim=-clock();
        compDists=0;
        for(auto &q:queryset)
        {
            std::vector<uint32_t> res=bbtree.SearchFixedRadiusNN(q,r);
            for(auto it=res.begin();it!=res.end();it++)
            {
                if(dist(q,dataset[*it])<=r) sum++;
            }
        }
        tim+=clock();
        ouf<<dataid<<","<<queryid<<",BB-tree,null,"<<buildEnd<<"ms,"<<mem_use<<"MB,"<<r<<","<<sum<<","
                <<compDists<<","<<tim/queryset.size()<<"ms"<<endl;
        cout<<dataid<<","<<queryid<<",BB-tree,null,"<<buildEnd<<"ms,"<<mem_use<<"MB,"<<r<<","<<sum<<","
                <<compDists<<","<<tim/queryset.size()<<"ms"<<endl;
    }
    return 0;
}
