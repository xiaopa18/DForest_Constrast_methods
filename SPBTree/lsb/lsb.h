#ifndef __LSB
#define __LSB

#include <memory.h>
#include "lsbEntry.h"
#include "lsbNode.h"
#include "lsbTree.h"
#include "../blockfile/cache.h"
#include "../gadget/gadget.h"

typedef char * charPtr;
typedef long long * intPtr;
typedef LSBtree * LSBtreePtr;

struct LSB_Hentry
{
	long long		d;
	long long		id;								/* id of the point */
	long long		lcl;							/* lowest common level with the query lcl = u - floor(llcp / m) */
	long long		lr;								/* moving left or right */
	long long		* pt;							/* coordinates */
	long long		treeId;
	double	dist;							/* distance to query */

	void setto(LSB_Hentry * _e)
	{
		d = _e->d;
		id = _e->id;
		lr = _e->lr;
		lcl = _e->lcl;
		memcpy(pt, _e->pt, sizeof(long long) * d);
		treeId = _e->treeId;
		dist = _e->dist;
	}
};

class LSB
{
public:
	//--=== on disk ===--
	long long			t;								/* largest coordinate of a dimension */
	long long			d;								/* dimensionality */
	long long			n;								/* cardinality */
	long long			B;								/* page size in words */
	long long			ratio;							/* approximation ratio */

	//--=== debug ==--
	long long			quiet;
	bool		emergency;

	//--=== others ===--

	char		dsPath[100];					/* folder containing the dataset file */
	char		dsName[100];					/* dataset file name */
	char		forestPath[100];				/* folder containing the forest */

	long long			f;
	long long			pz;								/* number of pages to store the Z-value of a point */
	long long			L;								/* number of lsb-trees */
	long long			m;								/* dimensionality of the hash space */
	long long			qNumTrees;						/* number of lsb-trees used to answer a query */
	long long			u;								/* log_2 (U/w) */

	double		* a_array;
	double		* b_array;						/* each lsb-tree requires m hash functions, and each has function
												   requires a d-dimensional vector a and a 1d value b. so a_array contains
												   l * m * d values totally and b_array contains l *m values */

	double		U;								/* each dimension of the hash space has domain [-U/2, U/2] */
	double		w;

	LSBtreePtr *trees;							/* lsbtrees */

	//--=== Functions ===--
	LSB();
	~LSB();

	//--=== internal ===--
	virtual	long long		cpFind_r			(long long * _r);
	virtual void	freadNextEntry		(FILE *_fp, long long * _son, long long * _key);
	virtual void	gen_vectors			();
	virtual void	getTreeFname		(long long _i, char *_fname);
	virtual void	getHashVector		(long long _tableID, long long *_key, double *_g);
	virtual void	getHashPara			(long long _u, long long _v, double **_a_vector, double *_b);
	virtual double	get1HashV			(long long _u, long long _v, long long *_key);
	virtual	long long		getLowestCommonLevel(long long *_z1, long long *_z2);
	virtual long long		get_m				(long long _r, double _w, long long _n, long long _B, long long _d);
	virtual double	get_rho				(long long _r, double _w);
	virtual long long		get_obj_size		(long long _dim);
	virtual long long		get_u				();
	virtual long long		getZ				(double *_g, long long *_z);
	virtual long long		insert				(long long _treeID, long long _son, long long * _key);
	virtual long long		readParaFile		(char *_fname);
	virtual	void	setHe				(LSB_Hentry *_he, B_Entry *_e, long long *_qz);
	virtual	double	updateknn			(LSB_Hentry * _rslt, LSB_Hentry *_he, long long _k);
	virtual long long		writeParaFile		(char *_fname);

	//--=== external ===--
	virtual long long		buildFromFile		(char *_dsPath, char *_forestPath);
	virtual long long		bulkload			(char *_dspath, char *_target_folder);
	virtual	long long		closepair			(long long _r, long long _k, double *_dist, long long *_pairid);
	virtual long long     cpFast				(long long _k, long long _numTrees, double *_dist, long long *_pairid);
	virtual void	init				(long long _t, long long _d, long long _n, long long _B, long long _L, long long _ratio);
	virtual long long		knn					(long long *_q, long long _k, LSB_Hentry *_rslt, long long _numTrees);
	virtual long long		restore				(char *_paraPath);
};

struct LSBqsortElem
{
	long long	* ds;
	long long	pos;		/* position of the object in array ds */
	long long	pz;			/* size of the z array below */
	long long * z;
};

struct LSBbiPtr
{
	B_Node	* nd;
	long long		entryId;
};

long long		LSB_hcomp		(const void *_e1, const void *_e2);
int		LSBqsortComp	(const void *_e1, const void *_e2);
void	LSB_hdestroy	(const void *_e);

#endif
