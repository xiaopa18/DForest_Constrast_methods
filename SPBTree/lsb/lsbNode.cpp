#include <math.h>
#include <memory.h>
#include "lsbNode.h"
#include "lsbEntry.h"
#include "lsbTree.h"
#include "../blockfile/blk_file.h"
#include "../blockfile/cache.h"


LSBnode::LSBnode()
{

}


LSBnode::~LSBnode()
{

}


B_Entry *LSBnode::new_one_entry()
{
	LSBentry *e = new LSBentry();

	return e;
}


B_Node *LSBnode::new_one_node()
{
	LSBnode *nd = new LSBnode();

	return nd;
}
