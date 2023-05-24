/*********************************************************************
*                                                                    *
* Copyright (c)                                                      *
* ZJU-DBL, Zhejiang University, Hangzhou, China                      *
*                                                                    *
* All Rights Reserved.                                               *
*                                                                    *
* Permission to use, copy, and distribute this software and its      *
* documentation for NON-COMMERCIAL purposes and without fee is       *
* hereby granted provided  that this copyright notice appears in     *
* all copies.                                                        *
*                                                                    *
* THE AUTHORS MAKE NO REPRESENTATIONS OR WARRANTIES ABOUT THE        *
* SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING  *
* BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,      *
* FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. THE AUTHOR  *
* SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A      *
* RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS    *
* DERIVATIVES.                                                       *
*                                                                    *
*********************************************************************/

#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <vector>
#include <sys/stat.h>
using namespace std;

#include "./blockfile/blk_file.h"
#include "./blockfile/cache.h"
#include "./gadget/gadget.h"
#include "./heap/binheap.h"
#include "./heap/heap.h"
#include "./rand/rand.h"
#include "./object.h"
#include "./RAF.h"
#include "./pb-tree.h"
#include <string>
double compdists = 0;
double IOread = 0;
double IOwrite = 0;
double cc = 0;
int MAXINT, BITS;
double EPS, MAXDIST,IOtime;
int bolcksize;
int dim, num_obj, func;

vector<vector<double>> read(string name);

// Note that the id of data in the file should begin with 0
Object ** readobjects(string filename, int num_obj, int dim)
{
	int record_count = 0;
	Object ** objset = new Object*[num_obj];
	for (int i = 0;i<num_obj;i++)
	{
		objset[i] = new Object();
	}

    vector<vector<double>> dataset=read(filename);
	{
		while (record_count<num_obj)
		{
			float d;
			objset[record_count]->id = record_count;
			objset[record_count]->size = dim;
			objset[record_count]->data = new float[objset[record_count]->size];
			for (int i = 0; i < objset[record_count]->size; i++)
			{
				objset[record_count]->data[i] = dataset[record_count][i];
			}
			record_count++;
		}
	}

	return objset;
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

Object * readobjects2(string filename)
{
	int record_count = 0;
	{
	    vector<vector<double>> dataset=read(filename);
	    num_obj=dataset.size(),dim=dataset[0].size();
	    func=2;
		Object * objset = new Object[num_obj];
		while (record_count < num_obj)
		{
			float d;
			objset[record_count].id = record_count;
			objset[record_count].size = dim;
			objset[record_count].data = new float[dim];
			for (int i = 0; i < dim; i++)
			{
				objset[record_count].data[i] = dataset[record_count][i];
			}
			record_count++;
		}
		return objset;
	}

}

clock_t bulidIndex(char* index_name, string filename, char*pname,int n_p)
{
	/*******
	index_name -- the path of the index to be stored
	filename -- the path of the dataset
	pname -- the path of the pivot file
	n_p -- the number of pivots
	*******/

	//pivot selection
	PB_Tree * pb = new PB_Tree();

	//parameter settings
	pb->eps = EPS;
	pb->num_cand = 40; // the number of candidates for selecting pivots (if we read the pivots from pivot file, it is useless here)
	pb->num_piv = n_p;
	pb->bits = BITS;
	int keysize;
	Object * os = readobjects2(filename); // the function needed to be rewirte to read differnt formats of the dataset

	pb->readptable(pname);

	IOread = IOwrite = compdists = 0;
	clock_t begin;

	begin = clock();
	pb->H_bulkload(os, num_obj);
	keysize = os[0].keysize;
	delete[] os;
	cout << "keysize:" << keysize << endl;
	pb->bplus = new B_Tree();
	pb->bplus->init(index_name, bolcksize, NULL, keysize);
	pb->bplus->bulkload("bulkload.txt");

	////build RAF
	pb->bplus->load_root();
	B_Node * node = pb->bplus->root_ptr;
	B_Node * temp;

	while (node->level != 0)
	{
		node = node->entries[0]->get_son();
	}
	int * obj_order = new int[num_obj];
	int k = 0;
	for (int i = 0;i<node->num_entries;i++)
	{
		obj_order[k] = node->entries[i]->son;

		k++;
	}
	temp = node->get_right_sibling();
	//delete node;
	while (temp != NULL)
	{

		node = temp;
		for (int i = 0;i<node->num_entries;i++)
		{
			obj_order[k] = node->entries[i]->son;
			k++;
		}
		temp = node->get_right_sibling();
		delete node;
	}

	pb->draf = new RAF();
	pb->draf->num_obj = num_obj;
	pb->draf->init(index_name, bolcksize, NULL);

	Object ** objS = readobjects(filename, num_obj, dim);
	int * result = pb->draf->buid_from_array(objS, obj_order);

	//delete object sets
	for (int i = 0;i<num_obj;i++)
		delete objS[i];
	delete[] obj_order;

	node = pb->bplus->root_ptr;


	while (node->level != 0)
	{
		node = node->entries[0]->get_son();
	}

	k = 0;
	for (int i = 0;i<node->num_entries;i++)
	{
		node->entries[i]->ptr = result[k];
		k++;
	}
	char * buffer = new char[bolcksize];
	node->write_to_buffer(buffer);

	pb->bplus->file->write_block(buffer, node->block);
	delete buffer;

	temp = node->get_right_sibling();
	//delete node;
	while (temp != NULL)
	{
		buffer = new char[bolcksize];
		node = temp;
		for (int i = 0;i<node->num_entries;i++)
		{
			node->entries[i]->ptr = result[k];
			k++;
		}
		node->write_to_buffer(buffer);

		pb->bplus->file->write_block(buffer, node->block);

		delete buffer;
		temp = node->get_right_sibling();
		delete node;
	}
	delete[] result;

	pb->bplus->close();
	return begin;
}

#define ARRAYLENGTH 100



int * build_MBR(B_Node * node, PB_Tree *pb)
{
	int* minp = new int[pb->num_piv];
	int* maxp = new int[pb->num_piv];
	for (int i = 0;i<pb->num_piv;i++)
	{
		minp[i] = MAXINT;
		maxp[i] = 0;
	}

	if (node->level == 0)
	{
		for (int i = 0;i<node->num_entries;i++)
		{
			int* t = pb->R_Zconvert(node->entries[i]->key);
			for (int j = 0;j<pb->num_piv;j++)
			{
				if (t[j]<minp[j])
					minp[j] = t[j];
				if (t[j]>maxp[j])
					maxp[j] = t[j];
			}
			delete[] t;
		}

	}
	else
	{
		for (int i = 0;i<node->num_entries;i++)
		{
			int* t = build_MBR(node->entries[i]->get_son(), pb);
			node->entries[i]->del_son();
			int * t1 = new int[pb->num_piv];
			for (int j = 0;j<pb->num_piv;j++)
			{
				t1[j] = t[pb->num_piv + j];
			}
			for (int j = 0;j<pb->num_piv;j++)
			{
				if (t[j]<minp[j])
					minp[j] = t[j];
				if (t1[j]>maxp[j])
					maxp[j] = t1[j];
			}

			node->entries[i]->min = pb->Zconvert(t);
			node->entries[i]->max = pb->Zconvert(t1);

			delete[]  t;
			delete[] t1;
		}

		char * buffer = new char[bolcksize];
		node->write_to_buffer(buffer);

		pb->bplus->file->write_block(buffer, node->block);
		delete buffer;


	}
	int * temp = new int[2 * pb->num_piv];
	for (int i = 0;i<pb->num_piv;i++)
	{
		temp[i] = minp[i];
		temp[pb->num_piv + i] = maxp[i];
	}

	delete[] minp;
	delete[] maxp;
	return temp;
}

int * H_build_MBR(B_Node * node, PB_Tree *pb)
{
	int* minp = new int[pb->num_piv];
	int* maxp = new int[pb->num_piv];
	for (int i = 0;i<pb->num_piv;i++)
	{
		minp[i] = MAXINT;
		maxp[i] = 0;
	}

	if (node->level == 0)
	{
		for (int i = 0;i<node->num_entries;i++)
		{

			unsigned * key = new unsigned[pb->num_piv];
			unsigned * t = new unsigned[pb->num_piv];
			for (int j = 0;j<pb->bplus->keysize;j++)
			{
				t[j] = 0;
				key[pb->num_piv + j - pb->bplus->keysize] = node->entries[i]->key[j];
			}
			for (int j = pb->bplus->keysize;j<pb->num_piv;j++)
			{
				t[j] = 0;
				key[j - pb->bplus->keysize] = 0;
			}
			pb->R_Hconvert(t, key, pb->num_piv);

			for (int j = 0;j<pb->num_piv;j++)
			{
				if (t[j]<minp[j])
					minp[j] = t[j];
				if (t[j]>maxp[j])
					maxp[j] = t[j];
			}
			delete[] t;
			delete[] key;
		}

	}
	else
	{
		for (int i = 0;i<node->num_entries;i++)
		{
			int* t = H_build_MBR(node->entries[i]->get_son(), pb);
			node->entries[i]->del_son();
			int * t1 = new int[pb->num_piv];
			for (int j = 0;j<pb->num_piv;j++)
			{
				t1[j] = t[pb->num_piv + j];
			}
			for (int j = 0;j<pb->num_piv;j++)
			{
				if (t[j]<minp[j])
					minp[j] = t[j];
				if (t1[j]>maxp[j])
					maxp[j] = t1[j];
			}

			unsigned * mi = new unsigned[pb->num_piv];
			unsigned * ma = new unsigned[pb->num_piv];
			pb->Hconvert(mi, (unsigned*)t, pb->num_piv);
			pb->Hconvert(ma, (unsigned*)t1, pb->num_piv);
			node->entries[i]->min = new unsigned[pb->bplus->keysize];
			node->entries[i]->max = new unsigned[pb->bplus->keysize];
			for (int j = 0;j<pb->bplus->keysize;j++)
			{
				node->entries[i]->min[j] = mi[j + pb->num_piv - pb->bplus->keysize];
				node->entries[i]->max[j] = ma[j + pb->num_piv - pb->bplus->keysize];
			}

			delete[] mi;
			delete[] ma;
			delete[]  t;
			delete[] t1;
		}

		char * buffer = new char[bolcksize];
		node->write_to_buffer(buffer);

		pb->bplus->file->write_block(buffer, node->block);
		delete buffer;

	}

	int * temp = new int[2 * pb->num_piv];
	for (int i = 0;i<pb->num_piv;i++)
	{
		temp[i] = minp[i];
		temp[pb->num_piv + i] = maxp[i];
	}

	delete[] minp;
	delete[] maxp;
	return temp;
}

int main(int argc, char** argv)
{

    ofstream ouf("./contrast_experiment_record.csv",ios::app);
    if(!ouf.is_open())
    {
        cout<<"open ./contrast_experiment_record.csv failed\n";
        exit(-1);
    }

	//******************************build the index***********
	clock_t begin, buildEnd, queryEnd;
	double buildComp, queryComp;
	struct stat sdata1;
	struct stat sdata2;

	int buffer_size = 32;
    string dataid=string(argv[1]);
    string queryid=string(argv[2]);
    string datafile="../data_set/"+dataid+"/"+dataid+"_afterpca.csv";
	char *pname = "./pivot.txt";// the path of input pivots
	char * indexfile = "./index";// the path to store the built index
	MAXDIST = 1e20;// the maximum distance for the input dataset
	int pn = stoi(argv[3]);// the number of pivots
	bolcksize = stoi(argv[4]);	// the page size
	vector<vector<double>> queryset=read("../data_set/"+dataid+"/"+dataid+"_"+queryid+"_afterpca.csv");
    int qcount=stoi(argv[5]);
	EPS = MAXDIST / 1000;
	MAXINT = (MAXDIST / EPS);
	BITS = ((int)log2(MAXINT) + 1); //  the bits to represent space filling curve values

	compdists = 0;
	IOread = IOwrite = 0;
	begin = bulidIndex(indexfile, datafile, pname,pn);

	PB_Tree * pb = new PB_Tree();

	Cache* c = new Cache(buffer_size, bolcksize);
	pb->c = c;

	pb->num_piv = pn;
	pb->readptable(pname);
	pb->eps = EPS;
	pb->bits = BITS;

	pb->bplus = new B_Tree();
	pb->bplus->init_restore(indexfile, NULL);
	pb->bplus->load_root();
	H_build_MBR(pb->bplus->root_ptr, pb);
	buildEnd = clock() - begin;
	buildComp = compdists;

	char * bfile = new char[strlen(indexfile) + 2];
	strcpy(bfile, indexfile);
	strcat(bfile, ".b");
	char * raffile = new char[strlen(indexfile) + 4];
	strcpy(raffile, indexfile);
	strcat(raffile, ".raf");
	stat(bfile, &sdata1);
	stat(raffile, &sdata2);
	//************end of build index*************************

	//************************ similarity searh***********************

	pb->draf = new RAF();
	pb->draf->init_restore(indexfile, c);


	Object * q = new Object();
	double io = 0;
	double dists = 0;
	q->size = dim;   // the dimension of the query object
	q->data = new float[q->size];
	double rad;
	cout << "start rangeSearching......" << endl;
	for (int k = 0; k < qcount; ++k) {
        double r=stod(argv[6+k]);
		begin = clock();
		IOread = IOwrite = IOtime = 0;
		dists = 0;
		rad = 0;
		double pf = 0;
		for (int j = 0;j < queryset.size(); j++)
		{

			c->clear();
			compdists = 0;

			for (int i = 0;i < q->size;i++)
			{
				q->data[i]=queryset[j][i];
			}

			rad += pb->ImprovedRQ_new(q, r);  // range query function

			pf += c->page_faults;
			dists += compdists;
		}
		queryEnd = clock() - begin;
		queryComp = dists;

		ouf<<dataid<<","<<queryid<<",SPB-Tree,"<<"pn:"<<pn<<" blocksize:"<<bolcksize<<","<<buildEnd<<"ms,"<<"MB,"<<r<<","<<rad<<","
                <<queryComp<<","<<1.0*queryEnd/queryset.size()<<"ms,"<<IOtime/queryset.size()<<"ms,"
                <<(queryEnd-IOtime)/queryset.size()<<"ms"<<endl;
        cout<<dataid<<","<<queryid<<",SPB-Tree,"<<"pn:"<<pn<<" blocksize:"<<bolcksize<<","<<buildEnd<<"ms,"<<"MB,"<<r<<","<<rad<<","
                <<queryComp<<","<<1.0*queryEnd/queryset.size()<<"ms,"<<IOtime/queryset.size()<<"ms,"
                <<(queryEnd-IOtime)/queryset.size()<<"ms"<<endl;
	}


    {
		begin = clock();
		IOread = IOwrite = 0;
		dists = 0;
		rad = 0;
		double pf = 0;
		for (int j = 0;j < queryset.size(); j++)
		{

			c->clear();
			compdists = 0;

			for (int i = 0;i < q->size;i++)
			{
				q->data[i]=queryset[j][i];
			}

			rad = pb->BFkNN(q, 1); //kNN query function

            c->clear();

			pf += c->page_faults;
			dists += compdists;
		}
		queryEnd = clock() - begin;
		queryComp = dists;


    }

    delete q;
    q = NULL;
    pb = NULL;

	//***********************end of similarity search*******************
	return 0;
}
