#include<iostream>
#include<cstring>
#include<algorithm>
#include<fstream>
#include<string>
#include<time.h>
#include"GNAT.hpp"
#include"db.hpp"
#include"index.hpp"
#include"monitoring.hpp"
#include<chrono>
using namespace std;
using namespace std::chrono;
double compDists;

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
    int pn=atoi(argv[3]);
    int querycount=atoi(argv[4]);
    int knnconut=atoi(argv[5+querycount]);
    vector<vector<double>> dataset=read("../data_set/"+dataid+"/"+dataid+"_afterpca.csv");
    vector<vector<double>> queryset=read("../data_set/"+dataid+"/"+dataid+"_"+queryid+"_afterpca.csv");
    printf("read dataset queryset over\n");
    auto tst=steady_clock::now(),ted=steady_clock::now();
    int current_pid=GetCurrentPid();
    auto mem_use=-GetMemoryUsage(current_pid);
    tst=steady_clock::now();
    L2_db_t *db=new L2_db_t();
    db->read(dataset);
    GNAT_t gnat(db,pn,100000000);
    gnat.build();
    ted=steady_clock::now();
    mem_use+=GetMemoryUsage(current_pid);
    double buildEnd=duration_cast<microseconds>(ted - tst).count()/1000.0;
    printf("build over\n");
    ofstream ouf("./record/"+dataid+"_rangequery.csv",ios::app);
    if(!ouf.is_open())
    {
        cout<<"open ./record/" + dataid + "_rangequery.csv" + "failed\n";
        exit(-1);
    }
    ouf.setf(ios::fixed);
    for(int i=0;i<querycount;i++)
    {
        compDists=0;
        double r=stod(string(argv[5+i]));
        double tim=0;
        double sum=0;
        for(auto &tmp:queryset)
        {
            vector<int> res;
            tst=steady_clock::now();
            gnat._rangeSearch(&(gnat.root),tmp,r,&res);
            ted=steady_clock::now();
            tim+=duration_cast<microseconds>(ted - tst).count()/1000.0;
            sum+=res.size();
        }
        cout<<dataid<<","<<queryid<<",GNAT,"<<"pn:"<<pn<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<r<<","<<sum/queryset.size()<<","
                <<compDists/queryset.size()<<","<<tim/queryset.size()<<"ms"<<endl;
        ouf<<dataid<<","<<queryid<<",GNAT,"<<"pn:"<<pn<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<r<<","<<sum/queryset.size()<<","
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
    for(int i=0;i<knnconut;i++)
    {
        int k=stoi(argv[6+querycount+i]);
        compDists = 0;
        double rad=0;
        clock_t start = clock();
        for (int j = 0; j < queryset.size();j++) {
            priority_queue<PDI> q;
            gnat._knnSearch(&(gnat.root),queryset[j],k,q);
            rad+=q.top().first;
        }
        clock_t times = clock() - start;
        ouf<<dataid<<","<<queryid<<",GNAT,"<<"pn:"<<pn<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<k<<","<<rad/queryset.size()<<","
            <<compDists/queryset.size()<<","<<1.0*times/queryset.size()<<"ms"<<endl;
        cout<<dataid<<","<<queryid<<",GNAT,"<<"pn:"<<pn<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<k<<","<<rad/queryset.size()<<","
            <<compDists/queryset.size()<<","<<1.0*times/queryset.size()<<"ms"<<endl;
    }
    ouf.close();
    return 0;
}
