#ifndef __Object
#define __Object

#include <string>
using namespace std;

#define MAX(x, y) ((x>y)? (x): (y))

class Object
{
public:
	long long id;
	float * data;
	long long size;
	unsigned long long* key;
	long long keysize;
	string keycomp;
	void compress();
	Object();
	Object(const Object & o);
	~Object();
	long long write_to_buffer(char* buffer);
	long long read_from_buffer(char* buffer);
	long long getsize();
	double distance(const Object& other) const;

    bool operator < (const Object &a) const ;
	bool operator > (const Object &a) const ;
};

#endif