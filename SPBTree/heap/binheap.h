#ifndef __BINHEAP_H
#define __BINHEAP_H

class BinHeapEntry
{
public:
	long long leftcnt;										//how many nodes in total in the left subtree
	long long rightcnt;										//in the right
	BinHeapEntry *leftchild;								
	BinHeapEntry *rightchild;	
	BinHeapEntry *parent;	
	void *data;

	//-----functions-----
	BinHeapEntry();
	~BinHeapEntry();
	BinHeapEntry * crude_insert(BinHeapEntry * _new_e);
	BinHeapEntry * crude_delete();
	void recursive_data_wipeout(void (*_destroy_data)(const void *_e));
};

typedef BinHeapEntry * BinHeapEntryptr;

class BinHeap
{
public:
	
	BinHeapEntry *root;
	long long (*compare_func)(const void *_e1, const void *_e2);
	void (*destroy_data)(const void *_e);

	//-----functions-----
	BinHeap();
	~BinHeap();
	void adjust_upward(BinHeapEntry *_he);
	void adjust_downward();
	virtual long long compare(const void *_e1, const void *_e2);
	void insert(BinHeapEntry *_he);
	BinHeapEntry * remove();
	void swap_data(BinHeapEntry *_e1, BinHeapEntry *_e2);
};

#endif