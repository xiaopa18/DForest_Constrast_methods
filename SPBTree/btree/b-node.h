/* this class defines class B_Node  */

#ifndef _B_Node
#define _B_Node

#include "b-def.h"
#include <vector>
using namespace std;
//-----------------------------------------------

class B_Tree;
class B_Entry;



class B_Node
{
public:
	//--==disk residential variables==--
	char level;
	long long num_entries;
	long long left_sibling;
	long long right_sibling;
	B_Entry **entries;


	long long* min;
	long long* max;

	//--==others==--
	bool dirty;
	long long block;
	long long capacity;
	B_Tree *my_tree;

	//--==functions==--
	B_Node();
	virtual ~B_Node();

	virtual void add_new_child(B_Node *_cnd);
	virtual bool chk_undrflw();
	virtual bool chk_ovrflw();
	virtual long long choose_subtree(B_Entry *_new_e);
	virtual BDEL delete_entry(B_Entry * _del_e);
	virtual void enter(B_Entry *_new_e);
	virtual long long get_header_size();
	virtual B_Entry ** get_entries(long long _cap);
	virtual B_Node* get_left_sibling();
	virtual B_Node* get_right_sibling();
	virtual void init(long long _level, B_Tree *_B_Tree);
	virtual void init(B_Tree *_B_Tree, long long _block);
	virtual BINSRT insert(B_Entry *_new_e, B_Node **_new_nd);
	virtual long long max_lesseq_key_pos(unsigned long long* _key);
	virtual long long max_lesseq_key_pos(B_Entry *_e);
	virtual B_Entry *new_one_entry();
	virtual B_Node *new_one_node();
	long long ptqry_next_follow(B_Entry *_e);
	virtual void read_from_buffer(char *_buf);
	virtual void rmv_entry(long long _pos);
	virtual void rmv_entry_in_memory(long long _pos); 
	virtual long long traverse(double *_info);
	virtual void trt_ovrflw(B_Node **_new_nd);
	virtual void trt_undrflw(long long _follow);
	virtual void write_to_buffer(char *_buf);
	virtual bool find_key(B_Entry* _k);
	virtual long long find_key(unsigned long long * key, unsigned long long* next_key, vector<long long> * v);
	virtual void print();
	virtual long long compareint(unsigned long long * a, unsigned long long *b,long long s);
	
};

//-----------------------------------------------

#endif