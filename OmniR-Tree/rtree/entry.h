/*entry.h
  this file defines class Entry*/
#ifndef __Entry 
#define __Entry
//------------------------------------------------------------
#include "../func/gendef.h"
#include <string>
using namespace std;
//------------------------------------------------------------
class RTNode;
class RTree;
struct Linkable;
//------------------------------------------------------------
class Entry 
{
public:
//--===on disk===--
	long long son;
	float *bounces; 

//--===others===--
	long long dimension;                      
	long long level;
	long long ptr;
    RTree *my_tree;                     
    RTNode *son_ptr;                    
   

//--===functions===--
	Entry();
	Entry(long long dimension, RTree *rt);
    ~Entry();

	void del_son();
	Linkable *gen_Linkable();
	long long get_size(); 
	RTNode *get_son();
	void init_entry(long long _dimension, RTree *_rt);
	void read_from_buffer(char *buffer);// reads data from buffer
    SECTION section(float *mbr);        // tests, if mbr intersects the box
	bool section_circle(float *center, float radius);
	void set_from_Linkable(Linkable *link);
    void write_to_buffer(char *buffer); // writes data to buffer

    virtual Entry & operator = (Entry &_d);
	bool operator == (Entry &_d);
};

#endif