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

#include "./btree/b-node.h"
#include "./btree/b-node.cpp"
#include "./btree/b-entry.h"
#include "./btree/b-entry.cpp"
#include "./btree/b-tree.h"
#include "./btree/b-tree.cpp"
#include "./blockfile/blk_file.h"
#include "./blockfile/blk_file.cpp"
#include "./blockfile/cache.h"
#include "./blockfile/cache.cpp"
#include "./heap/binheap.h"
#include "./heap/binheap.cpp"
#include "./heap/heap.h"
#include "./heap/heap.cpp"
#include "./rand/rand.h"
#include "./rand/rand.cpp"
#include "./object.h"
#include "./object.cpp"
#include "./RAF.h"
#include "./RAF.cpp"
#include "./pb-tree.h"
#include "./pb_tree.cpp"
#include "./gadget/gadget.h"
#include "./gadget/gadget.cpp"
#include <string>
#include"lsb_vector/m-entry.h"
#include"lsb_vector/m-entry.cpp"
double compdists = 0;
double IOread = 0;
double IOwrite = 0;
double cc = 0;
long long MAXINT, MAXNUM, MAXLEVEL;
double EPS, MAXDIST;
long long dim, num_obj, func;
long long blocksize;
double IOtime;

vector<vector<double>> read(string name);

// Note that the id of data in the file should begin with 0
Object ** readobjects(string filename, long long num_obj, long long dim)
{
	long long record_count = 0;
	Object ** objset = new Object*[num_obj];
	for (long long i = 0;i<num_obj;i++)
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
			for (long long i = 0; i < objset[record_count]->size; i++)
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
	long long record_count = 0;
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
			for (long long i = 0; i < dim; i++)
			{
				objset[record_count].data[i] = dataset[record_count][i];
			}
			record_count++;
		}
		return objset;
	}

}



clock_t bulidIndex(char* index_name, string filename,char *pname, long long n_p)
{
	/*******
	index_name -- the path of the index to be stored
	filename -- the path of the dataset
	pname -- the path of pivots
	n_p -- the number of pivots
	*******/

	//pivot selection
	PB_Tree * pb = new PB_Tree();

	//parameter settings
	pb->num_cand = 40; // the number of candidates for selecting pivots
	pb->num_piv = n_p;

	long long keysize;
	Object * os = readobjects2(filename); // the function needed to be rewirte to read differnt formats of the dataset
	pb->readptable(pname);


	IOread = IOwrite = compdists = 0;
	clock_t begin;
	char ptablefile[] = "ptable.txt";
	ofstream out1(ptablefile);
	out1 << pb->num_piv << endl;
	if(pb->num_piv<=MAXLEVEL)
		MAXLEVEL = pb->num_piv - 1;
	for (long long i = 0; i<pb->num_piv; i++)
	{
		out1 << pb->ptable[i].id << " " << pb->ptable[i].size << " ";
		for (long long j = 0; j<pb->ptable[i].size; j++)
			out1 << pb->ptable[i].data[j] << " ";
		out1 << endl;
	}
	out1.close();
	compdists = 0;
	begin = clock();
	pb->bulkload(os, num_obj);

	cout << "builing B+-tree" << endl;
	pb->bplus = new B_Tree();
	pb->bplus->init(index_name, blocksize, NULL, pb->num_piv);
	char bl[] = "bulkload.txt";
	pb->bplus->bulkload("bulkload.txt", os,num_obj);
	delete[] os;

	////build RAF

	cout << "building RAF" << endl;
	pb->bplus->load_root();
	B_Node * node = pb->bplus->root_ptr;
	B_Node * temp;

	while (node->level != 0)
	{
		node = node->entries[0]->get_son();
	}
	long long * obj_order = new long long[num_obj];
	long long k = 0;
	for (long long i = 0; i<node->num_entries; i++)
	{
		obj_order[k] = node->entries[i]->son;
		k++;
	}
	temp = node->get_right_sibling();
	//delete node;
	while (temp != NULL)
	{
		node = temp;
		for (long long i = 0; i<node->num_entries; i++)
		{
			obj_order[k] = node->entries[i]->son;
			k++;
		}
		temp = node->get_right_sibling();
		delete node;
	}

	pb->draf = new RAF();
	pb->draf->num_obj = num_obj;
	pb->draf->init(index_name, blocksize, NULL);

	Object ** objS = readobjects(filename, num_obj, dim);
	long long * result = pb->draf->buid_from_array(objS, obj_order);

	//delete object sets
	for (long long i = 0; i<num_obj; i++)
		delete objS[i];
	delete[] obj_order;

	node = pb->bplus->root_ptr;


	while (node->level != 0)
	{
		node = node->entries[0]->get_son();
	}

	k = 0;
	for (long long i = 0; i<node->num_entries; i++)
	{
		node->entries[i]->ptr = result[k];
		k++;
	}
	char * buffer = new char[blocksize];
	node->write_to_buffer(buffer);

	pb->bplus->file->write_block(buffer, node->block);
	delete buffer;

	temp = node->get_right_sibling();
	//delete node;
	while (temp != NULL)
	{
		buffer = new char[blocksize];
		node = temp;
		for (long long i = 0; i<node->num_entries; i++)
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



signed main(long long argc, char** argv)
{

	clock_t begin, buildEnd, queryEnd;
	double buildComp, queryComp;
	struct stat sdata1;
	struct stat sdata2;
	struct stat sdata3;


	long long buffer_size = 32;
    string dataid=string(argv[1]);
    //string dataid="audio";
    string queryid=string(argv[2]);
    //string queryid="uniform1000";
    string datafile="../../data_set/"+dataid+"/"+dataid+"_afterpca.csv"; // the path of input data file
	char * pname = "./pivot.txt"; // the path of input pivots
	char * indexfile = "./index"; // the path to store the built index

	MAXDIST = 1e15;  // the maximum distance for the input dataset

	EPS = MAXDIST / 1000;
	MAXINT = MAXDIST / EPS;
	MAXNUM = 1600; //  the number of objects in one cluster (can be any value set by the user)


	//long long pvnb = stoi(argv[3]);	// the number of pivots
	long long pvnb = stoi(argv[3]);
	blocksize = stoi(argv[4]); // the page size


	//***********************************************build the index****************************************

	long long pn = pvnb;
	MAXLEVEL = pn - 1; // MAXLEVEL is the height of dynamic cluster tree
	if (pn > 5)
		MAXLEVEL = 5;

	compdists = 0;
	IOread = IOwrite = 0;
	begin = bulidIndex(indexfile, datafile,pname, pn);

	char nodefile[] = "node.b";

	//compute the blocksize
	M_Entry te;
	te.num_piv = pn;
	long long blocklength = sizeof(long long) * 4 + pn*te.get_size();

	PB_Tree * pb = new PB_Tree();
	pb->bf = new BlockFile(nodefile, blocklength);

	Cache* c = new Cache(buffer_size, blocksize);
	Cache* b = new Cache(buffer_size, blocksize);
	pb->c = c;
	pb->num_piv = pn;
	pb->bplus = new B_Tree();
	pb->bplus->init_restore(indexfile, b);
	pb->readptable(pname);
    //cout<<123<<endl;
	buildEnd = clock() - begin;
	buildComp = compdists;
	//cout<<"123123"<<endl;
	//cout<<strlen(indexfile)<<endl;
	char * bfile = new char[strlen(indexfile) + 10];
	strcpy(bfile, indexfile);
	strcat(bfile, ".b");
	char * raffile = new char[strlen(indexfile) + 10];
	//cout<<"123123"<<endl;
	strcpy(raffile, indexfile);
	strcat(raffile, ".raf");
	stat(bfile, &sdata1);
	stat(raffile, &sdata2);
	stat(nodefile, &sdata3);

	pb->draf = new RAF();
	pb->draf->init_restore(indexfile, c);
    cout<<456<<endl;
	//******************************similarity query***********************************************

	Object * q = new Object();
    vector<vector<double>> queryset=read("../../data_set/"+dataid+"/"+dataid+"_"+queryid+"_afterpca.csv");
    long long qcount=stoi(argv[5]);
    long long kcount=stoi(argv[6+qcount]);
	double io = 0;
	double dists = 0;
	double pf = 0;
	q->size = dim;
	q->data = new float[q->size];
	double rad;

    ofstream ouf("./record/"+dataid+"_rangequery.csv",ios::app);
    if(!ouf.is_open())
    {
        cout<<"open ./record/" + dataid + "_rangequery.csv" + "failed\n";
        exit(-1);
    }
    ouf.setf(ios::fixed);

    cout << "start rangeSearching......" << endl;
    //ofstream ouf("./audio/audio_uniform1000_mindex.csv",ios::out);
    for (long long k = 0; k < qcount; k++) {
        IOtime=0.0;
        double r=stod(argv[6+k]);
        begin = clock();
        dists = 0;
        pf = 0;
        rad = 0;

        for (long long j = 0; j < queryset.size(); j++)
        {
            IOread = 0;
            c->clear();
            b->clear();
            compdists = 0;

            float temp;
            for (long long i = 0; i < q->size; i++)
            {
                temp=queryset[j][i];
                q->data[i] = temp / yiwan;
            }

            vector<long long> p=pb->rangequery_orignal(q, r / yiwan);
//            sort(p.begin(),p.end());
//            ouf<<p.size();
//            for(long long &idx:p) ouf<<","<<idx;
//            ouf<<"\n";
            rad += p.size();
            pf += b->page_faults + c->page_faults;
            dists += compdists;
        }
        queryEnd = clock() - begin;
        queryComp = dists;
        ouf<<dataid<<","<<queryid<<",M-index,"<<"pn:"<<pn<<" blocksize:"<<blocksize<<","<<buildEnd/1000.0<<"ms,"<<"MB,"<<r<<","<<rad/queryset.size()<<","
                <<queryComp/queryset.size()<<","<<pf/queryset.size()<<","<<1.0*queryEnd/queryset.size()/1000.0<<"ms,"<<IOtime/queryset.size()/1000.0<<"ms,"
                <<(queryEnd-IOtime)/queryset.size()/1000.0<<"ms"<<endl;
        cout<<dataid<<","<<queryid<<",M-index,"<<"pn:"<<pn<<" blocksize:"<<blocksize<<","<<buildEnd/1000.0<<"ms,"<<"MB,"<<r<<","<<rad/queryset.size()<<","
                <<queryComp/queryset.size()<<","<<pf/queryset.size()<<","<<1.0*queryEnd/queryset.size()/1000.0<<"ms,"<<IOtime/queryset.size()/1000.0<<"ms,"
                <<(queryEnd-IOtime)/queryset.size()/1000.0<<"ms"<<endl;
    }
    ouf.close();

    ouf.open("./record/"+dataid+"_knn.csv",ios::app);
    if(!ouf.is_open())
    {
        cout<<"open ./record/" + dataid + "_knn.csv" + "failed\n";
        exit(-1);
    }
    ouf.setf(ios::fixed);
    cout<<kcount<<"\n";
    for(long long ki=0;ki<kcount;ki++)
    {
        //cout<<"ki:"<<ki<<"\n";
        //cout<<7+qcount+ki<<" "<<argv[7+qcount+ki]<<"\n";
        long long k=stoi(argv[7+qcount+ki]);
        IOtime=0.0;
        begin = clock();
        dists = 0;
        pf = 0;
        rad = 0;
        //cout<<"ki:"<<ki<<"\n\n";
        for (long long j = 0; j < queryset.size(); j++)
        {
            IOread = 0;
            c->clear();
            b->clear();
            compdists = 0;

            float temp;
            for (long long i = 0; i < q->size; i++)
            {
                temp=queryset[j][i];
                q->data[i] = temp / yiwan;
            }
            //cout<<j<<endl;
            rad+=pb->knn(q, k);
            pf += b->page_faults + c->page_faults;
            dists += compdists;
        }
        queryEnd = clock() - begin;
        queryComp = dists;
        ouf<<dataid<<","<<queryid<<",M-index,"<<"pn:"<<pn<<" blocksize:"<<blocksize<<","<<buildEnd/1000.0<<"ms,"<<"MB,"<<k<<","<<rad/queryset.size()<<","
                <<queryComp/queryset.size()<<","<<pf/queryset.size()<<","<<1.0*queryEnd/queryset.size()/1000.0<<"ms,"<<IOtime/queryset.size()/1000.0<<"ms,"
                <<(queryEnd-IOtime)/queryset.size()/1000.0<<"ms"<<endl;
        cout<<dataid<<","<<queryid<<",M-index,"<<"pn:"<<pn<<" blocksize:"<<blocksize<<","<<buildEnd/1000.0<<"ms,"<<"MB,"<<k<<","<<rad/queryset.size()<<","
                <<queryComp/queryset.size()<<","<<pf/queryset.size()<<","<<1.0*queryEnd/queryset.size()/1000.0<<"ms,"<<IOtime/queryset.size()/1000.0<<"ms,"
                <<(queryEnd-IOtime)/queryset.size()/1000.0<<"ms"<<endl;
    }
    ouf.close();

    q = NULL;
    pb = NULL;
	return 0;
}


