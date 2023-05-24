#ifndef __Object
#define __Object

#include <string>
using namespace std;

#define MAX(x, y) ((x>y)? (x): (y))

class Object
{
public:
	int id;
	float * data;
	int size;
	unsigned* key;
	int keysize;
	string keycomp;
	void compress();
	Object();
	Object(const Object & o);
	~Object();
	int write_to_buffer(char* buffer);
	int read_from_buffer(char* buffer);
	int getsize();
	double distance(const Object& other) const;

    bool operator < (const Object &a) const ;
	bool operator > (const Object &a) const ;
};

#endif