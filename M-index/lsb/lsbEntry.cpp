#include <stdio.h>
#include <memory.h>
#include "../btree/b-entry.h"
#include "lsbentry.h"
#include "lsbnode.h"
#include "lsbtree.h"

LSBentry::LSBentry()           
{
	pt = NULL;
}


LSBentry::~LSBentry()
{
	if (pt)
	{
		delete [] pt;
		pt = NULL;
	}
}


long long LSBentry::get_size(long long _level)
{ 
	long long ret = 0;
	
	ret = B_Entry::get_size(_level);
	
	if (_level == 0) 
		ret += sizeof(long long) * ((LSBtree *) my_tree)->dim;

	return ret;
}


long long LSBentry::read_from_buffer(char *_buf)
{
	long long i = B_Entry::read_from_buffer(_buf);

	long long dim = ((LSBtree *)my_tree)->dim;

	if (level == 0)
	{
		for (long long j = 0; j < dim; j ++)
		{
			memcpy(&pt[j], &_buf[i], sizeof(long long));
			i += sizeof(long long);
		}
	}

	return i;
}

void LSBentry::init(B_Tree *_btree, long long _level)
{ 
	long long		i;

	B_Entry::init(_btree, _level);

	pt = new long long[((LSBtree *) _btree)->dim];

	for (i = 0; i < ((LSBtree *) _btree)->dim; i ++)
		pt[i] = -1;
}


long long LSBentry::write_to_buffer(char *_buf)
{
	long long i = B_Entry::write_to_buffer(_buf);

	long long dim = ((LSBtree *)my_tree)->dim;

	if (level == 0)
	{
		for (long long j = 0; j < dim; j ++)
		{
			memcpy(&_buf[i], &pt[j], sizeof(long long));
			i += sizeof(long long);
		}
	}

	return i;
}


void LSBentry::set_from(B_Entry *_e)
{
	B_Entry::set_from(_e);

	long long dim = ((LSBtree *) my_tree)->dim;
	memcpy(pt, ((LSBentry *) _e)->pt, sizeof(long long) * dim);
}

/*****************************************************************
check if two entries are identical

para:
- e

return value: true or false
*****************************************************************/

bool LSBentry::equal_to(B_Entry *_e)
{
	bool ret = B_Entry::equal_to(_e); 

	if (ret)
	{
		long long dim = ((LSBtree *) my_tree)->dim;

		for (long long i = 0; i < dim; i ++)
		{
			if (pt[i] != ((LSBentry *) _e)->pt[i])
			{
				ret = false;
				break;
			}
		}
	}

	return ret;
}


B_Node *LSBentry::new_one_node()
{
	LSBnode *nd = new LSBnode();
	return nd;
}


void LSBentry::close()
{
	if (pt)
	{
		delete [] pt;
		pt = NULL;
	}
	B_Entry::close();
}