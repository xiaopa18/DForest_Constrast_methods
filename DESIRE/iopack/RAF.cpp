#include "RAF.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fstream>
#include "../object.h"
#include "blk_file.h"
#include "gadget.h"
extern long long bolcksize; 
extern RAF* Obj_f;
extern RAF* Index_f;
extern Cache* Obj_c;
extern Cache* Index_c;

void add_fname_ext(char * _fname)
{
	strcat(_fname, ".raf");
}

void RAF::init(char *_fname, long long _b_length, Cache *_c)
{
	char *fname = new char[strlen(_fname) + 10]; //allow 9 characters for extension name
	strcpy(fname, _fname);
	add_fname_ext(fname);
	last_ptr= 2 * sizeof(long long);
	FILE *fp = fopen(fname, "r");
	if (fp)
	{
		fclose(fp);
	//	printf("The file \"%s\" exists. Replace? (y/n)", fname);
	//	char c = getchar(); getchar();
	//	if (c != 'y' && c != 'Y') 
	//		error("", true);
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
	last_ptr = 2 * sizeof(long long);
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

long long RAF::add_object(Object* obj) {
	char* buffer= new char[file->blocklength];
	long long i = last_ptr,sizeo= obj->getsize();
	long long fnum = last_ptr / file->blocklength;
	//cout << "trying " << last_ptr << " fnum " << fnum << endl;
//	cout << "number " << file->number << endl;
	if (last_ptr % file->blocklength + sizeo < file->blocklength) {
		i = last_ptr;
		file->read_block(buffer,fnum);
		obj->write_to_buffer(&buffer[last_ptr % file->blocklength]);
		if (Obj_c == NULL) // no cache
			file->write_block(buffer, fnum);
		else
			Obj_c->write_block(buffer, fnum, Obj_f);
	}
	else {
		i = (fnum + 1)* file->blocklength;
		obj->write_to_buffer(buffer);
		file->append_block(buffer);
	}
	last_ptr = i + sizeo;
	delete[] buffer;
	buffer = NULL;
	return i;
}

Object* RAF::get_object(long long ptr) {
	Object* q = new Object();
	//cout<<"obj inter "<< Obj_f->file->blocklength <<endl;
	char* buffer = new char[Obj_f->file->blocklength];
	//cout<<"obj inter "<< Obj_f->file->blocklength <<endl;
	long long i = ptr / file->blocklength;
	long long j = ptr % file->blocklength;
	//cout << "i = " << i << " j = " << j << endl;

	if (Obj_c == NULL) {
		//cout<<"to read "<<endl;
		file->read_block(buffer, i);
		//cout<<"read over"<<endl;
	}
	else {
	//	cout << "read from cache" << endl;
		Obj_c->read_block(buffer,i, Obj_f);		
	}

	q->read_from_buffer(&buffer[j]);
	delete[] buffer;
	return q;
	//printf("%lld %lld %f\n",o->id, o->size, o->data[0]);
}

long long* RAF::buid_from_array(Object ** objset)
{
	char * buffer;
	long long * ptrset = new long long[num_obj];


	//need to be modified
	//for(long long i =0;i<num_obj;i++)
		//order[i]--;
	//	printf("%lld \n",order[i]);

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
		while(i+objset[j]->getsize()<file->blocklength && j< num_obj)
		{
			//printf("%lld %lld %f\n",j, objset[order[j]]->id, objset[order[j]]->data[0]);
			ptrset[j] = file->number * file->blocklength +i;
			objset[j]->write_to_buffer(&buffer[i]);
			i += objset[j]->getsize();
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