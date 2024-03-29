#ifndef __RTNODE
#define __RTNODE
//------------------------------------------------------------
#include "../func/gendef.h"
#include <string>
#include <map>
using namespace std;

//------------------------------------------------------------
class SortedLinList;
class Entry;
class RTree;
//------------------------------------------------------------
class RTNode
{
public:
//--===on disk===--
	char level; 
	long long block;
	long long num_entries;
	Entry *entries;
//--===others===--
	bool dirty;
	long long capacity;
    long long dimension;
	RTree *my_tree;  
//--===functions===--
	RTNode(RTree *rt);
    RTNode(RTree *rt, long long _block);
    ~RTNode();

    long long choose_subtree(float *brm);
	R_DELETE delete_entry(Entry *e); 
	void enter(Entry *de);
	bool FindLeaf(Entry *e);
	float *get_mbr();
	long long get_num_of_data();
	R_OVERFLOW insert(Entry *d, RTNode **sn);
	bool is_data_node() { return (level==0) ? TRUE : FALSE ;};
	void NNSearch(float *QueryPoint, SortedLinList *res,
				      float *nearest_distanz, string query,map<long long,string> *keywords,long long k);
	void NNSearch(float *QueryPoint, SortedLinList *res, float *nearest_distanz, long long k);
	void print();
	void rangeQuery(float *mbr, SortedLinList *res);
    void read_from_buffer(char *buffer);
	long long split(float **mbr, long long **distribution);
	void split(RTNode *sn);
    void write_to_buffer(char *buffer); 
};

#endif