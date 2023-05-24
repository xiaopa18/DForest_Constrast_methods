#ifndef __GENERAL_DEFINITION
#define __GENERAL_DEFINITION
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <algorithm>
using namespace std;
//----BlockFile, CachedBlockFile, Cache---------------------
#define BFHEAD_LENGTH (sizeof(int)*2)    //file header size

#define TRUE 1
#define FALSE 0

#define SEEK_CUR 1
#define SEEK_SET 0
#define SEEK_END 2

typedef char Block[];
//-------------------All-------------------------------------
#define MAXREAL         1e20
#define FLOATZERO       1e-20
#define MAX_DIMENSION   256

int DIMENSION;

#define TRUE 1
#define FALSE 0
//-------------------Class and other data types--------------
class BlockFile;  //for BlockFile definition
class Cache;
class Cacheable   //inherit this class if you wish to design an external
                  //memory structure that can be cached
{
public:
	BlockFile *file;
	Cache *cache;
};
  //==========================================================
class CmdIntrpr  //this is the class of command interpretor.  for a new rtree decescendent
                  //inherit this class to obtain tailored command interpretor
{
public:
	int cnfrm_cmd(char *_msg)
	{ char c = ' ';
	  while (c != 'N' && c != 'Y')
	  { printf("%s (y/n)?", _msg);
	    c = getchar();
		char tmp;
		while ((tmp = getchar()) != '\n');
		c = toupper(c); }
	  if (c == 'N') return 0; else return 1; }

	void get_cmd(char *_msg, char *_cmd)
	{ printf("%s", _msg);
	  char *c = _cmd;
	  while (((*c) = getchar()) != '\n')
	    c++;
	  *c = '\0'; }

	virtual bool build_tree(char *_tree_fname, char *_data_fname, int _b_len, int _dim, int _csize) = 0;
	virtual void free_tree() = 0;
	virtual int qry_sngle(float *_mbr, int *_io_count) = 0;
	virtual void run() = 0;
	virtual void version() = 0;
};
  //==========================================================
enum SECTION {OVERLAP, INSIDE, S_NONE};
enum R_OVERFLOW {SPLIT, REINSERT, NONE};
enum R_DELETE {NOTFOUND,NORMAL,ERASED};
typedef float *floatptr;
  //==========================================================
struct SortMbr
{
    int dimension;
    float *mbr;
    float *center;
    int index;
};

struct BranchList
{
    int entry_number;
    float mindist;
    float minmaxdist;
    bool section;
};

//-----Global Functions--------------------------------------
void error(char *_errmsg, bool _terminate);

float area(int dimension, float *mbr);
float margin(int dimension, float *mbr);
float overlap(int dimension, float *r1, float *r2);
float* overlapRect(int dimension, float *r1, float *r2);
float objectDIST(float *p1, float *p2);
float MINMAXDIST(float *Point, float *bounces);
float MINDIST(float *Point, float *bounces);
float MAXDIST(float *p, float *bounces, int dim);
float MbrMINDIST(float *_m1, float *_m2, int _dim);
float MbrMAXDIST(float *_m1, float *_m2, int _dim);

bool inside(float &p, float &lb, float &ub);
void enlarge(int dimension, float **mbr, float *r1, float *r2);
bool is_inside(int dimension, float *p, float *mbr);
int pruneBrunchList(float *nearest_distanz, const void *activebrunchList,
		    int n);
bool section(int dimension, float *mbr1, float *mbr2);
bool section_c(int dimension, float *mbr1, float *center, float radius);

int sort_lower_mbr(const void *d1, const void *d2);
int sort_upper_mbr(const void *d1, const void *d2);
int sort_center_mbr(const void *d1, const void *d2);
int sortmindist(const void *element1, const void *element2);

#ifdef UNIX
void strupr(char *_msg);
#endif
//-----------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
// globals
//////////////////////////////////////////////////////////////////////////////

#ifdef UNIX
void strupr(char *_msg)
{
	int dist = 'A' - 'a';
	char *c_ptr = _msg;

	while (*c_ptr)
	{
		if (*c_ptr >= 'a')
			*c_ptr += dist;
		c_ptr ++;
	}
}
#endif

void error(char *t, bool ex)
{
    fprintf(stderr, t);
    if (ex)
        exit(0);
}

float area(int dimension, float *mbr)
// berechnet Flaeche (Volumen) eines mbr der Dimension dimension
{
    int i;
    float sum;

    sum = 1.0;
    for (i = 0; i < dimension; i++)
	sum *= mbr[2*i+1] - mbr[2*i];

    return sum;
}

float margin(int dimension, float *mbr)
// berechnet Umfang eines mbr der Dimension dimension
{
    float *ml, *mu, *m_last, sum;

    sum = 0.0;
    m_last = mbr + 2*dimension;
    ml = mbr;
    mu = ml + 1;
    while (mu < m_last)
    {
	sum += *mu - *ml;
	ml += 2;
	mu += 2;
    }

    return sum;
}

bool inside(float &p, float &lb, float &ub)
// ist ein Skalar in einem Intervall ?
{
    return (p >= lb && p <= ub);
}

bool inside(float *v, float *mbr, int dimension)
// ist ein Vektor in einer Box ?
{
    int i;

    for (i = 0; i < dimension; i++)
	if (!inside(v[i], mbr[2*i], mbr[2*i+1]))
	    return FALSE;

    return TRUE;
}

// calcutales the overlapping rectangle between r1 and r2
// if rects do not overlap returns null
float* overlapRect(int dimension, float *r1, float *r2)
{
        float *overlap = new float[2*dimension];
        for (int i=0; i<dimension; i++)
        {
            if ((r1[i*2]>r2[i*2+1]) || (r1[i*2+1]<r2[i*2])) // non overlapping
	    {
                delete [] overlap;
		return NULL;
	    }
	    overlap[2*i] = max(r1[i*2], r2[i*2]);
            overlap[2*i+1] = min(r1[i*2+1], r2[i*2+1]);
        }

        return overlap;
}

float overlap(int dimension, float *r1, float *r2)
// calcutales the overlapping area of r1 and r2
// calculate overlap in every dimension and multiplicate the values
{
    float sum;
    float *r1pos, *r2pos, *r1last, r1_lb, r1_ub, r2_lb, r2_ub;

    sum = 1.0;
    r1pos = r1; r2pos = r2;
    r1last = r1 + 2 * dimension;

    while (r1pos < r1last)
    {
	r1_lb = *(r1pos++);
	r1_ub = *(r1pos++);
	r2_lb = *(r2pos++);
	r2_ub = *(r2pos++);

        // calculate overlap in this dimension

        if (inside(r1_ub, r2_lb, r2_ub))
        // upper bound of r1 is inside r2
	{
            if (inside(r1_lb, r2_lb, r2_ub))
            // and lower bound of r1 is inside
                sum *= (r1_ub - r1_lb);
            else
                sum *= (r1_ub - r2_lb);
	}
	else
	{
            if (inside(r1_lb, r2_lb, r2_ub))
	    // and lower bound of r1 is inside
		sum *= (r2_ub - r1_lb);
	    else
	    {
		if (inside(r2_lb, r1_lb, r1_ub) &&
		    inside(r2_ub, r1_lb, r1_ub))
	        // r1 contains r2
		    sum *= (r2_ub - r2_lb);
		else
		// r1 and r2 do not overlap
		    sum = 0.0;
	    }
	}
    }

    return sum;
}

void enlarge(int dimension, float **mbr, float *r1, float *r2)
// enlarge r in a way that it contains s
{
    int i;

    *mbr = new float[2*dimension];
    for (i = 0; i < 2*dimension; i += 2)
    {
	(*mbr)[i]   = min(r1[i],   r2[i]);

	(*mbr)[i+1] = max(r1[i+1], r2[i+1]);
    }
}

bool section(int dimension, float *mbr1, float *mbr2)
{
    int i;

    for (i = 0; i < dimension; i++)
    {
	if (mbr1[2*i]     > mbr2[2*i + 1] ||
	    mbr1[2*i + 1] < mbr2[2*i])
	    return FALSE;
    }
    return TRUE;
}

bool section_c(int dimension, float *mbr1, float *center, float radius)
{
	float r2;

	r2 = radius * radius;

	if ( (r2 - MINDIST(center,mbr1)) < 1.0e-8)
		return TRUE;
	else
		return FALSE;

}

int sort_lower_mbr(const void *d1, const void *d2)
// Vergleichsfunktion fuer qsort, sortiert nach unterem Wert der mbr bzgl.
// der angegebenen Dimension
{
    SortMbr *s1, *s2;
    float erg;
    int dimension;

    s1 = (SortMbr *) d1;
    s2 = (SortMbr *) d2;
    dimension = s1->dimension;
    erg = s1->mbr[2*dimension] - s2->mbr[2*dimension];
    if (erg < 0.0)
	return -1;
    else if (erg == 0.0)
	return 0;
    else
	return 1;
}

int sort_upper_mbr(const void *d1, const void *d2)
// Vergleichsfunktion fuer qsort, sortiert nach oberem Wert der mbr bzgl.
// der angegebenen Dimension
{
    SortMbr *s1, *s2;
    float erg;
    int dimension;

    s1 = (SortMbr *) d1;
    s2 = (SortMbr *) d2;
    dimension = s1->dimension;
    erg = s1->mbr[2*dimension+1] - s2->mbr[2*dimension+1];
    if (erg < 0.0)
	return -1;
    else if (erg == 0.0)
	return 0;
    else
	return 1;
}

int sort_center_mbr(const void *d1, const void *d2)
// Vergleichsfunktion fuer qsort, sortiert nach center of mbr
{
    SortMbr *s1, *s2;
    int i, dimension;
    float d, e1, e2;

    s1 = (SortMbr *) d1;
    s2 = (SortMbr *) d2;
    dimension = s1->dimension;

    e1 = e2 = 0.0;
    for (i = 0; i < dimension; i++)
    {
        d = ((s1->mbr[2*i] + s1->mbr[2*i+1]) / 2.0) - s1->center[i];
        e1 += d*d;
        d = ((s2->mbr[2*i] + s2->mbr[2*i+1]) / 2.0) - s2->center[i];
        e2 += d*d;
    }

    if (e1 < e2)
	return -1;
    else if (e1 == e2)
	return 0;
    else
	return 1;
}

int sortmindist(const void *element1, const void *element2)
{
    //
    // Vergleichsfunktion fuer die Sortierung der BranchList bei der
    // NearestNarborQuery (Sort, Branch and Prune)
    //

	//The consequence of this sorting is that active branches are sorted
	//in acsending order of their mindist.

    BranchList *e1,*e2;

    e1 = (BranchList *) element1;
    e2 = (BranchList *) element2;

    if (e1->mindist < e2->mindist)
	return(-1);
    else if (e1->mindist > e2->mindist)
	return(1);
    else
	return(0);

}

int pruneBrunchList(float *nearest_distanz, const void *activebrunchList,
		    int n)
{

    // Schneidet im Array BranchList alle Eintraege ab, deren Distanz groesser
    // ist als die aktuell naeheste Distanz
    //

    BranchList *bl;

    int i,j,k, aktlast;

    bl = (BranchList *) activebrunchList;

    // 1. Strategie:
    //
    // Ist MINDIST(P,M1) > MINMAXDIST(P,M2), so kann der
    // NearestNeighbor auf keinen Fall mehr in M1 liegen!
    //


    aktlast = n;

    for( i = 0; (i < aktlast) && (*nearest_distanz >= bl[i].mindist) ; i++);

    aktlast = i;

    return (aktlast);

}

float objectDIST(float *p1, float *p2)
{

    //
    // Berechnet das Quadrat der euklid'schen Metrik.
    // (Der tatsaechliche Abstand ist uninteressant, weil
    // die anderen Metriken (MINDIST und MINMAXDIST fuer
    // die NearestNarborQuery nur relativ nie absolut
    // gebraucht werden; nur Aussagen "<" oder ">" sind
    // relevant.
    //

    float summe = 0;
    int i;

    for( i = 0; i < DIMENSION; i++)
	summe += pow(p1[i] - p2[i], 2 );

    //return( sqrt(summe) );
    return(summe);
}

/*****************************************************************
this function returns the maxdist of 2 mbrs
para:
m1: the bounces of the 1st mbr
m2: the bounces of the 2nd mbr
dim: dimensionality
*****************************************************************/

float MbrMAXDIST(float *_m1, float *_m2, int _dim)
{
	float dist=0;
	for (int i=0; i<_dim; i++)
	{
		float d1=fabs(_m1[2*i]-_m2[2*i+1]);
		float d2=fabs(_m1[2*i+1]-_m2[2*i]);
		float d=max(d1, d2);
		dist+=pow(d, 2);
	}
	return dist;
}

/*****************************************************************
this function returns the mindist of 2 mbrs
para:
m1: the bounces of the 1st mbr
m2: the bounces of the 2nd mbr
dim: dimensionality
*****************************************************************/

float MbrMINDIST(float *_m1, float *_m2, int _dim)
{
	float dist=0;
	for (int i=0; i<_dim; i++)
	{
		if (_m1[2*i]>_m2[2*i+1])
			dist+=pow(_m1[2*i]-_m2[2*i+1], 2);
		else if (_m1[2*i+1]<_m2[2*i])
			dist+=pow(_m1[2*i+1]-_m2[2*i], 2);
	}
	return dist;
}

float MINDIST(float *p, float *bounces)
{
    //
    // Berechne die kuerzeste Entfernung zwischen einem Punkt Point
    // und einem MBR bounces (Lotrecht!)
    //

    float summe = 0.0;
    float r;
    int i;

    for(i = 0; i < DIMENSION; i++)
    {
	if (p[i] < bounces[2*i])
	    r = bounces[2*i];
	else
	{
	    if (p[i] > bounces[2*i +1])
		r = bounces[2*i+1];
	    else
		r = p[i];
	}

	summe += pow( p[i] - r , 2);
    }
    return(summe);
}

float MAXDIST(float *p, float *bounces, int dim)
{
    float summe = 0.0;
    float r;
    int i;

    for(i = 0; i < dim; i++)
    {
		if (p[i] < bounces[2*i])
			r = bounces[2*i+1];
		else
		{
			if (p[i] > bounces[2*i+1])
			r = bounces[2*i];
			else if (p[i] - bounces[2*i] > bounces[2*i+1] -p[i])
					 r = bounces[2*i];
			     else
					 r = bounces[2*i+1];
		}

		summe += pow(p[i]-r,2);
    }

	return(summe);
}


float MINMAXDIST(float *p, float *bounces)
{

    // Berechne den kleinsten maximalen Abstand von einem Punkt Point
    // zu einem MBR bounces.
    // Wird benutzt zur Abschaetzung von Abstaenden bei NearestNarborQuery.
    // Kann als Supremum fuer die aktuell kuerzeste Distanz:
    // Alle Punkte mit einem Abstand > MINMAXDIST sind keine Kandidaten mehr
    // fuer den NearestNarbor
    // vgl. Literatur:
    // Nearest Narbor Query v. Roussopoulos, Kelley und Vincent,
    // University of Maryland

    float summe = 0;
    float minimum = 1.0e20;
    float S = 0;

    float rmk, rMi;
    int k,i;

    for( i = 0; i < DIMENSION; i++)
    {
	rMi = (	p[i] >= (bounces[2*i]+bounces[2*i+1])/2 )
	    ? bounces[2*i] : bounces[2*i+1];
	S += pow( p[i] - rMi , 2 );
    }

    for( k = 0; k < DIMENSION; k++)
    {

	rmk = ( p[k] <=  (bounces[2*k]+bounces[2*k+1]) / 2 ) ?
	    bounces[2*k] : bounces[2*k+1];

	summe = pow( p[k] - rmk , 2 );

	rMi = (	p[k] >= (bounces[2*k]+bounces[2*k+1]) / 2 )
	    ? bounces[2*k] : bounces[2*k+1];

	summe += S - pow( p[k] - rMi , 2 );

	minimum = min( minimum,summe);
    }

    return(minimum);


}

#endif
