#include<iostream>
#include"./deal_data_functions.hpp"
#include"./hnswlib/hnswlib.h"
#include"./monitoring.hpp"
#include<chrono>
using namespace std;
using namespace std::chrono;
using namespace hnswlib;
string dataid="trevi",queryid="uniform1000";
vector<int> ks{10,20,30,40,50};
int M=32,ef=200;
double dcmp;

HierarchicalNSW<float>* create_hnsw(vector<vector<float>> &dataset)
{
    L2Space *tmp = new L2Space(dataset[0].size());
    HierarchicalNSW<float>* alg_hnsw = new HierarchicalNSW<float>(tmp,2*dataset.size(),M,ef);
    int base=dataset.size()/10,i=0;
    for(int i=0;i<dataset.size();i++)
    {
        if(i%base==0) cout<<"build: "<<100.00*i/dataset.size()<<"%\n";
        alg_hnsw->addPoint(dataset[i].data(),i);
    }
    return alg_hnsw;
}

vector<vector<int>> restore_tag(vector<int> tag,int k)
{
    if(tag.size()%k)
    {
        cout<<"tag error"<<endl;
        exit(-1);
    }
    vector<vector<int>> res;
    for(int i=0;i<tag.size();i+=k)
    {
        vector<int> now;
        for(int j=0;j<k;j++)
            now.push_back(tag[i+j]);
        res.push_back(now);
    }
    return res;
}

int main(int argc,char **argv)
{
    if(argc!=1)
    {
        dataid=argv[1],queryid=argv[2];
        M=stoi(argv[3]),ef=stoi(argv[4]);
    }
    int current_pid = GetCurrentPid();
    vector<vector<float>> data,query;
    string data_file="../data_set/"+dataid+"/"+dataid+".csv";
    string query_file="../data_set/"+dataid+"/"+dataid+"_"+queryid+".csv";
    ofstream ouf("./record.csv",ios::out|ios::app);
    ouf.setf(ios::fixed);
    read_csv(data_file,data);
    read_csv(query_file,query);
    auto tist=steady_clock::now(),tied=steady_clock::now();
    double mem_use=-GetMemoryUsage(current_pid);
    tist=steady_clock::now();
    HierarchicalNSW<float>* hnsw=create_hnsw(data);
    tied=steady_clock::now();
    mem_use+=GetMemoryUsage(current_pid);
    double time=duration_cast<milliseconds>(tied - tist).count();
    printf("build time: %fs\n",time/1000);
    for(int k:ks)
    {
        double ave_recall=0,ave_time=0;
        dcmp=0;
        string tag_file="../data_set/"+dataid+"/knn_"+to_string(k)+".data";
        vector<int> tags;
        read_tag(tag_file,tags);
        vector<vector<int>> tag=restore_tag(tags,k);
        for(int i=0;i<query.size();i++)
        {
            tist=steady_clock::now();
            auto tmp=hnsw->searchKnn(query[i].data(),k);
            tied=steady_clock::now();
            ave_time+=duration_cast<microseconds>(tied - tist).count();
            vector<int> res;
            while(tmp.size())
            {
                res.push_back(tmp.top().se);
                tmp.pop();
            }
            ave_recall+=recall(res,tag[i]);
        }
        ave_recall/=query.size();
        ave_time/=1000*query.size();
        ouf<<dataid<<","<<queryid<<",hnsw,"<<"M:"<<M<<" ef:"<<ef<<","<<time/1000<<"s,"<<mem_use<<"MB,"<<k<<","<<ave_recall*100<<","<<dcmp/query.size()
            <<","<<ave_time<<endl;
        printf("k:%d recall:%.2f time:%fms\n",k,ave_recall*100,ave_time);
    }
    return 0;
}
