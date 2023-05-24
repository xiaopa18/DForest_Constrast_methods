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

	//modify by Lu Chen
	pd = NULL;
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

	if(pd)
	{
		delete[] pd;
		pd = NULL;
	}
}

//-----------------------------------------------

B_Entry::~B_Entry()
{
	if (son_ptr)
	{
		printf("B_Entry desctrutor -- son_ptr not NULL. maybe you forgot to call close\n");
		system("pause");
		
	}


	if (pd)
	{
		printf("B_Entry desctrutor -- pd not NULL. maybe you forgot to call close\n");
		system("pause");
	}
}

//-----------------------------------------------

int B_Entry::get_size(int _level)
{ 
	int size;
	
	if (_level == 0) 
		size = sizeof(son)+ my_tree->keysize*sizeof(double) + sizeof(double) + sizeof(ptr); //modify by Lu Chen
	else
		size = sizeof(son)  + sizeof(double) + sizeof(int);

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

int B_Entry::read_from_buffer(char *_buf)
{
	int i;
	memcpy(&key, _buf, sizeof(double));
	i = sizeof(double);

	memcpy(&son, &_buf[i], sizeof(son));
	i += sizeof(son);


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

		pd = new double[my_tree->keysize];
		memcpy(pd, &_buf[i], my_tree->keysize * sizeof(double));
		i+=my_tree->keysize * sizeof(double);
	}

	return i;
}

//-----------------------------------------------

void B_Entry::init(B_Tree *_B_Tree, int _level)
{ 
	my_tree = _B_Tree; 
	level = _level;
	if(_level = 0)
	{
		pd =  new double[my_tree->keysize];
	}
}

//-----------------------------------------------

int B_Entry::write_to_buffer(char *_buf)
{
	int i;
	memcpy(_buf, &key, sizeof(double));
	i =  sizeof(double);

	memcpy(&_buf[i], &son, sizeof(son));
	i += sizeof(son);


	if (level > 0)
	{
		memcpy(&_buf[i], &leafson, sizeof(leafson)); 
		i += sizeof(leafson);
	}

	if(level ==0)
	{
		memcpy(&_buf[i], &ptr, sizeof(ptr));
		i+=sizeof(ptr);

		memcpy(&_buf[i], pd, my_tree->keysize * sizeof(double));
		i += my_tree->keysize * sizeof(double);
	}

	return i;
}

//-----------------------------------------------

void B_Entry::set_from(B_Entry *_e)
{
	if (level != _e->level)
	{
		printf("Error in B_Entry::set_from -- different levels");
		system("pasue");
		
	}
	
	key =_e->key;

	if (level == 0)		
	{
		pd = new double[_e->my_tree->keysize];
		memcpy(pd, _e->pd, _e->my_tree->keysize * sizeof(double));
	}
	my_tree = _e->my_tree;
	son = _e->son;
	leafson = _e->leafson;
	son_ptr = _e->son_ptr;
	ptr = _e->ptr;
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

	if (level != _e->level || son != _e->son || leafson != _e->leafson || ptr!=_e->ptr||key!=_e->key) //mofify by Lu Chen
		ret = false;

	if (pd != NULL&&_e->pd != NULL)
	{
		for(int i =0;i<my_tree->keysize;i++)
			if (pd[i] != _e->pd[i])
			{
				ret = false;
				break;
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
	key =  _nd->entries[0]->key;
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

int B_Entry::compare_key(B_Entry *_e)
{
	int ret = 0; 
	if (key - _e->key<-0.00000001)
	{
		ret = -1;
	}
	else if (key - _e->key>0.00000001)
	{
		ret = 1;
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

int B_Entry::compare(B_Entry *_e)
{
	int ret = compare_key(_e);

	if (ret == 0)
	{
		if (leafson < _e->leafson)
			ret = -1;
		else if (leafson > _e->leafson)
			ret = 1; 
	}

	return ret;
}