#include "pb-tree.h"
#include "./gadget/gadget.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <queue>
#include <cstring>
using namespace std;
extern double cc;
extern double IOread;
extern long long EPS, MAXINT, BITS;
double* tpdists = NULL;
PB_Tree::PB_Tree()
{
	bplus = NULL;
	draf = NULL;
	ptable = NULL;
	cand = NULL;
	c = NULL;
	num_piv = -1;
	eps = -1;
}

PB_Tree::~PB_Tree()
{
	if(bplus!=NULL)
		delete bplus;
	if(draf!=NULL)
		delete draf;
	if(ptable!=NULL)
		delete[] ptable;
}


//find num_cand pivots in O using FFT
double** PB_Tree::FFT(Object * O, long long num)
{
	cand = new Object[num_cand];
	bool * indicator = new bool [num];
	for(long long i =0;i<num;i++)
		indicator[i]= true;
	long long * idset = new long long[num_cand];

	double d =0.0;
	double t;
	long long choose =0;

	double** distmatrix = new double*[num];
	for(long long i =0;i<num;i++)
	{
		distmatrix[i] = new double[num_cand];
		for(long long j =0;j<num_cand;j++)
			distmatrix[i][j]=0;
	}


	if(num_cand> 0)
	{

		for(long long i  =1 ;i<num;i++)
		{
			t = O[i].distance(O[0]);
			if(t>d)
			{
				d = t;
				choose = i;
			}
		}

		idset[0]= choose;
		cand[0]=O[choose];
		indicator[choose]=false;
	}

	if(num_cand>1)
	{
		d=0;
		for(long long i =0;i<num;i++)
		{
			if(indicator[i])
			{
				distmatrix[i][0]=cand[0].distance(O[i]);
				if(distmatrix[i][0]>d)
				{
					d=distmatrix[i][0];
					choose = i;
				}
			}
		}

		idset[1]= choose;
		cand[1]=O[choose];
		indicator[choose]=false;
	}

	d = 0;
	for(long long i = 2;i<num_cand;i++)
	{
		d= 0;
		for(long long j =0;j<num;j++)
		{
			if(indicator[j])
			{
				t =MAXREAL;
				for(long long k =0;k<i-1;k++)
				{
					if(distmatrix[j][k]<t)
						t= distmatrix[j][k];
				}
				distmatrix[j][i-1]=O[j].distance(cand[i-1]);
				if(distmatrix[j][i-1]<t)
					t = distmatrix[j][i-1];
				if(t>d)
				{
					d = t;
					choose = j;
				}
			}
		}

		idset[i]=choose;
		indicator[choose]=false;
		cand[i] = O[choose];
	}


	//print distance matrix
	for(long long i =0;i<num;i++)
	{
		if(indicator[i])
		{
			distmatrix[i][num_cand-1]=O[i].distance(cand[num_cand-1]);
		}
	}

	for(long long i =0;i<num_cand;i++)
		for(long long j = i+1;j<num_cand;j++)
			distmatrix[idset[i]][j]=O[idset[i]].distance(cand[j]);

	delete[] indicator;
	delete[] idset;

	return distmatrix;
}


//find num_cand pivots in O using maxpruning strategy
double** PB_Tree::MaxPrunning(Object * O, long long num)
{
	cand = new Object[num_cand];
	bool * indicator = new bool [num];
	for(long long i =0;i<num;i++)
		indicator[i]= true;
	long long * idset = new long long[num_cand];

	double d =0.0;
	double t;
	long long choose =0;

	double** distmatrix = new double*[num];
	for(long long i =0;i<num;i++)
	{
		distmatrix[i] = new double[num_cand];
		for(long long j =0;j<num_cand;j++)
			distmatrix[i][j]=0;
	}

	if(num_cand> 0)
	{

		for(long long i  =1 ;i<num;i++)
		{
			t = O[i].distance(O[0]);
			if(t>d)
			{
				d = t;
				choose = i;
			}
		}

		idset[0]= choose;
		cand[0]=O[choose];
		indicator[choose]=false;
	}

	if(num_cand>1)
	{
		d=0;
		for(long long i =0;i<num;i++)
		{
			if(indicator[i])
			{
				distmatrix[i][0]=cand[0].distance(O[i]);
				if(distmatrix[i][0]>d)
				{
					d=distmatrix[i][0];
					choose = i;
				}
			}
		}

		idset[1]= choose;
		cand[1]=O[choose];
		indicator[choose]=false;
	}

	double edge = d;
	d= MAXREAL;
	for(long long i = 2;i<num_cand;i++)
	{
		d= MAXREAL;
		for(long long j =0;j<num;j++)
		{
			if(indicator[j])
			{
				t =0;
				for(long long k =0;k<i-1;k++)
				{
					t+=fabs(edge-distmatrix[j][k]);
				}
				distmatrix[j][i-1]=O[j].distance(cand[i-1]);
				t +=fabs(edge-distmatrix[j][i-1]);
				if(t<d)
				{
					d = t;
					choose = j;
				}
			}
		}

		idset[i]=choose;
		indicator[choose]=false;
		cand[i] = O[choose];
	}


	//print distance matrix
	for(long long i =0;i<num;i++)
	{
		if(indicator[i])
		{
			distmatrix[i][num_cand-1]=O[i].distance(cand[num_cand-1]);
		}
	}

	for(long long i =0;i<num_cand;i++)
		for(long long j = i+1;j<num_cand;j++)
			distmatrix[idset[i]][j]=O[idset[i]].distance(cand[j]);

	delete[] indicator;
	delete[] idset;

	return distmatrix;
}

void PB_Tree::PivotSelect(Object * O, Object * Q, long long o_num, long long q_num, double thred)
{
	double ** O_P_matrix = MaxPrunning(O, o_num);
	vector<long long> pivots;

	double ** Q_O_matrix = new double *[q_num];
	double ** Q_P_matrix = new double*[q_num];
	double ** esti = new double* [q_num];
	for(long long i =0;i<q_num;i++)
	{
		Q_O_matrix[i]= new double[o_num];
		Q_P_matrix[i] = new double[num_cand];
		esti[i] = new double[o_num];
	}
	bool* indicator = new bool[num_cand];
	for(long long i =0 ;i<num_cand;i++)
		indicator[i] = true;


	for(long long i =0;i<q_num;i++)
	{
		for(long long j =0;j<o_num;j++)
		{
			Q_O_matrix[i][j] = Q[i].distance(O[j]);
			esti[i][j]=0;
		}
		for(long long j=0;j<num_cand;j++)
		{
			Q_P_matrix[i][j] = Q[i].distance(cand[j]);
		}
	}

	double d =0;
	double t =0;
	double last =0;
	long long choose;
	ptable = new Object[num_piv];
	long long i;
	while(true)
	{
		choose = -1;
		for(long long j =0;j<num_cand;j++)
		{
			if(indicator[j])
			{
				t= 0;
				for(long long m = 0;m < q_num;m++)
				{
					for(long long n =0;n <o_num;n++)
					{
						if(Q_O_matrix[m][n] != 0)
						{
							t+= (MAX(fabs(Q_P_matrix[m][j]-O_P_matrix[n][j]),esti[m][n]))/Q_O_matrix[m][n];
						}
					}
				}
				t = t/(q_num*o_num);
				if(t>d)
				{
					choose = j;
					d = t;
				}
			}
		}

		printf("%d %f\n",choose, d);
		if(choose == -1||d-last<thred)
			break;
		last = d;
		indicator[choose]= false;
		pivots.push_back(choose);
		for(long long m =0;m< q_num;m++)
			for(long long n=0;n<o_num;n++)
				esti[m][n]=MAX(fabs(Q_P_matrix[m][choose]-O_P_matrix[n][choose]),esti[m][n]);
	}


	long long victim;
	long long iter=0;
	while(true)
	{
		d=1;
		iter++;
		victim= -1;
		for(long long i =0;i<pivots.size();i++)
		{
			for(long long m =0;m<q_num;m++)
				for(long long n=0;n<o_num;n++)
					esti[m][n]=0;

			for(long long j=0;j<pivots.size();j++)
			{
				if(j!=i)
				{
					for(long long m =0;m<q_num;m++)
						for(long long n =0;n<o_num;n++)
							esti[m][n]=MAX(fabs(Q_P_matrix[m][pivots[j]]-O_P_matrix[n][pivots[j]]),esti[m][n]);
				}
			}

			t=0;
			for(long long m=0;m<q_num;m++)
			{
				for(long long n=0;n<o_num;n++)
				{
					if(Q_O_matrix[m][n]!=0)
						t+=esti[m][n]/Q_O_matrix[m][n];
				}
			}

			t = t/(q_num*o_num);
			printf("contribution %d %f\n",i, last-t);
			if(last-t<d)
			{
				victim=i;
				d=last-t;
			}
		}

		for(long long m =0;m<q_num;m++)
				for(long long n=0;n<o_num;n++)
					esti[m][n]=0;

		for(long long j=0;j<pivots.size();j++)
		{
			if(j!=victim)
			{
				for(long long m =0;m<q_num;m++)
					for(long long n =0;n<o_num;n++)
						esti[m][n]=MAX(fabs(Q_P_matrix[m][pivots[j]]-O_P_matrix[n][pivots[j]]),esti[m][n]);
			}
		}

		printf("victim %d %f\n",victim, d);

		choose = -1;
		d= last;
		for(long long j =0;j<num_cand;j++)
		{
			if(indicator[j])
			{
				t= 0;
				for(long long m = 0;m < q_num;m++)
				{
					for(long long n =0;n <o_num;n++)
					{
						if(Q_O_matrix[m][n] != 0)
						{
							t+= (MAX(fabs(Q_P_matrix[m][j]-O_P_matrix[n][j]),esti[m][n]))/Q_O_matrix[m][n];
						}
					}
				}
				t = t/(q_num*o_num);
				if(t>d)
				{
					choose = j;
					d = t;
				}
			}
		}

		printf("%d %f\n",choose, d);
		if(choose == -1||d-last<thred)
			break;
		last = d;
		indicator[choose]=false;
		indicator[pivots[victim]]=true;
		pivots.erase(pivots.begin()+victim);
		pivots.push_back(choose);
	}

	num_piv = pivots.size();
	ptable = new Object [num_piv];
	for(long long i =0;i<num_piv;i++)
		ptable[i]= cand[pivots[i]];

	for(i=0;i<q_num;i++)
	{
		delete[] esti[i];
		delete[] Q_P_matrix[i];
		delete[] Q_O_matrix[i];
	}
	delete[] indicator;
	for(i =0;i<o_num;i++)
		delete[] O_P_matrix[i];
}

void PB_Tree::PivotSelect(Object * O, Object * Q, long long o_num, long long q_num)
{
	double ** O_P_matrix = MaxPrunning(O, o_num);

	FILE *ft = fopen("cand.txt", "w");
	for (long long i = 0; i < num_cand; ++i) {
		fprintf(ft, "%d ", cand[i].id);
	}
	fclose(ft);

	double ** Q_O_matrix = new double *[q_num];
	double ** Q_P_matrix = new double*[q_num];
	double ** esti = new double* [q_num];
	for(long long i =0;i<q_num;i++)
	{
		Q_O_matrix[i]= new double[o_num];
		Q_P_matrix[i] = new double[num_cand];
		esti[i] = new double[o_num];
	}
	bool* indicator = new bool[num_cand];
	for(long long i =0 ;i<num_cand;i++)
		indicator[i] = true;


	for(long long i =0;i<q_num;i++)
	{
		for(long long j =0;j<o_num;j++)
		{
			Q_O_matrix[i][j] = Q[i].distance(O[j]);
			esti[i][j]=0;
		}
		for(long long j=0;j<num_cand;j++)
		{
			Q_P_matrix[i][j] = Q[i].distance(cand[j]);
		}
	}


	double d =0;
	double t =0;
	long long choose;
	ptable = new Object[num_piv];
	long long i;
	for(i =0;i<num_piv;i++)
	{
		choose = -1;
		for(long long j =0;j<num_cand;j++)
		{
			if(indicator[j])
			{
				t= 0;
				for(long long m = 0;m < q_num;m++)
				{
					for(long long n =0;n <o_num;n++)
					{
						if(Q_O_matrix[m][n] != 0)
						{
							t+= (MAX(fabs(Q_P_matrix[m][j]-O_P_matrix[n][j]),esti[m][n]))/Q_O_matrix[m][n];
						}
					}
				}
				t = t/(q_num*o_num);
				if(t>d)
				{
					choose = j;
					d = t;
				}
			}
		}
		printf("%d %f\n",choose, d);
		if(choose == -1)
			break;
		indicator[choose]= false;
		ptable[i]=cand[choose];
		for(long long m =0;m<q_num;m++)
			for(long long n=0;n<o_num;n++)
				esti[m][n]=MAX(fabs(Q_P_matrix[m][choose]-O_P_matrix[n][choose]),esti[m][n]);
	}

	if(i<num_piv)
	{
		num_piv = i;
	}

	for(i=0;i<q_num;i++)
	{
		delete[] esti[i];
		delete[] Q_P_matrix[i];
		delete[] Q_O_matrix[i];
	}
	delete[] indicator;
	for(i =0;i<o_num;i++)
		delete[] O_P_matrix[i];

}

void PB_Tree::RPivotSelect(Object * O, Object * Q, long long o_num, long long q_num)
{
	ptable = new Object[num_piv];
	double ** O_P_matrix = MaxPrunning(O,o_num);
	double ** Q_O_matrix = new double *[q_num];
	double ** Q_P_matrix = new double*[q_num];

	double ** esti = new double* [q_num];
	for(long long i =0;i<q_num;i++)
	{
		Q_O_matrix[i]= new double[o_num];
		Q_P_matrix[i] = new double[num_cand];
		esti[i] = new double[o_num];
	}

	double *** esti_temp = new double **[num_cand];
	for(long long i =0;i<num_cand;i++)
	{
		esti_temp[i]= new double*[q_num];
		for(long long j =0;j<q_num;j++)
			esti_temp[i][j]= new double[o_num];
	}

	for(long long i =0;i<q_num;i++)
	{
		for(long long j =0;j<o_num;j++)
		{
			Q_O_matrix[i][j] = Q[i].distance(O[j]);
			esti[i][j]=0;
		}
		for(long long j=0;j<num_cand;j++)
		{
			Q_P_matrix[i][j] = Q[i].distance(cand[j]);
		}
	}

	double score =0;

	for(long long i=0;i<q_num;i++)
	{
		for(long long j=0;j<o_num;j++)
		{
			for(long long k =0;k<num_cand;k++)
			{
				esti_temp[k][i][j]=fabs(Q_P_matrix[i][k]-O_P_matrix[j][k]);
				if(esti[i][j]<esti_temp[k][i][j])
					esti[i][j]=esti_temp[k][i][j];
			}
			if(Q_O_matrix[i][j]!=0)
				score += esti[i][j]/Q_O_matrix[i][j];
		}
	}
	score = score/(q_num*o_num);
	cout<<"score: "<<score<<endl;

	bool * flag_c = new bool[num_cand];

	for(long long i=0;i<num_cand;i++)
	{
		flag_c[i]=true;
	}

	double * contribution = new double[num_cand];
	double temp = MAXREAL;
	long long choose;
	double t;
	for(long long i=0;i<num_cand-num_piv;i++)
	{
		temp = MAXREAL;
		for(long long j=0;j<num_cand;j++)
		{
			if(flag_c[j])
			{
				contribution[j]=0;
				for(long long m =0;m<q_num; m++)
				{
					for(long long n=0;n<o_num; n++)
					{
						if(esti[m][n]==esti_temp[j][m][n])
						{
							t=0;
							for(long long k =0;k<num_cand;k++)
							{
								if(flag_c[k] && k!=j && esti_temp[k][m][n] >t)
									t = esti_temp[k][m][n];

							}
							if(Q_O_matrix[m][n]!=0)
								contribution[j] += (esti[m][n]-t)/Q_O_matrix[m][n];
						}
					}
				}
				if(contribution[j]<temp)
				{
					temp = contribution[j];
					choose = j;
				}
			}
		}
		flag_c[choose]=false;
		cout<<choose<<" "<<contribution[choose]<<endl;
		for(long long m =0;m<q_num; m++)
		{
			for(long long n=0;n<o_num; n++)
			{
				if(esti[m][n]==esti_temp[choose][m][n])
				{
					t=0;
					for(long long k =0;k<num_cand;k++)
					{
						if(flag_c[k])
							if(esti_temp[k][m][n] >t)
								t = esti_temp[k][m][n];
					}
					esti[m][n]= t;
				}
			}
		}
	}
	score =0;
	for(long long i=0;i<q_num;i++)
		for(long long j=0;j<o_num;j++)
		{
			if(Q_O_matrix[i][j]!=0)
				score += esti[i][j]/Q_O_matrix[i][j];
		}
	cout<<score/(q_num*o_num)<<endl;

	choose =0;
	for(long long i=0;i<num_cand;i++)
		if(flag_c[i])
		{
			ptable[choose]= cand[i];
			choose++;
		}

	delete[] flag_c;
	delete[] contribution;

	for(long long i=0;i<q_num;i++)
	{
		delete[] esti[i];
		delete[] Q_P_matrix[i];
		delete[] Q_O_matrix[i];
	}
	for(long long i =0;i<o_num;i++)
		delete[] O_P_matrix[i];
	for(long long i=0;i<num_cand;i++)
	{
		for(long long j =0;j<q_num;j++)
			delete[] esti_temp[i][j];
		delete [] esti_temp[i];
	}
	delete[] esti_temp;

}

//******need to set bits and eps values******
//bits: the number bits to represent the distance
//eps: to partion the domain of the distance
unsigned long long * PB_Tree:: Zconvert(long long * dist)
{
	long long keysize = (num_piv * bits)/32;
	if((num_piv * bits)%32!=0)
		keysize = keysize +1;
	unsigned long long * zvalue = new unsigned long long[keysize];
	long long temp = 32*keysize;
	long long* zbits = new long long[temp];
    memset(zbits, 0, sizeof(long long) * temp);
	for(long long i = 0;i<num_piv;i++)
	{
		bitset<32> bs(dist[i]);
		for(long long j = 1;j<=bits;j++)
			zbits[j*num_piv-i-1]=bs.test(j-1);
	}
	for(long long i =0;i<keysize;i++)
	{
		temp-=32;
		bitset<32> key;
		for(long long j =0;j<32;j++)
			if(zbits[temp+j])
				key.set(j);
		zvalue[i]= key.to_ulong();
	}
	delete[] zbits;
	return zvalue;
}

void PB_Tree::Zconvert(Object *o)
{
	long long keysize = (num_piv * bits)/32;
	if((num_piv * bits)%32!=0)
		keysize = keysize +1;
	o->keysize = keysize;
	o->key = new unsigned long long [keysize];

	long long * dists = new long long[num_piv];
	long long temp = 32*keysize;
	long long* zbits = new long long[temp];
	memset(zbits, 0, sizeof(long long) * temp);
	for(long long i =0;i<num_piv;i++)
	{
		if(eps >0)
			dists[i] = (long long) floor(o->distance(ptable[i])/eps);
		else
			dists[i]= (long long) o->distance(ptable[i]);
		bitset<32> bs(dists[i]);
		for(long long j = 1;j<=bits;j++)
		{
			zbits[j*num_piv-i-1]=bs.test(j-1);
		}
	}
	for(long long i =temp-1;i>=0;i--)
	{
		if(zbits[i]==0)
		{
			o->keycomp.append("0");
		}
		else
		{
			o->keycomp.append("1");
		}
	}

	for(long long i =0;i<keysize;i++)
	{
		temp -=32;
		bitset<32> key;
		for(long long j =0;j<32;j++)
		{
			if(zbits[temp+j])
				key.set(j);
		}
		o->key[i] = key.to_ulong();
	}
	delete[] dists;
	delete[] zbits;
}

long long* PB_Tree::R_Zconvert(unsigned long long * okey)
{
	long long keysize = (num_piv * bits)/32;
	if((num_piv * bits)%32!=0)
		keysize = keysize +1;
	unsigned long long * key = new unsigned long long [keysize];
	memcpy(key, okey, keysize*sizeof(unsigned long long));
	long long temp = 32*keysize;
	long long * zbits = new long long[temp];
	memset(zbits, 0, sizeof(long long) * temp);
	for(long long i =0;i<keysize;i++)
	{
		long long tt = (keysize-1-i)*32;
		for(long long j = 0;j<32 && key[i]!=0;key[i]>>=1,j++)
		{
			if(key[i]&1)
				zbits[tt+j]=1;
		}
	}

	long long* d = new long long[num_piv];
	for(long long i=0;i<num_piv;i++)
		d[i]=0;

	for(long long j =0;j<num_piv;j++)
	{
		bitset<32> data;
		for(long long i=0;i<bits;i++)
		{
			if(zbits[i*num_piv+num_piv-1-j])
				data.set(i);
		}
		d[j]=data.to_ulong();
	}
	delete[] zbits;

	delete[] key;
	okey = NULL;
	return d;
}

void PB_Tree::Hconvert(Object *o)
{
	long long keysize = (num_piv * bits)/32;
	if((num_piv * bits)%32!=0)
		keysize = keysize +1;
	o->keysize = keysize;
	o->key = new unsigned long long [keysize];

	unsigned long long  * dists = new unsigned long long[num_piv];
	unsigned long long * key = new unsigned long long[num_piv];
	for(long long i =0;i<num_piv;i++)
	{
		key[i] =0;
		if(eps >0)
			dists[i] = (long long) floor(o->distance(ptable[i])/eps);
		else
			dists[i]= (long long) o->distance(ptable[i]);
	}

	Hconvert(key, dists, num_piv);

	for(long long i =0;i<keysize;i++)
	{
		o->key[i]=key[num_piv-keysize+i];
	}

	delete[] dists;
	delete[] key;
}

unsigned long long * PB_Tree:: Hconvert ( unsigned long long* hcode,  unsigned long long* point, long long DIMS ) {
    unsigned long long mask = ( unsigned long long ) 1 << WORDBITS - 1, element, temp1, temp2,
             A, W = 0, S, tS, T, tT, J, P = 0, xJ;
    long long	i = NUMBITS * DIMS - DIMS, j;

    // initialise hcode
    memset ( hcode, 0, sizeof ( unsigned long long ) * DIMS );

    for ( j = A = 0; j < DIMS; j++ )
        if ( point[j] & mask )
            A |= ( 1 << ( DIMS-1-j ) );

    S = tS = A;

    P |= S & ( 1 << ( DIMS-1 ) );
    for ( j = 1; j < DIMS; j++ )
        if ( S & 1 << ( DIMS-1-j ) ^ ( P >> 1 ) & ( 1 << ( DIMS-1-j ) ) )
            P |= ( 1 << ( DIMS-1-j ) );

    /* add in DIMS bits to hcode */
    element = i / WORDBITS;
    if ( i % WORDBITS > WORDBITS - DIMS ) {
        hcode[element] |= P << i % WORDBITS;
        hcode[element + 1] |= P >> WORDBITS - i % WORDBITS;
    } else
        hcode[element] |= P << i - element * WORDBITS;

    J = DIMS;
    for ( j = 1; j < DIMS; j++ )
        if ( ( P >> j & 1 ) == ( P & 1 ) )
            continue;
        else
            break;
    if ( j != DIMS )
        J -= j;
    xJ = J - 1;

    if ( P < 3 )
        T = 0;
    else if ( P % 2 )
        T = ( P - 1 ) ^ ( P - 1 ) / 2;
    else
        T = ( P - 2 ) ^ ( P - 2 ) / 2;
    tT = T;


    for ( i -= DIMS, mask >>= 1; i >=0; i -= DIMS, mask >>= 1 ) {
        for ( j = A = 0; j < DIMS; j++ )
            if ( point[j] & mask )
                A |= ( 1 << ( DIMS-1-j ) );

        W ^= tT;
        tS = A ^ W;
        if ( xJ % DIMS != 0 ) {
            temp1 = tS << xJ % DIMS;
            temp2 = tS >> DIMS - xJ % DIMS;
            S = temp1 | temp2;
            S &= ( ( unsigned long long ) 1 << DIMS ) - 1;
        } else
            S = tS;

        P = S & ( 1 << ( DIMS-1 ) );
        for ( j = 1; j < DIMS; j++ )
            if ( S & ( 1 << ( DIMS-1-j ) ) ^ ( P >> 1 ) & ( 1 << ( DIMS-1-j ) ) )
                P |= ( 1 << ( DIMS-1-j ) );

        /* add in DIMS bits to hcode */
        element = i / WORDBITS;
        if ( i % WORDBITS > WORDBITS - DIMS ) {
            hcode[element] |= P << i % WORDBITS;
            hcode[element + 1] |= P >> WORDBITS - i % WORDBITS;
        } else
            hcode[element] |= P << i - element * WORDBITS;

        if ( i > 0 ) {
            if ( P < 3 )
                T = 0;
            else if ( P % 2 )
                T = ( P - 1 ) ^ ( P - 1 ) / 2;
            else
                T = ( P - 2 ) ^ ( P - 2 ) / 2;

            if ( xJ % DIMS != 0 ) {
                temp1 = T >> xJ % DIMS;
                temp2 = T << DIMS - xJ % DIMS;
                tT = temp1 | temp2;
                tT &= ( ( unsigned long long ) 1 << DIMS ) - 1;
            } else
                tT = T;

            J = DIMS;
            for ( j = 1; j < DIMS; j++ )
                if ( ( P >> j & 1 ) == ( P & 1 ) )
                    continue;
                else
                    break;
            if ( j != DIMS )
                J -= j;

            xJ += J - 1;
        }


    }

	unsigned long long * temp = new unsigned long long [DIMS];

	for(long long i =0;i<DIMS;i++)
	{
		temp[i]=hcode[i];
	}

	for(long long i =0;i<DIMS;i++)
	{
		hcode[i]=temp[DIMS-i-1];
	}

	delete[] temp;
    return hcode;
}

unsigned long long * PB_Tree:: R_Hconvert ( unsigned long long* point, unsigned long long* hc, long long DIMS )
{

	unsigned long long * hcode = new unsigned long long [DIMS];
	for(long long i =0;i<DIMS;i++)
		hcode [i] = hc[DIMS-i-1];

	unsigned long long   mask = (unsigned long long)1 << WORDBITS - 1, element, temp1, temp2,
                A, W = 0, S, tS, T, tT, J, P = 0, xJ;
        long long     i = NUMBITS * DIMS - DIMS, j;


        /*--- P ---*/
        element = i / WORDBITS;
        P = hcode[element];
        if (i % WORDBITS > WORDBITS - DIMS)
        {
                temp1 = hcode[element + 1];
                P >>= i % WORDBITS;
                temp1 <<= WORDBITS - i % WORDBITS;
                P |= temp1;
        }
        else
                P >>= i % WORDBITS;     /* P is a DIMS bit hcode */

        /* the & masks out spurious highbit values */
        #if DIMS < WORDBITS
                P &= (1 << DIMS) -1;
        #endif

        /*--- xJ ---*/
        J = DIMS;
        for (j = 1; j < DIMS; j++)
                if ((P >> j & 1) == (P & 1))
                        continue;
                else
                        break;
        if (j != DIMS)
                J -= j;
        xJ = J - 1;

        /*--- S, tS, A ---*/
        A = S = tS = P ^ P / 2;


        /*--- T ---*/
        if (P < 3)
                T = 0;
        else
                if (P % 2)
                        T = (P - 1) ^ (P - 1) / 2;
                else
                        T = (P - 2) ^ (P - 2) / 2;

        /*--- tT ---*/
        tT = T;

        /*--- distrib bits to coords ---*/
        for (j = DIMS - 1; P > 0; P >>=1, j--)
                if (P & 1)
                        point[j] |= mask;


        for (i -= DIMS, mask >>= 1; i >=0; i -= DIMS, mask >>= 1)
        {
                /*--- P ---*/
                element = i / WORDBITS;
                P = hcode[element];
                if (i % WORDBITS > WORDBITS - DIMS)
                {
                        temp1 = hcode[element + 1];
                        P >>= i % WORDBITS;
                        temp1 <<= WORDBITS - i % WORDBITS;
                        P |= temp1;
                }
                else
                        P >>= i % WORDBITS;     /* P is a DIMS bit hcode */

                /* the & masks out spurious highbit values */
                #if DIMS < WORDBITS
                        P &= (1 << DIMS) -1;
                #endif

                /*--- S ---*/
                S = P ^ P / 2;

                /*--- tS ---*/
                if (xJ % DIMS != 0)
                {
                        temp1 = S >> xJ % DIMS;
                        temp2 = S << DIMS - xJ % DIMS;
                        tS = temp1 | temp2;
                        tS &= ((unsigned long long)1 << DIMS) - 1;
                }
                else
                        tS = S;

                /*--- W ---*/
                W ^= tT;

                /*--- A ---*/
                A = W ^ tS;

                /*--- distrib bits to coords ---*/
                for (j = DIMS - 1; A > 0; A >>=1, j--)
                        if (A & 1)
                                point[j] |= mask;

                if (i > 0)
                {
                        /*--- T ---*/
                        if (P < 3)
                                T = 0;
                        else
                                if (P % 2)
                                        T = (P - 1) ^ (P - 1) / 2;
                                else
                                        T = (P - 2) ^ (P - 2) / 2;

                        /*--- tT ---*/
                        if (xJ % DIMS != 0)
                        {
                                temp1 = T >> xJ % DIMS;
                                temp2 = T << DIMS - xJ % DIMS;
                                tT = temp1 | temp2;
                                tT &= ((unsigned long long)1 << DIMS) - 1;
                        }
                        else
                                tT = T;

                        /*--- xJ ---*/
                        J = DIMS;
                        for (j = 1; j < DIMS; j++)
                                if ((P >> j & 1) == (P & 1))
                                        continue;
                                else
                                        break;
                        if (j != DIMS)
                                J -= j;
                        xJ += J - 1;
                }
        }
		delete[] hcode;
        return point;
}

void PB_Tree::readptable(char *pname)
{
	ifstream in1(pname,ios::in);
	ptable = new Object[num_piv];
	for(long long i =0;i<num_piv;i++)
	{
		in1>>ptable[i].id>>ptable[i].size;
		ptable[i].data = new float[ptable[i].size];
		for(long long j =0;j<ptable[i].size;j++)
		{
			in1>>ptable[i].data[j];
		}
	}
	in1.close();
}

void PB_Tree::H_bulkload(Object * O, long long o_num)
{
	vector<HEntry> H;
	for(long long i =0;i<o_num;i++)
	{
		Hconvert(&O[i]);
		HEntry h(O[i].key, O[i].keysize, i);
		H.push_back(h);
	}
	sort(H.begin(), H.end());
	cout<<H.size()<<endl;

	FILE* f = fopen("bulkload.txt","w+");

	if(f!=NULL)
	{
		for(long long i=0;i<o_num;i++)
		{
			if(H[i].id==0)
				cout<<" ****************"<<endl;
			fprintf(f, "%d ", H[i].id);
			for(long long j =0;j<H[i].keysize;j++)
				fprintf(f, "%u ", H[i].key[j]);
			fprintf(f,"\n");
		}
		fclose(f);
	}
	H.clear();
}

void PB_Tree::bulkload(Object * O, long long o_num)
{
	vector<TEntry> v;
	for(long long i =0;i<o_num;i++)
	{
		Zconvert(&O[i]);
		TEntry e;
		e.id = i;
		e.key = O[i].keycomp;
		v.push_back(e);
	}
	sort(v.begin(),v.end());

	FILE* f = fopen("bulkload.txt","w+");

	if(f!=NULL)
	{
		for(long long i=0;i<o_num;i++)
		{
			fprintf(f, "%d ", O[v[i].id].id);
			for(long long j =0;j<O[v[i].id].keysize;j++)
				fprintf(f, "%u ", O[v[i].id].key[j]);
			fprintf(f,"\n");
		}
		fclose(f);
	}
	v.clear();
}

void clear(vector<B_Node *> * inodes)
{
	for(long long i =0;i<inodes->size();i++)
	{
		if((*inodes)[i]->num_entries ==0)
		{
			inodes->erase(inodes->begin()+i);
			i--;
		}
	}
};
long long PB_Tree::idist(long long * q, long long* mi, long long* ma)
{
	long long dist =0;
	long long temp;
	for(long long i =0;i<num_piv;i++)
	{
		if(q[i]<mi[i])
			temp = mi[i]-q[i];
		else if(q[i]>ma[i])
			temp = q[i]-ma[i];
		else
			temp =0;
		if(dist<temp)
			dist = temp;
	}
	return dist;
}

double PB_Tree::BFkNN(Object*q, long long k)
{
	long long * dists = new long long[num_piv];
	for(long long i =0;i<num_piv;i++)
	{
		if(eps >0)
			dists[i] = (long long) floor(q->distance(ptable[i])/eps);
		else
			dists[i]= (long long) q->distance(ptable[i]);
	}
	long long addition =0;
	long long temp;
	if(eps>0)
		temp = eps;
	else
		temp = 1;
	if(eps >0)
		addition = eps;

	vector<SortEntry> kNN;
	double kdist = MAXREAL;
	priority_queue<kNNEntry> queue;
	bplus->load_root();
	B_Node* node = bplus->root_ptr;
	for(long long i =0;i<node->num_entries;i++)
	{
		unsigned long long * key1 = new unsigned long long [num_piv];
		unsigned long long * mm1 = new unsigned long long [num_piv];
		for(long long j =0;j<bplus->keysize;j++)
		{
			mm1[j]=0;
			key1[num_piv+j-bplus->keysize] = node->entries[i]->min[j];
		}
		for(long long j = bplus->keysize;j<num_piv;j++)
		{
			mm1[j]=0;
			key1[j-bplus->keysize] = 0;
		}
		R_Hconvert(mm1, key1, num_piv);

		unsigned long long * key2 = new unsigned long long [num_piv];
		unsigned long long * mm2 = new unsigned long long [num_piv];
		for(long long j =0;j<bplus->keysize;j++)
		{
			mm2[j]=0;
			key2[num_piv+j-bplus->keysize] = node->entries[i]->max[j];
		}
		for(long long j = bplus->keysize;j<num_piv;j++)
		{
			mm2[j]=0;
			key2[j-bplus->keysize] = 0;
		}
		R_Hconvert(mm2, key2, num_piv);
		delete[] key1;
		delete[] key2;
		long long * m1 = new long long [num_piv];
		long long * m2 = new long long [num_piv];
		for(long long j =0;j<num_piv;j++)
		{
			m1[j]= mm1[j];
			m2[j] = mm2[j];
		}
		delete[] mm1;
		delete[] mm2;
		kNNEntry e;
		e.edist=idist(dists,m1,m2);

		delete[] m1;
		delete[] m2;
		if(node->level>0)
			e.ptr = node->entries[i]->son;
		else
			e.ptr= node->entries[i]->ptr;
		e.level = node->entries[i]->level;
		queue.push(e);
	}
	delete node;
	node = NULL;
	while(!queue.empty())
	{
		kNNEntry e = queue.top();
		queue.pop();
		if(e.edist*temp>=kdist+addition)
		{
			break;
		}
		if(e.level>0) // pop an intermediate entry and push its qualified sub entries
		{
			node = bplus->read_node(e.ptr);
			if(e.level>1)
			{
				for(long long i =0;i<node->num_entries;i++)
				{
					unsigned long long * key1 = new unsigned long long [num_piv];
					unsigned long long * mm1 = new unsigned long long [num_piv];
					for(long long j =0;j<bplus->keysize;j++)
					{
						mm1[j]=0;
						key1[num_piv+j-bplus->keysize] = node->entries[i]->min[j];
					}
					for(long long j = bplus->keysize;j<num_piv;j++)
					{
						mm1[j]=0;
						key1[j-bplus->keysize] = 0;
					}
					R_Hconvert(mm1, key1, num_piv);

					unsigned long long * key2 = new unsigned long long [num_piv];
					unsigned long long * mm2 = new unsigned long long [num_piv];
					for(long long j =0;j<bplus->keysize;j++)
					{
						mm2[j]=0;
						key2[num_piv+j-bplus->keysize] = node->entries[i]->max[j];
					}
					for(long long j = bplus->keysize;j<num_piv;j++)
					{
						mm2[j]=0;
						key2[j-bplus->keysize] = 0;
					}
					R_Hconvert(mm2, key2, num_piv);
					delete[] key1;
					delete[] key2;
					long long * m1 = new long long [num_piv];
					long long * m2 = new long long [num_piv];
					for(long long j =0;j<num_piv;j++)
					{
						m1[j]= mm1[j];
						m2[j] = mm2[j];
					}
					delete[] mm1;
					delete[] mm2;
					kNNEntry entry;
					entry.edist=idist(dists,m1,m2);

					delete[] m1;
					delete[] m2;
					entry.level= node->level;
					entry.ptr = node->entries[i]->son;
					if(entry.edist*temp<kdist+addition)
					{
						queue.push(entry);
					}
				}
			}
			else
			{
				for(long long i =0;i<node->num_entries;i++)
				{
					unsigned long long * tem = new unsigned long long [num_piv];
					unsigned long long * key = new unsigned long long [num_piv];
					for(long long j =0;j<bplus->keysize;j++)
					{
						tem[j]=0;
						key[j+num_piv-bplus->keysize]= node->entries[i]->key[j];
					}
					for(long long j=bplus->keysize;j<num_piv;j++)
					{
						tem[j]=0;
						key[j-bplus->keysize]=0;
					}
					R_Hconvert(tem, key, num_piv);
					delete[] key;

					long long d = edist(dists, tem);
					delete[] tem;
					if(d*temp<kdist+addition)
					{
						kNNEntry ke;
						ke.level = -1;
						ke.edist = d;
						ke.ptr = node->entries[i]->ptr;
						queue.push(ke);
					}

				}
			}
			delete node;
			node = NULL;
		}
		else  //pop a leaf entry and verify if it is a kNN
		{
			SortEntry se;
			Object* o = getobject(e.ptr);
			se.key=q->distance(*o);
			if(se.key<=kdist)
			{
				se.id=o->id;
				kNN.push_back(se);
				if(kNN.size()>=k)
				{
					sort(kNN.begin(),kNN.end());
					while(kNN.size()>k)
						kNN.pop_back();
					kdist=kNN.back().key;
				}
			}
			delete o;
		}
	}
	bplus->root_ptr = NULL;
	delete[] dists;
    cout<<kNN.size()<<" ";
   return kdist;
}


// false: point is not in region [oqmin, oqmax]
// true: point is in region [oqmin, oqmax]
bool PB_Tree::inregion(long long* point, long long* oqmin, long long* oqmax)
{
	for(long long i =0;i<num_piv;i++)
	{
		if(point[i]<oqmin[i]||point[i]>oqmax[i])
			return false;
	}
	return true;
}


long long PB_Tree::edist(long long * query, long long* o)
{
	long long ed = 0;
	long long temp;
	for(long long i =0;i<num_piv;i++)
	{
		temp= abs(query[i]-o[i]);
		if(temp>ed)
			ed = temp;
	}
	return ed;
}

long long PB_Tree::edist(long long * query, unsigned long long * o)
{
	long long ed = 0;
	long long temp;
	for(long long i =0;i<num_piv;i++)
	{
		temp = o[i];
		temp= abs(query[i]-temp);
		if(temp>ed)
			ed = temp;
	}
	return ed;
}

// to deretmin the intersection relation between two MBBs [min1, max1] and [min2, max2]
// return 0, 1, 2 or -1
long long PB_Tree::determine(long long * min1, long long * max1, long long* min2, long long* max2)
{
	long long c1, c2, c3;
	c1= c2=c3=0;
	for(long long i =0;i<num_piv;i++)
	{
		if (max2[i] <= tpdists[i])
			return 2;
		if(min2[i]>= tpdists[i] + 1 && min2[i] >= min1[i]  &&max2[i]<=max1[i])
			c1++;
		if(max2[i]<min1[i])
			return -1;
		if(min2[i]>max1[i])
			return -1;
	}
	if(c1 ==num_piv)
		return 0;
	return 1;
}

vector<long long> RQ;

void RQv(B_Node*  l, B_Node*  r)
{
	if (l->level != 0)
	{
		RQv(l->entries[0]->get_son(), r->entries[r->num_entries-1]->get_son());
		l->entries[0]->del_son();
		r->entries[r->num_entries-1]->del_son();
	}
	else
	{
		B_Node* n = NULL;
		do
		{
			for (long long i = 0; i<l->num_entries; i++)
				RQ.push_back(l->entries[i]->ptr);
			n = l->get_right_sibling();
			delete l;
			l = n;
			n = NULL;
		} while (l->block != r->block);
		for (long long i = 0; i<r->num_entries; i++)
			RQ.push_back(r->entries[i]->ptr);
	}
}

void PB_Tree::RRQH_new(B_Node* node, vector<long long>* result, long long* qmin, long long* qmax, long long* min, long long *max, bool flag, bool validate, long long* query, long long radius)
{
	if (node->level != 0)
	{
		if (flag)
		{
			for (long long i = 0; i < node->num_entries; i++)
			{
				RRQH_new(node->entries[i]->get_son(), result, qmin, qmax, NULL, NULL, true, false, NULL, radius);
				node->entries[i]->del_son();
			}
		}
		else if (validate)
		{
			if (node->level > 1)
			{
				RQv(node->entries[0]->get_son(), node->entries[node->num_entries - 1]->get_son());
				node->entries[0]->del_son();
				node->entries[node->num_entries-1]->del_son();
			}
			else
			{
				for (long long i = 0; i < node->num_entries; i++)
				{
					RRQH_new(node->entries[i]->get_son(), result, qmin, qmax, NULL, NULL, false, true, NULL, radius);
					node->entries[i]->del_son();
				}
			}
		}
		else
		{
			for (long long i = 0; i < node->num_entries; i++)
			{
				unsigned long long * key1 = new unsigned long long[num_piv];
				unsigned long long * mm1 = new unsigned long long[num_piv];
				for (long long j = 0; j < bplus->keysize; j++)
				{
					mm1[j] = 0;
					key1[num_piv + j - bplus->keysize] = node->entries[i]->min[j];
				}
				for (long long j = bplus->keysize; j < num_piv; j++)
				{
					mm1[j] = 0;
					key1[j - bplus->keysize] = 0;
				}
				R_Hconvert(mm1, key1, num_piv);

				unsigned long long * key2 = new unsigned long long[num_piv];
				unsigned long long * mm2 = new unsigned long long[num_piv];
				for (long long j = 0; j < bplus->keysize; j++)
				{
					mm2[j] = 0;
					key2[num_piv + j - bplus->keysize] = node->entries[i]->max[j];
				}
				for (long long j = bplus->keysize; j < num_piv; j++)
				{
					mm2[j] = 0;
					key2[j - bplus->keysize] = 0;
				}
				R_Hconvert(mm2, key2, num_piv);
				delete[] key1;
				delete[] key2;
				long long * m1 = new long long[num_piv];
				long long * m2 = new long long[num_piv];
				for (long long j = 0; j < num_piv; j++)
				{
					m1[j] = mm1[j];
					m2[j] = mm2[j];
				}
				delete[] mm1;
				delete[] mm2;

				long long t = determine(qmin, qmax, m1, m2);
				if (t == 0)
				{
					RRQH_new(node->entries[i]->get_son(), result, qmin, qmax, NULL, NULL, true, false, NULL, radius);
					node->entries[i]->del_son();
				}
				else if (t == 2)
				{
					RRQH_new(node->entries[i]->get_son(), result, qmin, qmax, NULL, NULL, false, true, NULL, radius);
					node->entries[i]->del_son();
				}
				else if(t==1)
				{
					if(node->level == 1)
						RRQH_new(node->entries[i]->get_son(), result, qmin, qmax, m1, m2, false, false, query, radius);
					else
						RRQH_new(node->entries[i]->get_son(), result, qmin, qmax, NULL, NULL, false, false, query, radius);
					node->entries[i]->del_son();
				}
				delete[] m1;
				delete[] m2;
			}
		}
	}
	else
	{

		if(flag)
		{
			for(long long i =0;i<node->num_entries;i++)
				result->push_back(node->entries[i]->ptr);
		}
		else if (validate)
		{
			for (long long i = 0; i<node->num_entries; i++)
				RQ.push_back(node->entries[i]->ptr);
		}
		else
		{
			long long* r1 = new long long[num_piv];
			long long* r2 = new long long[num_piv];
			double count = 1;
			for(long long i =0;i<num_piv;i++)
			{
				if(qmin[i]<min[i])
					r1[i]=min[i];
				else
					r1[i]=qmin[i];
				if(r1[i]<0)
					r1[i]=0;
				if(qmax[i]<max[i])
					r2[i]=qmax[i];
				else
					r2[i]=max[i];
				if(r2[i]>MAXINT)
					r2[i]= MAXINT;
				count = count * (r2[i]-r1[i]+1);
			}

			if(count<node->num_entries)
			{
				vector<long long> S;

				for(long long i = r1[0];i<=r2[0];i++)
				{
					S.push_back(i);
				}

				vector<vector<long long>> SS;
				vector<vector<long long>> SS_t;
				long long ** ss = new long long* [(long long)count];
				for(long long i =0;i<count;i++)
					ss[i]= new long long[num_piv];

				for(long long i =0;i<S.size();i++)
				{
					vector<long long> s;
					s.push_back(S[i]);
					SS.push_back(s);
				}

				for(long long k =0;k<num_piv-1;k++)
				{
					for(long long i =0;i<SS.size();i++)
					{
						vector<long long> s = SS[i];
						for(long long j =r1[k+1];j<=r2[k+1];j++)
						{
							s.push_back(j);
							SS_t.push_back(s);
							s.pop_back();
						}
					}
					SS.clear();
					SS = SS_t;
					SS_t.clear();
				}

				vector<IntArray> Z;
				bool f = true;
				for(long long i=0;i<count;i++)
				{
					f = true;
					for(long long j =0;j<num_piv;j++)
					{
						ss[i][j]=SS[i][j];
						if (ss[i][j] <= tpdists[j])
						{
							f = false;
						}
					}
					IntArray ia;
					ia.f = f;
					unsigned long long *key = new unsigned long long [num_piv];
					Hconvert(key, (unsigned long long *) ss[i], num_piv);
					ia.key = new unsigned long long[bplus->keysize];
					for(long long j =0;j<bplus->keysize;j++)
						ia.key[j] = key[num_piv-bplus->keysize+j];
					ia.keysize = bplus->keysize;
					delete[] key;
					Z.push_back(ia);
				}
				S.clear();
				SS.clear();
				sort(Z.begin(),Z.end());
				long long j =0;
				for(long long i =0;i<count&&j<node->num_entries;)
				{
					long long temp = node->compareint(Z[i].key,node->entries[j]->key,bplus->keysize);
					if(temp==0)
					{
						if (Z[i].f)
							result->push_back(node->entries[j]->ptr);
						else
							RQ.push_back(node->entries[j]->ptr);
						j++;
					}
					else if(temp<0)
					{
						i++;
					}
					else
					{
						j++;
					}
				}

				for(long long i =0;i<Z.size();i++)
					delete[] Z[i].key;
				Z.clear();
				for(long long i=0;i<count;i++)
					delete[] ss[i];
				delete[] ss;
				delete[] r1;
				delete[] r2;

			}
			else
			{
				for (long long i = 0;i < node->num_entries;i++)
				{
					unsigned long long * key = new unsigned long long[num_piv];
					unsigned long long * temp = new unsigned long long[num_piv];
					for (long long j = 0;j < bplus->keysize;j++)
					{
						temp[j] = 0;
						key[j + num_piv - bplus->keysize] = node->entries[i]->key[j];
					}
					for (long long j = bplus->keysize;j < num_piv;j++)
					{
						temp[j] = 0;
						key[j - bplus->keysize] = 0;
					}
					R_Hconvert(temp, key, num_piv);
					bool flag = false;
					for (long long x = 0;x < num_piv;x++)
					{
						if (temp[x] - tpdists[x] <0.0000001)
						{
							flag = true;
							RQ.push_back(node->entries[i]->ptr);
							break;
						}
						if (temp[x]<qmin[x] || temp[x]>qmax[x])
						{
							flag = true;
							break;
						}
					}
				  if(!flag)
					  result->push_back(node->entries[i]->ptr);
					delete[] temp;
					delete[] key;
				}
			}
		}
	}
}

long long PB_Tree::ImprovedRQ_new(Object * q, double radius)
{
	long long * dists = new long long[num_piv];
	tpdists= new double[num_piv];
	vector<long long> tp;
	for(long long i =0;i<num_piv;i++)
	{
		tpdists[i] = q->distance(ptable[i]);

		if(eps>0)
			dists[i] = (long long) floor(tpdists[i]/eps);
		else
			dists[i] = (long long) tpdists[i];

		tpdists[i] = radius - tpdists[i];
		if (eps>0 && tpdists[i]>0)
			tpdists[i] = floor(tpdists[i] / eps) - 1;
	}

	long long t_rad;
	if(eps>0)
		t_rad = (long long) ceil(radius/eps) + 1;
	else
		t_rad = (long long) radius;

	long long* qmin = new long long[num_piv];
	long long* qmax = new long long[num_piv];

	for(long long i =0;i<num_piv;i++)
	{
		qmin[i] = dists[i] - t_rad;

		if(qmin[i]<0)
			qmin[i]=0;
		qmax[i] = dists[i] + t_rad;
		if(qmax[i]>MAXINT)
			qmax[i]=MAXINT;
	}

	bplus->load_root();
	vector<long long> * result = new vector<long long>;
	RQ.clear();
	//*********************************************************Space Filling curve**************************
	RRQH_new(bplus->root_ptr,result, qmin, qmax, NULL, NULL, false, false, dists, t_rad);
	delete[] qmin;
	delete[] qmax;
	delete[] dists;


	Object * o= NULL;
	for(long long j =0;j<result->size();j++)
	{
		o = getobject((*result)[j]);
		if(q->distance(*o)-radius<0.000001)
		{
			RQ.push_back(o->id);
		}
		delete o;
	}
	result->clear();
	delete result;
	delete[] tpdists;
	tpdists = NULL;
	delete bplus->root_ptr;
	bplus->root_ptr=NULL;

	return RQ.size();
}


Object* PB_Tree::getobject(long long ptr)
{
	long long i = ptr/draf->file->blocklength  ;
	char * buffer = new char[draf->file->blocklength];

	if(c==NULL)
	{
		draf->file->read_block(buffer,i);
	}
	else
	{
		c->read_block(buffer,i,draf);
	}

	long long j = ptr- i*draf->file->blocklength;
	Object* o = new Object();
	o->read_from_buffer(&buffer[j]);

	delete[] buffer;

	return o;
}
