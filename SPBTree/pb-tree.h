
#ifndef __PBTREE
#define __PBTREE

#include "./btree/b-tree.h"
#include "./object.h"
#include "./RAF.h"
#include "./blockfile/cache.h"
#include "./blockfile/blk_file.h"
#include "./btree/b-node.h"
#include "./btree/b-entry.h"
#include "./SortEntry.h"
#include <cmath>
using namespace std;
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

	Object * ptable;
	Object * cand;
	long long num_cand;
	long long num_piv;
	double eps;
	long long bits;


	PB_Tree();
	~PB_Tree();

	void Zconvert(Object * o);
	unsigned long long* Zconvert(long long * dists);
	long long* R_Zconvert(unsigned long long* key);

	unsigned long long* Hconvert(unsigned long long* hcode,  unsigned long long* point, long long DIMS );
	unsigned long long* R_Hconvert(unsigned long long* point, unsigned long long* hcode, long long DIMS );
	void Hconvert(Object * o);

	void readptable(char* pname);

	double** MaxPrunning(Object* O, long long num);
	double** FFT(Object* O, long long num);

	void PivotSelect(Object * O, Object * Q, long long o_num, long long q_num);
	void PivotSelect(Object * O, Object * Q, long long o_num, long long q_num, double thred);
	void RPivotSelect(Object * O, Object * Q, long long o_num, long long q_num);
    void bulkload(Object* O,long long o_num);
	void H_bulkload(Object* O,long long o_num);
	long long idist(long long * q, long long* mi, long long* ma);
	long long edist(long long * query, long long* o);
	long long edist(long long * query, unsigned long long *o);
	long long determine(long long * min1, long long * max1, long long* min2, long long* max2);
	void RRQH_new(B_Node* node, vector<long long>* result, long long* qmin, long long* qmax, long long* min, long long *max, bool flag, bool validate, long long* query, long long radius);
	long long ImprovedRQ_new(Object* q, double radius); //spb
	double BFkNN(Object*q, long long k);
	bool  inregion(long long * point, long long* oqmin, long long* oqmax);
	Object * getobject (long long ptr);

};
#endif
