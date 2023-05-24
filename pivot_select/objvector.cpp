
#include <math.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <vector>
using namespace std;

class Objvector{
public:
    vector<double> value;
    Objvector() {}
	Objvector(const Objvector &obj);
	Objvector(vector<double> v);
	Objvector & operator = (const Objvector & obj);
	vector<double> & getValue();

	void setValue(vector<double> v);
};

Objvector::Objvector(const Objvector & obj)
{
	value = obj.value;
}

Objvector::Objvector(vector<double> v)
{
	value = v;
	//dim = d;
}

Objvector & Objvector::operator=(const Objvector & obj)
{
	// TODO: insert return statement here
	if (this == &obj) {
		return *this;
	}
	else {
		value = obj.value;
		//dim = obj.dim;
	}
}

vector<double> & Objvector::getValue()
{
	return value;
}

//int Objvector::getDim()
//{
//	return dim;
//}

void Objvector::setValue(vector<double> v)
{
	value = v;
}


typedef struct sEuclDB
{
	double *nums;  /* coords all together */
	int nnums;	  /* number of vectors (with space for one more) */
	int coords;  /* coordinates */
	double(*df) (double *p1, double *p2, int k); /* distance to use */
	double *nobj;
} EuclDB;

static int never = 1;
static EuclDB DB;

double mydistance(int i,int j)
{
    double res=0;
    for(int k=0;k<DB.coords;k++)
        res+=(DB.nums[i*DB.coords+k]-DB.nums[j*DB.coords+k])*
            (DB.nums[i*DB.coords+k]-DB.nums[j*DB.coords+k]);
    return sqrt(res);
}

#define db(p) (DB.nums + DB.coords*(int)p)

/* L2 distance */

static double L2D(double *p1, double *p2, int k)
{
	register int i;
	double tot = 0, dif;
	for (i = 0; i < k; i++)
	{
		dif = (p1[i] - p2[i]);
		tot += dif*dif;
	}
	return sqrt(tot);
}

static double L5D(double *p1, double *p2, int k)
{
	register int i;
	double tot = 0, dif;
	for (i = 0; i < k; i++)
	{
		dif = fabs(p1[i] - p2[i]);
		tot += pow(dif, 5);
	}
	return pow(tot, 0.2);
}

static double L1D(double *p1, double *p2, int k)
{
	register int i;
	double tot = 0, dif;
	for (i = 0; i < k; i++)
	{
		dif = (p1[i] - p2[i]);
		if (dif < 0) dif = -dif;
		tot += dif;
	}
	return tot;
}

static double LiD(double *p1, double *p2, int k)
{
	register int i;
	double max = 0, dif;
	for (i = 0; i < k; i++)
	{
		dif = (p1[i] - p2[i]);
		if (dif < 0) dif = -dif;
		if (dif > max) max = dif;
	}
	return max;
}

vector<vector<double>> read(string name)
{
    ifstream inf(name,ios::in);
    if(!inf.is_open())
    {
        cout<<"open "<<name<<" failed\n";
        exit(-1);
    }
    vector<vector<double>> res;
    string tmp;
    while(getline(inf,tmp))
    {
        vector<double> vc;
        string x="";
        for(char ch:tmp)
        {
            if(ch==',')
            {
                if(x!="") vc.push_back(stod(x));
                x="";
            }else x.push_back(ch);
        }
        if(x!="") vc.push_back(stod(x));
        res.push_back(vc);
    }
    return res;
}

int openDB(string name)
{
	cout << name << endl;
	int dim, num, func;
	vector<vector<double>> dataset=read(name);
	dim=dataset[0].size();
	num=dataset.size();
	func=2;
	if (func == 1) DB.df = L1D;
	else if (func == 2) DB.df = L2D;
	else if (func == 5) DB.df = L5D;
	else DB.df = LiD;
	DB.coords = dim;
	DB.nnums = num;
	DB.nums = (double *)malloc((DB.nnums + 1) * sizeof(double)* DB.coords);
	DB.nobj = (double *)malloc(DB.coords * sizeof(double));

	for (int i = 0; i < DB.nnums; ++i)
	{
		for (int j = 0; j < DB.coords; ++j)
		{
			DB.nums[i*DB.coords + j]=dataset[i][j];
		}
//		fscanf(f, "\n");

	}

	return DB.nnums;
}

void closeDB(void)

{
	if (never) { DB.nums = NULL; never = 0; }
	if (DB.nums == NULL) return;
	free(DB.nums);
	DB.nums = NULL;
}

