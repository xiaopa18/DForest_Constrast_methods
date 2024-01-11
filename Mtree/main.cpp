#include<iostream>
#include<cstring>
#include<algorithm>
#include<fstream>
#include<string>
#include<time.h>
#include"functions.hpp"
#include"mtree.hpp"
#include"monitoring.hpp"
#include"./Node.hpp"
#include<chrono>
using namespace std;
using namespace mt;
using namespace std::chrono;
double compDists;
typedef mtree<vector<double>,mt::functions::euclidean_distance,
		mt::functions::split_function<mt::functions::random_promotion,mt::functions::balanced_partition>>
		Mtree;

vector<vector<double>> read(string file_name)
{
    ifstream inf(file_name,ios::in);
    if(!inf.is_open())
    {
        cout<<"open "<<file_name<<" failed"<<endl;
        exit(-1);
    }
    vector<vector<double>> res;
    string tmp;
    while(getline(inf,tmp))
    {
        vector<double> vc;
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
    int mn=atoi(argv[3]);
    int mx=atoi(argv[4]);
    int querycount=atoi(argv[5]);
    int knncount=atoi(argv[6+querycount]);
    //cout<<"sad"<<endl;
    vector<vector<double>> dataset=read("../../data_set/"+dataid+"/"+dataid+"_afterpca.csv");
    vector<vector<double>> queryset=read("../../data_set/"+dataid+"/"+dataid+"_"+queryid+"_afterpca.csv");
    auto tst=steady_clock::now(),ted=steady_clock::now();
    int current_pid=GetCurrentPid();
    auto mem_use=-GetMemoryUsage(current_pid);
    tst=steady_clock::now();
    Mtree mtree(mn,mx);
    for(int i=0;i<dataset.size();i++)
    {
        //cout<<i<<"\n";
        mtree.add(dataset[i]);
    }
    cout<<"build over\n";
    ted=steady_clock::now();
    mem_use+=GetMemoryUsage(current_pid);
    ofstream ouf("./record/"+dataid+"_rangequery.csv",ios::app);
    if(!ouf.is_open())
    {
        cout<<"open ./record/" + dataid + "_rangequery.csv" + "failed\n";
        exit(-1);
    }
    ouf.setf(ios::fixed);
    double buildEnd=duration_cast<microseconds>(ted - tst).count()/1000.0;
    for(int i=0;i<querycount;i++)
    {
        compDists=0;
        double r=stod(string(argv[6+i]));
        double sum=0;
        double tim=-clock();
        for(auto &tmp:queryset)
        {
            auto p=mtree.get_nearest_by_range(tmp,r);
            for(auto it=p.begin();it!=p.end();it++) sum++;
        }
        tim+=clock();
        ouf<<dataid<<","<<queryid<<",Mtree,"<<"mn:"<<mn<<" mx:"<<mx<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<r<<","<<sum/queryset.size()<<","
                <<compDists/queryset.size()<<","<<tim/queryset.size()/1000<<"ms"<<endl;
        cout<<dataid<<","<<queryid<<",Mtree,"<<"mn:"<<mn<<" mx:"<<mx<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<r<<","<<sum/queryset.size()<<","
                <<compDists/queryset.size()<<","<<tim/queryset.size()/1000<<"ms"<<endl;
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
        compDists=0;
        double rad=0,dis=0;
        int k=atoi(argv[7+querycount+i]);
        double tim=-clock();
        for(auto &tmp:queryset)
        {
            dis=0;
            auto p=mtree.get_nearest_by_limit(tmp,k);
            for(auto it=p.begin();it!=p.end();it++) dis=max(dis,it->distance);
            rad+=dis;
        }
        tim+=clock();
        ouf<<dataid<<","<<queryid<<",Mtree,"<<"mn:"<<mn<<" mx:"<<mx<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<k<<","<<rad/queryset.size()<<","
                <<compDists/queryset.size()<<","<<tim/queryset.size()/1000<<"ms"<<endl;
        cout<<dataid<<","<<queryid<<",Mtree,"<<"mn:"<<mn<<" mx:"<<mx<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<k<<","<<rad/queryset.size()<<","
                <<compDists/queryset.size()<<","<<tim/queryset.size()/1000<<"ms"<<endl;
    }
    ouf.close();
    return 0;
}
