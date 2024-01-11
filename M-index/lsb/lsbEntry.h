/* this file defines class BEntry */

#ifndef __LSBEntry
#define __LSBEntry

#include "../btree/b-entry.h"

//-----------------------------------------------

class B_Node;
class B_Tree;
class LSBtree;
class LSBnode;

class LSBentry : public B_Entry
{
public:
	long long *pt;												//for storing the coordinates of a point
															//consider integer coordinates
	//--==functions==--
	LSBentry();
	virtual ~LSBentry();
	virtual void close();
	virtual bool equal_to(B_Entry *_e);
	virtual long long get_size(long long _level);
	virtual void init(B_Tree *_btree, long long _level);
	virtual B_Node *new_one_node();
	virtual long long read_from_buffer(char *_buf);
	virtual void set_from(B_Entry *_e);
	virtual long long write_to_buffer(char *_buf);
};

typedef LSBentry * LSBentryPtr;

//-----------------------------------------------

#endif