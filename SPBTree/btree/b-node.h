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
	int num_entries;
	int left_sibling;
	int right_sibling;
	B_Entry **entries;


	int* min;
	int* max;

	//--==others==--
	bool dirty;
	int block;
	int capacity;
	B_Tree *my_tree;

	//--==functions==--
	B_Node();
	virtual ~B_Node();

	virtual void add_new_child(B_Node *_cnd);
	virtual bool chk_undrflw();
	virtual bool chk_ovrflw();
	virtual int choose_subtree(B_Entry *_new_e);
	virtual BDEL delete_entry(B_Entry * _del_e);
	virtual void enter(B_Entry *_new_e);
	virtual int get_header_size();
	virtual B_Entry ** get_entries(int _cap);
	virtual B_Node* get_left_sibling();
	virtual B_Node* get_right_sibling();
	virtual void init(int _level, B_Tree *_B_Tree);
	virtual void init(B_Tree *_B_Tree, int _block);
	virtual BINSRT insert(B_Entry *_new_e, B_Node **_new_nd);
	virtual int max_lesseq_key_pos(unsigned* _key);
	virtual int max_lesseq_key_pos(B_Entry *_e);
	virtual B_Entry *new_one_entry();
	virtual B_Node *new_one_node();
	int ptqry_next_follow(B_Entry *_e);
	virtual void read_from_buffer(char *_buf);
	virtual void rmv_entry(int _pos);
	virtual void rmv_entry_in_memory(int _pos); 
	virtual int traverse(double *_info);
	virtual void trt_ovrflw(B_Node **_new_nd);
	virtual void trt_undrflw(int _follow);
	virtual void write_to_buffer(char *_buf);
	virtual bool find_key(B_Entry* _k);
	virtual int find_key(unsigned * key, unsigned* next_key, vector<int> * v);
	virtual void print();
	virtual int compareint(unsigned * a, unsigned *b,int s);
	
};

//-----------------------------------------------

#endif