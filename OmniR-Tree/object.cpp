#include "object.h"
#include <stdio.h>
#include <memory.h>
extern double compdists;
#include <math.h>
#include <sstream>
#include <iostream>
#ifndef MIN
#define MIN(x, y) ((x<y)? (x): (y))
#define MAX(x, y) ((x>y)? (x): (y))
#endif
long long ** table1 = NULL;
extern long long func;

Object::Object()
{
	x = NULL;
}

Object::Object(const Object& o)
{
	id = o.id;
	size = o.size;
	key =o.key;
	num_piv = o.num_piv;
	if(o.x!=NULL)
	{
		x = new float[size];
		for(long long i =0;i<size;i++)
			x[i] = o.x[i];
	}
	else
		x= NULL;
}


Object::~Object()
{
	if(x!= NULL)
		delete[] x;
	x = NULL;
}


long long Object::getsize()
{
	return size*sizeof(float)+2*sizeof(long long);
}

long long Object::read_from_buffer(char * buffer)
{
	long long i;
	memcpy(&id, buffer, sizeof(id));
	i = sizeof(id);

	memcpy(&size, &buffer[i], sizeof(size));
	i+= sizeof(size);

	x = new float[size];

	memcpy(x, &buffer[i], size * sizeof(float));
	i += size * sizeof(float);

	return i;
}

long long Object::write_to_buffer(char *buffer)
{
	long long i;

	memcpy(buffer, &id, sizeof(id));
	i = sizeof(id);

	memcpy(&buffer[i], &size, sizeof(size));
	i += sizeof(size);

	memcpy(&buffer[i], x, size * sizeof(float));
	i += size * sizeof(float);
	return i;
}

double Object::distance(const Object& other) const
{
	double dist=0.0;

	compdists++;

	if (func == 1) {
		for (long long i = 0; i < size; i++) dist += fabs(x[i] - other.x[i]); // L1
	}
	else if (func == 2) {
		for (long long i = 0; i < size; i++) dist += pow(x[i] - other.x[i], 2); // L2
		dist = sqrt(dist);
	}
	else if (func == 5) {
		for (long long i = 0;i < size;i++) dist += pow(fabs(x[i] - other.x[i]), 5);
		dist = pow(dist, 0.2);
	}
	else {
		for (long long i = 0; i<size; i++) dist = MAX(dist, fabs(x[i] - other.x[i])); // Linf
	}
	return dist;
}
