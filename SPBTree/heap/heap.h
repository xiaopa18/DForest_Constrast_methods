/* this file contains implementation of the class Heap */
#ifndef HEAP_H
#define HEAP_H

#define MAX_HEAP_SIZE 100000

class HeapEntry;
class Heap;

class HeapEntry
{
public:
	long long dim;
	long long level;
	long long son;
	double key;

	//-----functions-----
	HeapEntry();
	virtual ~HeapEntry();
	virtual void init_HeapEntry(long long _dim);
	virtual void copy(HeapEntry *_he);
};

typedef HeapEntry * HeapEntryptr;

class Heap
{
public:
	long long b;            // needed by HB for access condition
	long long hsize;        // the heap size
	long long used;         // number of used places
	long long maxused;
	HeapEntryptr *cont;  // content of the heap

	//-----functions-----
	Heap();
	virtual ~Heap();
	virtual bool check();
	virtual void enter(HeapEntry *_he, long long _pos);
	virtual void init(long long _dim, long long _hsize=MAX_HEAP_SIZE);
	virtual void insert(HeapEntry *_he);
	virtual HeapEntry * new_one_HeapEntry();
	virtual bool remove(HeapEntry *_he);
};

#endif