#include "lsb.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lsbEntry.h"
#include "lsbTree.h"
#include "../blockfile/cache.h"
#include "../blockfile/f_def.h"
#include "../gadget/gadget.h"
#include "../rand/rand.h"
#include<algorithm>
#define __int64 long long

LSB::LSB()
{
	t = -1;
	d = -1;
	n = -1;
	m = -1;
	L = -1;
	B = -1;
	w = -1;
	f = -1;
	U = -1;
	pz = -1;
	ratio = -1;
	quiet = 0;

	trees = NULL;
	a_array = NULL;
	b_array = NULL;

	emergency = false;
}

LSB::~LSB()
{
	if (a_array)
	{
		delete [] a_array;
		a_array = NULL;
	}

	if (b_array)
	{
		delete [] b_array;
		b_array = NULL;
	}

	if (trees)
	{
		for (long long i = 0; i < L; i ++)
		{
			trees[i]->close();
			delete trees[i];
			trees[i] = NULL;
		}

		delete [] trees;
		trees = NULL;
	}
}

/*****************************************************************
get the lowest common level of two z-values z1, z2.

-para-
z1		as above
z2		as above

-return-
the level
*****************************************************************/

long long LSB::getLowestCommonLevel(long long *_z1, long long *_z2)
{
	long long ret		= 0;

	long long	c		= -1;
	long long	i		= -1;
	long long	j		= -1;
	long long	mask	= -1;

	c = u * m;

	for (i = 0; i < pz; i++)
	{
		mask = 1 << 30;

		for (j = 0; j < 31 && c > 0; j++)
		{
			if ((_z1[i] & mask) == (_z2[i] & mask))
			{
				ret++;
				c--;
				mask >>= 1;
			}
			else
			{
				i = pz; j = 32;
			}
		}
	}

	ret /= m;
	ret = u - ret;

	return ret;
}

/*****************************************************************
get m, the dimensionality of the hash space.

-para-
r	 approximation ratio
w	 a "width" parameter needed to compute \rho.
n	 cardinality
B	 page size in words
d	 dimensionality

-return-
m
*****************************************************************/

long long LSB::get_m(long long _r, double _w, long long _n, long long _B, long long _d)
{
	long long				ret = -1;

	const double	PI	= 3.14159265;
	double			p2	= 1;

	p2 -= 2 * normal_cdf(-_w/_r, (double) 0.001);
	p2 -= (2 * _r / (sqrt(2*PI) * _w)) * (1 - exp(-_w*_w / (2*_r*_r)));

	ret = (long long) ceil( log( ((double) _n) * _d/_B) / log(1.0/p2));

	return ret;
}

/*****************************************************************
get the value of rho. remember the query time of LSB is (n/b)^\rho.

-para-
r	approximation ratio
w	the width of a hash function. for lsh, a hash function has the
	form \lfloor (a \cdot o + b) / w \rfloor.

-notes-
see sec 4.1 of "locality-sensitive hashing scheme based on p-stable
distributions" by mayur datar et al. in scg 04.

-return-
rho
*****************************************************************/

double LSB::get_rho(long long _r, double _w)
{
	double ret = -1;

	const double PI = 3.14159265;

	double p1 = 1;
	double p2 = 1;

	p2 -= 2 * normal_cdf(-_w / _r, (double) 0.001);
	p2 -= (2 * _r / (sqrt(2 * PI) * _w)) * (1 - exp(-_w * _w / (2 * _r * _r)));

	p1 -= 2 * normal_cdf(-_w / 1, (double) 0.001);
	p1 -= (2 * 1 / (sqrt(2 * PI) * _w)) * (1 - exp(-_w * _w / 2));

	ret = log(1 / p1) / log(1 / p2);

	return ret;
}

/*****************************************************************
init the parameters of a lsb forest

-para-
t		largest coordinate at a dimension
d		dimensionality
n		cardinality
B		page size in words
L		number of lsb-trees (set it to 0 if you want to build a
		complete forest)
ratio	approximation ratio
*****************************************************************/

void LSB::init(long long _t, long long _d, long long _n, long long _B, long long _L, long long _ratio)
{
	t		= _t;
	d		= _d;
	n		= _n;
	B		= _B;
	ratio	= _ratio;

	w		= 4;
	f		= (long long) ceil( log((double) d)/log(2.0) + log((double) t)/log(2.0) );
	m		= get_m(ratio, w, n, B, d);

	L	= (long long) ceil( pow( (double) n*d/B, (double) 1/ratio) );
	if (_L > 0 && _L < L)
		L = _L;

	gen_vectors();

	u		= get_u();
	if (u > 30)
		error("u too large\n", true);

	pz		= (long long) ceil( ((double) (u * m)) / 31 );

	U = (1 << u) * w;

	if (quiet <= 9)
	{
		printf("Parameters:\n");
		printf("\tm = %d\n", m);
		printf("\tl = %d\n", L);
		printf("\tu = %d\n", u);
		printf("\tU = %.1f\n\n", U);
	}
}

/*****************************************************************
read the parameter file.

-para-
fname		full path of the para file

-return-
0		success
1		failure
*****************************************************************/

long long LSB::readParaFile(char *_fname)
{
	long long		ret		= 0;

	FILE	*fp		= NULL;
	long long		acnt	= 0;
	long long		bcnt	= 0;
	long long		i		= -1;
	long long		j		= -1;
	long long		k		= -1;

	fp = fopen(_fname, "r");

	if (!fp)
	{
		printf("Could not open %s.\n", _fname);

		ret = 1;
		goto recycle;
	}

	fscanf(fp, "%s\n", dsPath);
	getFNameFromPath(dsPath, dsName);

	fscanf(fp, "B = %d\n", &B);
	fscanf(fp, "n = %d\n", &n);
	fscanf(fp, "d = %d\n", &d);
	fscanf(fp, "t = %d\n", &t);
	fscanf(fp, "ratio = %d\n", &ratio);
	fscanf(fp, "l = %d\n", &L);

	w	= 4;
	f	= (long long) ceil( log((double) d)/log(2.0) + log((double) t)/log(2.0) );

	m = get_m(ratio, w, n, B, d);

	a_array = new double[L * m * d];
	b_array = new double[L * m];

	acnt = 0;
	bcnt = 0;

	for (i = 0; i < L; i ++)
	{
		for (j = 0; j < m; j ++)
		{
			for (k = 0; k < d; k ++)
			{
				fscanf(fp, "%lf", &a_array[acnt]);

				acnt++;
			}

			fscanf(fp, "\n");

			fscanf(fp, "%lf\n", &b_array[bcnt]);

			bcnt++;

			fscanf(fp, "\n");
		}
	}

	u = get_u();

	pz = (long long) ceil( ((double) (u * m)) / 31 );

	U = (1 << u) * w;

recycle:

	if (fp)
		fclose(fp);

	return ret;
}

/*****************************************************************
load an existing lsb forest.

-para-
paramath		The full path of the parameter file

-return-
0		success
1		failure
*****************************************************************/

long long LSB::restore(char *_paraPath)
{
	long long		ret			= 0;

	char	fname[100];
	long long		i			= -1;
	long long		len			= -1;

	strcpy(forestPath, _paraPath);

	len = strlen(forestPath);
	if (forestPath[len - 1] != '/' && forestPath[len - 1] != '\\')
		strcat(forestPath, "/");

	strcpy(fname, forestPath);
	strcat(fname, "para");

	if (readParaFile(fname))
	{
		ret = 1;
		goto recycle;
	}


	trees = new LSBtreePtr[L];

	for (i = 0; i < L; i ++)
	{
		getTreeFname(i, fname);

		trees[i] = new LSBtree();
		trees[i]->init_restore(fname, NULL);
	}

recycle:
	return ret;
}

/*****************************************************************
get the file name of a lsb-tree

-para-
i		which tree it is, from 0 to L- 1
fname	(out) the file name
*****************************************************************/

void LSB::getTreeFname(long long _i, char *_fname)
{
	char c[100];

	strcpy(_fname, forestPath);
	//itoa(_i, c, 10);
	snprintf(c, 10, "%d", _i);
	strcat(_fname, c);
}

/*****************************************************************
builds a LSB-forest from a dataset. data format:
id coordinate_1 _2 ... _dim

-para-
dsPath			the dataset path
forestPath		the folder containing the forest

-return-
0				success
1				failure

-prior-
all tree parameters properly set.
*****************************************************************/

long long LSB::buildFromFile(char *_dsPath, char *_forestPath)
{
	long long		ret			= 0;

	char	fname[100];
	long long		cnt			= -1;
	long long		i			= -1;
	long long		* key		= NULL;
	long long		son			= -1;
	FILE	* fp		= NULL;

	fp = fopen(_dsPath, "r");

	if (!fp)
	{
		printf("Could not open the source file.\n");

		ret = 1;

		goto recycle;
	}

	fclose(fp);

	strcpy(dsPath, _dsPath);
	getFNameFromPath(dsPath, dsName);

	strcpy(forestPath, _forestPath);
	if (forestPath[strlen(forestPath) - 1] != '/')
		strcat(forestPath, "/");

	strcpy(fname, forestPath);
	strcat(fname, "para");
	if (writeParaFile(fname))
	{
		ret = 1;
		goto recycle;
	}

	key = new long long[d];
	trees = new LSBtreePtr[L];

	for (i = 0; i < L; i ++)
	{
		if (quiet <= 9)
			printf("Tree %d (out of %d)\n", i+1, L);

		trees[i] = new LSBtree();

		getTreeFname(i, fname);

		if (trees[i]->init(fname, 4 * B, d, pz))
		{
			ret = 1;
			goto recycle;
		}


		cnt = 0;
		fp = fopen(dsPath, "r");

		while (!feof(fp) && cnt < n)
		{
			freadNextEntry(fp, &son, key);

			if (son <= 0)
			{
				printf("Sorry no negative id please.\n");

				ret = 1;
				goto recycle;
			}

			insert(i, son, key);

			cnt++;

			if (cnt % 1000 == 0 && quiet <= 8)
			{
				printf("\tInserted %d (%d%%)\n", cnt, cnt*100 / n);
			}
		}

		if (cnt == n && !feof(fp))
		{
			printf("The dataset is larger than you said. Giving up...\n", n);

			ret = 1;
			goto recycle;
		}

		fclose(fp);
	}

	for (i = 0; i < L; i ++)
	{
		trees[i]->close();
		delete trees[i];
		trees[i] = NULL;
	}

	delete [] trees;
	trees = NULL;

recycle:

	if (key)
		delete [] key;

	if (fp)
		fclose(fp);

	return ret;
}
/*****************************************************************
build a LSB forest by bulkloading. this function will be very
fast if you have enough memory. otherwise, use incremental
insertion

-para-
dspath		path of the source file
forestPath	folder of the forest

-return-
0		success
1		failure

-b4 calling-
all tree parameters properly set.
*****************************************************************/

long long LSB::bulkload(char *_dsPath, char *_forestPath)
{
	long long				ret			= 0;

	char			c;
	char			fname[100];
	FILE			* fp		= NULL;
	long long				cnt			= -1;
	long long				* ds		= NULL;		/* dataset sorted by id */
	long long				i			= -1;
	long long				j			= -1;
	long long				memSize		= -1;
	double			* g			= NULL;
	LSBqsortElem	* dsz		= NULL;		/* dataset sorted by z-value */

	memSize = 4 * n * (d + 1) + 4 * (pz + 3) * n;

	if (memSize > 100000000)
	{
		printf("I am going to need around %d mega bytes ram.\n", memSize / 1000000);
		printf("Can you afford it (y/n)?\n");

		c = getchar(); getchar();
		if (c != 'y' && c != 'Y')
		{
			ret = 1;
			goto recycle;
		}
	}

	fp = fopen(_dsPath, "r");
	if (!fp)
	{
		printf("Could not open the data file.\n");

		ret = 1;
		goto recycle;
	}

	fclose(fp);

	strcpy(dsPath, _dsPath);
	getFNameFromPath(dsPath, dsName);

	strcpy(forestPath, _forestPath);
	if (forestPath[strlen(forestPath) - 1] != '/')
		strcat(forestPath, "/");

	strcpy(fname, forestPath);
	strcat(fname, "para");
	if (writeParaFile(fname))
	{
		ret = 1;
		goto recycle;
	}

	ds = new long long[n * (d + 1)];

	fp = fopen(dsPath, "r");

	cnt = 0;
	while(!feof(fp) && cnt < n)
	{
		freadNextEntry(fp, &ds[cnt * (d+1)], &ds[cnt * (d+1) + 1]);
		cnt ++;
	}

	if (cnt != n)
	{
		printf("Dataset smaller than you said.\n");

		ret = 1;
		goto recycle;
	}
	else if (!feof(fp))
	{
		printf("Dataset bigger than you said. \n");

		ret = 1;
		goto recycle;
	}

	fclose(fp);

	g = new double[m];

	dsz = new LSBqsortElem[n];
	for (i = 0; i < n; i ++)
	{
		dsz[i].ds	= ds;
		dsz[i].pos	= i;
		dsz[i].pz	= pz;
		dsz[i].z	= new long long[pz];
	}

	for (i = 0; i < L; i ++)
	{
		for (j = 0; j < n; j ++)
			dsz[j].pos	= j;

		if (quiet <= 9)
		{
			printf("Tree %d (out of %d)\n", i+1, L);
		}

		if (quiet <= 5)
		{
			printf("\tComputing hash values...\n");
		}

		for (j = 0; j < n; j ++)
		{
			getHashVector(i, &(ds[1+(d+1)*j]), g);

			if (getZ(g, dsz[j].z))
			{
				ret = 1;
				goto recycle;
			}
		}

		if (quiet <= 5)
		{
			printf("\tSorting...\n", i);
		}

		//std::sort(dsz,dsz+n,LSBqsortComp);
		qsort(dsz, n, sizeof(LSBqsortElem), LSBqsortComp);

		if (quiet <= 5)
		{
			printf("\tBulkloading...\n", i);
		}

		getTreeFname(i, fname);

		LSBtree * bt = new LSBtree();

		bt->quiet = quiet;
		bt->init(fname, B * 4, d, pz);

		if (bt->bulkload2(dsz, n))
		{
			ret = 1;
			goto recycle;
		}

		bt->close();

		delete bt;
		bt = NULL;
	}

recycle:
	if (fp)
		fclose(fp);

	if (g)
		delete [] g;

	if (ds)
		delete [] ds;

	if (dsz)
		delete [] dsz;

	return ret;
}


/*****************************************************************
reads the next pt from the data file.

-para-
fp		handle of the file
son		(out) pt id
key		pt coordinates
*****************************************************************/

void LSB::freadNextEntry(FILE *_fp, long long * _son, long long * _key)
{
	long long		 i;

	fscanf(_fp, "%d", _son);

	for (i = 0; i < d; i ++)
		fscanf(_fp, " %d", &(_key[i]));

	fscanf(_fp, "\n");
}

/*****************************************************************
gets the m-dimensional hash vector of a point in a hash table.

-para-
tableID		which hash table, from 0 to L-1
key			raw coordinates
g			(out) the returned hash vector
*****************************************************************/

void LSB::getHashVector(long long _tableID, long long *_key, double *_g)
{
	long long i;

	for (i = 0; i < m; i ++)
	{
		_g[i] = get1HashV(_tableID, i, _key);
	}
}

/*****************************************************************
cacluate one hash value.

-para-
u		which hash table, from 0 to L - 1
v		which hash function, from 0 to m - 1
key		the raw coordinates

-return-
the hash value
*****************************************************************/

double LSB::get1HashV(long long _u, long long _v, long long *_key)
{
	double	ret			= 0;

	double	* a_vector	= NULL;
	double	b;
	long long		i;

	getHashPara(_u, _v, &a_vector, &b);

	for (i = 0; i < d; i ++)
	{
		ret += a_vector[i] * _key[i];
	}

	ret += b;

	return ret;
}


/*****************************************************************
generates a_array and b_array. these are the parameters of the
hash functions needed in all the l lsb-trees

more specifically, each hash structure requires m hash functions.
and each has function requires a d-dimensional vector a and a 1d
value b. so a_array contains l * m * d values totally and b_array
contains l *m values.
*****************************************************************/

void LSB::gen_vectors()
{
	long long		i		= -1;
	double	max_b	= (double) pow(2.0, f) * w * w;

	a_array = new double[L * m * d];
	for (i = 0; i < L * m * d; i ++)
		a_array[i] = gaussian(0, 1);

	b_array = new double[L * m];
	for (i = 0; i < L * m; i ++)
		b_array[i] = new_uniform(0, max_b);
}

/*****************************************************************
get u

-return-
u
*****************************************************************/

long long LSB::get_u()
{
	long long		ret			= -1;

	long long		i			= -1;
	long long		j			= -1;
	double	absSum		= -1;
	double	* aVector	= NULL;
	double	b			= -1;
	double	maxV		= -1;
	double	thisV		= -1;

	maxV = (double) pow(2.0, f);

	for (i = 0; i < L; i ++)
	{
		for (j = 0; j < m; j ++)
		{
			getHashPara(i, j, &aVector, &b);

			absSum = 0;
			for (u = 0; u < d; u ++)
			{
				absSum += (double) fabs(aVector[u]);
			}

			thisV = 2 * (absSum * t + b) / w;
			if (maxV < thisV)
				maxV = thisV;
		}
	}

	ret = (long long) ceil(log((double) maxV) / log(2.0) - 1) + 1;

	return ret;
}

/*****************************************************************
returns the a-vector and b (offset) of the v-th hash function
in the u-th hash table.

-para-
u			see above
v			see above
a_vector	(out) starting address of the a-vector, which is an array of size d.
b			(out) address of the b value
*****************************************************************/

void LSB::getHashPara(long long _u, long long _v, double **_a_vector, double *_b)
{
	(*_a_vector) = &(a_array[_u * (m * d) + _v * d]);
	(*_b) = b_array[_u * m + _v];
}


/*****************************************************************
gets how many bytes an object occupies

para:
- dim

return:
- the # of bytes
*****************************************************************/

long long LSB::get_obj_size(long long _dim)
{
	long long ret = sizeof(long long) + sizeof(long long) + _dim * sizeof(long long);

	//for id, z-order value, and _dim coordinates
	return ret;
}

/*****************************************************************
transforms an m-dimensional hash vector into a z-value

-para-
g		the m-dimensional hash vector
z		(out) the z-value stored in an array of size m.

-return-
0	success
1	failure
*****************************************************************/

long long LSB::getZ(double *_g, long long *_z)
{
	long long ret = 0;

	long long		c			= -1;
	long long		cc			= -1;
	long long		numCell_Dim	= -1;
	long long		* g			= NULL;
	long long		i			= -1;
	long long		j			= -1;
	long long		mask		= -1;
	long long		v			= -1;

	numCell_Dim = 1 << u;

	g = new long long[m];

	for (i = 0; i < m; i ++)
	{
		g[i] = (long long) ((_g[i] + U/2) / w);

		if (g[i] < 0 || g[i] >= numCell_Dim)
		{
			printf("Illegal coordinate in the hash space found.\n");

			ret = 1;
			goto recycle;
		}
	}

	c = 0;
	cc = 0;
	v = 0;

	for (i = u - 1; i >= 0; i --)
	{
		mask = 1 << i;

		for (j = 0; j < m; j ++)
		{
			v <<= 1;

			if (g[j] & mask)
				v ++;

			c ++;

			if (c == 31)
			{
				_z[cc] = v;
				cc++;

				c = 0;
				v = 0;
			}
		}
	}

	if (c != 31)
	{
		v <<= (31 - c);
		_z[cc] = v;
		cc++;
	}

	if (cc != pz || c != (u * m - (u * m / 31 * 31)))
	{
		printf("Error in LSB::getZ().\n");

		goto recycle;
	}

recycle:
	if (g)
		delete [] g;

	return ret;
}

/*****************************************************************
insert a point into the forest.

-para-
treeID	into which tree are we inserting (from 0 to L-1)
son		the object's id
key		its coordinates

 -return-
0		success
1		failure
*****************************************************************/

long long LSB::insert(long long _treeID, long long _son, long long * _key)
{
	long long		ret		= 0;

	B_Entry	* e		= NULL;
	double	* g		= NULL;
	long long		* z		= NULL;

	g = new double[m];
	z = new long long[pz];

	getHashVector(_treeID, _key, g);
	if (getZ(g, z))
	{
		ret = 1;
		goto recycle;
	}

	e = trees[_treeID]->new_one_entry();

	((LSBentry *) e)->init(trees[_treeID], 0);
	e->son = e->leafson = _son;
	memcpy(((LSBentry *) e)->key, z, pz * sizeof(long long));
	memcpy(((LSBentry *) e)->pt, _key, d * sizeof(long long));

	trees[_treeID]->insert(e);

recycle:
	if (g)
		delete [] g;
	if (z)
		delete [] z;

	return ret;
}

/*****************************************************************
find the r such that the closest pair problem should be reduced
to the close pair problem at r.

-para-
_r			(out) the returned r at finish

-note-
must use a full forest.

-return-
normally the cost. -1 if something wrong.
*****************************************************************/

long long LSB::cpFind_r(long long * _r)
{
	long long			ret			= -1;

	B_Node		* oldnd		= NULL;
	B_Node		* nd		= NULL;
	bool		again		= false;
	__int64		* bktcnt	= 0;
	__int64		* cnt		= 0;
	__int64		limit		= -1;
	long long			block		= -1;
	long long			i			= -1;
	long long			ii			= -1;
	long long			j			= -1;
	long long			jj			= -1;
	intPtr		* stZ		= NULL;

	if (L < (long long) ceil( pow( (double) n*d/B,(double) 0.5) ))
	{
		printf("I must play with a full forest for that.\n");

		goto recycle;
	}

	limit = (__int64) 2 * B * n * L;

	if (limit < 0)
	{
		printf("Dataset too big for me to handle.\n");

		ret = -1;
		goto recycle;
	}

	bktcnt = new __int64[u];
	cnt = new __int64[u];

	for (i = 0; i < u; i ++)
		cnt[i] = bktcnt[i] = 0;

	stZ = new intPtr[u];
	for (i = 0; i < u; i ++)
		stZ[i] = new long long[pz];

	ret = 0;

	for (i = 0; i < L; i ++)
	{
		block = trees[i]->root;

		nd = new LSBnode();
		nd->init(trees[i], block);

		ret ++;

		while (!nd || nd->level > 0)
		{
			block = nd->entries[0]->son;

			delete nd;
			nd = new LSBnode();
			nd->init(trees[i], block);

			ret ++;
		}

		for (j = 0; j < u; j ++)
		{
			memcpy(stZ[j], nd->entries[0]->key, sizeof(long long) * pz);

			bktcnt[j] = 1;
		}

		j = 1;

		if (n > 1)
			again = true;

		while (again)
		{
			for (ii = 0; ii < u; ii ++)
			{

				if (jj <= ii)
					bktcnt[ii] ++;
				else
				{
					cnt[ii] += bktcnt[ii] * (bktcnt[ii] - 1) / 2;

					memcpy(stZ[ii], nd->entries[j]->key, sizeof(long long) * pz);

					bktcnt[ii] = 1;
				}
			}

			j ++;

			if (j == nd->num_entries)
			{
				oldnd = nd;

				nd = nd->get_right_sibling();

				delete oldnd;
				oldnd = NULL;

				if (nd)
				{
					j = 0;

					ret ++;
				}
				else
					again = false;

			}
		}

		for (j = 0; j < u; j ++)
		{
			cnt[j] += bktcnt[j] * (bktcnt[j] - 1) / 2;

			bktcnt[j] = 0;
		}

		if ((i + 1) % 5 == 0)
		{
			printf("\tFinished %d trees (%d%%).\n", i + 1, (i + 1) * 100 / L);
		}
	}

	for (i = 0; i < u; i ++)
		if (cnt[i] > limit)
			break;

	*_r = 2 << i;

recycle:
	if (nd)
		delete nd;

	if (bktcnt)
		delete [] bktcnt;

	if (cnt)
		delete [] cnt;

	if (stZ)
	{
		for (i = 0; i < u; i ++)
			delete [] stZ[i];

		delete [] stZ;
	}

	return ret;
}

/*****************************************************************
performs 2 approximate close pair search. Given a radius r, the
result is:
- if there is a pair of points within distance r, give a pair of
  points within distance at most 2r.
- if no pair of points within distance 2r, return nothing.

-para-
r			see above. must be a power of 2.
k			see below
dist		in this array the function returns the distances
			of the k closest pairs encountered. the smallest
			distance satisfies the requirement of 2 approximate
			close pair.
pairid		size of 2k. stores the ids of the k pairs reported
			in dist.

-note-
must use a full forest. this is to ensure theoretically high
success probability.

-return-
normally, the number of node accesses. -1 if something wrong.
*****************************************************************/

long long LSB::closepair(long long _r, long long _k, double *_dist, long long *_pairid)
{
	long long			ret			= -1;

	B_Node		* oldnd		= NULL;
	B_Node		* nd		= NULL;
	bool		done		= false;
	bool		again		= false;
	bool		alreadyin	= false;
	double		thisDist	= -1;
	__int64		cnt			= 0;
	__int64		limit		= -1;
	long long			block		= -1;
	long long			i			= -1;
	long long			ii			= -1;
	long long			iii			= -1;
	long long			id1			= -1;
	long long			id2			= -1;
	long long			j			= -1;
	long long			jj			= -1;
	long long			* pool		= NULL;
	long long			* pool2		= NULL;
	long long			poolSize	= -1;
	long long			poolUsed	= 0;
	long long			* stZ	= NULL;
	long long			zlevel		= -1;

	if (L < (long long) ceil( pow( (double) n*d/B, (double) 0.5) ))
	{
		printf("I must play with a full forest for that.\n");

		goto recycle;
	}

	zlevel = (long long) (log( (double) _r ) / log(2.0));

	for (i = 0; i < _k; i ++)
	{
		_dist[i] = (double) MAXREAL;

		_pairid[2 * i] = _pairid[2 * i + 1] = -1;
	}

	poolSize = 100;
	poolUsed = 0;

	pool = new long long[poolSize * (d+1)];

	stZ = new long long[pz];

	cnt = 0;
	limit = (__int64) 2 * B * n * L;

	if (limit < 0)
	{
		printf("Dataset too big for me to handle.\n");

		ret = -1;
		goto recycle;
	}

	ret = 0;

	for (i = 0; i < L; i ++)
	{
		poolUsed = 0;

		block = trees[i]->root;

		nd = new LSBnode();
		nd->init(trees[i], block);

		ret ++;

		while (nd->level > 0)
		{
			block = nd->entries[0]->son;

			delete nd;
			nd = new LSBnode();
			nd->init(trees[i], block);

			ret ++;
		}

		memcpy(stZ, nd->entries[0]->key, sizeof(long long) * pz);

		pool[0] = nd->entries[0]->son;
		memcpy(&pool[1], ((LSBentry *) nd->entries[0])->pt, sizeof(long long) * d);
		poolUsed++;

		j = 1;

		if (n > 1)
			again = true;

		while (again)
		{

			if (ii > zlevel)
			{
				memcpy(stZ, nd->entries[j]->key, sizeof(double) * pz);

				poolUsed = 0;

				pool[0] = nd->entries[j]->son;
				memcpy(&pool[1], ((LSBentry *) nd->entries[j])->pt, sizeof(long long) * d);

				poolUsed ++;
			}
			else
			{
				for (ii = 0; ii < poolUsed; ii ++)
				{
					id1 = min(nd->entries[j]->son, pool[ii * (d + 1)]);
					id2 = max(nd->entries[j]->son, pool[ii * (d + 1)]);

					alreadyin = false;

					for (jj = 0; jj < _k; jj++)
						if (_pairid[2 * jj] == id1 && _pairid[2 * jj + 1] == id2)
						{
							alreadyin = true;

							break;
						}

					if (!alreadyin)
					{
						thisDist = l2_dist_int(((LSBentry *) nd->entries[j])->pt, &(pool[ii * (d + 1) + 1]), d);

						for (jj = 0; jj < _k; jj++)
						{
							if (compfloats(thisDist, _dist[jj]) == -1)
								break;
						}

						if (jj < _k)
						{
							for (iii = _k - 1; iii >= jj + 1; iii --)
							{
								_dist[iii] = _dist[iii - 1];

								_pairid[2 * iii] = _pairid[2 * (iii - 1)];
								_pairid[2 * iii + 1] = _pairid[2 * (iii - 1) + 1];
							}

							_dist[jj] = thisDist;

							_pairid[2 * jj] = id1;
							_pairid[2 * jj + 1] = id2;
						}
					}

					cnt ++;

					if (cnt == limit)
					{
						again = false;
						done = true;

						break;
					}
				}

				if (!again)
					break;

				pool[poolUsed * (d + 1)] = nd->entries[j]->son;
				memcpy(&pool[poolUsed * (d + 1) + 1], ((LSBentry *) nd->entries[j])->pt, sizeof(long long) * d);

				poolUsed ++;

				if (poolUsed == poolSize)
				{
					pool2 = pool;

					pool = new long long [2 * poolSize * (d + 1)];

					memcpy(pool, pool2, sizeof(long long) * poolSize * (d + 1));

					delete [] pool2;
					pool2 = NULL;

					poolSize *= 2;
				}
			}

			if (!again)
				break;

			j ++;

			if (j == nd->num_entries)
			{
				oldnd = nd;

				nd = nd->get_right_sibling();

				delete oldnd;
				oldnd = NULL;

				if (nd)
				{
					j = 0;

					ret ++;
				}
				else
					again = false;

			}
		}

		{
			printf("\tFinished %d trees (%d%%).\n", i + 1, (i + 1) * 100 / L);
		}

		if (done)
			break;
	}

recycle:
	if (nd)
		delete nd;

	if (pool)
		delete [] pool;

	if (stZ)
		delete [] stZ;

	return ret;
}

/*****************************************************************
approximate close pair search using any number of lsb-trees. no
quality guarantee but faster

-para-
k			see below
numTrees	how many trees you would like to use
dist		in this array the function returns the distances
			of the k closest pairs encountered.
pairid		size of 2k. stores the ids of the k pairs reported
			in dist.

-return-
normally, the number of node accesses. -1 if something wrong.
*****************************************************************/

long long LSB::cpFast(long long _k, long long _numTrees, double *_dist, long long *_pairid)
{
	long long			ret			= -1;

	B_Node		* oldnd		= NULL;
	B_Node		* nd1		= NULL;
	B_Node		* nd2		= NULL;
	bool		again		= false;
	bool		alreadyin	= false;
	double		thisDist	= -1;
	long long			block		= -1;
	long long			i			= -1;
	long long			ii			= -1;
	long long			iii			= -1;
	long long			id1			= -1;
	long long			id2			= -1;
	long long			j			= -1;
	long long			jj			= -1;
	long long			numTrees	= -1;
	long long			* stZ	= NULL;

	for (i = 0; i < _k; i ++)
	{
		_dist[i] = (double) MAXREAL;

		_pairid[2 * i] = _pairid[2 * i + 1] = -1;
	}

	numTrees = min(L, _numTrees);

	stZ = new long long[pz];

	ret = 0;

	for (i = 0; i < numTrees; i ++)
	{
		block = trees[i]->root;

		nd1 = new LSBnode();
		nd1->init(trees[i], block);

		ret ++;

		while (nd1->level > 0)
		{
			block = nd1->entries[0]->son;

			delete nd1;
			nd1 = new LSBnode();
			nd1->init(trees[i], block);

			ret ++;
		}

		while(true)
		{
			for (j = 0; j < nd1->num_entries; j ++)
			{
				for (ii = j + 1; ii < nd1->num_entries; ii++)
				{
					alreadyin = false;

					if (numTrees > 1)
					{
						id1 = min(nd1->entries[j]->son, nd1->entries[ii]->son);
						id2 = max(nd1->entries[j]->son, nd1->entries[ii]->son);

						for (jj = 0; jj < _k; jj ++)
						{
							if (_pairid[2 * jj] == id1 && _pairid[2 * jj + 1] == id2)
							{
								alreadyin = true;
								break;
							}
						}
					}

					if (!alreadyin)
					{
						thisDist = l2_dist_int(((LSBentry *) nd1->entries[j])->pt, ((LSBentry *) nd1->entries[ii])->pt, d);

						for (jj = 0; jj < _k; jj ++)
						{
							if (compfloats(thisDist, _dist[jj]) == -1)
								break;
						}

						if (jj < _k)
						{
							for (iii = _k - 1; iii >= jj + 1; iii --)
							{
								_dist[iii] = _dist[iii - 1];

								_pairid[2 * iii] = _pairid[2 * (iii - 1)];
								_pairid[2 * iii + 1] = _pairid[2 * (iii - 1) + 1];
							}

							_dist[jj] = thisDist;

							_pairid[2 * jj] = id1;
							_pairid[2 * jj + 1] = id2;
						}
					}
				}
			}

			memcpy(stZ, nd1->entries[j - 1]->key, sizeof(long long) * pz);

			nd2 = nd1;

			again = true;
			while (again)
			{
				oldnd = nd2;

				nd2 = nd2->get_right_sibling();

				if (nd1 != oldnd)
				{
					delete oldnd;
					oldnd = NULL;
				}

				if (!nd2)
					break;
				else
					ret ++;

				for (j = 0; j < nd2->num_entries; j ++)
				{

					if (sqrt(_dist[_k - 1]) < (1 << (ii + 1)))
					{
						again = false;

						break;
					}

					for (ii = 0; ii < nd1->num_entries; ii ++)
					{
						alreadyin = false;

						if (numTrees > 1)
						{
							id1 = min(nd1->entries[ii]->son, nd2->entries[j]->son);
							id2 = max(nd1->entries[ii]->son, nd2->entries[j]->son);

							for (jj = 0; jj < _k; jj ++)
							{
								if (_pairid[2 * jj] == id1 && _pairid[2 * jj + 1] == id2)
								{
									alreadyin = true;
									break;
								}
							}
						}

						if (!alreadyin)
						{
							thisDist = l2_dist_int(((LSBentry *) nd1->entries[ii])->pt, ((LSBentry *) nd2->entries[j])->pt, d);

 							for (jj = 0; jj < _k; jj ++)
							{
								if (compfloats(thisDist, _dist[jj]) == -1)
									break;
							}

							if (jj < _k)
							{
								for (iii = _k - 1; iii >= jj + 1; iii --)
								{
									_dist[iii] = _dist[iii - 1];

									_pairid[2 * iii] = _pairid[2 * (iii - 1)];
									_pairid[2 * iii + 1] = _pairid[2 * (iii - 1) + 1];
								}

								_dist[jj] = thisDist;

								_pairid[2 * jj] = id1;
								_pairid[2 * jj + 1] = id2;
							}
						}
					}
				}
			}

			oldnd =  nd1;

			nd1 = nd1->get_right_sibling();

			delete oldnd;
			oldnd = NULL;

			if (!nd1)
				break;
			else
				ret ++;
		}
	}

	if (nd1)
		delete nd1;

	if (oldnd)
		delete oldnd;

	if (nd2)
		delete nd2;

	if (stZ)
		delete [] stZ;

	return ret;
}

/*****************************************************************
knn query

-para-
q			query
k			the number of neighbors returned
rslt		(out) an array storing the k nns, sorted in ascending
			order of their distances to the query
numTrees	number of trees used (1 to L). set it to 0 has the
			same effect as L

-return-
normally, the number of node accesses. -1 if something wrong.
*****************************************************************/

long long LSB::knn(long long *_q, long long _k, LSB_Hentry *_rslt, long long _numTrees)
{
	long long				ret				= 0;

	BinHeap			* hp			= NULL;
	BinHeapEntry	* bhe			= NULL;
	B_Node			* nd			= NULL;
	B_Node			* oldnd			= NULL;
	bool			again			= false;
	bool			E1				= true;
	bool			lescape			= false;
	double			knnDist			= -1;
	double			old_knnDist		= -1;
	double			* qg			= NULL;
	long long				block			= -1;
	long long				curLevel		= -1;
	long long				follow			= -1;
	long long				i				= -1;
	long long				limit			= -1;
	long long				numTrees		= -1;
	long long				pos				= -1;
	long long				readCnt			= 0;
	long long				rsltCnt			= 0;
	long long				thisLevel		= -1;
	long long				thisTreeId		= -1;
	long long				** qz			= NULL;
	LSBbiPtr		* lptrs			= NULL;
	LSBbiPtr		* rptrs			= NULL;
	LSBentry		* e				= NULL;
	LSBentry		* qe			= NULL;
	LSB_Hentry		* he			= NULL;

	if (_numTrees < 0)
	{
		printf("-l must be followed by a nonnegative integer.\n", _numTrees);

		ret = -1;
		goto recycle;
	}

	if (_numTrees == 0)
		numTrees = L;
	else
		numTrees = min(L, _numTrees);

	if (_k > 1 || numTrees < (long long) ceil( pow( (double) n*d/B, (double) 1/ratio) ))
		E1 = false;

	lptrs = new LSBbiPtr[numTrees];
	rptrs = new LSBbiPtr[numTrees];

	for (i = 0; i < numTrees; i ++)
	{
		lptrs[i].nd = rptrs[i].nd = NULL;
		lptrs[i].entryId = rptrs[i].entryId = -1;
	}

	for (i = 0; i < _k; i ++)
	{
		_rslt[i].id		= -1;
		_rslt[i].dist	= (double) MAXREAL;
	}

	hp = new BinHeap();
	hp->compare_func = &LSB_hcomp;
	hp->destroy_data = &LSB_hdestroy;

	qg = new double[m];

	qz = new intPtr[numTrees];
	for (i = 0; i < numTrees; i ++)
		qz[i] = new long long[pz];

	qe = new LSBentry();
	qe->init(trees[0], 0);
	memcpy(qe->pt, _q, sizeof(long long) * d);

	for (i = 0; i < numTrees; i ++)
	{
		getHashVector(i, _q, qg);
		if (getZ(qg, qz[i]))
		{
			printf("getZ error in LSB::knn.\n");

			ret = 1;
			goto recycle;
		}

		memcpy(qe->key, qz[i], sizeof(long long) * pz);

		block = trees[i]->root;

		nd = new LSBnode();
		nd->init(trees[i], block);

		ret++;

		lescape = false;

		while (nd->level > 0)
		{
			follow = nd->ptqry_next_follow(qe);

			if (follow == -1)
			{
				if (lescape)
					follow = 0;
				else
				{
					if (block != trees[i]->root)
					{
						printf("LSB::knn.\n No branch found\n");

						ret = -1;
						goto recycle;
					}
					else
					{
						follow = 0;
						lescape = true;
					}
				}
			}

			block = nd->entries[follow]->son;

			delete nd;

			nd = new LSBnode();
			nd->init(trees[i], block);

			ret++;
		}

		if (lescape)
		{
			rptrs[i].nd	= nd;
			rptrs[i].entryId = 0;
		}
		else
		{
			follow = nd->ptqry_next_follow(qe);

			lptrs[i].nd = nd;
			lptrs[i].entryId = follow;

			if (follow < nd->num_entries - 1)
			{
				rptrs[i].nd = nd;
				rptrs[i].entryId = follow + 1;
			}
			else
			{
				rptrs[i].nd = nd->get_right_sibling();

				if (rptrs[i].nd)
				{
					rptrs[i].entryId = 0;
					ret++;
				}

			}
		}

		if (lptrs[i].nd)
		{
			e			= (LSBentry *) lptrs[i].nd->entries[lptrs[i].entryId];

			he			= new LSB_Hentry;

			he->d		= d;
			he->pt		= new long long[d];
			he->treeId	= i;
			he->lr		= 0;
			setHe(he, e, qz[i]);
			he->dist	= l2_dist_int(he->pt, _q, d);

			bhe			= new BinHeapEntry();
			bhe->data	= he;

			hp->insert(bhe);
		}

		if (rptrs[i].nd)
		{
			e			= (LSBentry *) rptrs[i].nd->entries[rptrs[i].entryId];

			he  		= new LSB_Hentry;

			he->d		= d;
			he->pt		= new long long[d];
			he->treeId	= i;
			he->lr		= 1;
			setHe(he, e, qz[i]);
			he->dist	= l2_dist_int(he->pt, _q, d);

			bhe			= new BinHeapEntry();
			bhe->data	= he;

			hp->insert(bhe);
		}
	}

	qe->close();
	delete qe;
	qe = NULL;

	knnDist = (double) MAXREAL;

	readCnt = 0;
	rsltCnt = 0;

	if (!E1)
		limit = n * L;
	else
		limit = (long long) ceil ((double) 2 * B * L / d);

	again = true;
	while (again)
	{
		bhe = hp->remove();

		if (!bhe)
			again = false;
		else
		{
			he = (LSB_Hentry *) bhe->data;

			thisLevel = he->lcl;
			thisTreeId = he->treeId;

			readCnt ++;

			old_knnDist = knnDist;

			knnDist = (double) sqrt(updateknn(_rslt, he, _k));

			if (knnDist <= pow((double) ratio, thisLevel + 1))
			{
				again = false;
			}

			if (again && readCnt < limit)
			{
				if (he->lr == 0)
				{
					nd = lptrs[thisTreeId].nd;

					pos = lptrs[thisTreeId].entryId;

					if (pos > 0)
					{
						lptrs[thisTreeId].entryId--;

						setHe(he, nd->entries[pos - 1], qz[thisTreeId]);
						he->dist = l2_dist_int(he->pt, _q, d);

						hp->insert(bhe);
					}
					else
					{
						oldnd = nd;

						nd = nd->get_left_sibling();

						ret++;

						if (nd)
						{
							lptrs[thisTreeId].nd = nd;
							lptrs[thisTreeId].entryId = nd->num_entries - 1;

							setHe(he, nd->entries[nd->num_entries - 1], qz[thisTreeId]);
							he->dist = l2_dist_int(he->pt, _q, d);

							hp->insert(bhe);
						}
						else
						{
							lptrs[thisTreeId].nd = NULL;
							lptrs[thisTreeId].entryId = -1;

							delete [] he->pt;
							he->pt = NULL;
							delete he;

							bhe->data = NULL;
							delete bhe;
						}

						if (rptrs[thisTreeId].nd != (LSBnode *) oldnd)
							delete oldnd;
					}
				}
				else
				{
					nd = rptrs[thisTreeId].nd;

					pos = rptrs[thisTreeId].entryId;

					if (pos < nd->num_entries - 1)
					{
						rptrs[thisTreeId].entryId++;

						setHe(he, nd->entries[pos + 1], qz[thisTreeId]);
						he->dist = l2_dist_int(he->pt, _q, d);

						hp->insert(bhe);
					}
					else
					{
						oldnd = nd;

						nd = nd->get_right_sibling();

						ret++;

						if (nd)
						{
							rptrs[thisTreeId].nd = nd;
							rptrs[thisTreeId].entryId = 0;

							setHe(he, nd->entries[0], qz[thisTreeId]);
							he->dist = l2_dist_int(he->pt, _q, d);

							hp->insert(bhe);
						}
						else
						{
							rptrs[thisTreeId].nd = NULL;
							rptrs[thisTreeId].entryId = -1;

							delete [] he->pt;
							he->pt = NULL;
							delete he;

							bhe->data = NULL;
							delete bhe;
						}

						if (lptrs[thisTreeId].nd != (LSBnode *) oldnd)
							delete oldnd;
					}
				}
			}
			else
			{
				again = false;

				delete [] he->pt;
				he->pt = NULL;
				delete he;

				bhe->data = NULL;
				delete bhe;
			}
		}
	}

recycle:

	if (qz)
	{
		for (i = 0; i < numTrees; i ++)
			delete [] qz[i];

		delete [] qz;
	}

	if (qe)
	{
		qe->close();
		delete qe;
	}

	if (qg)
		delete [] qg;

	if (hp->root)
		hp->root->recursive_data_wipeout(hp->destroy_data);
	delete hp;

	for (i = 0; i < numTrees; i ++)
	{
		if (lptrs[i].nd)
		{
			if (rptrs[i].nd == lptrs[i].nd)
				rptrs[i].nd = NULL;

			delete lptrs[i].nd;
			lptrs[i].nd = NULL;
		}

		if (rptrs[i].nd)
		{
			delete rptrs[i].nd;
			rptrs[i].nd = NULL;
		}
	}

	if (lptrs)
		delete [] lptrs;

	if (rptrs)
		delete [] rptrs;

	return ret;
}

/*----------------------------------------------------------------
auxiliary function called by LSB:knn.
----------------------------------------------------------------*/

void LSB::setHe(LSB_Hentry *_he, B_Entry *_e, long long *_qz)
{
	memcpy(_he->pt, ((LSBentry *)_e)->pt, sizeof(long long) *_he->d);
	_he->id = _e->son;
}

/*----------------------------------------------------------------
auxiliary function called by LSB:knn.
----------------------------------------------------------------*/

double LSB::updateknn(LSB_Hentry * _rslt, LSB_Hentry *_he, long long _k)
{
	double	ret			= -1;

	long long		i			= -1;
	long long		pos			= -1;
	bool	alreadyIn	= false;

	for (i = 0; i < _k; i ++)
	{
		if (_he->id == _rslt[i].id)
		{
			alreadyIn = true;
			break;
		}
		else if (compfloats(_he->dist, _rslt[i].dist) == -1)
		{
			break;
		}
	}

	pos = i;

	if (!alreadyIn && pos < _k)
	{
		for (i = _k - 1; i > pos; i--)
			_rslt[i].setto(&(_rslt[i - 1]));

		_rslt[pos].setto(_he);
	}

	ret = _rslt[_k - 1].dist;

	return ret;
}

/*****************************************************************
write the para file to the disk

-para-
fname		path of the file

-return-
0			success
1			failure
*****************************************************************/

long long LSB::writeParaFile(char *_fname)
{
	long long		ret			= 0;

	long long		i			= -1;
	long long		j			= -1;
	long long		u			= -1;
	FILE	* fp		= NULL;
	double	* aVector	= NULL;
	double	b			= -1;

	fp = fopen(_fname, "r");

	if (fp)
	{
		printf("Forest exists. ");

		ret = 1;

		goto recycle;
	}

	fp = fopen(_fname, "w");
	if (!fp)
	{
		printf("I could not create %s.\n", _fname);
		printf("Perhaps no such folder %s?\n", forestPath);

		ret = 1;

		goto recycle;
	}

	fprintf(fp, "%s\n", dsPath);
	fprintf(fp, "B = %d\n", B);
	fprintf(fp, "n = %d\n", n);
	fprintf(fp, "d = %d\n", d);
	fprintf(fp, "t = %d\n", t);
	fprintf(fp, "ratio = %d\n", ratio);
	fprintf(fp, "l = %d\n", L);

	for (i = 0; i < L; i ++)
	{
		for (j = 0; j < m; j ++)
		{
			getHashPara(i, j, &aVector, &b);

			fprintf(fp, "%f", aVector[0]);
			for (u = 1; u < d; u ++)
			{
				fprintf(fp, " %f", aVector[u]);
			}
			fprintf(fp, "\n");


			fprintf(fp, "%f\n", b);

		}
	}

recycle:
	if (fp)
		fclose(fp);

	return ret;
}

/*----------------------------------------------------------------
auxiliary function called by LSB:knn.
----------------------------------------------------------------*/

long long LSB_hcomp(const void *_e1, const void *_e2)
{
	LSB_Hentry * e1 = (LSB_Hentry *) _e1;
	LSB_Hentry * e2 = (LSB_Hentry *) _e2;

	long long ret = 0;

	if (e1->lcl < e2->lcl)
		ret = -1;
	else if (e1->lcl > e2->lcl)
		ret = 1;

	return ret;
}

/*----------------------------------------------------------------
auxiliary function called by LSB:knn.
----------------------------------------------------------------*/

void LSB_hdestroy(const void *_e)
{
	LSB_Hentry * e = (LSB_Hentry *) _e;

	delete [] e->pt;
	delete e;
	e = NULL;
}

/*----------------------------------------------------------------
comparison function for qsort called by LSB::bulkload()
----------------------------------------------------------------*/

int LSBqsortComp(const void *_e1, const void *_e2)
{
	bool ret				= 0;

	long long	i				= -1;
	LSBqsortElem * e1	= (LSBqsortElem *) _e1;
	LSBqsortElem * e2	= (LSBqsortElem *) _e2;

	for (i = 0; i < e1->pz; i ++)
	{
		if (e1->z[i] < e2->z[i])
		{
			ret = -1;
			break;
		}
		else if (e1->z[i] > e2->z[i])
		{
			ret = 1;
			break;
		}
	}

	return ret;
}

/*----------------------------------------------------------------
auxliary func used in LSB::knn

para:
- rslt: the array of current knn pts
- he: new pt
- rslt_cnt: how many pts currently in the array
----------------------------------------------------------------*/

void LSBdestroyHPentryData(const void *_e)
{
	LSB_Hentry * e = (LSB_Hentry *) _e;

	delete [] e->pt;
	delete e;
}
