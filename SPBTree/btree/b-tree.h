#ifndef __B_Tree
#define __B_Tree

#include "b-def.h"
#include "../blockfile/cache.h"
//-----------------------------------------------

class BlockFile;
class B_Entry;
class B_Node;
class Cache;
class Heap;

class B_Tree : public Cacheable
{
public:
	//--==write to disk==--
	long long root;
	long long keysize;													//size of the key array of each entry

	//--== others ==--
	B_Node *root_ptr;

	//--=== for debugging ===--
	long long last_leaf;
	long long last_right_sibling;
	long long debug_info[100];
	long long quiet;														//control how often info is displayed
																	//higher value means fewer msgs
	bool emergency;

	//--== functions ==--
	B_Tree();
	virtual ~B_Tree();
	virtual void add_fname_ext(char * _fname);
	virtual void bulkload(char *_fname);
	virtual long long bulkload2(void *_ds, long long _n);
	virtual void build_from_file(char *_dsname);
	virtual void close();
	virtual bool delete_entry(B_Entry * _del_e);
	virtual void delroot();
	virtual void fread_next_entry(FILE *_fp, B_Entry *_d);
	virtual void init(char *_fname, long long _b_length, Cache *_c, long long _keysize);
	virtual void init_restore(char *_fname, Cache *_c);
	virtual void insert(B_Entry *_new_e);
	virtual void load_root();
	virtual B_Node* read_node(long long ptr);
	virtual B_Entry *new_one_entry();
	virtual B_Node *new_one_node();
	virtual long long read_header(char *_buf);
	virtual long long read_next_entry(void **_ptr, B_Entry *_d){return 1;};
	virtual long long traverse(double *_info);
	virtual long long write_header(char *_buf);
};

//-----------------------------------------------

#endif