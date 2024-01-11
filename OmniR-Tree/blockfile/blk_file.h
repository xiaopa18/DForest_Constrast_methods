/*blk_file.h:
  this file specifies the class BlockFile, a block file simulator
*/
#ifndef __BLK_FILE
#define __BLK_FILE

//----------------------------------------------------------
#include <stdio.h>
#include "../func/gendef.h"
//----------------------------------------------------------
class BlockFile
{
public:
   FILE* fp;			// os file pointer
   char* filename;		 
   long long blocklength;	    // length of a block
   long long act_block; 	    // block # of fp's position (fp can stay at block boundaries)
   long long number;		    // total # of blocks
   bool new_flag;		// specifies if this is a new file

     //=====================================================
   BlockFile(char* name, long long b_length);
   			        // Filename und Blocklaenge (fuer neue Files)
   ~BlockFile();

   void put_bytes(char* bytes,long long num)
		{ fwrite(bytes,num,1,fp); 
	    }

   void get_bytes(char* bytes,long long num)	     
		{ fread(bytes,num,1,fp); 
       	}

   void fwrite_number(long long num);	

   long long fread_number();		

   void seek_block(long long bnum)    
		{ fseek(fp,(bnum-act_block)*blocklength,SEEK_CUR); }

   void read_header(char * header);

   void set_header(char* header);
   					
   bool read_block(Block b,long long i);	

   bool write_block(Block b,long long i);

   long long append_block(Block b);	

   bool delete_last_blocks(long long num);

   bool file_new()			
		{ return new_flag; }

   long long get_blocklength()	
		{ 
	       return blocklength; }

   long long get_num_of_blocks()	
		{ return number; }
};

//-------------------------------------------------------------------------
class CachedBlockFile : public BlockFile
{
public:
   enum uses {free,used,fixed};
   long long ptr;		        // current position in cache
   long long cachesize;		// //the number of blocks kept in memory
   long long page_faults;     

   long long *cache_cont;	    // array of the indices of blocks that are in cache
   uses *fuf_cont; 		// indicator array that shows whether one cache block is free, used or fixed
   long long  *LRU_indicator; // indicator that shows how old (unused) is a page in the cache
   bool  *dirty_indicator;  // indicator that shows if a page has been modified

   char **cache;   		// Cache

	//=====================================================

   CachedBlockFile(char* name, long long blength, long long csize);
   					
   ~CachedBlockFile();

   long long next();		

   long long in_cache(long long index);	// liefert Pos. im Cache oder -1

   bool read_block(Block b,long long i);

   bool write_block(Block b,long long i);

   bool fix_block(long long i);

   bool unfix_block(long long i);

   void unfix_all();			

   void set_cachesize(long long s);

   void flush();
};


#endif // __BLK_FILE
