#pragma once

#include<vector>
#include<fstream>
#include<cmath>
#include<algorithm>
#include<fstream>
#include<queue>
#include<set>
#include<unordered_set>
#include<iostream>
#define fi first
#define se second

using namespace std;
typedef pair<double,int> PDI;

void read_csv(string file_name,vector<vector<string>> &res)
{
    ifstream inf(file_name,ios::in);
    if(!inf.is_open())
    {
        cout<<file_name<<"can't open\n";
        exit(-1);
    }
    string line;
    while(getline(inf,line))
    {
       string tmp="";
       vector<string> now;
       for(char ch:line)
       {
           if(ch==',')
           {
               now.push_back(tmp);
               tmp="";
           }else tmp.push_back(ch);
       }
       if(tmp!="") now.push_back(tmp);
       if(now.size()) res.push_back(now);
    }
}

void read_csv(string file_name,vector<vector<float>> &res)
{
    ifstream inf(file_name,ios::in);
    if(!inf.is_open())
    {
        cout<<file_name<<"can't open\n";
        exit(-1);
    }
    string line;
    while(getline(inf,line))
    {
       string tmp="";
       vector<float> now;
       for(char ch:line)
       {
           if(ch==',')
           {
               now.push_back(stof(tmp));
               tmp="";
           }else tmp.push_back(ch);
       }
       if(tmp!="")
       {
           try{
               float x=stof(tmp);
               now.push_back(x);
           }catch(...){}
       }
       if(now.size()) res.push_back(now);
    }
}

vector<string> splitby(string s,char ch)
{
    string tmp="";
    vector<string> res;
    for(char &c:s)
    {
        if(c==ch)
        {
            if(tmp!="") res.push_back(tmp);
            tmp="";
        }else tmp.push_back(c);
    }
    if(tmp!="") res.push_back(tmp);
    return res;
}

double recall(vector<int> &a,vector<int> &b)
{
    unordered_set<int> S;
    int fz=0;
    for(int i=0;i<a.size();i++) S.insert(a[i]);
    for(int i=0;i<b.size();i++)
        if(S.count(b[i]))
            fz++;
    return 1.0*fz/a.size();
}

void write_data(string file_name,vector<vector<float>> &data)
{
    ofstream ouf(file_name,ios::out|ios::binary);
    int n=data.size(),dim=data[0].size();
    ouf.write((char*)&n,sizeof n);
    ouf.write((char*)&dim,sizeof dim);
    for(int i=0;i<n;i++)
        for(int j=0;j<dim;j++)
            ouf.write((char*)&data[i][j],sizeof data[i][j]);
    ouf.close();
}

void write_data(string file_name,vector<vector<string>> &data)
{
    vector<vector<float>> dt;
    for(int i=0;i<data.size();i++)
    {
        vector<float> tmp;
        for(int j=0;j<data[0].size();j++)
        {
            tmp.push_back(stof(data[i][j]));
        }
        dt.push_back(tmp);
    }
    write_data(file_name,dt);
}

void csv_to_data(string file_name)
{
    vector<vector<string>> data;
    read_csv(file_name,data);
    while(file_name.back()!='.') file_name.pop_back();
    file_name+="data";
    write_data(file_name,data);
}

bool read_data(string file_name,vector<vector<float>> &res)
{
    ifstream inf(file_name,ios::in|ios::binary);
    //cout<<file_name<<endl;
    if(!inf.is_open())
    {
        cout<<file_name<<" open failed!\n";
        return false;
    }
    int n,dim;
    inf.read((char*)&n,sizeof n);
    inf.read((char*)&dim,sizeof dim);
   // cout<<"asd:"<<n<<" "<<dim<<endl;
    for(int i=0;i<n;i++)
    {
        vector<float> tmp(dim,0);
        for(int j=0;j<dim;j++)
            inf.read((char*)&tmp[j],sizeof tmp[j]);
        res.push_back(tmp);
    }
    return true;
}

bool read_tag(string file_name,vector<int> &res)
{
    ifstream inf(file_name,ios::in|ios::binary);
    //cout<<file_name<<endl;
    if(!inf.is_open())
    {
        cout<<file_name<<" open failed!\n";
        return false;
    }
    int x;
    while(inf.read((char*)&x,sizeof x)) res.push_back(x);
    return true;
}

