/* blk_file.cpp;
 implementation of class BlockFile and CachedBlockFile

 when these classes are updated, remember to modify the version info in the
 ructors*/

/*note points
1) 2 types of block # are used (i.e. the internal # and external #).  internal
# is one larger than external # because the first block of the file is used to
store header info.  data info is stored starting from the 2nd block.  both types
of # start from 0.
2) "actblock" is internal block #.  "number" is the # of data block (i.e. excluding
the header block).  maximum actblock equals to number.  maximum external block #
equals to number - 1
3) cache_cont records the internal numbers.
*/

#include <string.h>
#include "blk_file.h"
#include "../gadget/gadget.h"
//--------------------------BlockFile-----------------------------

extern double IOread;
extern double IOwrite;
extern double IOtime;

void BlockFile::fwrite_number(long long value)
{
   put_bytes((char *) &value, sizeof(long long));
}

//----------------------------------------------------------------

long long BlockFile::fread_number()
{
   char ca[sizeof(long long)];

   get_bytes(ca,sizeof(long long));
   return *((long long *)ca);
}

//----------------------------------------------------------------

BlockFile::BlockFile()
{
}

BlockFile::BlockFile(char* name,long long b_length)
{
   char *buffer;
   long long l;

   filename = new char[strlen(name) + 1];
   strcpy(filename,name);
   blocklength = b_length;

   number = 0;         //number is the # of blocks stored in a file

   if((fp=fopen(name,"rb+"))!=0)
   {
      new_flag = FALSE;
      blocklength = fread_number();
      number = fread_number();
   }
   else
   {
      if (blocklength < BFHEAD_LENGTH)
    	 error("could not open file (failure in blockfile constructor)\n",TRUE);

      fp=fopen(filename,"wb+");
      if (fp == NULL)
		 error("BlockFile::BlockFile -- could not create file\n", TRUE);

      new_flag = TRUE;
      fwrite_number(blocklength);
      fwrite_number(0);       //write # of blocks in the file

	  buffer = new char[(l=blocklength-(long long)ftell(fp))];
      memset(buffer, 0, sizeof(buffer));
      put_bytes(buffer,l);    //fill in the rest of block with 0

      delete [] buffer;
   }

   fseek(fp,0,SEEK_SET);
   act_block=0;
}

void BlockFile::init(char* name, long long b_length)
{
	char *buffer;
	long long l;

	filename = new char[strlen(name) + 1];
	strcpy(filename, name);
	blocklength = b_length;

	number = 0;         //number is the # of blocks stored in a file

	if ((fp = fopen(name, "r+")) != 0)
	{
		new_flag = FALSE;
		blocklength = fread_number();
		number = fread_number();
	}
	else
	{
		if (blocklength < BFHEAD_LENGTH)
			error("could not open file (failure in blockfile constructor)\n", TRUE);

		fp = fopen(filename, "wb+");
		if (fp == NULL)
			error("BlockFile::BlockFile -- could not create file\n", TRUE);

		new_flag = TRUE;
		fwrite_number(blocklength);
		fwrite_number(0);       //write # of blocks in the file

		buffer = new char[(l = blocklength - (long long)ftell(fp))];
		memset(buffer, 0, sizeof(buffer));
		put_bytes(buffer, l);    //fill in the rest of block with 0

		delete[] buffer;
	}

	fseek(fp, 0, SEEK_SET);
	act_block = 0;
}

//----------------------------------------------------------------

BlockFile::~BlockFile()
{
   delete[] filename;
   fclose(fp);
}

//----------------------------------------------------------------

void BlockFile::read_header(char* buffer)
  //note that this func does not read the header of blockfile.  it
  //fetches the info in the first block excluding the header of blockfile
{
   fseek(fp,BFHEAD_LENGTH,SEEK_SET);
   get_bytes(buffer,blocklength-BFHEAD_LENGTH);

   if(number<1)
   {
       fseek(fp,0,SEEK_SET);
       act_block=0;
   }
   else
       act_block=1;
}

//----------------------------------------------------------------

void BlockFile::set_header(char* header)
{
    fseek(fp,BFHEAD_LENGTH,SEEK_SET);
    put_bytes(header,blocklength-BFHEAD_LENGTH);

   if(number<1)
   {
       fseek(fp,0,SEEK_SET);
       act_block=0;
   }
   else
       act_block=1;
}

//----------------------------------------------------------------

bool BlockFile::read_block(Block b,long long pos)
{
    IOtime-=clock();
   IOread++;
   pos++;     //external block to internal block
   if (pos<=number && pos>0)
       seek_block(pos);
   else
   {
	   printf("Requested block %d is illegal.", pos - 1);  error("\n", true);
   }

   get_bytes(b,blocklength);
   if (pos+1>number)
   {
       fseek(fp,0,SEEK_SET);
       act_block=0;  //if the file pointer reaches the end, rewind to the beginning
   }
   else
       act_block=pos+1;
    IOtime+=clock();
   return TRUE;
}

//----------------------------------------------------------------

bool BlockFile::write_block(Block block, long long pos)
  //note that this function can only write to an already allocated block.  to
  //allocate a new block, use append_block instead.
{
    IOtime-=clock();
   IOwrite++;
   pos++;      //external # to interal #

   if (pos<=number && pos>0)
       seek_block(pos);
   else
   {
	   printf("Requested block %d is illegal.", pos - 1);  error("\n", true);
   }
   put_bytes(block,blocklength);
   if (pos+1>number)
   {
       fseek(fp,0,SEEK_SET);
       act_block=0;
   }
   else
       act_block=pos+1;
    IOtime+=clock();
   return TRUE;
}

//----------------------------------------------------------------

long long BlockFile::append_block(Block block)
{
    IOtime-=clock();
   IOwrite++;// modify by Lu Chen
   fseek(fp,0,SEEK_END);
   put_bytes(block,blocklength);
   number++;
   fseek(fp,sizeof(long long),SEEK_SET);
   fwrite_number(number);
   fseek(fp,-blocklength,SEEK_END);
    IOtime+=clock();
   return (act_block=number)-1;
}

//----------------------------------------------------------------

bool BlockFile::delete_last_blocks(long long num)
{
   if (num>number)
      return FALSE;

   number -= num;
   fseek(fp,sizeof(long long),SEEK_SET);
   fwrite_number(number);
   fseek(fp,0,SEEK_SET);
   act_block=0;
     //note that blocks are deleted logically (only modifying the total # of
     //blocks).
   return TRUE;
}

//========================CachedBlockFile=========================

long long CachedBlockFile::next()
{
   long long ret_val, tmp;

   if (cachesize == 0) return -1;
   else
   {
       if (fuf_cont[ptr] == free)   //ptr points to a cache page
       {
		   ret_val = ptr++;   ptr = ptr % cachesize;
		   return ret_val;
	   }
	   else
	   {
		 tmp= (ptr + 1) % cachesize;

		 //find the first free block
		 while (tmp != ptr && fuf_cont[tmp] != free)
		    tmp = (tmp + 1) % cachesize;

		 if (ptr == tmp)	//failed to find a free block
		 {
		        // select a victim page to be written back to disk
             long long lru_index = 0; // the index of the victim page

			 for (long long i = 1; i < cachesize; i++)
                if (LRU_indicator[i] > LRU_indicator[lru_index])
                    lru_index=i;        /*the replacement policy is least recently used.  pick
										out the page with the maximum ilde time counter*/
             ptr = lru_index;

			   /*adding the following line*/
			 if (dirty_indicator [ptr] == true)
			   /*line added by tao yufei*/
    			BlockFile::write_block(cache[ptr],cache_cont[ptr]-1);

    		 fuf_cont[ptr] = free;  dirty_indicator [ptr] = false;

    		 ret_val = ptr++;  ptr = ptr % cachesize;
    	 }
    	 else  //a free block is found
		    return tmp;
	   }
	 }

     return false;
}

//----------------------------------------------------------------

long long CachedBlockFile::in_cache(long long index)
// liefert Pos. eines Blocks im Cache, sonst -1
{
   long long i;
   long long ret_val = -1;

   for (i = 0; i < cachesize; i++)
	   if (cache_cont[i] == index && fuf_cont[i] != free)
	   {
	       LRU_indicator[i]=0;
		   ret_val = i;
	   }
	   else if (fuf_cont[i] != free)
                LRU_indicator[i]++; // increase indicator for this block
   return ret_val;
}

//----------------------------------------------------------------

CachedBlockFile::CachedBlockFile(char* name,long long blength, long long csize)
   : BlockFile(name,blength)
{
	printf("CachedBlockFile Version 1.0\n");

	long long i;

	ptr=0;

	if (csize>=0) cachesize=csize;
	else error("Cache size cannot be negative",TRUE);

	cache_cont = new long long[cachesize];
	fuf_cont = new uses[cachesize];
	LRU_indicator = new long long[cachesize];
	dirty_indicator = new bool[cachesize];

	for (i=0; i<cachesize; i++)
	{
		cache_cont[i] = 0;
		fuf_cont[i]=free;
		LRU_indicator[i] = 0;
		dirty_indicator[i] = false;
	}

	cache = new char*[cachesize];
	for (i = 0; i < cachesize; i++)
		cache[i] = new char[blength];

	page_faults = 0;
}

//----------------------------------------------------------------

CachedBlockFile::~CachedBlockFile()
{
	flush();
	delete[] cache_cont;
	delete[] fuf_cont;
	delete[] LRU_indicator;
	delete[] dirty_indicator;

	for (long long i=0;i<cachesize;i++)
		delete[] cache[i];

	delete[] cache;
}

//----------------------------------------------------------------

bool CachedBlockFile::read_block(Block block, long long index)
{
	long long c_ind;

	index++;	// Externe Num. --> interne Num.

	if(index <= get_num_of_blocks() && index>0)
	{
		if((c_ind = in_cache(index)) >= 0) // get cached?
			memcpy(block, cache[c_ind], get_blocklength());
		else
		{
			  //adding line
			page_faults ++;
			  //line added by tao yufei
			c_ind = next();
			if (c_ind >= 0)
			{
				BlockFile::read_block(cache[c_ind], index - 1); // ext. Num.
				cache_cont[c_ind] = index;
				fuf_cont[c_ind] = used;
				LRU_indicator[c_ind] = 0;
				memcpy(block, cache[c_ind], get_blocklength());
			}
			else //this happens when (i)cache size=0 (ii)all the blocks are pinned
				BlockFile::read_block(block, index - 1); // read-through (ext.Num.) without caching
		}
		return TRUE;
	}
	else
	{
		printf("The requested block %d is illegal", index - 1);  error("\n", true);
		return FALSE;  //adding this line is just to avoid the warning "not all the
		               //path return values".
	}
}

//----------------------------------------------------------------

bool CachedBlockFile::write_block(Block block, long long index)
{
	long long c_ind;

	index++;	// Externe Num. --> interne Num.

	if(index <= get_num_of_blocks() && index > 0)
	{
		c_ind = in_cache(index);
		if(c_ind >= 0)	// get cached?
		{
			memcpy(cache[c_ind], block, get_blocklength());
			dirty_indicator[c_ind] = true;
		}
		else
		{
			c_ind = next();
			if (c_ind >= 0)
			{
				memcpy(cache[c_ind], block, get_blocklength());
				cache_cont[c_ind] = index;
				fuf_cont[c_ind] = used;
				LRU_indicator[c_ind] = 0;
				dirty_indicator[c_ind] = true;
			}
			else
				BlockFile::write_block(block, index - 1);  // write-through (ext.Num.)
		}
		return TRUE;
	}
	else
	{
	   printf("Requested block %d is illegal.", index - 1); error("\n", true);
	   return FALSE;
	}
}

//----------------------------------------------------------------

bool CachedBlockFile::fix_block(long long index)
  //call the function to pin a certain block in the memory.
{
	long long c_ind;

	index++;	// External Num. --> internal Num.

	if (index <= get_num_of_blocks() && index > 0)
	{
		if((c_ind = in_cache(index)) >= 0)
		{
			return TRUE;
			fuf_cont[c_ind] = fixed;
		}
		else
			return FALSE;
	}
	else
	{
		printf("The Requested block %d is illegal.", index - 1);  error("\n", true);
    }

	return false;
}
//----------------------------------------------------------------
bool CachedBlockFile::unfix_block(long long index)
// Fixierung eines Blocks durch fix_block wieder aufheben
{
	long long i;

	i = 0;
	index++;	// Externe Num. --> interne Num.
	if(index <= get_num_of_blocks() && index>0)
	{
		while(i<cachesize && (cache_cont[i]!=index || fuf_cont[i] == free))
			i++;
		if (i != cachesize)
			fuf_cont[i] = used;

		return TRUE;
	}
	else
		return FALSE;
}

//----------------------------------------------------------------

void CachedBlockFile::unfix_all()
{
	long long i;

	for (i = 0; i < cachesize; i++)
		if (fuf_cont[i] == fixed)
			fuf_cont[i] = used;
}

//----------------------------------------------------------------

void CachedBlockFile::set_cachesize(long long size)
{
	long long i;

	if (size>=0)
	{
		ptr = 0;
		flush();

		for(i = 0; i < cachesize; i++)
			delete[] cache[i];

		delete[] cache;

		delete[] cache_cont;
		delete[] fuf_cont;
		delete[] dirty_indicator;
		delete[] LRU_indicator;

		cachesize = size;

		cache_cont = new long long[cachesize];
		fuf_cont = new uses[cachesize];
		LRU_indicator = new long long[cachesize];
		dirty_indicator = new bool[cachesize];

		for (i=0; i<cachesize; i++)
		{
			cache_cont[i] = 0;
			fuf_cont[i]=free;
			LRU_indicator[i] = 0;
			dirty_indicator[i] = false;
		}

		cache = new char*[cachesize];
		for (i = 0; i < cachesize; i++)
			cache[i] = new char[get_blocklength()];
	}
	else
		error("Cache size cannot be negative\n",TRUE);
}

//----------------------------------------------------------------

void CachedBlockFile::flush()
// schreibt den  Cache auf Platte, gibt ihn nicht frei
{
	long long i;

	for (i = 0; i < cachesize; i++)
		if (fuf_cont[i] != free && dirty_indicator[i])
			BlockFile::write_block(cache[i], cache_cont[i] - 1); // ext.Num.
}











