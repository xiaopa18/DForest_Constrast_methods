//
// Created by YANGHANYU on 8/21/15.
//

#include "main.h"
#include "Objvector.h"
#include "Interpreter.h"
#include "Tuple.h"
#include "Cache.h"
#include "./monitoring.hpp"
#define MAX(x, y) ((x>y)? (x): (y))
#include <iostream>
#include<chrono>
using namespace std;
using namespace std::chrono;
// parse raw data
Interpreter* pi;
// each line pivId sequence
int **pivotSeq;
// pivId set
set<int> pivotSet;
// Tuple 2-dim array
Tuple ** gb;
float* DB;
Tuple* G;
// random sequence generation array
int * sequence;

double compDists = 0;

int LGroup;
float* nobj;
#ifdef RESULT
ofstream ret_range("ret_range.txt", ios::out);
#endif
double  recyc_times;
// calculate L2 distance between two data
double calculateDistance(int v1, int v2)
{
	compDists++;
	return pi->df(DB + pi->dim * v1, DB + pi->dim * v2, pi->dim);
}

double calculateDistance(Objvector o1, int v2)
{
	for (int d = 0; d < pi->dim; ++d) {
		nobj[d] = o1.getValue()[d];
	}
	compDists++;

	return pi->df(nobj, DB + pi->dim * v2, pi->dim);
}

double calculateDistance(float* o1, int v2)
{
	compDists++;
	return pi->df(o1, DB + pi->dim * v2, pi->dim);
}

// calculate average L2 distance between each data in datalist and given pivot
double calculateMiu(int pid)
{
	double miu = 0;
	for (int i = 0; i < pi->num; i++) {
		miu += calculateDistance(i, pid);
	}
	return miu / pi->num;
}


// provide non-repeated pivot selecting
void getNonRepeatedRandomNum(int * &rs, int nums)
{
	int end = pi->num;
	int num;
	for (int i = 0; i < nums; i++) {
		num = rand() % end;
		rs[i] = sequence[num];
		sequence[num] = sequence[end - 1]; // move the last value to the selected position
		end--; // decrease random range by one
	}
	sort(rs, rs + nums);
}

// calculate variance of X(distance between data and their pivot)
double calculateVx2(int group)
{
	double mean = 0, vx2 = 0;
	// calculate mean first
	for (int i = 0; i < pi->num; i++)
	{
		mean += gb[group][i].getDistance();
	}
	mean = mean / pi->num;
	for (int j = 0; j < pi->num; j++) {
		vx2 += pow((gb[group][j].getDistance() - mean), 2);
	}
	return vx2 / pi->num;
}

double calculateVy2()
{
	double mean = 0, vy2 = 0;
	int sampleSize = pi->num / 100;
	int * sample = (int *)malloc(sizeof(int) * sampleSize);
	// sampling
	getNonRepeatedRandomNum(sample, sampleSize);

	// calculate mean first
	int count = 0;
	for (int i = 0; i < sampleSize; i++) {
		for (int j = i + 1; j < sampleSize; j++) {
			mean += calculateDistance(sample[i], sample[j]);
			++count;
		}

	}
	mean = mean / count;
	for (int i = 0; i < sampleSize; i++) {
		for (int j = i + 1; j < sampleSize; j++) {
			vy2 += pow((calculateDistance(sample[i], sample[j]) - mean), 2);
		}
	}
	free(sample);
	return vy2 / count;
}

double calculateR2()
{
	double radius = 100;
	return radius;
}


void algorithm1(int group)
{

	double dis;
	double miu;

	cout << "algorithm1 start." << endl;

	// randomly select one pivot
	// add to pivot set
	pivotSet.insert(pivotSeq[group][0]);

	// initialize g array
	for (int i = 0; i < pi->num; i++) {
		Tuple t;
		t.setPivId(pivotSeq[group][0]);
		t.setObjId(i);
		t.setDistance(calculateDistance(i, pivotSeq[group][0]));
		gb[group][i] = t;
	}

	for (int i = 1; i < MPivots; i++) {
		// randomly select one pivot
		// add to pivot set
		pivotSet.insert(pivotSeq[group][i]);

		// calculate miu value
		miu = calculateMiu(pivotSeq[group][i]);

		for (int j = 0; j < pi->num; j++) {
			// update g array
			dis = calculateDistance(j, pivotSeq[group][i]);
			if (fabs(dis - miu) > fabs(gb[group][j].getDistance() - miu)) {
				Tuple t;
				t.setPivId(pivotSeq[group][i]);
				t.setObjId(j);
				t.setDistance(dis);
				gb[group][j] = t;
			}
		}
	}

	cout << "algorithm1 end." << endl;
}

void algorithm2(int group)
{

	double dis;
	double miu;
	double cost, prev;
	double vx2 = 0, vy2 = 0, r2 = 0;
	int m;


	cout << "algorithm2 start." << endl;

	// estimate vy^2 and r^2
	vy2 = calculateVy2();
	r2 = calculateR2();

	// randomly select one pivot
	// add to pivot set
	pivotSet.insert(pivotSeq[group][0]);

	// initialize g array
	for (int i = 0; i < pi->num; i++) {
		Tuple t;
		t.setPivId(pivotSeq[group][0]);
		t.setObjId(i);
		t.setDistance(calculateDistance(i, pivotSeq[group][0]));
		gb[group][i] = t;
	}
	m = 1;

	vx2 = calculateVx2(group); // calculate variance of X

	cost = m*LGroup + pi->num*pow((1 - (vx2 + vy2) / r2), LGroup); // calculate cost

	while (true) {
		// randomly select one pivot
		pivotSet.insert(pivotSeq[group][m]); // add to pivot set

											 // calculate miu value
		miu = calculateMiu(pivotSeq[group][m]);

		for (int j = 0; j < pi->num; j++) {
			// update g array
			dis = calculateDistance(j, pivotSeq[group][m]);
			if (fabs(dis - miu) > fabs(gb[group][j].getDistance() - miu)) {
				Tuple t;
				t.setPivId(pivotSeq[group][m]);
				t.setDistance(dis);
				gb[group][j] = t;
			}
		}
		m += 1; // increment m by one
		vx2 = calculateVx2(group); // update variance of X
		prev = cost; // record old cost for comparison
		cost = m*LGroup + pi->num*pow((1 - (vx2 + vy2) / r2), LGroup); // update cost
		if (cost >= prev) {
			break;
		}
	}

	cout << "algorithm2 end." << endl;
}

#define MAXREAL 1e20
#define num_cand 40
int * cand;
bool *ispivot;
double** O_P_matrix;
vector<int> samples_objs;

//find num_cand pivots in O using maxpruning strategy
double** MaxPrunning(int num)
{
	cand = new int[num_cand];
	bool * indicator = new bool[num];
	for (int i = 0;i<num;i++)
		indicator[i] = true;
	int * idset = new int[num_cand];

	double d = 0.0;
	double t;
	int choose = 0;

	double** distmatrix = new double*[num];
	for (int i = 0;i<num;i++)
	{
		distmatrix[i] = new double[num_cand];
		for (int j = 0;j<num_cand;j++)
			distmatrix[i][j] = 0;
	}


	if (num_cand> 0)
	{

		for (int i = 1;i<num;i++)
		{
			t = calculateDistance(samples_objs[i], samples_objs[0]);
			if (t>d)
			{
				d = t;
				choose = i;
			}
		}

		idset[0] = choose;
		cand[0] = choose;
		indicator[choose] = false;

	}


	if (num_cand>1)
	{
		d = 0;
		for (int i = 0;i<num;i++)
		{
			if (indicator[i])
			{
				distmatrix[i][0] = calculateDistance(samples_objs[cand[0]], samples_objs[i]);
				if (distmatrix[i][0]>d)
				{
					d = distmatrix[i][0];
					choose = i;
				}
			}
		}

		idset[1] = choose;
		cand[1] = choose;
		indicator[choose] = false;

	}


	double edge = d;
	d = MAXREAL;
	for (int i = 2;i<num_cand;i++)
	{
		d = MAXREAL;
		for (int j = 0;j<num;j++)
		{
			if (indicator[j])
			{
				t = 0;
				for (int k = 0;k<i - 1;k++)
				{
					t += fabs(edge - distmatrix[j][k]);
				}
				distmatrix[j][i - 1] = calculateDistance(samples_objs[j], samples_objs[cand[i - 1]]);
				t += fabs(edge - distmatrix[j][i - 1]);
				if (t<d)
				{
					d = t;
					choose = j;
				}
			}
		}

		idset[i] = choose;

		indicator[choose] = false;
		cand[i] = choose;
	}




	for (int i = 0;i<num;i++)
	{
		if (indicator[i])
		{
			distmatrix[i][num_cand - 1] = calculateDistance(samples_objs[i], samples_objs[cand[num_cand - 1]]);
		}
	}

	for (int i = 0;i<num_cand;i++)
		for (int j = i + 1;j<num_cand;j++)
			distmatrix[idset[i]][j] = calculateDistance(samples_objs[idset[i]], samples_objs[cand[j]]);

	delete[] indicator;
	delete[] idset;

	return distmatrix;
}

int PivotSelect(int objId, int o_num, int q_num, int pivotNum)
{

	double ** Q_O_matrix = new double *[q_num];
	double ** Q_P_matrix = new double*[q_num];
	double ** esti = new double*[q_num];
	for (int i = 0;i<q_num;i++)
	{
		Q_O_matrix[i] = new double[o_num];
		Q_P_matrix[i] = new double[num_cand];
		esti[i] = new double[o_num];
	}
	bool* indicator = new bool[num_cand];
	for (int i = 0;i<num_cand;i++)
		indicator[i] = true;


	for (int i = 0;i<q_num;i++)
	{
		for (int j = 0;j<o_num;j++)
		{
			Q_O_matrix[i][j] = calculateDistance(objId, samples_objs[j]);
			esti[i][j] = 0;
		}
		for (int j = 0;j<num_cand;j++)
		{
			Q_P_matrix[i][j] = calculateDistance(objId, samples_objs[cand[j]]);
		}
	}

	double d = 0;
	double t = 0;
	int choose;

	int i;
	for (i = 0;i<pivotNum; i++)
	{
		choose = -1;
		for (int j = 0;j<num_cand;j++)
		{
			if (indicator[j])
			{
				t = 0;
				for (int m = 0;m < q_num;m++)
				{
					for (int n = 0;n <o_num;n++)
					{
						if (Q_O_matrix[m][n] != 0)
						{

							t += (MAX(fabs(Q_P_matrix[m][j] - O_P_matrix[n][j]), esti[m][n])) / Q_O_matrix[m][n];
						}
					}
				}
				t = t / (q_num*o_num);
				if (t>d)
				{
					choose = j;
					d = t;
				}
			}
		}

		if (choose == -1)
			break;
		indicator[choose] = false;
		if (!ispivot[choose])
			ispivot[choose] = true;
		G[objId * pivotNum + i].setObjId(objId);
		G[objId * pivotNum + i].setPivId(choose);
		G[objId * pivotNum + i].setDistance(Q_P_matrix[0][choose]);
		for (int m = 0;m<q_num;m++)
			for (int n = 0;n<o_num;n++)
				esti[m][n] = MAX(fabs(Q_P_matrix[m][choose] - O_P_matrix[n][choose]), esti[m][n]);

	}

	if (i < pivotNum)
	{
		for(int j =0;j<num_cand;j++)
			if (indicator[j])
			{
				indicator[j] = false;

				G[objId * pivotNum + i].setObjId(objId);
				G[objId * pivotNum + i].setPivId(j);
				G[objId * pivotNum + i].setDistance(Q_P_matrix[0][j]);

				if (!ispivot[j])
					ispivot[j] = true;
				i++;
				if (i == pivotNum)
					break;
			}

	}

	for (i = 0;i<q_num;i++)
	{
		delete[] esti[i];
		delete[] Q_P_matrix[i];
		delete[] Q_O_matrix[i];
	}
	delete[] indicator;
	return pivotNum;
}

void sample_data(int nums, int objs_num)
{
	set<int> samples;

	int id;
	while (samples.size() < nums)
	{
		id = rand() % objs_num;
		if (samples.find(id) == samples.end())
		{
			samples.insert(id);
		}
	}
	id = 0;
	for (set<int>::iterator it = samples.begin(); it != samples.end(); ++it)
	{
		samples_objs.push_back(id + rand() % 200);
		id += 200;
	}
}

bool readIndex(Tuple* t, char* fname);

void algorithm3()
{
	int sampleSize = pi->num / 200;
	sample_data(sampleSize, pi->num);
	ispivot = new bool[num_cand];
	for (int i = 0; i < num_cand; i++)
		ispivot[i] = false;
	O_P_matrix = MaxPrunning(sampleSize);
	G = (Tuple*)malloc(sizeof(Tuple) * pi->num * LGroup);
    for (int i = 0; i < pi->num; i++) {
        // initialize random pivot selecting sequence
        //g[i] = (Tuple*)malloc(sizeof(Tuple) * LGroup);

        PivotSelect(i, sampleSize, 1, LGroup);
    }
	/*
	for (int i = 0; i < pi->num; i++) {
		// initialize random pivot selecting sequence
		g[i] = (Tuple*)malloc(sizeof(Tuple) * LGroup);

		PivotSelect(i, sampleSize, 1, LGroup);
		if (i % 10000 == 0)
		{
			cout << i << endl;
		}
	}
	*/
	for (int i = 0;i<sampleSize;i++)
		delete[] O_P_matrix[i];
}


//int EP_rangeQuery(Objvector q, double radius, int qj)
//{
//	vector<int> result;
//	double* pivotQueryDist = new double[num_cand];
//	for (int i = 0; i < num_cand; i++) {
//		if (ispivot[i])
//			pivotQueryDist[i] = calculateDistance(q, samples_objs[cand[i]]);
//	}
//
//	double tempdist;
//	bool next;
//	for (int i = 0; i < pi->num; i++) {
//		next = false;
//		for (int j = 0; j < LGroup; j++) {
//			if (fabs(pivotQueryDist[g[i][j].getPivId()] - g[i][j].getDistance()) > radius) {
//				next = true;
//				break;
//			}
//		}
//		if (!next) {
//			tempdist = calculateDistance(q, i);
//			if (tempdist <= radius) {
//				result.push_back(i);
//			}
//		}
//	}
//
//	delete[] pivotQueryDist;
//	return result.size();
//}
//
//double EP_kNNQuery(Objvector q, int k)
//{
//	vector<SortEntry> result;
//	double* pivotQueryDist = new double[num_cand];
//	//memset(pivotQueryDist, 0, sizeof(double) * num_cand);
//	for (int i = 0; i < num_cand;i++) {
//		if (ispivot[i])
//		{
//			pivotQueryDist[i] = calculateDistance(q, samples_objs[cand[i]]);
//		}
//	}
//
//	double kdist = numeric_limits<double>::max();
//	double tempdist;
//	bool next;
//	int j;
//
//	for (int i = 0; i < pi->num; i++) {
//		next = false;
//		for (j = 0; j < LGroup; j++) {
//			recyc_times++;
//			if (fabs(pivotQueryDist[g[i][j].getPivId()] - g[i][j].getDistance()) > kdist) {
//				next = true;
//				break;
//			}
//		}
//		if (!next) {
//			tempdist = calculateDistance(q, i);
//			if (tempdist <= kdist) {
//				SortEntry s;
//				s.dist = tempdist;
//				result.push_back(s);
//				sort(result.begin(), result.end());
//				if (result.size() > k) {
//					result.pop_back();
//				}
//				if (result.size() >= k)
//					kdist = result.back().dist;
//			}
//		}
//	}
//	//delete[] pivotQueryDist;
//	free(pivotQueryDist);
//	return kdist;
//}

double KNNQuery(float* q, int k)
{
	vector<SortEntry> result;
	double pivotsquery[num_cand];
	for (int i = 0; i < num_cand; ++i)
	{
		if (ispivot[i])
		{
			pivotsquery[i] = calculateDistance(q, samples_objs[cand[i]]);
		}
	}
	double kdist = 1e20;
	bool next = false;
	double tempdist;
	int pos = -LGroup;
	for (int i = 0; i < pi->num; ++i)
	{
		next = false;
		pos += LGroup;
		for (int j = 0; j < LGroup; ++j)
		{
			if (fabs(pivotsquery[G[pos+j].pivId] - G[pos+j].distance) > kdist)
			{
				next = true;
				break;
			}
		}
		if (!next)
		{
			tempdist = calculateDistance(q, i);
			if (tempdist <= kdist)
			{
				SortEntry s;
				s.id=i;
				s.dist = tempdist;
				result.push_back(s);
				sort(result.begin(), result.end());
				if (result.size() > k) {
					result.pop_back();
				}
				if (result.size() >= k)
					kdist = result.back().dist;
			}
		}
	}

	return kdist;
}

int rangeQuery(float* q, double radius, int qj)
{
	int result;
	double pivotsquery[num_cand];
	for (int i = 0; i < num_cand; ++i)
	{
		if (ispivot[i])
		{
			pivotsquery[i] = calculateDistance(q, samples_objs[cand[i]]);
		}
	}
	bool next = false;
	result = 0;
	int pos = -LGroup;
	for (int i = 0; i < pi->num;  ++i)
	{
		next = false;
		pos += LGroup;
		for (int j = 0; j < LGroup; ++j)
		{
			if (fabs(pivotsquery[G[pos+j].pivId] - G[pos+j].distance) > radius)
			{
				next = true;
				break;
			}
		}
		if (!next)
		{
			if (calculateDistance(q, i) <= radius)
			{
				result++;
			}
		}
	}

	return result;
}

void saveIndex(Tuple * t, char *fname)
{
	long int j, i;
	FILE *fp;

	if ((fp = fopen(fname, "wb")) == NULL)
	{
		fprintf(stderr, "Error opening output file\n");
		exit(-1);
	}
	int pid, oid;
	double d;
	for (i = 0; i < pi->num; i++) {
		for (j = 0; j < LGroup; j++) {
			pid = t[i*LGroup + j].getPivId();
			oid = t[i * LGroup + j].getObjId();
			d = t[i * LGroup + j].getDistance();
			fwrite(&pid, sizeof(int), 1, fp);
			fwrite(&oid, sizeof(int), 1, fp);
			fwrite(&d, sizeof(double), 1, fp);
		}
	}
	fclose(fp);
}


bool readIndex(Tuple* t, char* fname)
{
	long int j, i;
	FILE* fp;

	if ((fp = fopen(fname, "rb")) == NULL)
	{
		//fprintf(stderr, "Error opening output file\n");
		return false;
	}
	int pid, oid;
	double d;
	for (i = 0; i < pi->num; i++) {
		//t[i] = (Tuple*)malloc(sizeof(Tuple) * LGroup);
		for (j = 0; j < LGroup; j++) {
			fread(&pid, sizeof(int), 1, fp);
			fread(&oid, sizeof(int), 1, fp);
			fread(&d, sizeof(double), 1, fp);
			t[i * LGroup + j].setPivId(pid);
			t[i * LGroup + j].setObjId(oid);
			t[i * LGroup + j].setDistance(d);
			ispivot[pid] = true;
		}
	}
	fclose(fp);

	return true;
}

int main(int argc, char **argv)
{
	double begin, buildEnd, queryEnd;
	double buildComp, queryComp;
    int current_pid = GetCurrentPid();

	srand((unsigned int)time(NULL)); // randomize seed

	string dataid=string(argv[1]);
	string queryid=string(argv[2]);

    int pn = atoi(argv[3]);
	//parse raw data into structural data

    vector<vector<double>> queryset;
    ifstream inf("../../data_set/"+dataid+"/"+dataid+"_"+queryid+"_afterpca.csv",ios::in);
    if(!inf.is_open())
    {
        cout<<"open "<<"../data_set/"+dataid+"/"+dataid+"_"+queryid+"_afterpca.csv"<<" failed"<<endl;
        exit(-1);
    }
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
        queryset.push_back(vc);
    }
    inf.close();
    cout<<"read queryset over"<<endl;
	{
	    auto tist=steady_clock::now(),tied=steady_clock::now();
	    auto mem_use=-GetMemoryUsage(current_pid);
	    pi = new Interpreter();
		string fn=("../../data_set/"+dataid+"/"+dataid+"_afterpca.csv");
        pi->parseRawData(fn.c_str());
        cout<<"read dataset over"<<endl;
		LGroup = pn;
		tist=steady_clock::now();
		compDists = 0;
		// build EPT
		algorithm3();
		tied=steady_clock::now();
		buildEnd=duration_cast<milliseconds>(tied - tist).count();
		cout<<"builde EPT over"<<endl;
        mem_use+=GetMemoryUsage(current_pid);
		buildComp = compDists;

        int querycount=atoi(argv[4]);
        int knnconut=atoi(argv[5+querycount]);

        ofstream ouf("./record/"+dataid+"_rangequery.csv",ios::app);
        if(!ouf.is_open())
        {
            cout<<"open ./record/" + dataid + "_rangequery.csv" + "failed\n";
            exit(-1);
        }
        ouf.setf(ios::fixed);
		float* obj = new float[pi->dim];
		memset(obj, 0, sizeof(obj));
		double rad;
		for (int k = 0; k < querycount; ++k) {
            double r=stod(string(argv[5+k]));
			compDists = 0;
			rad = 0;
			tist=steady_clock::now();
			for (int j = 0; j < queryset.size();j++) {
				for (int x = 0; x < pi->dim; x++)
                {
					obj[x]=queryset[j][x];
					//cout<<obj[x]<<" ";
                }
				int temp = rangeQuery(obj, r, j);
				rad += temp;
			}
			tied=steady_clock::now();
			ouf<<dataid<<","<<queryid<<",EPT,"<<"pn:"<<LGroup<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<r<<","<<rad/queryset.size()<<","
                <<compDists/queryset.size()<<","<<1.0*duration_cast<milliseconds>(tied - tist).count()/queryset.size()<<"ms"<<endl;
            cout<<dataid<<","<<queryid<<",EPT,"<<"pn:"<<LGroup<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<r<<","<<rad/queryset.size()<<","
                <<compDists/queryset.size()<<","<<1.0*duration_cast<milliseconds>(tied - tist).count()/queryset.size()<<"ms"<<endl;
		}
        ouf.close();

        ouf.open("./record/"+dataid+"_knn.csv",ios::app);
        if(!ouf.is_open())
        {
            cout<<"open ./record/" + dataid + "_knn.csv" + "failed\n";
            exit(-1);
        }
        ouf.setf(ios::fixed);
		for(int i=0;i<knnconut;i++)
        {
            int k=stoi(argv[6+querycount+i]);
            compDists = 0;
			rad=0;
            tist=steady_clock::now();
			for (int j = 0; j < queryset.size();j++) {
				for (int x = 0; x < pi->dim; x++)
                {
					obj[x]=queryset[j][x];
					//cout<<obj[x]<<" ";
                }
                rad+=KNNQuery(obj,k);
			}
			tied=steady_clock::now();
			ouf<<dataid<<","<<queryid<<",EPT,"<<"pn:"<<LGroup<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<k<<","<<rad/queryset.size()<<","
                <<compDists/queryset.size()<<","<<1.0*duration_cast<milliseconds>(tied - tist).count()/queryset.size()<<"ms"<<endl;
            cout<<dataid<<","<<queryid<<",EPT,"<<"pn:"<<LGroup<<","<<buildEnd<<"ms,"<<mem_use<<"MB,"<<k<<","<<rad/queryset.size()<<","
                <<compDists/queryset.size()<<","<<1.0*duration_cast<milliseconds>(tied - tist).count()/queryset.size()<<"ms"<<endl;
        }
        ouf.close();
		//free(g);
		free(cand);

		delete[] DB;
		delete[] nobj;
		//g = NULL;
		DB = NULL;
	}
	return 0;
}
