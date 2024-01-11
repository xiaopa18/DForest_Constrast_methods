#include "RAF.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fstream>
#include "./object.h"
#include "./blockfile/blk_file.h"
#include "./gadget/gadget.h"
extern long long bolcksize;


void add_fname_ext(char * _fname)
{
	strcat(_fname, ".raf");
}

void RAF::init(char *_fname, long long _b_length, Cache *_c)
{
	char *fname = new char[strlen(_fname) + 10]; //allow 9 characters for extension name
	strcpy(fname, _fname);
	add_fname_ext(fname);

	FILE *fp = fopen(fname, "r");
	if (fp)
	{
		fclose(fp);
		remove(fname);
	}

    file = new BlockFile(fname, _b_length);
    cache = _c;

	delete []fname;
}

void RAF::init_restore(char *_fname, Cache *_c)
{
	char *fname = new char[strlen(_fname) + 10]; //allow 9 characters for extension name
	strcpy(fname, _fname);
	add_fname_ext(fname);

	//--=== testing if the file exists ===--
	FILE *fp = fopen(fname, "r");
	if (!fp)
	{
		printf("RAF file %s does not exist\n", fname);
		delete []fname;
		error("", true);
	}

	fclose(fp);
	//--=== done testing the file existence ===--

    file = new BlockFile(fname, bolcksize);
    cache = _c;

	delete [] fname;
}

long long* RAF::buid_from_array(Object ** objset, long long * order)
{
	char * buffer;
	long long * ptrset = new long long[num_obj];

	long long i = 0;
	long long j = 0;
	bool flag = true;
	
	while(j<num_obj)
	{
		buffer = new char[file->blocklength];
		if(flag)
		{
			i+=2*sizeof(long long);
			flag = false;
		}
		else
			i=0;
		while(i+objset[order[j]]->getsize()<file->blocklength && j< num_obj)
		{
			ptrset[j] = file->number * file->blocklength +i;
			objset[order[j]]->write_to_buffer(&buffer[i]);
			i += objset[order[j]]->getsize();
			j++;
			if(j>=num_obj)
				break;
		}
		file->append_block(buffer);
		delete[] buffer;
		buffer = NULL;
	}

	return ptrset;
}
