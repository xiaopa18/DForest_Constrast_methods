/* this file defines class B_Entry */

#include <stdio.h>
#include <memory.h>
#include "b-node.h"
#include "b-entry.h"
#include "b-tree.h"
#include "b-def.h"
#include "../gadget/gadget.h"

//-----------------------------------------------

B_Entry::B_Entry() 
		          
{
	son = -1;
	key = NULL;

	min = NULL;
	max = NULL;
	mi = NULL;
	ma = NULL;
	son_ptr = NULL;
	level = -1;
	ptr = -1;
	my_tree = NULL;
}

/****************************************************************
frees up the memory
*****************************************************************/

void B_Entry::close()
{
	if (son_ptr)
	{
		if(son_ptr != NULL)
			delete son_ptr;
		son_ptr = NULL;
	}

	if (key)
	{
		if(key!=NULL)
			delete [] key;
		key = NULL;
	}

	if(min)
	{
		if(min!=NULL)
			delete [] min;
		min = NULL;
	}
	if(max)
	{
		if(max!=NULL)
			delete []max;
		max = NULL;
	}
	if(mi)
	{
		delete[] mi;
		mi = NULL;
	}

	if(ma)
	{
		delete[] ma;
		ma = NULL;
	}
}

//-----------------------------------------------

B_Entry::~B_Entry()
{
	if (son_ptr)
	{
		error("B_Entry desctrutor -- son_ptr not NULL. maybe you forgot to call close\n", true);
	}

	if (key)
	{
		error("B_Entry desctrutor -- key not NULL. maybe you forgot to call close\n", true);
	}

	if (min)
	{
		error("B_Entry desctrutor -- min not NULL. maybe you forgot to call close\n", true);
	}

	if (max)
	{
		error("B_Entry desctrutor -- max not NULL. maybe you forgot to call close\n", true);
	}

	if (ma)
	{
		error("B_Entry desctrutor -- ma not NULL. maybe you forgot to call close\n", true);
	}
	if (mi)
	{
		error("B_Entry desctrutor -- mi not NULL. maybe you forgot to call close\n", true);
	}
}

//-----------------------------------------------

long long B_Entry::get_size(long long _level)
{ 
	long long size;
	
	if (_level == 0) 
		size = sizeof(son) + sizeof(unsigned long long) * my_tree->keysize + sizeof(ptr); 
	else
		size = sizeof(son) + sizeof(leafson) + 3*sizeof(unsigned long long) * my_tree->keysize; 

	return size;
}

//-----------------------------------------------

B_Node *B_Entry::get_son()
{
	if (son_ptr == NULL)
	{
		son_ptr = new_one_node();
		son_ptr->init(my_tree, son); 
	}
	return son_ptr;
}


//-----------------------------------------------

void B_Entry::del_son()
{
	if (son_ptr) 
	{ 
		delete son_ptr; 
		son_ptr = NULL;
	}  
}

//-----------------------------------------------

long long B_Entry::read_from_buffer(char *_buf)
{
	long long i;
	memcpy(key, _buf, my_tree->keysize * sizeof(unsigned long long));
	i = my_tree->keysize * sizeof(unsigned long long);

	memcpy(&son, &_buf[i], sizeof(son));
	i += sizeof(son);

	if(level >0)
	{
	memcpy(min, &_buf[i], my_tree->keysize * sizeof(unsigned long long));
	i+=my_tree->keysize * sizeof(unsigned long long);

	memcpy(max, &_buf[i], my_tree->keysize * sizeof(unsigned long long));
	i+=my_tree->keysize * sizeof(unsigned long long);
	}

	if (level > 0)
	{
		memcpy(&leafson, &_buf[i], sizeof(leafson));
		i += sizeof(leafson);
	}
	else
		leafson = son;


	if(level ==0)
	{
		memcpy(&ptr, &_buf[i], sizeof(ptr));
		i+= sizeof(ptr);
	}

	return i;
}

//-----------------------------------------------

void B_Entry::init(B_Tree *_B_Tree, long long _level)
{ 
	my_tree = _B_Tree; 
	level = _level;
	key = new unsigned long long[my_tree->keysize];
	mi = NULL;
	ma = NULL;
	if(_level > 0)
	{
		min =  new unsigned long long[my_tree->keysize];
		max = new unsigned long long[my_tree->keysize];
	}
	else
	{
		min = NULL;
		max = NULL;
	}
}

//-----------------------------------------------

long long B_Entry::write_to_buffer(char *_buf)
{
	long long i;
	memcpy(_buf, key, my_tree->keysize * sizeof(unsigned long long));
	i = my_tree->keysize * sizeof(unsigned long long);

	memcpy(&_buf[i], &son, sizeof(son));
	i += sizeof(son);

	if(level >0)
	{
		memcpy(&_buf[i], min, my_tree->keysize * sizeof(unsigned long long));
		i += my_tree->keysize * sizeof(unsigned long long);

		memcpy(&_buf[i], max, my_tree->keysize * sizeof(unsigned long long));
		i += my_tree->keysize * sizeof(unsigned long long);
	}
	
	if (level > 0)
	{
		memcpy(&_buf[i], &leafson, sizeof(leafson)); 
		i += sizeof(leafson);
	}

	if(level ==0)
	{
		memcpy(&_buf[i], &ptr, sizeof(ptr));
		i+=sizeof(ptr);
	}

	return i;
}

//-----------------------------------------------

void B_Entry::set_from(B_Entry *_e)
{
	if (level != _e->level)
		error("Error in B_Entry::set_from -- different levels",	true);

	memcpy(key, _e->key, _e->my_tree->keysize * sizeof(unsigned long long));

	if(level>0)
	{
		memcpy(min, _e->min, _e->my_tree->keysize * sizeof(unsigned long long));
		memcpy(max, _e->max, _e->my_tree->keysize * sizeof(unsigned long long));
	}
	my_tree = _e->my_tree;
	son = _e->son;
	leafson = _e->leafson;
	son_ptr = _e->son_ptr;
	ptr = _e->ptr;
	mi = _e->mi;
	ma = _e->ma;
}

/*****************************************************************
check if two entries are identical

para:
- e

return value: true or false
*****************************************************************/

bool B_Entry::equal_to(B_Entry *_e)
{
	bool ret = true; 

	if (level != _e->level || son != _e->son || leafson != _e->leafson || ptr!=_e->ptr || min!=_e->min || max !=_e->max) //mofify by Lu Chen
		ret = false;

	if (ret)
	{
		for (long long i = 0; i < my_tree->keysize; i ++)
		{
			if (key[i] != _e->key[i])
			{
				ret = false; break;
			}
		}
	}

	return ret;
}

/*****************************************************************
sets the info of this entry from the child node

para:
- nd: the child node
*****************************************************************/

void B_Entry::set_from_child(B_Node *_nd)
{
	son = _nd->block;
	memcpy(key, _nd->entries[0]->key, my_tree->keysize * sizeof(unsigned long long)); 
	leafson = _nd->entries[0]->leafson;
}

B_Node *B_Entry::new_one_node()
{
	B_Node *nd = new B_Node();

	return nd;
}

/*****************************************************************
compares which of two entries comes first by their keys only

para:
- e: the entry to compare with

return
- -1: the host entry comes first
  0: tie
  1: the host entry comes later
*****************************************************************/

long long B_Entry::compare_key(B_Entry *_e)
{
	long long ret = 0; 

	for (long long i = 0; i < my_tree->keysize; i++)
	{
		if (key[i] < _e->key[i])
		{
			ret = -1;
			break;
		}
		else if (key[i] > _e->key[i])
		{
			ret = 1;
			break;
		}
	}
	return ret;
}

/*****************************************************************
compares which of two entries comes first.

para:
- e: the entry to compare with

return
- -1: the host entry comes first
  0: tie
  1: the host entry comes later
*****************************************************************/

long long B_Entry::compare(B_Entry *_e)
{
	long long ret = compare_key(_e);

	if (ret == 0)
	{
		if (leafson < _e->leafson)
			ret = -1;
		else if (leafson > _e->leafson)
			ret = 1; 
	}

	return ret;
}