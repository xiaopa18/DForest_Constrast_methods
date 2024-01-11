#ifndef _RAF
#define _RAF
class BlockFile;
class Cache;
class Object;
#include "blockfile/cache.h"

class RAF: public Cacheable
{
public:
	long long num_obj; // the total number of objects

	void init(char *_fname, long long _b_length, Cache *_c);
	void init_restore(char *_fname, Cache *_c);

	long long* buid_from_array(Object** objset, long long * order);
};

#endif
