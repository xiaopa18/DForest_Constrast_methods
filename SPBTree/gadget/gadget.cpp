#include "gadget.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../heap/binheap.h"
#include<algorithm>


ExtSort::ExtSort()
{
	src_fname = tar_fname = NULL;
	working_folder = NULL;

	compare_func = NULL;
}

ExtSort::~ExtSort()
{
	if (working_folder)
		delete [] working_folder;
}

/*****************************************************************
para:
- n: mem size in the # of elements
- width: element size in the # of bytes
- src_fname: source file name
- tar_fname: target file name
*****************************************************************/

void ExtSort::init(long long _n, char *_src_fname, char *_tar_fname)
{
	n = _n;
	if (n < 2)
		error("at least 2 memory slots for external sort\n", true);

	src_fname = _src_fname;
	tar_fname = _tar_fname;

	long long len = strlen(_tar_fname);
	working_folder = new char[len + 1];
	strcpy(working_folder, _tar_fname);

	long long pos = -1; 
	for (long long i = len - 1; i >= 0; i --)
		if (working_folder[i] == '/')
		{
			pos = i;
			break;
		}
	working_folder[pos + 1] = '\0';
}

/*****************************************************************
this is an auxiliary function needed by esort. it returns the file 
name of a given run

para:
- step: the step count
- run: the run count of this step
- (out) fname: the constructed file name
*****************************************************************/

void ExtSort::get_run_fname(long long _step, long long _run, char *_fname)
{
	char *step_name = new char[100];
	char *run_name = new char[100];
	
	//itoa(_step, step_name, 10);
	//itoa(_run, run_name, 10);
	snprintf(step_name, 10, "%d", _step);
	snprintf(run_name, 10, "%d", _run);
	strcpy(_fname, working_folder);
	strcat(_fname, step_name);
	strcat(_fname, "-");
	strcat(_fname, run_name);
	strcat(_fname, ".es");

	delete [] run_name;
	delete [] step_name;
}

/*****************************************************************
get initial runs

para:
- pass: which merging pass this is
- run_num: the # of runs from the last pass

return value:
- the number of runs produced in this pass
*****************************************************************/

long long ExtSort::merge_runs(long long _pass, long long _run_num)
{
	long long ret = 0;
	long long num_completed_runs = 0;

	char fname[100];
	FILE **files = new FILEptr[n];

	voidptr *mem = new voidptr[n];
	for (long long i = 0; i < n; i ++)
		mem[i] = new_elem();

	while (num_completed_runs < _run_num)
	{
		long long num_act_runs = min(n, _run_num - num_completed_runs);

		for (long long i = 0; i < num_act_runs; i ++)
		{
			get_run_fname(_pass, num_completed_runs + i + 1, fname);

			files[i] = fopen(fname, "r");
			if (!files[i])
			{
				printf("reached the OS' limit of the # of opened files\n", fname);
				printf("working with fewer runs instead, i.e., %d \n", i - 1);

				fclose(files[i - 1]);							//reserving a file handle for ofp
				num_act_runs = i - 1;

				if (num_act_runs < 2)
				{
					printf("now only %d active run(s). quitting...\n");
					exit(1);
				}
			}
		}

		get_run_fname(_pass + 1, ret + 1, fname);
		FILE *ofp = fopen(fname, "w");
		if (!ofp)
		{
			printf("could not create file %s.\n", fname);
			printf("quitting...\n");
			exit(1);
		}

		long long num_still_active = num_act_runs;

		ExtSortBinHeap *hp = new ExtSortBinHeap();
		hp->ext_sort_comp_func = compare_func;

		//----- get the first record from each file -----
		for (long long i = 0; i < num_act_runs; i ++)
		{
			ExtSort_heap_entry_data *item = new ExtSort_heap_entry_data;
			item->file_id = i;
			item->elemadd = &mem[i];
			read_elem(files[i], * (item->elemadd));

			BinHeapEntry *he = new BinHeapEntry();
			he->data = item;
			hp->insert(he);
		}

		//----- repetitively get the smallest element in memory ------
		while (num_still_active > 0)
		{
			BinHeapEntry *he = hp->remove();
			ExtSort_heap_entry_data *item = (ExtSort_heap_entry_data *) he->data;
			write_elem(ofp, * item->elemadd);

			if(read_elem(files[item->file_id], * item->elemadd))
				hp->insert(he);
			else
			{
				fclose(files[item->file_id]);
				item->elemadd = NULL;
				delete item;
				he->data = NULL;
				delete he;
				num_still_active --;
			}
		}

		if (hp->root != NULL)
		{
			printf("extsort::merge_runs - heap not empty\n");
			exit(1);
		}

		delete hp;
		fclose(ofp);
		ret ++;

		num_completed_runs += num_act_runs;
	}

	for (long long i = 0; i < n; i ++)
		destroy_elem(&mem[i]);
	delete [] mem;

	delete [] files;

	//----- remove the run files of the previous pass -----
	for (long long i = 0; i < _run_num; i ++)
	{
		get_run_fname(_pass, i + 1, fname);
		remove(fname);
	}

	return ret;
}

/*****************************************************************
reads an element from the file. 

para:

return value:
- if successful return true. otherwise (i.e., end of file) return false
*****************************************************************/

bool exExtSort::read_elem(FILE *_fp, void *_elem)
{
	bool ret = true; 
	double *v = (double *) _elem;

	if (!feof(_fp))
	{
		fscanf(_fp, "%lf", v);

		for (long long i = 1; i < keysize; i ++)
		{
			fscanf(_fp, " %lf", &v[i]);
		}

		fscanf(_fp, "\n");
	}
	else 
		ret = false;

	return ret;
}

/*****************************************************************
write an element to a file. 

para:

return value:
- if successful return true. otherwise (i.e., end of file) return false
*****************************************************************/

void exExtSort::write_elem(FILE *_fp, void *_elem)
{
	double *v = (double *) _elem;

	fprintf(_fp, "%d", (long long) v[0]);

	for (long long i = 1; i < keysize; i ++)
		fprintf(_fp, " %d", (long long) v[i]);

	fprintf(_fp, "\n");
}

/*****************************************************************
initiates the memory allocated to an element

para:

return value:
- an element
*****************************************************************/

void * exExtSort::new_elem()
{
	double *v = new double[keysize];
	return v;
}

/*****************************************************************
destroys the memory allocated to an element

para:
- elem:

return value:
- the number of runs produced
*****************************************************************/

void exExtSort::destroy_elem(void **_elem)
{
	double *v = (double *) (*_elem);
	delete [] v;
	(*_elem) = NULL;
}

/*****************************************************************
get initial runs

para:

return value:
- the number of runs produced
*****************************************************************/

long long ExtSort::get_initial_runs()
{
	FILE * src_fp = fopen(src_fname, "r");
			
	char fname[100];
	voidptr *mem = new voidptr[n];
	for (long long i = 0; i < n; i ++)
		mem[i] = new_elem();

	long long run = 0;												//the label of the current run
	long long r_cnt =	0;												//counts how many records read

	bool again = true;
	while (again)
	{
		if (!feof(src_fp))
		{
			read_elem(src_fp, mem[r_cnt]);
			r_cnt++;
		}
		
		if (r_cnt == n || feof(src_fp))
		{
			//sort the last batch of objects read
			std::sort(mem,mem+r_cnt,compare_func);
			//qsort(mem, r_cnt, sizeof(void *), compare_func);

			//write these objects to the disk
			get_run_fname(0, run + 1, fname);
			FILE *ofp = fopen(fname, "w");
			if (!ofp)
			{
				printf("failed to create %s\n", fname);
				exit(1);
			}
	
			for (long long i = 0; i < r_cnt; i ++)
			{
				write_elem(ofp, mem[i]);
			}	
			fclose(ofp);

			run ++;
			r_cnt = 0;
		}

		if (feof(src_fp))
		{
			again = false;
		}
	}

	for (long long i = 0; i < n; i ++)
		destroy_elem(&mem[i]);
	delete [] mem;

	fclose(src_fp);

	return run;
}

/*****************************************************************
external sort 

para:
*****************************************************************/

void ExtSort::esort()
{
	if (!compare_func)
	{
		printf("extsort::esort - compare_func not set\n");
		exit(1);
	}

	//----- check path correctness -----
	FILE * src_fp = fopen(src_fname, "r");
	FILE * tar_fp = fopen(tar_fname, "w");
	if (!src_fp || !tar_fp)
	{
		error("wrong source or destination path\n", true);
	}
	fclose(src_fp);
	fclose(tar_fp);
	remove(tar_fname);
	//----------------------------------

	long long pass = 0;

	//----- initial runs -----
	long long run_num = get_initial_runs();
	//------------------------

	//----- merging -----
	while (run_num > 1)
	{
		run_num = merge_runs(pass, run_num);
		pass ++;
	}
	//-------------------

	char fname[100];
	get_run_fname(pass, 1, fname);
	rename(fname, tar_fname);
}

long long ExtSortBinHeap::compare(const void *_e1, const void *_e2)
{
	ExtSort_heap_entry_data * data1 = (ExtSort_heap_entry_data *) _e1;
	ExtSort_heap_entry_data * data2 = (ExtSort_heap_entry_data *) _e2;

	return ext_sort_comp_func(data1->elemadd, data2->elemadd);
}

/*****************************************************************
prints an err msg 

para:
- msg: 
- exit: true if you want to exit the program
*****************************************************************/

void error(char *_msg, bool _exit)
{ 
	printf(_msg); 

	if (_exit) 
		exit(1); 
}

/*****************************************************************
compares two double numbers

para:
- v1:
- v2:

return:
- -1: v1 smaller
  0: tie
  1: v2 smaller
*****************************************************************/

long long compfloats(double _v1, double _v2)
{
	long long ret = 0; 

	if (_v1 - _v2 < -FLOATZERO)
		ret = -1;
	else if (_v1 - _v2 > FLOATZERO)
		ret = 1;
	else ret = 0;

	return ret;
}

/*****************************************************************
checks if v is a power of 2

para:
- v

return:
- true or false
*****************************************************************/

bool is_pow_of_2(long long _v)
{
	long long x = (long long) (log( (double) _v) / log(2.0));
	long long y = (long long) pow(2.0, x);

	return (_v == y);
}

/*****************************************************************
get the part of a path after the last blackslash.

e.g., given path = "./ex/ex/ex.h", return "ex.h"

para
- path:			the given path. 
- (out) fname:	the returned part (usually a file name)
*****************************************************************/

void getFNameFromPath(char *_path, char *_fname)
{
	long long i;
	long long len = strlen(_path);
	long long pos = -1;

	for (i = len - 1; i >= 0; i --)
	{
		if (_path[i] == '/')
		{
			pos = i;
			break;
		}
	}

	pos ++;

	for (i = pos; i <= len; i ++)
	{
		_fname[i - pos] = _path[i];
	}

	_fname[i - pos] = '\0';
}

/*****************************************************************
this function gets the part of the given path up to the last folder.
e.g, given ./ex/ex/1.zip, the function returns ./ex/ex/

para:
- path
- (out) folder: 
*****************************************************************/

void get_leading_folder(char *_path, char *_folder)
{
	long long len = strlen(_path);
	long long pos = -1;

	for (long long i = len - 1; i >= 0; i --)
	{
		if (_path[i] == '/')
		{
			pos = i; 
			break;
		}
	}

	long long i;
	for (long long i = 0; i <= pos; i ++)
	{
		_folder[i] = _path[i];
	}

	_folder[i] = '\0';
}

/*****************************************************************
just gets n tabs on the screen.

para:
- n
*****************************************************************/

void blank_print(long long _n)
{
	for (long long i = 0; i < _n; i ++)
		printf("   ");
}

double l2_dist_int(long long *_p1, long long *_p2, long long _dim)
{
	double ret = 0;
	for (long long i = 0; i < _dim; i ++)
	{
		double dif = (double) (_p1[i] - _p2[i]);
		ret += dif * dif;
	}

	return ret;
}

/*----------------------------------------------------------------
DESCRIPTION:
Get the next word from a string.

PARA:

_s (in/out):		the string. At finish, _s will point to the
                    char right after the word returned.
_w (out):			the word returned.


RETURN:
the character. 
----------------------------------------------------------------*/

void getnextWord(char **_s, char *_w)
{
	while (**_s == ' ')
	{
		(*_s)++;
	}

	while (**_s != ' ' && **_s != '\0')
	{
		*_w = **_s;
		(*_s)++; _w++;
	}

	*_w = '\0';
}

/*----------------------------------------------------------------
DESCRIPTION:
Get the next non-white character from a string.

PARA:
_s (in/out):		the string. when the function finishes, _s
					will point to the character after the one
					returned.

RETURN:
the character. 
----------------------------------------------------------------*/

char getnextChar(char **_s)
{
	char ret;

	while (**_s == ' ')
	{
		(*_s)++;
	}

	ret = **_s;
	(*_s)++;

	return ret;
}

/*****************************************************************
print the last m bits of an integer n

-para-
n
m		
*****************************************************************/

void printINT_in_BIN(long long _n, long long _m)
{
	long long		i		= -1;
	long long		mask	= -1;
	long long		n		= -1;

	mask = 1 << _m;
	mask --;
	
	n = _n & mask;

	mask = 1 << (_m - 1);

	for (i = 0; i < _m; i ++)
	{
		if (mask & n)
			printf("1");
		else
			printf("0");

		mask >>= 1;
	}
	printf("\n");
}
