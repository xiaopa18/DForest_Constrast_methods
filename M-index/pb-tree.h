
#ifndef __PBTREE
#define __PBTREE
#include<vector>
#include ".\btree\b-tree.h"
#include ".\object.h"
#include ".\RAF.h"
#include ".\blockfile\cache.h"
#include ".\blockfile\blk_file.h"
#include ".\btree\b-node.h"
#include ".\btree\b-entry.h"
#include ".\SortEntry.h"
#define WORDBITS 32
#define NUMBITS 32

class kNNEntry
{
public:
	int ptr;
	int edist;
	int level;
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
	int num_cand;
	int num_piv;


	PB_Tree();
	~PB_Tree();


	void readptable(char *pname);
	void costmodel(Object * q, Object * O, double radius);
	double kNNesti(Object *q, Object *O, int k);

	double** MaxPrunning(Object* O, int num);
	double** FFT(Object* O, int num);

	void PivotSelect(Object * O, Object * Q, int o_num, int q_num);
	void PivotSelect(Object * O, Object * Q, int o_num, int q_num, double thred);
	void RPivotSelect(Object * O, Object * Q, int o_num, int q_num);
    void bulkload(Object* O,int o_num);

	double idist(double * q, double * mi, double* ma);
	int edist(int * query, int* o);
	int edist(int * query, unsigned *o);
	int determine(int * min1, int * max1, int* min2, int* max2);
	bool  inregion(double * point, double* oqmin, double* oqmax);
	Object * getobject (int ptr);

	vector<int> rangequery(Object* q, double r);
	vector<int> rangequery_orignal(Object* q, double r);
	double knn(Object* a, int k);
	double knn_orignal(Object* a, int k, double inc, int datasize);
	double knn_optimal(Object* q, int k);
	double knn_range(Object* q, int k);

};
#endif
