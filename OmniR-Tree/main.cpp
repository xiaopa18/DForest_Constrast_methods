#include <math.h>
#include <time.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include "./rtree/rtree.h"
#include "./rtree/rtnode.h"
#include "./rtree/entry.h"
#include "./blockfile/blk_file.h"
#include "./blockfile/cache.h"
#include "./linlist/linlist.h"
#include <time.h>
#include "object.h"
#include <fstream>
#include <iostream>
#include "RAF.h"
#include <stack>
#include <vector>
#include<queue>
#include "heap\heap.h"

#include "./rtree/rtree.cpp"
#include "./rtree/rtnode.cpp"
#include "./rtree/entry.cpp"
#include "./blockfile/blk_file.cpp"
#include "./blockfile/cache.cpp"
#include "./linlist/linlist.cpp"
#include "object.cpp"
#include "RAF.cpp"
#include "heap/heap.cpp"
using namespace std;

int dimension;
extern int DIMENSION;
int DataNum;
int func;
int pn;
double IOread;
double IOwrite;
double IOtime;
double compdists;
int BLOCK_SIZE = 4096;

Object * ptable;
Object * cand;
RAF* draf;
RTree *rt;
Cache * c;

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

// Note that the id of data in the file should begin with 0
Object ** readobjects(string filename)
{
	int record_count = 0;
	Object ** objset = new Object*[DataNum];
	for (int i = 0;i<DataNum;i++)
	{
		objset[i] = new Object();
	}

    vector<vector<double>> dataset=read(filename);
	{
		while (record_count<DataNum)
		{
			float d;
			objset[record_count]->id = record_count;
			objset[record_count]->size = dimension;
			objset[record_count]->x = new float[objset[record_count]->size];
			for (int i = 0; i < objset[record_count]->size; i++)
			{
				objset[record_count]->x[i] = dataset[record_count][i];
			}
			record_count++;
		}
	}

	return objset;
}

Object * readobjects2(string filename)
{
	int record_count = 0;
	{
	    vector<vector<double>> dataset=read(filename);
	    DataNum=dataset.size(),dimension=dataset[0].size();
	    func=2;
		Object * objset = new Object[DataNum];
		while (record_count < DataNum)
		{
			float d;
			objset[record_count].id = record_count;
			objset[record_count].size = dimension;
			objset[record_count].x = new float[dimension];
			for (int i = 0; i < dimension; i++)
			{
				objset[record_count].x[i] = dataset[record_count][i];
			}
			record_count++;
		}
		return objset;
	}

}


double** MaxPrunning(Object * O, int num)
{
	int num_cand = 40;
	cand = new Object[num_cand];
	bool * indicator = new bool[num];
	for (int i = 0; i < num; i++)
		indicator[i] = true;
	int * idset = new int[num_cand];

	double d = 0.0;
	double t;
	int choose = 0;

	double** distmatrix = new double*[num];
	for (int i = 0; i < num; i++)
	{
		distmatrix[i] = new double[num_cand];
		for (int j = 0; j < num_cand; j++)
			distmatrix[i][j] = 0;
	}

	if (num_cand > 0)
	{

		for (int i = 1; i < num; i++)
		{
			t = O[i].distance(O[0]);
			if (t > d)
			{
				d = t;
				choose = i;
			}
		}

		idset[0] = choose;
		cand[0] = O[choose];
		indicator[choose] = false;
	}


	if (num_cand > 1)
	{
		d = 0;
		for (int i = 0; i < num; i++)
		{
			if (indicator[i])
			{
				distmatrix[i][0] = cand[0].distance(O[i]);
				if (distmatrix[i][0] > d)
				{
					d = distmatrix[i][0];
					choose = i;
				}
			}
		}

		idset[1] = choose;
		cand[1] = O[choose];
		indicator[choose] = false;

	}


	double edge = d;
	d = MAXREAL;
	for (int i = 2; i < num_cand; i++)
	{
		d = MAXREAL;
		for (int j = 0; j < num; j++)
		{
			if (indicator[j])
			{
				t = 0;
				for (int k = 0; k < i - 1; k++)
				{
					t += fabs(edge - distmatrix[j][k]);
				}
				distmatrix[j][i - 1] = O[j].distance(cand[i - 1]);
				t += fabs(edge - distmatrix[j][i - 1]);
				if (t < d)
				{
					d = t;
					choose = j;
				}
			}
		}

		idset[i] = choose;

		indicator[choose] = false;
		cand[i] = O[choose];
	}


	//print distance matrix

	for (int i = 0; i < num; i++)
	{
		if (indicator[i])
		{
			distmatrix[i][num_cand - 1] = O[i].distance(cand[num_cand - 1]);
		}
	}

	for (int i = 0; i < num_cand; i++)
		for (int j = i + 1; j < num_cand; j++)
			distmatrix[idset[i]][j] = O[idset[i]].distance(cand[j]);

	delete[] indicator;
	delete[] idset;

	return distmatrix;
}

void readptable(char *pname)
{
	ifstream in1(pname,ios::in);
	ptable = new Object[pn];
	for (int i = 0; i < pn; i++)
	{
		in1 >> ptable[i].id;
		in1 >> dimension;
		ptable[i].size = dimension;
		ptable[i].x = new float[dimension];
		for (int j = 0; j < dimension; j++)
		{
			in1 >> ptable[i].x[j];
		}
	}
	in1.close();
}

void PivotSelect(Object * O, Object * Q, int o_num, int q_num)
{
	int num_cand = 40;
	double ** O_P_matrix = MaxPrunning(O, o_num);

	double ** Q_O_matrix = new double *[q_num];
	double ** Q_P_matrix = new double*[q_num];
	double ** esti = new double*[q_num];
	for (int i = 0; i < q_num; i++)
	{
		Q_O_matrix[i] = new double[o_num];
		Q_P_matrix[i] = new double[num_cand];
		esti[i] = new double[o_num];
	}
	bool* indicator = new bool[num_cand];
	for (int i = 0; i < num_cand; i++)
		indicator[i] = true;

	for (int i = 0; i < q_num; i++)
	{
		for (int j = 0; j < o_num; j++)
		{
			Q_O_matrix[i][j] = Q[i].distance(O[j]);
			esti[i][j] = 0;
		}
		for (int j = 0; j < num_cand; j++)
		{
			Q_P_matrix[i][j] = Q[i].distance(cand[j]);
		}
	}


	double d = 0;
	double t = 0;
	int choose;
	ptable = new Object[pn];
	int i;
	for (i = 0; i < pn; i++)
	{
		choose = -1;
		for (int j = 0; j < num_cand; j++)
		{
			if (indicator[j])
			{
				t = 0;
				for (int m = 0; m < q_num; m++)
				{
					for (int n = 0; n < o_num; n++)
					{
						if (Q_O_matrix[m][n] != 0)
						{
							t += (MAX(fabs(Q_P_matrix[m][j] - O_P_matrix[n][j]), esti[m][n])) / Q_O_matrix[m][n];
						}
					}
				}
				t = t / (q_num*o_num);
				if (t > d)
				{
					choose = j;
					d = t;
				}
			}
		}
		printf("%d %f\n", choose, d);
		if (choose == -1)
			break;
		indicator[choose] = false;
		ptable[i] = cand[choose];
		for (int m = 0; m < q_num; m++)
			for (int n = 0; n < o_num; n++)
				esti[m][n] = MAX(fabs(Q_P_matrix[m][choose] - O_P_matrix[n][choose]), esti[m][n]);
	}

	if (i < pn)
	{
		pn = i;
	}

	for (i = 0; i < q_num; i++)
	{
		delete[] esti[i];
		delete[] Q_P_matrix[i];
		delete[] Q_O_matrix[i];
	}
	delete[] indicator;
	for (i = 0; i < o_num; i++)
		delete[] O_P_matrix[i];
}

clock_t bulidIndex(char* index_name, string filename,char *pname, int n_p)
{
	/*******
	index_name -- the path of the index to be stored
	filename -- the path of the dataset
	pname -- the path of pivot file
	n_p -- the number of pivots
	*******/

	//pivot selection
	Object * os = readobjects2(filename); // the function needed to be rewirte to read differnt formats of the dataset
	readptable(pname);
    //PivotSelect(os,os,DataNum/100,300);

	IOread = IOwrite = compdists = 0;

	clock_t begin;
	begin = clock();
	char bl[] = "bulkload.txt";
	FILE* f = fopen(bl, "w+");
	if (f != NULL)
	{
		for (int i = 0; i < DataNum; i++)
		{
			fprintf(f, "%d ", os[i].id);
			for (int j = 0; j < pn; j++)
			{
				fprintf(f, "%f ", os[i].distance(ptable[j]));
			}
			fprintf(f, "\n");
		}
	}
	fclose(f);
	delete[] os;

	c = new Cache(32, BLOCK_SIZE);
 	rt = new RTree("bulkload.txt", index_name, BLOCK_SIZE, c, pn);

	stack<RTNode*> s;
	// build RAF
	rt->load_root();
	s.push(rt->root_ptr);
	RTNode * node;
	int * obj_order = new int[DataNum];
	int k = 0;
	while (!s.empty())
	{
		node = s.top();
		s.pop();
		if (node->level != 0)
		{
			for (int i = node->num_entries - 1; i >= 0; i--)
			{
				s.push(node->entries[i].get_son());
				node->entries[i].son_ptr = NULL;
			}
		}
		else
		{
			for (int i = 0; i < node->num_entries; i++)
			{
				obj_order[k] = node->entries[i].son;
				k++;
			}
		}
		delete node;
	}

	rt->root_ptr = NULL;

	draf = new RAF();
	draf->num_obj = DataNum;
	draf->init(index_name, BLOCK_SIZE, NULL);

	Object ** objS = readobjects(filename);
	int * result = draf->buid_from_array(objS, obj_order);

	//delete object sets
	for (int i = 0; i < DataNum; i++)
		delete objS[i];
	delete[] obj_order;

	rt->load_root();
	s.push(rt->root_ptr);
	k = 0;
	while (!s.empty())
	{
		node = s.top();
		s.pop();
		if (node->level != 0)
		{
			for (int i = node->num_entries - 1; i >= 0; i--)
			{
				s.push(node->entries[i].get_son());
				node->entries[i].son_ptr = NULL;
			}
		}
		else
		{
			for (int i = 0; i < node->num_entries; i++)
			{
				node->entries[i].ptr = result[k];
				k++;
			}
			char * buffer = new char[BLOCK_SIZE];
			node->write_to_buffer(buffer);
			rt->file->write_block(buffer, node->block);
			delete[] buffer;
		}
		delete node;
	}
	rt->root_ptr = NULL;
	delete[] result;
	return begin;
}


float MINDIST(float *p, float *bounces, int dim)
{
	float summe = 0.0;
	float r;
	int i;

	for (i = 0; i < dim; i++)
	{
		if (p[i] < bounces[2 * i])
			r = bounces[2 * i] - p[i];
		else
		{
			if (p[i] > bounces[2 * i + 1])
				r = p[i] - bounces[2 * i + 1];
			else
				r = 0;
		}

		if (r > summe)
			summe = r;
	}
	return(summe);
}

Object* getobject(int ptr)
{
	int i = ptr / draf->file->blocklength;
	char * buffer = new char[draf->file->blocklength];

	if (c == NULL)
	{
		draf->file->read_block(buffer, i);
	}
	else
	{
		c->read_block(buffer, i, draf);
	}

	int j = ptr - i*draf->file->blocklength;
	Object* o = new Object();
	o->read_from_buffer(&buffer[j]);

	delete[] buffer;

	return o;
}

bool intersect(float *p, float *bounces, int dim, double rad)
{
	float r;
	int i;
	for (i = 0; i < dim; i++)
	{
		if (p[i] < bounces[2 * i])
			r = bounces[2 * i] - p[i];
		else
		{
			if (p[i] > bounces[2 * i + 1])
				r = p[i] - bounces[2 * i + 1];
			else
				r = 0;
		}
		if (r > rad)
			return false;
	}
	return true;
}


bool prune(double *ldist, double* udist, float *bounces, int dim)
{
	for (int i = 0; i < dim; i++)
	{
		if (udist[i] < bounces[2 * i])
			return true;
		else if (ldist[i] > bounces[2 * i + 1])
		{
			return true;
		}
	}
	return false;
}


int contain(double* ldist, double* udist, float *bounces, int dim)
{
	int d = 0;
	for (int i = 0; i < dim; i++)
	{
		if (udist[i] < bounces[2 * i])
			return -1;
		else if (ldist[i] > bounces[2 * i + 1])
		{
			return -1;
		}
		else if (ldist[i] <= bounces[2 * i] && udist[i] >= bounces[2 * i + 1])
		{
			d++;
		}
	}
	if (d == dim)
		return 1;
	else
		return 0;
}

double rangequery(Object* q, double radius)
{
	vector<int> result;
	double* point = new double[pn];
	double * ldist = new double[pn];
	double * udist = new double[pn];
	for (int i = 0; i < pn; i++)
	{
		point[i] = q->distance(ptable[i]);
		ldist[i] = point[i] - radius;
		udist[i] = point[i] + radius;
	}

	rt->load_root();
	vector<NEntry> v;
	NEntry n;
	RTNode* node;
	n.node = rt->root_ptr;
	n.flag = false;
	v.push_back(n);
	int d;
	while (!v.empty())
	{
		n = v.back();
		node = n.node;
		v.pop_back();
		if (node->level != 0)
		{
			if (n.flag)
			{
				for (int i = 0; i < node->num_entries; i++)
				{
					NEntry ne;
					ne.node = node->entries[i].get_son();
					ne.flag = true;
					v.push_back(ne);
					node->entries[i].son_ptr = NULL;
				}
			}
			else
			{
				for (int i = 0; i < node->num_entries; i++)
				{
					d = contain(ldist, udist, node->entries[i].bounces, pn);
					if (d>0)
					{
						NEntry ne;
						ne.node = node->entries[i].get_son();
						ne.flag = true;
						v.push_back(ne);
						node->entries[i].son_ptr = NULL;
					}
					else if (d == 0)
					{
						NEntry ne;
						ne.node = node->entries[i].get_son();
						ne.flag = false;
						v.push_back(ne);
						node->entries[i].son_ptr = NULL;
					}
				}
			}
		}
		else
		{
			if (n.flag)
			{
				for (int i = 0; i < node->num_entries; i++)
				{
					Object* o = getobject(node->entries[i].ptr);
					if ( (o->distance(*q) <= radius)) ///0.00001
					{
						result.push_back(o->id);
					}
					delete o;
				}
			}
			else
			{
				for (int i = 0; i < node->num_entries; i++)
				{
					if (!prune(ldist, udist, node->entries[i].bounces, pn))
					{
						Object* o = getobject(node->entries[i].ptr);
						if ( (o->distance(*q) <= radius))
						{
							result.push_back(o->id);
						}
						delete o;
					}
				}
			}
		}
		delete node;
	}
	rt->root_ptr = NULL;
	delete[] point;
	delete[] ldist;
	delete[] udist;
	return result.size();
}

double kNNquery(Object* q, int k)
{
	vector<TEntry> answer;
	double kdist = MAXREAL;

	float* point = new float[pn];
	for (int i = 0; i < pn; i++)
	{
		point[i] = q->distance(ptable[i]);
	}
	rt->load_root();
	priority_queue<KEntry> queue;
	KEntry ke;
	ke.id = rt->root;
	ke.isobject = false;
	ke.key = 0;
	queue.push(ke);
	RTNode* node;
	int ptr;
	while (!queue.empty())
	{
		ke = queue.top();
		queue.pop();
		if (ke.key > kdist)
			break;
		if (ke.isobject)
		{
			Object* o = getobject(ke.id);
			double temp = q->distance(*o);
			if (temp <= kdist)
			{
				TEntry te;
				te.id = o->id;
				te.key = temp;
				answer.push_back(te);
				sort(answer.begin(), answer.end());
				if (answer.size() > k)
					answer.pop_back();
				if (answer.size() >= k &&kdist > answer[k - 1].key)
				{
					kdist = answer[k - 1].key;
				}
			}
			delete o;
		}
		else
		{
			double temp;
			node = new RTNode(rt, ke.id);
			if (node->level != 0)
			{
				for (int i = 0; i < node->num_entries; i++)
				{
					temp = MINDIST(point, node->entries[i].bounces, pn);
					if (temp <= kdist)
					{
						KEntry e;
						e.id = node->entries[i].son;
						e.key = temp;
						e.isobject = false;
						queue.push(e);
					}
				}
			}
			else
			{
				for (int i = 0; i < node->num_entries; i++)
				{
					temp = MINDIST(point, node->entries[i].bounces, pn);
					if (temp <= kdist)
					{
						KEntry e;
						e.id = node->entries[i].ptr;
						e.key = temp;
						e.isobject = true;
						queue.push(e);
					}
				}
			}
			delete node;
		}
	}

	delete[] point;
	return kdist;
}

int main(int argc, char** argv)
{
	clock_t begin, buildEnd, queryEnd;
	double buildComp, queryComp;
    string dataid=string(argv[1]);
    string queryid=string(argv[2]);
	string datafile="../data_set/"+dataid+"/"+dataid+"_afterpca.csv"; // the path of input data file
	char * pname = "./pivot.txt"; // the path of input pivots
	char * indexfile = "./index"; // the path to store the built index
	char* querydata = "./audio/audio_uniform1000_afterpca.txt";// the path of input query data
	pn = stoi(argv[3]);// the number of pivot
    BLOCK_SIZE=stoi(argv[4]);
    vector<vector<double>> queryset=read("../data_set/"+dataid+"/"+dataid+"_"+queryid+"_afterpca.csv");
    int qcount=stoi(argv[5]);
    int kcount=stoi(argv[6+qcount]);
//***********************************************build the index****************************************
	cout<<"start build"<<"\n";
	begin = bulidIndex(indexfile, datafile, pname,pn);

	buildEnd = clock() - begin;
	buildComp = compdists;
	//******************************similarity query***********************************************
	Object* q = new Object();
	q->size = dimension;
	q->x = new float[q->size];
	double io = 0;
	double dists = 0;
	double pf = 0;
	double rad;
	//readptable(pname);
	draf->cache = c;
	rt->cache = NULL;

    ofstream ouf("./record/"+dataid+"_rangequery.csv",ios::app);
    if(!ouf.is_open())
    {
        cout<<"open ./record/" + dataid + "_rangequery.csv" + "failed\n";
        exit(-1);
    }
    ouf.setf(ios::fixed);

	cout << "start rangeSearching......" << endl;
	for (int k = 0; k < qcount; k++) {
		begin = clock();
		double r=stod(argv[6+k]);
		dists = 0;
		pf = 0;
		rad = 0;
		IOread = 0;
		IOtime=0;
		for (int j = 0; j < queryset.size(); j++)
		{
			c->clear();
			compdists = 0;
			float temp;

			for (int i = 0; i < q->size; i++)
			{
				q->x[i] = queryset[j][i];
			}
			rad += rangequery(q,r);
			dists += compdists;
		}
		queryEnd = clock() - begin;
		queryComp = dists;
        cout<<dataid<<","<<queryid<<",OmniR-Tree,"<<"pn:"<<pn<<" blocksize:"<<BLOCK_SIZE<<","<<buildEnd<<"ms,"<<"MB,"<<r<<","<<rad/queryset.size()<<","
                <<queryComp/queryset.size()<<","<<IOread/queryset.size()<<","<<1.0*queryEnd/queryset.size()<<"ms,"<<IOtime/queryset.size()<<"ms,"
                <<(queryEnd-IOtime)/queryset.size()<<"ms"<<endl;
        ouf<<dataid<<","<<queryid<<",OmniR-Tree,"<<"pn:"<<pn<<" blocksize:"<<BLOCK_SIZE<<","<<buildEnd<<"ms,"<<"MB,"<<r<<","<<rad/queryset.size()<<","
                <<queryComp/queryset.size()<<","<<IOread/queryset.size()<<","<<1.0*queryEnd/queryset.size()<<"ms,"<<IOtime/queryset.size()<<"ms,"
                <<(queryEnd-IOtime)/queryset.size()<<"ms"<<endl;
	}
    ouf.close();

    ouf.open("./record/"+dataid+"_knn.csv",ios::app);
    if(!ouf.is_open())
    {
        cout<<"open ./record/" + dataid + "_knn.csv" + "failed\n";
        exit(-1);
    }
    ouf.setf(ios::fixed);
    for(int ki=1;ki<=kcount;ki++)
    {
        int k=stoi(argv[6+qcount+ki]);
        dists = 0;
		pf = 0;
		rad = 0;
		IOread = 0;
		IOtime=0;
		for (int j = 0; j < queryset.size(); j++)
		{
			c->clear();
			compdists = 0;
			float temp;

			for (int i = 0; i < q->size; i++)
			{
				q->x[i] = queryset[j][i];
			}
			rad += kNNquery(q,k);
			dists += compdists;
		}
		queryEnd = clock() - begin;
		queryComp = dists;
        cout<<dataid<<","<<queryid<<",OmniR-Tree,"<<"pn:"<<pn<<" blocksize:"<<BLOCK_SIZE<<","<<buildEnd<<"ms,"<<"MB,"<<k<<","<<rad/queryset.size()<<","
                <<queryComp/queryset.size()<<","<<IOread/queryset.size()<<","<<1.0*queryEnd/queryset.size()<<"ms,"<<IOtime/queryset.size()<<"ms,"
                <<(queryEnd-IOtime)/queryset.size()<<"ms"<<endl;
        ouf<<dataid<<","<<queryid<<",OmniR-Tree,"<<"pn:"<<pn<<" blocksize:"<<BLOCK_SIZE<<","<<buildEnd<<"ms,"<<"MB,"<<k<<","<<rad/queryset.size()<<","
                <<queryComp/queryset.size()<<","<<IOread/queryset.size()<<","<<1.0*queryEnd/queryset.size()<<"ms,"<<IOtime/queryset.size()<<"ms,"
                <<(queryEnd-IOtime)/queryset.size()<<"ms"<<endl;
    }

    ouf.close();

	return 0;
}


