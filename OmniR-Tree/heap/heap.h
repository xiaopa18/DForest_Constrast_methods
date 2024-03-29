/* this file contains implementation of the class Heap */
#ifndef HEAP_H
#define HEAP_H
#include <math.h>
#include "../rtree/rtnode.h";
#define MAX_HEAP_SIZE 5000000

class HeapEntry;
class Heap;

class TEntry
{
public:
	long long id;
	double key;

	TEntry()
	{
		id = -1;
		key = -1;
	}

	TEntry(const TEntry & se)
	{
		id = se.id;
		key = se.key;
	}

	bool operator < (const TEntry &a) const
	{
		if (key - a.key<-0.000000001)
			return true;
		else if (fabs(key - a.key)<0.000000001)
			return id<a.id;
		else
			return false;
	}
};

class NEntry
{
public:
	RTNode* node;
	bool flag;

	NEntry()
	{
		node = NULL;
	}
	NEntry(const NEntry & se)
	{
		node = se.node;
		flag = se.flag;
	}
	~NEntry()
	{
		node = NULL;
	}
	NEntry& operator = (const NEntry &a)
	{
		node = a.node;
		flag = a.flag;
		return *this;
	}

};


class KEntry
{
public:
	long long id;
	double key;
	bool isobject;

	KEntry()
	{
		id = -1;
		key = -1;
	}

	KEntry(const KEntry & se)
	{
		id = se.id;
		key = se.key;
		isobject = se.isobject;
	}

	bool operator < (const KEntry &a) const
	{
		if (key - a.key<-0.000000001)
			return false;
		else
			return true;
	}
};


class HeapEntry
{
public:
	long long dim;
	long long level;
	long long son1;
	long long son2;
	float key;

	//-----functions-----
	HeapEntry();
	~HeapEntry();
	void init_HeapEntry(long long _dim);
	void copy(HeapEntry *_he);
};

class Heap
{
public:
	long long b;            // needed by HB for access condition
	long long hsize;        // the heap size
	long long used;         // number of used places
	long long maxused;
	HeapEntry *cont;  // content of the heap

	//-----functions-----
	Heap();
	~Heap();
	bool check();
	void clean(float _dist);
	void enter(HeapEntry *_he, long long _pos);
	void init(long long _dim, long long _hsize=MAX_HEAP_SIZE);
	void insert(HeapEntry *_he);
	bool remove(HeapEntry *_he);
};

#endif