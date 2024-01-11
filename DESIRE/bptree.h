#ifndef __bptree
#define __bptree

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
#include "iopack/RAF.h"
#include "iopack/blk_file.h"
#include "object.h"
#include "bpnode.h"
using namespace std;
extern long long f;
extern float eps;
class bptree //: public Cacheable
{
public:
	/*






	void delnode();
	void update();

	*/
	//bptree* querydis(float d);
	long long block, dirty, fa, isleaf, leafnum;
	float dis;
	float tmpdis;
	bpnode** son;
	void outnode(string str);
	long long getsize();
	long long write_to_buffer(char* buffer);
	long long read_from_buffer(char* buffer);
	void flush();
	void bfs(string str);
	void loadroot(long long page);
	void insert(bpnode* q);
	bptree* prenode(long long loc);
	bptree* lastnode();
	void delnode(long long loc);
	void update();
	bptree();
	~bptree();

	void entere(bpnode* q);
	void split(long long flag,bptree* rt);

};

#endif
