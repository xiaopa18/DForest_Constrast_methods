/*cache.cpp
  the implementation of class Cache
*/
#include <string.h>
#include "cache.h"
#include "blk_file.h"
#include "../gadget/gadget.h"
//----------------------------------------------------------------
Cache::Cache(long long csize, long long blength)
{
	long long i;

	ptr=0;
	blocklength = blength;

	if (csize >= 0) cachesize=csize;
	else error("Cache size cannot be negative\n", TRUE);

	cache_cont = new long long[cachesize];
	cache_tree = new Cacheable*[cachesize];
	fuf_cont = new uses[cachesize];
	LRU_indicator = new long long[cachesize];
	dirty_indicator = new bool[cachesize];

	for (i = 0; i < cachesize; i++)
	{
    	cache_cont[i] = 0;
    	cache_tree[i] = NULL;
    	fuf_cont[i] = free;
		LRU_indicator[i] = 0;
		dirty_indicator[i] = false;
	}

	cache = new char*[cachesize];
	for (i = 0; i < cachesize; i++)
		cache[i] = new char[blocklength];

	page_faults = 0;
}



//----------------------------------------------------------------
Cache::~Cache()
{
	//flush();
	  //Note that here we no longer flush the buffers in the destructor.  You should
	  //flush all the contents in the tree destructor instead
      
	delete[] cache_cont;
	delete[] fuf_cont;
	delete[] LRU_indicator;
	delete[] cache_tree;
	delete [] dirty_indicator;

	for (long long i=0;i<cachesize;i++)
	    delete[] cache[i];
	delete[] cache;
}
//----------------------------------------------------------------
long long Cache::next()
{
   long long ret_val, tmp;

   if (cachesize == 0) return -1;
   else
   {
       if (fuf_cont[ptr] == free)
       {
    	   ret_val = ptr++;  ptr = ptr % cachesize;
    	   return ret_val;
	   }
	   else
	   {
		 tmp= (ptr+1) % cachesize;
		   //find the first free block
		 while (tmp != ptr && fuf_cont[tmp] != free)
		    tmp = (tmp + 1) % cachesize;

		 if (ptr == tmp)	//failed to find a free block
		 {
	         // select a victim page to be written back to disk
             long long lru_index = 0; // the index of the victim page
             for (long long i = 0; i < cachesize; i++)
                if (LRU_indicator[i] > LRU_indicator[lru_index])
                    lru_index = i;

             ptr = lru_index;

    		 if (dirty_indicator[ptr])
			 {
				 cache_tree[ptr] -> file -> write_block(cache[ptr], cache_cont[ptr]-1);
				 //*****line added by TAO Yufei*****
				 page_faults++;
				 //*********************************
			 }

    		 fuf_cont[ptr] = free;  dirty_indicator[ptr] = false;

    		 ret_val=ptr++;  ptr = ptr % cachesize;
				
    		 return ret_val;
    	 }
    	 else  //a cache page has been found
		     return tmp;
		}
   }
}

void Cache::clear()
{
	for (long long i = 0; i < cachesize; i++)
	{
    	cache_cont[i] = 0;
    	cache_tree[i] = NULL;
    	fuf_cont[i] = free;
		LRU_indicator[i] = 0;
		dirty_indicator[i] = false;
	}
	page_faults = 0;
}
//----------------------------------------------------------------
long long Cache::in_cache(long long index, Cacheable *rt)
{
   long long i;
   long long ret_val = -1;
   for (i = 0; i < cachesize; i++)
	   if ((cache_cont[i] == index) && (cache_tree[i] == rt) && (fuf_cont[i] != free))
	   {
	       LRU_indicator[i]=0;
		   //return i;  line commented by TAO Yufei
		     //adding line
		   ret_val = i;
		     //line added
	   }
	   else if (fuf_cont[i] != free)
           LRU_indicator[i]++;   // increase indicator for this block
   return ret_val;
}
//----------------------------------------------------------------
bool Cache::read_block(Block block,long long index, Cacheable *rt)
{
	long long c_ind;

	index++;	// Externe Num. --> interne Num.
	if(index <= rt->file->get_num_of_blocks() && index>0)
	{
    	if((c_ind = in_cache(index,rt))>=0)
	   		memcpy(block, cache[c_ind], blocklength);
    	else // not in Cache
    	{
    	    page_faults++;
    		c_ind = next();
    		if (c_ind >= 0) // a block has been freed in cache
    		{
        		rt -> file -> read_block(cache[c_ind],index-1); // ext. Num.
        		cache_cont[c_ind] = index;
        		cache_tree[c_ind] = rt;
        		fuf_cont[c_ind] = used;
        		LRU_indicator[c_ind] = 0;
        		memcpy(block, cache[c_ind], blocklength);
    		}
    		else
    		    rt -> file -> read_block(block,index - 1); // read-through (ext.Num.)
    	}
    	return TRUE;
	}
	else
	{
		printf("Requested block %d is illegal.", index - 1);  error("\n", true);
    }

	return false;
}
//----------------------------------------------------------------
bool Cache::write_block(Block block, long long index, Cacheable *rt)
{
	long long c_ind;

	index++;	// Externe Num. --> interne Num.
	if(index <= rt->file->get_num_of_blocks() && index > 0)
	{
    	c_ind = in_cache(index, rt);
    	if(c_ind >= 0)	
		{
			memcpy(cache[c_ind], block, blocklength);
			dirty_indicator[c_ind] = true;
		}
    	else		// not in Cache
    	{
    		c_ind = next();
    		if (c_ind >= 0)
    		{
        		memcpy(cache[c_ind],block,blocklength);
        		cache_cont[c_ind] = index;
        		cache_tree[c_ind] = rt;
        		fuf_cont[c_ind] = used;
        		LRU_indicator[c_ind] = 0;
				dirty_indicator[c_ind] = true;
    		}
    		else
			{
		  		rt -> file -> write_block(block,index - 1);
				//*****line added by TAO Yufei*****
				page_faults++;
				//*********************************
			}
	    }
	    return TRUE;
	}
	else
	{
		printf("Requested block %d is illegal.", index - 1);  error("\n", true);
    }

	return false;
}
//----------------------------------------------------------------
bool Cache::fix_block(long long index, Cacheable *rt)
{
	long long c_ind;

	index++;	// Externe Num. --> interne Num.
	if (index <= rt -> file -> get_num_of_blocks() && index>0)
	{
    	if((c_ind = in_cache(index, rt)) >= 0)
		{
			fuf_cont[c_ind] = fixed;
			return true;
		}
		else
			return false;
	}
	else
	{
		printf("Requested block %d is illegal.", index - 1);  error("\n", true);
    }

	return false;;
}
//----------------------------------------------------------------
bool Cache::unfix_block(long long index, Cacheable *rt)
{
	long long i;

	i = 0;
	index++;	// Externe Num. --> interne Num.
	if(index <= rt -> file -> get_num_of_blocks() && index > 0)
	{
    	while(i < cachesize && (cache_cont[i] != index || fuf_cont[i] == free))
    		i++;
    	if (i != cachesize)
    		fuf_cont[i] = used;
    	return TRUE;
	}
	else
	{
		printf("Requested block %d is illegal.", index - 1);  error("\n", true);
    }

	return false;
}
//----------------------------------------------------------------
void Cache::unfix_all()
{
	long long i;

	for (i = 0; i < cachesize; i++)
		if (fuf_cont[i] == fixed)
			fuf_cont[i] = used;
}
//----------------------------------------------------------------
void Cache::set_cachesize(long long size)
{
	long long i;

	if (size >= 0)
	{
    	ptr = 0;
    	flush();
    	for(i = 0; i < cachesize; i++)
    		delete[] cache[i];
    	delete[] cache;

    	delete[] cache_cont;
    	delete[] cache_tree;
    	delete[] fuf_cont;
    	delete[] LRU_indicator;
		delete[] dirty_indicator;

    	cachesize = size;
    	cache_cont = new long long[cachesize];
    	cache_tree = new Cacheable*[cachesize];
    	LRU_indicator = new long long[cachesize];
    	fuf_cont = new uses[cachesize];
		dirty_indicator = new bool[cachesize];

    	for (i=0; i<cachesize; i++)
    	{
    		cache_cont[i] = 0;
    		cache_tree[i] = NULL;
    		fuf_cont[i] = free;
			LRU_indicator[i] = 0;
			dirty_indicator[i] = 0;
    	}

    	cache = new char*[cachesize];
    	for (i = 0; i < cachesize; i++)
    		cache[i] = new char[blocklength];
	}
	else
	{
	    error("Cache size cannot be negative\n",TRUE);
	}
}
//----------------------------------------------------------------
void Cache::flush()
{
	long long i;

	for (i=0; i<cachesize; i++)
	{
		if (fuf_cont[i] != free && dirty_indicator[i])
		{
				cache_tree[i]->file->write_block(cache[i], cache_cont[i]-1); // ext.Num.
				page_faults++; //add by Lu Chen
		}
		fuf_cont[i] = free;
	}
}
