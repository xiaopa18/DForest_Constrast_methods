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
	data = NULL;
	key = NULL;
}

Object::Object(const Object& o)
{
	id = o.id;
	size = o.size;
	keysize = o.keysize;
	if(o.data!=NULL)
	{
		data = new float[size];
		for(long long i =0;i<size;i++)
			data[i] = o.data[i];
	}
	else
		data = NULL;
	if(o.key!=NULL)
	{
		key = new unsigned long long[keysize];
		for(long long i =0;i<keysize;i++)
			key[i] = o.key[i];
	}
	else
		key = NULL;
	keycomp = o.keycomp;
}

void Object::compress()
{	
	for(long long i=0;i<keysize;i++)
	{
		stringstream ss;
		string t;
		ss<<key[i];
		ss>>t;
		keycomp.append(t);
	}
}

Object::~Object()
{
	if(data!= NULL)
		delete[] data;
	data = NULL;
	if(key!= NULL)
		delete[] key;
	key = NULL;
}

  bool  Object::operator< (const Object &a) const 
  {
	  bool flag = true;
	  for(long long i =keysize -1;i>=0;i--)
	  {
		  if(key[i] < a.key[i])
		  {
			  break;
		  }
		  if(key[i]>a.key[i])
		  {
			  flag = false;
			  break;
		  }
	  }
	  return flag;
  }

  bool  Object::operator> (const Object &a) const 
  {
	  bool flag = false;
	  for(long long i =keysize-1;i>=0;i--)
	  {
		  if(key[i] < a.key[i])
		  {
			  break;
		  }
		  if(key[i]>a.key[i])
		  {
			  flag = true;
			  break;
		  }
	  }
	  return flag;
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

	data = new float[size];

	memcpy(data, &buffer[i], size * sizeof(float));
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

	memcpy(&buffer[i], data, size * sizeof(float));
	i += size * sizeof(float);
	return i;
}

double Object::distance(const Object& other) const
{
	double dist=0.0;

	compdists++;

	if (func == 1) {
		for (long long i = 0; i < size; i++) dist += fabs(data[i] - other.data[i]); // L1
	}
	else if (func == 2) {
		for (long long i = 0; i < size; i++) dist += pow(data[i] - other.data[i], 2); // L2
		dist = sqrt(dist);
	} 
	else if (func == 5) {
		for (long long i = 0;i < size;i++) dist += pow(fabs(data[i] - other.data[i]), 5);  // L5
		dist = pow(dist, 1 / 5.0);
	}
	else {
		for(long long i=0; i<size; i++) dist=MAX(dist, fabs(data[i]-other.data[i])); // Linf
	}
	return dist;
}
