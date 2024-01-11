#include <math.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fstream>
#include "lsb.h"
#include "lsbEntry.h"
#include "lsbNode.h"
#include "lsbTree.h"
#include "../btree/b-tree.h"
#include "../blockfile/blk_file.h"
#include "../gadget/gadget.h"


LSBtree::LSBtree()
{

}


LSBtree::~LSBtree()
{
}

/*****************************************************************
read the tree header. 

-para-
buf		starting address of the buffer

-return--
number of bytes read
*****************************************************************/

long long LSBtree::read_header(char *_buf)
{
	long long	ret = 0;

	ret = B_Tree::read_header(_buf);

	memcpy(&dim, &(_buf[ret]), sizeof(dim));
	ret += sizeof(dim);

	return ret;
}

/*****************************************************************
write the tree header. 

-para-
buf		starting address of the buffer

-return--
number of bytes written
*****************************************************************/

long long LSBtree::write_header(char *_buf)
{
	long long ret = 0;
	
	ret = B_Tree::write_header(_buf);

	memcpy(&(_buf[ret]), &dim, sizeof(dim));
	ret += sizeof(dim);

	return ret;
}

/*****************************************************************
read the next object from a data file.

-para-
fp		file handle
d		B_Entry instance used to store the object
*****************************************************************/

void LSBtree::fread_next_entry(FILE *_fp, B_Entry *_d)
{
	fscanf(_fp, "%d ", &(_d->son));

	for (long long i = 0; i < keysize; i ++)
		fscanf(_fp, "%d", &(_d->key[i]));

	for (long long i = 0; i < dim; i ++)
		fscanf(_fp, "%d", &((LSBentry *) _d)->pt[i]);
	
	fscanf(_fp, "\n");

	_d->leafson = _d->son;
}

/*****************************************************************
adds the extension "lsb" to the file name of a tree.
*****************************************************************/

void LSBtree::add_fname_ext(char * _fname)
{
	strcat(_fname, ".lsb");
}


B_Entry *LSBtree::new_one_entry()
{
	LSBentry *e = new LSBentry();
	return e;
}


B_Node *LSBtree::new_one_node()
{
	LSBnode *nd = new LSBnode();
	return nd;
}

/*****************************************************************
creates a new tree.

-para-
fname		file name of the tree (without extension)
blen		page size
dim			dimensionality 
keysize		number of integers needed to represent a z-value

-return-
0			success
1			failure
*****************************************************************/

long long LSBtree::init(char *_fname, long long _blen, long long _dim, long long _keysize)
{
	long long		ret		= 0; 

	char	*fname	= NULL;

	fname = new char[strlen(_fname) + 10];		/* allow 9 characters for extension name */
	strcpy(fname, _fname);
	add_fname_ext(fname);

	FILE *fp = fopen(fname, "r");

	if (fp)
	{
		printf("File %s exists.\n"); 
		ret = 1;

		goto recycle;
	}

    file = new BlockFile(fname, _blen);
    cache = NULL;

	dim = _dim;
	keysize = _keysize;

	/* deliberately waste a node so that all nodes' addresses are positive */
    root_ptr = new_one_node();
	root_ptr->init(0, this);
    root = root_ptr->block;
	delroot();

	root_ptr = new_one_node();
	root_ptr->init(0, this);
	root = root_ptr->block;
	delroot();

recycle: 
	if (fname)
		delete [] fname;

	if (fp)
		fclose(fp);

	return ret;
}

/*****************************************************************
this function is called by B_Tree::bulkload2(). it reads from 
a memory buffer an entry.

-para-
ptr			(out) the pointer to the next position to be read in
			the memory buffer. at finish, this pointer will be
			moved forward to the beginning of the next entry.
e			(out) the entry read.

-return-
0			success
1			failure
*****************************************************************/

long long LSBtree::read_next_entry(void **_ptr, B_Entry *_e)
{
	long long				ret		= 0;

	long long				pos		= -1;
	LSBqsortElem	* ptr	= (LSBqsortElem *) (*_ptr);

	pos = ptr->pos;

	_e->son = ptr->ds[pos * (dim + 1)];

	memcpy(_e->key, ptr->z, sizeof(long long) * ptr->pz);

	memcpy(((LSBentry *) _e)->pt, &(ptr->ds[pos * (dim + 1) + 1]), sizeof(long long) * dim);

	_e->leafson = _e->son;

	(* _ptr) = ptr + 1;

	return ret;
}
