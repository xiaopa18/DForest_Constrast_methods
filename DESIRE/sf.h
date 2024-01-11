#ifndef __sforest
#define __sforest
#include "object.h"
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>
#include <cmath>
#include<queue>
#include<cstdlib>


using namespace std;
extern long long f;
extern double IOread;
extern double IOwrite;
extern double IOtime;
class Treearr
{
public:
	long long tree;
	long long metric;
	long long len;
	long long tot;
	char* datas;
	float* datad;
	Treearr();
	Treearr(Object* o, long long m);
	void outnode();
	~Treearr();
};

class AnsLeaf
{
public:
	long long metric;
	long long obj;
	float dis;
	float radi;
	long long loc;
	Object* o;
	AnsLeaf(long long i, long long j, float k);
	~AnsLeaf();
};

class SF
{
public:
	vector<Treearr*> arr;
	vector<AnsLeaf*> knn(Object* que, long long metric, long long k);
	vector<AnsLeaf*> rnn(Object* que, long long metric, float r,long long fgg);
	//float dis;
	//float maxdis;
	SF();
	~SF();
	void readfrom(string s);
	void writeto(string s);
};



#endif
