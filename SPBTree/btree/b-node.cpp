/* this file implements class B_Node */

#include <math.h>
#include <memory.h>
#include "b-node.h"
#include "b-entry.h"
#include "b-tree.h"
#include "b-def.h"
#include "../blockfile/blk_file.h"
#include "../blockfile/cache.h"
#include "../gadget/gadget.h"


//-----------------------------------------------

B_Node::B_Node()
{
	block = -1;
	capacity = -1;
	dirty = false;
	entries = NULL;
	left_sibling = right_sibling = -1;
	level = -1;
	my_tree = NULL;
	num_entries = -1;

	min= NULL;
	max = NULL;
}

/*****************************************************************
use this function to init a new node on disk

para:
- level
- B_Tree: the instance of the B-tree that this node belongs to
*****************************************************************/

void B_Node::init(int _level, B_Tree *_B_Tree)
{
	my_tree = _B_Tree;
	int b_length = my_tree -> file -> get_blocklength();
	
	left_sibling = -1;
	right_sibling = -1;
	num_entries = 0;
	level = _level;

	min = NULL;
	max = NULL;

	B_Entry *e = new_one_entry();
	e->init(_B_Tree, _level);
	int entry_size = e->get_size(level);  
	e->close();
	delete e;

	capacity = (b_length - get_header_size()) / entry_size;
	if (capacity < 3)
	{
		printf("capacity = %d\n", capacity);
		error("b_node::init - capacity too small.\n", true);
	}
	entries = get_entries(capacity);

	char *blk = new char[b_length];
    block = my_tree -> file -> append_block(blk);
    delete [] blk;

	dirty = true;
}

/*****************************************************************
use this function to load a node from the disk disk

para:
- B_Tree: the instance of the B-tree that this node belongs to
- block: the address to load from
*****************************************************************/

void B_Node::init(B_Tree *_B_Tree, int _block)
{
    my_tree = _B_Tree;
    num_entries = 0;
	dirty = false;

	min = NULL;
	max = NULL;

    int header_size = get_header_size();
	int b_len = my_tree -> file -> get_blocklength();
    
    block = _block;
    char *blk = new char[b_len];
	{
        my_tree -> file -> read_block(blk, block);
	}

	memcpy(&level, blk, sizeof(level));
	B_Entry *e = new_one_entry();
	e->init(_B_Tree, level);
	int entry_size = e->get_size(level);  
	e->close();
	delete e;

	capacity = (b_len - header_size) / entry_size;
	if (capacity < 3)
	{
		printf("capacity = %d\n", capacity);
		error("b_node::init - capcity too small.\n", true);
	}
	entries = get_entries(capacity);

	read_from_buffer(blk);
    delete [] blk;
}

//-----------------------------------------------

B_Node::~B_Node()
{
	char *buf;
    if (dirty)
    {
        buf = new char[my_tree -> file -> get_blocklength()];
        write_to_buffer(buf);

        if (my_tree -> cache == NULL) // no cache
            my_tree -> file-> write_block(buf, block);
        else
            my_tree -> cache -> write_block(buf, block, my_tree);

        delete [] buf;
    }

	if (entries)
	{
		for (int i = 0; i < capacity; i ++)
		{
			if(entries[i])
			{
				entries[i]->close();
			}
			delete entries[i];
			entries[i] = NULL;
			
		}
	}

	if(min)
		delete[] min;
	if(max)
		delete[] max;

    delete [] entries;
	entries = NULL;
}

//-----------------------------------------------

void B_Node::add_new_child(B_Node *_cnd)
{
	B_Entry *e = new_one_entry();
	e->init(my_tree, level); 

	e->set_from_child(_cnd);

	enter(e);
	e->close();
	delete e;
	delete _cnd;
}

//-----------------------------------------------

bool B_Node::chk_ovrflw()
{
	if (num_entries == capacity - 1) return true;
	else return false;
}

//-----------------------------------------------

bool B_Node::chk_undrflw()
{
	if (my_tree->root_ptr->level==level)
		if (num_entries==1 && level>0)
			return true;
		else return false;
	if (num_entries <= (capacity-2)/2) return true;
	else return false;
}

//-----------------------------------------------

int B_Node::choose_subtree(B_Entry *_new_e)
{
	int follow = max_lesseq_key_pos(_new_e); 
	
	return follow;
}

//-----------------------------------------------

void B_Node::enter(B_Entry *_new_e)
{
	int pos = -1;
	
	pos = max_lesseq_key_pos(_new_e);
	pos ++;														//this is the position of the new key

	for (int i = num_entries; i > pos; i --)
	{
		entries[i]->set_from(entries[i - 1]);
	}

	entries[pos]->set_from(_new_e);
	num_entries ++;
	dirty = true;
}

//-----------------------------------------------

bool B_Node::find_key(B_Entry* _k)
{
	bool ret = false;

	if (level == 0)
	{
		for (int i = 0; i < num_entries; i ++)
		{
			if (entries[i]->key[0] = _k->key[0] && entries[i]->son == _k->leafson) 
			{ 
				ret = true; i = num_entries; 
				return ret;
			}
		
		}
	}
	int follow = max_lesseq_key_pos(_k);

	if (follow != -1)
	{
		B_Node *succ = entries[follow]->get_son();
		printf("%f %d\n",_k->key[0], follow);
		succ->print();
		ret = succ -> find_key(_k);		
		entries[follow]->del_son();
	}

	return ret;
}

int B_Node::compareint(unsigned * a, unsigned *b,int s)
{
	int f = 0;
	for(int i=0;i<s;i++)
	{
		if(a[i]>b[i])
		{
			f=1;
			break;
		}
		else if(a[i]<b[i])
		{
			f=-1;
			break;
		}
	}
	a=NULL;
	b=NULL;
	return f;
}

int B_Node::find_key(unsigned* key, unsigned * next_key, vector<int> * v)
{
	int flag = -1;	
	if (level == 0)
	{
		int tt =0;
		for (int i = 0; i < num_entries; i ++)
		{
	
			int t = compareint(key,entries[i]->key,my_tree->keysize);
			if(t==0)
				v->push_back(entries[i]->ptr);
			else if(t<0)
			{
				memcpy(next_key,entries[i]->key,my_tree->keysize*sizeof(unsigned));
				
				tt=-1;
				break;
			}
		}
		if(compareint(key,entries[0]->key,my_tree->keysize)==0)
		{
			B_Node* n = get_left_sibling();
			while(n!=NULL)
			{
				int i=-1;
				for( i=n->num_entries-1;i>=0;i--)
				{
					int t = compareint(key,n->entries[i]->key,my_tree->keysize);
					if(t==0)
						v->push_back(n->entries[i]->ptr);
					else
						break;
				}
				if(i>=0)
				{
					delete n;
					n=NULL;
				}
				else
				{
					B_Node* t = n;
					n=n->get_left_sibling();
					delete t;
					t=NULL;
				}
			}
		}
		if(tt<0)
			return 1;
		B_Node* n = get_right_sibling();
		while(n!=NULL)
		{
			for(int i =0;i<n->num_entries;i++)
			{
				int t = compareint(key,n->entries[i]->key,my_tree->keysize);
				if(t==0)
					v->push_back(n->entries[i]->ptr);
				else if(t<0)
				{
					memcpy(next_key,n->entries[i]->key,my_tree->keysize*sizeof(unsigned));
					delete n;
					return 1;
				} 
			}
			B_Node* t = n;
			n=n->get_right_sibling();
			delete t;
			t=NULL;
		}
		return 1;
	}
	
	int follow = max_lesseq_key_pos(key);

	if (follow != -1)
	{
		B_Node *succ = entries[follow]->get_son();
		
		flag = succ -> find_key(key,next_key, v);		
		entries[follow]->del_son();
	}
	else
		flag = 0;
	return flag;
}

void B_Node::print()
{
	if(level >0)
	{
		for(int i =0;i<num_entries;i++)
		{
			printf("%d %d\n", entries[i]->key[0], entries[i]->leafson);
		}
	}
	else
	{
		for(int i =0;i<num_entries;i++)
		{
			printf("%d %d %d \n", entries[i]->key[0], entries[i]->son, entries[i]->ptr, entries[i]->level);
		}
	}
	printf("\n");
}
//-----------------------------------------------

B_Entry ** B_Node::get_entries(int _cap)
{
	B_Entry **en = new B_Entryptr [_cap];
	for (int i = 0; i < _cap; i ++)
	{
		en[i] = new_one_entry();
		en[i]->init(my_tree, level);
	}

	return en;
}

//-----------------------------------------------

int B_Node::get_header_size()
{ 
	return sizeof(level) + sizeof(block) + sizeof(left_sibling) + sizeof(right_sibling);
}

//-----------------------------------------------

BINSRT B_Node::insert(B_Entry *_new_e, B_Node **_new_nd)
{
	BINSRT ret = B_NRML;
	if (level == 0)
	{
		enter(_new_e);  
		_new_e->close();
		delete _new_e;
		if (chk_ovrflw())
		{
			trt_ovrflw(_new_nd);
			ret = B_OVRFLW;
		}
		return ret;
	}

	int follow = choose_subtree(_new_e); 

	bool need_update = false;

	if (follow == -1)
	{
		need_update = true;
		follow = 0;
	}

	B_Node *succ = entries[follow]->get_son();  
	B_Node *new_nd = NULL;
	BINSRT c_ret = succ->insert(_new_e, &new_nd);
	
	if (need_update)
	{
		entries[follow]->set_from_child(succ);
		dirty = true;
	}

	entries[follow]->del_son();  
	
	if (c_ret == B_OVRFLW)
		add_new_child(new_nd); 
	if (chk_ovrflw())
	{
		trt_ovrflw(_new_nd);
		ret = B_OVRFLW;
	}
	return ret;
}

//-----------------------------------------------

int B_Node::max_lesseq_key_pos(unsigned * _key)
{
	int pos = -1;
	int temp;
	int tt=-1;
	for (int i = num_entries - 1; i >= 0; i --)
	{
		temp =compareint(entries[i]->key,_key,my_tree->keysize);
		if (temp <0 )
		{ pos = i; i = -1;}
		else if(temp ==0)
			tt =i;
	}
	if(tt>=0)
		return tt;
	else
		return pos;
}


/*****************************************************************
finds the entry that is just before the given (key, leafson) pair

para:
- key: 
- leafson:
*****************************************************************/

int B_Node::max_lesseq_key_pos(B_Entry *_e)
{
	int pos = -1;
	for (int i = num_entries - 1; i >= 0; i --)
	{
		int rslt = entries[i]->compare(_e);
		if (rslt == -1 || rslt == 0)
		{
			pos = i;
			break;
		}
	}

	return pos;
}

/*****************************************************************
return:
- return the left sibling node if it exists. otherwise, null.
*****************************************************************/

B_Node* B_Node::get_left_sibling()
{
	B_Node *ret = NULL;

	if (left_sibling != -1)
	{
		ret = new_one_node();
		ret->init(my_tree, left_sibling);
	}

	return ret;
}

/*****************************************************************
return:
- return the right sibling node if it exists. otherwise, null.
*****************************************************************/

B_Node* B_Node::get_right_sibling()
{
	B_Node *ret = NULL;

	if (right_sibling != -1)
	{
		ret = new_one_node();
		ret->init(my_tree, right_sibling);
	}

	return ret;
}

/*****************************************************************
finds the entry to be followed in finding a key

para:
- _e: the entry containing the key

return:
- the entry id
*****************************************************************/

int B_Node::ptqry_next_follow(B_Entry *_e)
{
	int pos = num_entries - 1;
	for (int i = 0; i < num_entries; i ++)
	{
		int rslt = entries[i]->compare_key(_e);
		if (rslt == 0)
		{
			pos = i;
			break;
		}
		else if (rslt == 1)
		{
			pos = i - 1;
			break;
		}
	}

	return pos;
}

//-----------------------------------------------

void B_Node::read_from_buffer(char *_buf)
{
	int i;
	memcpy(&level, _buf, sizeof(level));
	i = sizeof(level);
	memcpy(&num_entries, &_buf[i], sizeof(num_entries));
	i += sizeof(num_entries);
	memcpy(&left_sibling, &_buf[i], sizeof(left_sibling));
	i += sizeof(left_sibling);
	memcpy(&right_sibling, &_buf[i], sizeof(right_sibling));
	i += sizeof(right_sibling);

	for (int j = 0; j < num_entries; j ++)
	{
		entries[j]->read_from_buffer(&_buf[i]); 
		i += entries[j]->get_size(entries[j]->level);
	}
}

void B_Node::rmv_entry(int _pos)
{
	for (int i = _pos; i < num_entries - 1; i ++)
	{
		entries[i]->set_from(entries[i + 1]);
	}
	num_entries--;
	dirty = true;
}

void B_Node::rmv_entry_in_memory(int _pos)
{
	for (int i = _pos; i < num_entries - 1; i ++)
	{
		entries[i]->set_from(entries[i + 1]);
	}
	num_entries--;
}

//-----------------------------------------------

void B_Node::trt_ovrflw(B_Node **_new_nd)
{
	*_new_nd = new_one_node();
	(*_new_nd)->init(level, my_tree);  
	(*_new_nd)->level = level;

	int i = (capacity - 1) / 2 ;
	while (i < num_entries)
	{
		(*_new_nd)->enter(entries[i]);
		rmv_entry(i);
	}

	(*_new_nd)->right_sibling = right_sibling;
	(*_new_nd)->left_sibling = block;
	right_sibling = (*_new_nd) -> block;

	if ((*_new_nd)->right_sibling != -1)
	{
		B_Node *nd = new_one_node();
		nd->init(my_tree, (*_new_nd)->right_sibling);
		nd->left_sibling = (*_new_nd)->block;
		nd->dirty = true;
		delete nd; 
	}
}

//-----------------------------------------------

void B_Node::write_to_buffer(char *_buf)
{
	int i;
	memcpy(_buf, &level, sizeof(level));
	i = sizeof(level);
	memcpy(&_buf[i], &num_entries, sizeof(num_entries));
	i += sizeof(num_entries);
	memcpy(&_buf[i], &left_sibling, sizeof(left_sibling));
	i += sizeof(left_sibling);
	memcpy(&_buf[i], &right_sibling, sizeof(right_sibling));
	i += sizeof(right_sibling);

	for (int j = 0; j < num_entries; j ++)
	{
		entries[j]->write_to_buffer(&_buf[i]); 
		i += entries[j]->get_size(entries[j]->level);
	}
}

/*****************************************************************
this function deletes an entry from the b-tree

para:
- del_e: the entry to be deleted
*****************************************************************/

BDEL B_Node::delete_entry(B_Entry *_del_e)
{
	BDEL ret = B_NOTFOUND;
	if (level == 0)
	{
		for (int i = 0; i < num_entries; i++)
		{
			if (entries[i]->equal_to(_del_e))
			{
				_del_e->close();
				delete _del_e;

				rmv_entry(i);
				if (chk_undrflw())
					ret = B_UNDRFLW;
				else
					ret = B_NONE;  
				break; 
			}
		}
		return ret;
	}
	
	int follow = choose_subtree(_del_e);

	if (follow == -1)
		error("follow = -1 in B_Node::delete_entry, meaning entry not found.\n", true);

	B_Node *succ = entries[follow]->get_son();
	BDEL cret = succ->delete_entry(_del_e);

	entries[follow]->set_from_child(succ);
	dirty = true;

	entries[follow]->del_son();
	
	if (cret == B_NONE)
		ret = B_NONE;
	if (cret == B_UNDRFLW)
	{
		trt_undrflw(follow);
		if (chk_undrflw())
			ret = B_UNDRFLW;
		else
			ret = B_NONE;
	}

	return ret;
}

/*****************************************************************
this function fixes an underflow by merging

para:
follow: the subscript of the non-leaf entry whose child node incurs an underflow
*****************************************************************/

void B_Node::trt_undrflw(int _follow)
{
	int mergesub = _follow + 1;									//the subscript of the non-leaf entry to merge with
	if (_follow == num_entries - 1)
		mergesub = _follow - 1;
	
	B_Node *succ1 = entries[mergesub]->get_son();
	B_Node *succ2 = entries[_follow]->get_son();

	int totalnum = succ1->num_entries + succ2->num_entries;
	if (totalnum >= capacity-1)
	{
		//the merged node does not fit in one page.
		//remember succ1 points to mergesub and succ2 points to follow

		int n = succ1->num_entries; 
		if (mergesub > _follow)
		{
			for (int i = 0; i < totalnum / 2 - succ2->num_entries; i++)
			{
				succ2->enter(succ1->entries[0]);
				succ1->rmv_entry(0);
			}
		}
		else
		{
			for (int i = totalnum / 2; i < n; i++)
			{
				succ2->enter(succ1->entries[totalnum / 2]);
				succ1->rmv_entry(totalnum / 2);
			}
		}
		entries[mergesub]->set_from_child(succ1); 
		entries[_follow]->set_from_child(succ2);
		
		entries[mergesub]->del_son();
		entries[_follow]->del_son();
	}
	else
	{
		//copy all the entries from succ2 to succ1
		//remember succ1 points to mergesub and succ2 points to follow

		for (int i = 0; i < succ2->num_entries; i++)
		{
			succ1->enter(succ2->entries[i]);
		}

		//next, fix the sibling pointers

		if (_follow < mergesub)
		{
			succ1->left_sibling = succ2->left_sibling;

			if (succ1->left_sibling != -1)
			{
				B_Node *nd = new_one_node();
				nd->init(my_tree, succ1->left_sibling);
				nd->right_sibling = succ1->block;
				nd->dirty = true;
				delete nd;
			}
		}
		else
		{
			succ1->right_sibling = succ2->right_sibling;

			if (succ1->right_sibling != -1)
			{
				B_Node *nd = new_one_node();
				nd->init(my_tree, succ1->right_sibling);
				nd->left_sibling = succ1->block;
				nd->dirty = true;
				delete nd;
			} 
		}

		succ1->dirty = true;

		entries[mergesub]->set_from_child(succ1);

		entries[mergesub]->del_son();
		entries[_follow]->del_son();

		rmv_entry(_follow);
	}

	dirty = true;
}

/*****************************************************************
traverse the suB_Tree 

para:
- info: an array for taking info out of the function

return value: return 1 if everything is all right. otherwise 0.
*****************************************************************/

int B_Node::traverse(double *_info)
{
	int ret = 1;
	_info[1] ++;     //Counting the number of nodes.

	//first check if the ordering of the entries is correct
	int i;
	for (i = 0; i < num_entries - 1; i ++)
	{
		if (entries[i]->compare(entries[i + 1]) != -1)
		{
			printf("Entry ordering error\n");
			printf("Block %d, level %d, entry %d\n", block, level, i);
			error("I am aborting...\n", true);
		}
	}

	if (level > 0)
	{
		for (int i = 0; i < num_entries; i ++)
		{
			B_Node *nd = entries[i]->get_son();

			bool equal = true; 

			for (int j = 0; j < my_tree->keysize; j ++)
				if (compfloats((double) entries[i]->key[j], (double) nd->entries[0]->key[j]) != 0)
				{
					equal = false; break;
				}

			if (!equal ||
				entries[i]->leafson != nd->entries[0]->leafson)
			{
				printf("Entry content error\n");
				printf("Block %d, level %d, entry %d\n", block, level, i);
				error("I am aborting...\n", true);
			}
			int cret = nd->traverse(_info);

			if (cret == 0)
				ret = 0;
			entries[i]->del_son();
		}
	}
	else
	{
		_info[0] += num_entries;     //Counting the number of objects.

		//Now check the sibling pointers.

		if (left_sibling != my_tree->last_leaf)
		{
			printf("Left sibling ptr error\n");
			printf("Block %d, level %d, entry %d\n", block, level, i);
			printf("Capacity = %d\n", capacity);
			printf("Seen %d entries so far.\n", (int) _info[0]);
			error("I am aborting...\n", true);
		}

		if (my_tree->last_right_sibling != -1 && my_tree->last_right_sibling != block)
		{
			printf("Right sibling ptr error\n");
			printf("Block %d, level %d, entry %d\n", block, level, i);
			error("I am aborting...\n", true);
		}
		
		my_tree->last_leaf = block;
		my_tree->last_right_sibling = right_sibling;
	}

	return ret;
}

B_Entry *B_Node::new_one_entry()
{
	B_Entry *e = new B_Entry();

	return e;
}

B_Node *B_Node::new_one_node()
{
	B_Node *nd = new B_Node();

	return nd;
}