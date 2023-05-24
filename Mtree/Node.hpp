#ifndef NODE_H_
#define NODE_H_
#include<vector>
#include<cmath>
using namespace std;

struct Node
{
    constexpr static double eps=1e-5;
    vector<double> data;
    int idx,dim;
    double loss;
    auto begin() const
    {
        return data.begin();
    }
    auto end() const
    {
        return data.begin() + dim;
    }
    size_t size()
    {
        return data.size();
    }
    Node(vector<double> dt,int id):data(dt),idx(id){
        dim = (int)dt.size();
    }
    void get_dim(double e)
    {
        loss=0;
        double e2=e*e;
        int idx=(int)data.size();
        while(idx)
        {
            if(loss+data[idx-1]*data[idx-1]>e2) break;
            loss+=data[idx-1]*data[idx-1];
            idx--;
        }
        loss=sqrt(loss);
        dim=idx;
    }
    Node(){}
    bool operator<(const Node &t)const{
        for(int i=0;i<dim;i++)
            if(fabs(data[i]-t.data[i])>eps)
                return data[i]<t.data[i];
        return false;
    }
    bool operator==(const Node &t)const{
        for(int i=0;i<dim;i++)
            if(fabs(data[i]-t.data[i])>eps)
                return false;
        return true;
    }
};

#endif
