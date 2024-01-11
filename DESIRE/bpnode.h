#pragma once
#ifndef __bpnode
#define __bpnode

#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>
#include <cmath>
#include "object.h"
using namespace std;
extern RAF* Obj_f;
extern long long* metricm;
class bpnode
{
public:	
	long long metric;
	long long objloc;
	long long len;
	long long son;
	char* datas;
	float* datad;
	float disl,disr;
	float dis;
	void outnode(string str);
	long long getsize();
	long long write_to_buffer(char* buffer);
	long long read_from_buffer(char* buffer);
	bpnode();
	void copy(bpnode* m);
	float distance(bpnode* m);
	bpnode(long long m, Object* o);
	~bpnode();
};

#endif