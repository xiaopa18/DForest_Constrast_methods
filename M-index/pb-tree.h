
#ifndef __PBTREE
#define __PBTREE
#include<vector>
#include "./btree/b-tree.h"
#include "./object.h"
#include "./RAF.h"
#include "./blockfile/cache.h"
#include "./blockfile/blk_file.h"
#include "./btree/b-node.h"
#include "./btree/b-entry.h"
#include "./SortEntry.h"
#define WORDBITS 32
#define NUMBITS 32

class kNNEntry
{
public:
	long long ptr;
	long long edist;
	long long level;
	kNNEntry()
	{
	}

	kNNEntry(const kNNEntry &a)
	{
		ptr = a.ptr;
		edist = a.edist;
		level = a.level;
	}
	bool operator < (const kNNEntry &a) const
	{
		if(fabs((double) edist-a.edist)<0.0001)
			return level >a.level;
		return (edist>a.edist);
	}
};

class PB_Tree
{
public:
	B_Tree * bplus;
	RAF * draf;
	Cache * c;
	BlockFile * bf;

	Object * ptable;
	Object * cand;
	long long num_cand;
	long long num_piv;


	PB_Tree();
	~PB_Tree();


	void readptable(char *pname);
	void costmodel(Object * q, Object * O, double radius);
	double kNNesti(Object *q, Object *O, long long k);

	double** MaxPrunning(Object* O, long long num);
	double** FFT(Object* O, long long num);

	void PivotSelect(Object * O, Object * Q, long long o_num, long long q_num);
	void PivotSelect(Object * O, Object * Q, long long o_num, long long q_num, double thred);
	void RPivotSelect(Object * O, Object * Q, long long o_num, long long q_num);
    void bulkload(Object* O,long long o_num);

	double idist(double * q, double * mi, double* ma);
	long long edist(long long * query, long long* o);
	long long edist(long long * query, unsigned *o);
	long long determine(long long * min1, long long * max1, long long* min2, long long* max2);
	bool  inregion(double * point, double* oqmin, double* oqmax);
	Object * getobject (long long ptr);

	vector<long long> rangequery(Object* q, double r);
	vector<long long> rangequery_orignal(Object* q, double r);
	double knn(Object* a, long long k);
	double knn_orignal(Object* a, long long k, double inc, long long datasize);
	double knn_optimal(Object* q, long long k);
	double knn_range(Object* q, long long k);

};
#endif
