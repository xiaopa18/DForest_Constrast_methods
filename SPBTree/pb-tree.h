
#ifndef __PBTREE
#define __PBTREE

#include ".\btree\b-tree.h"
#include ".\object.h"
#include ".\RAF.h"
#include ".\blockfile\cache.h"
#include ".\blockfile\blk_file.h"
#include ".\btree\b-node.h"
#include ".\btree\b-entry.h"
#include ".\SortEntry.h"
#include <cmath>
using namespace std;
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

	Object * ptable;
	Object * cand;  
	int num_cand;    
	int num_piv;    
	double eps;
	int bits; 
	

	PB_Tree();
	~PB_Tree();

	void Zconvert(Object * o);
	unsigned* Zconvert(int * dists);
	int* R_Zconvert(unsigned* key);
	
	unsigned* Hconvert(unsigned* hcode,  unsigned* point, int DIMS );
	unsigned* R_Hconvert(unsigned* point, unsigned* hcode, int DIMS );
	void Hconvert(Object * o);

	void readptable(char* pname);

	double** MaxPrunning(Object* O, int num);
	double** FFT(Object* O, int num);

	void PivotSelect(Object * O, Object * Q, int o_num, int q_num);
	void PivotSelect(Object * O, Object * Q, int o_num, int q_num, double thred);
	void RPivotSelect(Object * O, Object * Q, int o_num, int q_num);
    void bulkload(Object* O,int o_num);
	void H_bulkload(Object* O,int o_num);
	int idist(int * q, int* mi, int* ma);
	int edist(int * query, int* o);
	int edist(int * query, unsigned *o);
	int determine(int * min1, int * max1, int* min2, int* max2);
	void RRQH_new(B_Node* node, vector<int>* result, int* qmin, int* qmax, int* min, int *max, bool flag, bool validate, int* query, int radius); 
	int ImprovedRQ_new(Object* q, double radius); //spb
	double BFkNN(Object*q, int k);
	bool  inregion(int * point, int* oqmin, int* oqmax);
	Object * getobject (int ptr);
   
};
#endif