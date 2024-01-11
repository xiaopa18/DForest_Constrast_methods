/* this file implements the class Heap */

#include "heap.h"
#include "../func/gendef.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

//------------------------------------------
HeapEntry::HeapEntry()
{
}
//------------------------------------------
HeapEntry::~HeapEntry()
{
}
//------------------------------------------
void HeapEntry::init_HeapEntry(long long _dim)
{
	dim = _dim;
}
//------------------------------------------
void HeapEntry::copy(HeapEntry *_he)
{
	key = _he -> key;
	level = _he -> level;
	son1 = _he -> son1;
	son2 = _he -> son2;
}
//------------------------------------------
//------------------------------------------
//------------------------------------------
Heap::Heap()
{
	cont = NULL;
}
//------------------------------------------
Heap::~Heap()
{
	printf("maximum heap entry=%lld\n", maxused);
	delete [] cont;
	cont = NULL;
}
//------------------------------------------
void Heap::enter(HeapEntry *_he, long long _pos)
//this function enters a new entry into the heap at position _pos
{

	for (long long i = used - 1; i >= _pos; i --)
	{
		cont[i + 1].copy(&(cont[i]));
	}
	cont[_pos].copy(_he);
	used ++;

	if (maxused<used)
		maxused=used;
}
//------------------------------------------
void Heap::insert(HeapEntry *_he)
{
	long long pos = used;  //pos is the position _he will be inserted

	enter(_he, pos);
	// now perform swapping
	pos++;
	long long parent = pos;
	while (parent != 1)
	{
		long long child = parent;
		parent /= 2;
		if (cont[parent - 1].key > cont[child - 1].key)
		{
			HeapEntry *the = new HeapEntry();
			the -> init_HeapEntry(cont[parent - 1].dim);
			the -> copy(&(cont[parent - 1]));
			cont[parent - 1].copy(&(cont[child - 1]));
			cont[child - 1].copy(the);
			delete the;
		}
		else 
			parent = 1;
	}

	if (used > hsize)  
		//this is why the heap size is initiated with one more place than hsize (to facilitate
		//coding)
	{
		error("heap exceeded...\n", true);
	}
}
//------------------------------------------
bool Heap::remove(HeapEntry *_he)
//this function deheaps an entry. the return value indicates whether successful: false
//means heap is already empty
{
	if (used==0) 
		return false;
	_he -> copy(&(cont[0]));
	used--;
	cont[0].copy(&(cont[used]));
	long long parent = 1;
	while (2 * parent <= used)
	{
		long long child = 2 * parent;
		if (2 * parent + 1 > used)
			child = 2 * parent;
		else
			if (cont[2 * parent - 1].key < cont[2 * parent].key)
				child = 2 * parent;
			else 
				child = 2 * parent + 1;

		if (cont[parent - 1].key > cont[child - 1].key)
		{
			HeapEntry *the = new HeapEntry();
			the -> init_HeapEntry(cont[parent - 1].dim);
			the -> copy(&(cont[parent - 1]));
			cont[parent - 1].copy(&(cont[child - 1]));
			cont[child - 1].copy(the);
			delete the;
			parent = child; 
		}
		else
			parent = used;
	}
	return true;
};
//------------------------------------------
void Heap::clean(float _dist)
//this function cleans those entries with keys greater thab _dist
//in the memory
{
	for (long long i = 0; i < used; i ++)
	{
		if (cont[i].key > _dist)
			used = i;
	}
}

/*****************************************************************
this function checks the integrity of the heap. it is an auxiliary
function for debugging
*****************************************************************/

bool Heap::check()
{
	for (long long i = 0; i < used; i ++)
	{
		if (cont[i].son1<0 || cont[i].son2<0)
			return false;
	}
	return true;
}
//------------------------------------------
void Heap::init(long long _dim, long long _hsize)
{
	if (cont)
		delete [] cont;
	hsize = _hsize;
	cont = new HeapEntry [hsize + 1];   // +1 is to facilitate insert
	for (long long i = 0; i < hsize + 1; i ++)
		cont[i].init_HeapEntry(_dim);
	used = 0;
	maxused=0;
}
//------------------------------------------
	