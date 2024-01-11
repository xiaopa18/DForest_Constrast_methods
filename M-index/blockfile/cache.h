#ifndef __CACHE
#define __CACHE

#include "f_def.h"
//----------------------------------------------------------------
class Cache;
class BlockFile;

class Cacheable   //inherit this class if you wish to design an external
                  //memory structure that can be cached
{
public:
	BlockFile *file;
	Cache *cache;
};

//----------------------------------------------------------------
class Cache
{
public:
   enum uses {free,used,fixed};	// for fuf_cont
   long long ptr;		        //current position in cache
   long long cachesize;		//the number of blocks kept in memory
   long long blocklength;
   long long page_faults;
   long long *cache_cont;	    // array of the indices of blocks that are in cache
   Cacheable **cache_tree;  // array of ptrs to the correspondent Cacheables where the blocks belong to
   uses *fuf_cont; 		//indicator array that shows whether one cache block is free, used or fixed
   long long  *LRU_indicator; //indicator that shows how old (unused) is a page in the cache
   bool *dirty_indicator;  //indicator that shows if a cache page has been written
	   
   char **cache;   		// Cache

	//=====================================================
   
   long long next();		

   long long in_cache(long long index, Cacheable *rt);

   Cache(long long csize, long long blength);

  

   ~Cache();

   bool read_block(Block b, long long i, Cacheable *rt);

   bool write_block(Block b, long long i, Cacheable *rt);

   bool fix_block(long long i, Cacheable *rt);

   bool unfix_block(long long i, Cacheable *rt);

   void unfix_all();

   void set_cachesize(long long s);

   void flush();		
   void clear();	
};


#endif // __CACHE
