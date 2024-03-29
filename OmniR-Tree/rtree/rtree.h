/* rtree.h
   this file defines the class RTree*/

#ifndef __RTREE
#define __RTREE
//------------------------------------------------------------
#include "../func/gendef.h"
#include <string>
#include<map>
using namespace std;

//------------------------------------------------------------
class LinList;
class SortedLinList;
class Cache;
class RTNode;
class Entry;
//------------------------------------------------------------
class RTree : public Cacheable
{
public:
//--===on disk===--
	long long dimension;                       
	long long num_of_data;	                 
    long long num_of_dnodes;	                 
    long long num_of_inodes;	                 
	long long root;                            
	bool root_is_data;                   
//--===others===--
	RTNode *root_ptr;
    bool *re_level;  
    LinList *re_data_cands; 
	LinList *deletelist;
//--===functions===--
	RTree(char *fname, long long _b_length, Cache* c, long long _dimension);
    RTree(char *fname, Cache* c);
    RTree(char *inpname, char *fname, long long _blength, Cache* c, long long _dimension,map<long long,string> * keywords);
	RTree(char *inpname, char *fname, long long _blength, Cache* c, long long _dimension);
    ~RTree();

	bool delete_entry(Entry *d);
	bool FindLeaf(Entry *e);
    long long get_num() { return num_of_data; }
	void insert(Entry *d);
	void load_root();  
	void NNQuery(float *QueryPoint, SortedLinList *res,string query,map<long long,string> * keywords,long long k);
	void rangeQuery(float *mbr, SortedLinList *res);
	void read_header(char *buffer);      
	void write_header(char *buffer);    
};

#endif // __RTREE
