/* this file defines class B_Entry */

#ifndef __B_Entry
#define __B_Entry

//-----------------------------------------------

class B_Tree;
class B_Node;

class B_Entry
{
public:
	//--==write to disk==--

	unsigned long long *key;

	long long leafson;										//this field is needed only at nonleaf levels
														//(key, leafson) forms the search key. 
														//this is to ensure fast deletions when many leaf entries have
														//the same key.
	long long ptr;
	unsigned long long* min;
	unsigned long long* max;
	long long * mi;
	long long * ma;

	long long son;
	long long level; 

	//--==others==--
	B_Tree *my_tree;
	B_Node *son_ptr;	

	//--==functions==--
	B_Entry();
	virtual ~B_Entry();
	virtual void close();
	virtual long long compare_key(B_Entry *_e);
	virtual long long compare(B_Entry *_e);
	virtual void del_son();
	virtual bool equal_to(B_Entry *_e);
	virtual long long get_size(long long _level);
	virtual B_Node *get_son();
	virtual void init(B_Tree *_B_Tree, long long _level);
	virtual B_Node *new_one_node();
	virtual long long read_from_buffer(char *_buf);
	virtual void set_from(B_Entry *_e);
	virtual void set_from_child(B_Node *_nd);
	virtual long long write_to_buffer(char *_buf);
};

typedef B_Entry * B_Entryptr;

//-----------------------------------------------

#endif