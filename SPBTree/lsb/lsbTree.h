#ifndef __LSBTree
#define __LSBTree

#include "lsb.h"
#include "../btree/b-tree.h"
//-----------------------------------------------

class B_Entry;
class B_Node;
class LSB;

class LSBtree : public B_Tree
{
public:
	long long		dim;

	//--==functions==--
	LSBtree();
	virtual ~LSBtree();

	virtual void add_fname_ext(char * _fname);
	virtual void fread_next_entry(FILE *_fp, B_Entry *_d);
	virtual long long init(char *_fname, long long _b_length, long long _dim, long long _keysize);
	virtual B_Entry *new_one_entry();
	virtual B_Node *new_one_node();
	virtual long long read_header(char *_buf);
	virtual long long read_next_entry(void **_ptr, B_Entry *_e);
	virtual long long write_header(char *_buf);
};

//-----------------------------------------------

#endif