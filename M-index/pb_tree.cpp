#include "pb-tree.h"
#include ".\gadget\gadget.h"
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <queue>
#include "lsb_vector\m-entry.h"
#include "lsb_vector\m-node.h"

using namespace std;
extern double cc;
extern double IOread;
extern int MAXNUM;
extern double MAXDIST;

PB_Tree::PB_Tree()
{
	bf = NULL;
	bplus = NULL;
	draf = NULL;
	ptable = NULL;
	cand = NULL;
	c = NULL;
	num_piv = -1;
}

PB_Tree::~PB_Tree()
{
	if (bf != NULL)
		delete bf;
	if (bplus != NULL)
		delete bplus;
	if (draf != NULL)
		delete draf;
	if (ptable != NULL)
		delete[] ptable;
}


//find num_cand pivots in O using FFT
double** PB_Tree::FFT(Object * O, int num)
{
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


	d = 0;
	for (int i = 2; i < num_cand; i++)
	{
		d = 0;
		for (int j = 0; j < num; j++)
		{
			if (indicator[j])
			{
				t = MAXREAL;
				for (int k = 0; k < i - 1; k++)
				{
					if (distmatrix[j][k] < t)
						t = distmatrix[j][k];
				}
				distmatrix[j][i - 1] = O[j].distance(cand[i - 1]);
				if (distmatrix[j][i - 1] < t)
					t = distmatrix[j][i - 1];
				if (t > d)
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


//find num_cand pivots in O using maxpruning strategy
double** PB_Tree::MaxPrunning(Object * O, int num)
{
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

void PB_Tree::PivotSelect(Object * O, Object * Q, int o_num, int q_num, double thred)
{
	vector<int> pivots;

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
	double last = 0;
	int choose;
	ptable = new Object[num_piv];
	int i;
	while (true)
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
		if (choose == -1 || d - last < thred)
			break;
		last = d;
		indicator[choose] = false;
		pivots.push_back(choose);
		for (int m = 0; m < q_num; m++)
			for (int n = 0; n < o_num; n++)
				esti[m][n] = MAX(fabs(Q_P_matrix[m][choose] - O_P_matrix[n][choose]), esti[m][n]);
	}


	int victim;
	int iter = 0;
	while (true)
	{
		d = 1;
		iter++;
		victim = -1;
		for (int i = 0; i < pivots.size(); i++)
		{
			for (int m = 0; m < q_num; m++)
				for (int n = 0; n < o_num; n++)
					esti[m][n] = 0;

			for (int j = 0; j < pivots.size(); j++)
			{
				if (j != i)
				{
					for (int m = 0; m < q_num; m++)
						for (int n = 0; n < o_num; n++)
							esti[m][n] = MAX(fabs(Q_P_matrix[m][pivots[j]] - O_P_matrix[n][pivots[j]]), esti[m][n]);
				}
			}

			t = 0;
			for (int m = 0; m < q_num; m++)
			{
				for (int n = 0; n < o_num; n++)
				{
					if (Q_O_matrix[m][n] != 0)
						t += esti[m][n] / Q_O_matrix[m][n];
				}
			}

			t = t / (q_num*o_num);
			printf("contribution %d %f\n", i, last - t);
			if (last - t < d)
			{
				victim = i;
				d = last - t;
			}
		}

		for (int m = 0; m < q_num; m++)
			for (int n = 0; n < o_num; n++)
				esti[m][n] = 0;

		for (int j = 0; j < pivots.size(); j++)
		{
			if (j != victim)
			{
				for (int m = 0; m < q_num; m++)
					for (int n = 0; n < o_num; n++)
						esti[m][n] = MAX(fabs(Q_P_matrix[m][pivots[j]] - O_P_matrix[n][pivots[j]]), esti[m][n]);
			}
		}

		printf("victim %d %f\n", victim, d);

		choose = -1;
		d = last;
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
		if (choose == -1 || d - last < thred)
			break;
		last = d;
		indicator[choose] = false;
		indicator[pivots[victim]] = true;
		pivots.erase(pivots.begin() + victim);
		pivots.push_back(choose);
	}

	num_piv = pivots.size();
	ptable = new Object[num_piv];
	for (int i = 0; i < num_piv; i++)
		ptable[i] = cand[pivots[i]];

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

void PB_Tree::PivotSelect(Object * O, Object * Q, int o_num, int q_num)
{
	double ** O_P_matrix = MaxPrunning(O, o_num);

	double ** Q_O_matrix = new double *[q_num];
	double ** Q_P_matrix = new double*[q_num];
	double ** esti = new double*[q_num];
	for (int i = 0;i < q_num;i++)
	{
		Q_O_matrix[i] = new double[o_num];
		Q_P_matrix[i] = new double[num_cand];
		esti[i] = new double[o_num];
	}
	bool* indicator = new bool[num_cand];
	for (int i = 0;i < num_cand;i++)
		indicator[i] = true;


	for (int i = 0;i < q_num;i++)
	{
		for (int j = 0;j < o_num;j++)
		{
			Q_O_matrix[i][j] = Q[i].distance(O[j]);
			esti[i][j] = 0;
		}
		for (int j = 0;j < num_cand;j++)
		{
			Q_P_matrix[i][j] = Q[i].distance(cand[j]);
		}
	}


	double d = 0;
	double t = 0;
	int choose;
	ptable = new Object[num_piv];
	int i;
	for (i = 0;i < num_piv;i++)
	{
		choose = -1;
		for (int j = 0;j < num_cand;j++)
		{
			if (indicator[j])
			{
				t = 0;
				for (int m = 0;m < q_num;m++)
				{
					for (int n = 0;n < o_num;n++)
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
		for (int m = 0;m < q_num;m++)
			for (int n = 0;n < o_num;n++)
				esti[m][n] = MAX(fabs(Q_P_matrix[m][choose] - O_P_matrix[n][choose]), esti[m][n]);
	}

	if (i < num_piv)
	{
		num_piv = i;
	}

	for (i = 0;i < q_num;i++)
	{
		delete[] esti[i];
		delete[] Q_P_matrix[i];
		delete[] Q_O_matrix[i];
	}
	delete[] indicator;
	for (i = 0;i < o_num;i++)
		delete[] O_P_matrix[i];
}

void PB_Tree::RPivotSelect(Object * O, Object * Q, int o_num, int q_num)
{
	ptable = new Object[num_piv];
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

	double *** esti_temp = new double **[num_cand];
	for (int i = 0; i < num_cand; i++)
	{
		esti_temp[i] = new double*[q_num];
		for (int j = 0; j < q_num; j++)
			esti_temp[i][j] = new double[o_num];
	}

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

	double score = 0;

	for (int i = 0; i < q_num; i++)
	{
		for (int j = 0; j < o_num; j++)
		{
			for (int k = 0; k < num_cand; k++)
			{
				esti_temp[k][i][j] = fabs(Q_P_matrix[i][k] - O_P_matrix[j][k]);
				if (esti[i][j] < esti_temp[k][i][j])
					esti[i][j] = esti_temp[k][i][j];
			}
			if (Q_O_matrix[i][j] != 0)
				score += esti[i][j] / Q_O_matrix[i][j];
		}
	}
	score = score / (q_num*o_num);
	cout << "score: " << score << endl;

	bool * flag_c = new bool[num_cand];

	for (int i = 0; i < num_cand; i++)
	{
		flag_c[i] = true;
	}

	double * contribution = new double[num_cand];
	double temp = MAXREAL;
	int choose;
	double t;
	for (int i = 0; i < num_cand - num_piv; i++)
	{
		temp = MAXREAL;
		for (int j = 0; j < num_cand; j++)
		{
			if (flag_c[j])
			{
				contribution[j] = 0;
				for (int m = 0; m < q_num; m++)
				{
					for (int n = 0; n < o_num; n++)
					{
						if (esti[m][n] == esti_temp[j][m][n])
						{
							t = 0;
							for (int k = 0; k < num_cand; k++)
							{
								if (flag_c[k] && k != j && esti_temp[k][m][n] > t)
									t = esti_temp[k][m][n];

							}
							if (Q_O_matrix[m][n] != 0)
								contribution[j] += (esti[m][n] - t) / Q_O_matrix[m][n];
						}
					}
				}
				if (contribution[j] < temp)
				{
					temp = contribution[j];
					choose = j;
				}
			}
		}
		flag_c[choose] = false;
		cout << choose << " " << contribution[choose] << endl;
		for (int m = 0; m < q_num; m++)
		{
			for (int n = 0; n < o_num; n++)
			{
				if (esti[m][n] == esti_temp[choose][m][n])
				{
					t = 0;
					for (int k = 0; k < num_cand; k++)
					{
						if (flag_c[k])
							if (esti_temp[k][m][n] > t)
								t = esti_temp[k][m][n];
					}
					esti[m][n] = t;
				}
			}
		}
	}
	score = 0;
	for (int i = 0; i < q_num; i++)
		for (int j = 0; j < o_num; j++)
		{
			if (Q_O_matrix[i][j] != 0)
				score += esti[i][j] / Q_O_matrix[i][j];
		}

	choose = 0;
	for (int i = 0; i < num_cand; i++)
		if (flag_c[i])
		{
			ptable[choose] = cand[i];
			choose++;
		}

	delete[] flag_c;
	delete[] contribution;

	for (int i = 0; i < q_num; i++)
	{
		delete[] esti[i];
		delete[] Q_P_matrix[i];
		delete[] Q_O_matrix[i];
	}
	for (int i = 0; i < o_num; i++)
		delete[] O_P_matrix[i];
	for (int i = 0; i < num_cand; i++)
	{
		for (int j = 0; j < q_num; j++)
			delete[] esti_temp[i][j];
		delete[] esti_temp[i];
	}
	delete[] esti_temp;

}


void PB_Tree::readptable(char* pname)
{
	ifstream in1(pname,ios::in);
	ptable = new Object[num_piv];
	for (int i = 0; i < num_piv; i++)
	{
		in1 >> ptable[i].id >> ptable[i].size;
		ptable[i].data = new float[ptable[i].size];
		for (int j = 0; j < ptable[i].size; j++)
		{
			in1 >> ptable[i].data[j];
		}
	}
	in1.close();
}

void PB_Tree::bulkload(Object * O,  int o_num)
{
	int* power = new int[num_piv];
	for (int i = 0; i < MAXLEVEL; i++)
		power[i] = pow(num_piv, MAXLEVEL - i - 1);
	int** arraysort = new int*[o_num];
	for (int i = 0; i < o_num; i++)
		arraysort[i] = new int[MAXLEVEL];

	vector<TEntry> GH;

	double key = 0;
	for (int i = 0; i < o_num; i++)
	{
		vector<TEntry> H;
		O[i].pd = new double[num_piv];
		for (int j = 0; j < num_piv; j++)
		{
			TEntry te;
			te.id = j;
			O[i].pd[j] = te.key = O[i].distance(ptable[j]);
			H.push_back(te);

		}
		sort(H.begin(), H.end());
		key = 0;
		for (int j = 0; j < MAXLEVEL; j++)
		{
			key += power[j] * (H[j].id + 1);
			arraysort[i][j] = H[j].id;
		}
		key = key*MAXDIST + H[0].key;
		O[i].key = key;
		TEntry t;
		t.id = i;
		t.key = key;
		GH.push_back(t);
		H.clear();
	}

	sort(GH.begin(), GH.end());


	char bl[] = "bulkload.txt";
	FILE* f = fopen(bl, "w+");
	if (f != NULL)
		for (int i = 0; i < o_num; i++)
		{
			fprintf(f, "%d\n", GH[i].id);
		}
	fclose(f);
	int size = (pow(num_piv, MAXLEVEL) - 1)*num_piv / (num_piv - 1) + 1;
	cout << size << endl;
	M_Entry ** entries = new M_Entry*[size];
	for (int i = 0; i < size; i++)
	{
		entries[i] = NULL;
	}
	for (int i = 0; i < o_num; i++)
	{
		int id = 0;
		for (int j = 0; j < MAXLEVEL; j++)
			id += power[j] * (arraysort[GH[i].id][j] + 1);

		if (entries[id] == NULL)
		{
			entries[id] = new M_Entry();
			entries[id]->num_piv = num_piv;
			entries[id]->level = MAXLEVEL;
			entries[id]->max = new double[num_piv];
			entries[id]->min = new double[num_piv];
			for (int j = 0; j < num_piv; j++)
			{
				entries[id]->max[j] = 0;
				entries[id]->min[j] = MAXDIST;
			}
			entries[id]->mkey = GH[i].key;
			entries[id]->mxkey = GH[i].key;
			entries[id]->minkey = GH[i].key - MAXDIST*id;
			entries[id]->maxkey = GH[i].key - MAXDIST*id;
			entries[id]->pivots = new int[MAXLEVEL];
			memcpy(entries[id]->pivots, arraysort[GH[i].id], MAXLEVEL * sizeof(int));
		}
		entries[id]->num++;
		if (entries[id]->maxkey - GH[i].key + MAXDIST*id < -0.000000001)
			entries[id]->maxkey = GH[i].key - MAXDIST*id;
		if (entries[id]->mxkey - GH[i].key < -0.000000001)
			entries[id]->mxkey = GH[i].key;
		if (entries[id]->mkey - GH[i].key  > 0.000000001)
			entries[id]->mkey = GH[i].key;
		if (entries[id]->minkey - GH[i].key + MAXDIST*id  > 0.000000001)
			entries[id]->minkey = GH[i].key - MAXDIST*id;

		for (int j = 0; j < num_piv; j++)
		{
			if (entries[id]->max[j] - O[GH[i].id].pd[j] < -0.000000001)
				entries[id]->max[j] = O[GH[i].id].pd[j];
			if (entries[id]->min[j] - O[GH[i].id].pd[j] > 0.000000001)
				entries[id]->min[j] = O[GH[i].id].pd[j];
		}
	}

	for (int i = size - 1; i > num_piv; i--)
	{
		if (entries[i] == NULL)
			continue;
		int id = 0;
		int t = MAXLEVEL - entries[i]->level + 1;
		for (int j = 0; j < entries[i]->level - 1; j++)
			id += power[j + t] * (entries[i]->pivots[j] + 1);
		if (entries[id] == NULL)
		{
			entries[id] = new M_Entry();
			entries[id]->num_piv = num_piv;
			entries[id]->level = entries[i]->level - 1;
			entries[id]->max = new double[num_piv];
			entries[id]->min = new double[num_piv];
			for (int j = 0; j < num_piv; j++)
			{
				entries[id]->max[j] = 0;
				entries[id]->min[j] = MAXDIST;
			}

			entries[id]->mkey = entries[i]->mkey;
			entries[id]->mxkey = entries[i]->mxkey;
			entries[id]->minkey = entries[i]->minkey;
			entries[id]->maxkey = entries[i]->maxkey;
			entries[id]->pivots = new int[MAXLEVEL];
			memcpy(entries[id]->pivots, entries[i]->pivots, MAXLEVEL * sizeof(int));
		}
		if (entries[id]->mkey - entries[i]->mkey > 0.000000001)
			entries[id]->mkey = entries[i]->mkey;
		if (entries[id]->mxkey - entries[i]->mxkey < -0.000000001)
			entries[id]->mxkey = entries[i]->mxkey;
		if (entries[id]->minkey - entries[i]->minkey > 0.000000001)
			entries[id]->minkey = entries[i]->minkey;
		if (entries[id]->maxkey - entries[i]->maxkey < -0.000000001)
			entries[id]->maxkey = entries[i]->maxkey;
		entries[id]->num += entries[i]->num;
		for (int j = 0; j < num_piv; j++)
		{
			if (entries[id]->max[j] - entries[i]->max[j] < -0.000000001)
				entries[id]->max[j] = entries[i]->max[j];
			if (entries[id]->min[j] - entries[i]->min[j] > 0.000000001)
				entries[id]->min[j] = entries[i]->min[j];
		}
	}

	delete[] power;

	for (int i = size - 1; i > 0;)
	{
		int count = 0;
		for (int j = 0; j < num_piv; j++)
		{
			if (entries[i - j] != NULL)
				count += entries[i - j]->num;
		}
		if (count <= MAXNUM)
		{
			for (int j = 0; j < num_piv; j++)
			{
				if (entries[i - j] != NULL)
				{
					delete entries[i - j];
					entries[i - j] = NULL;
				}
			}
		}
		i -= num_piv;
	}
	M_Entry te;
	te.num_piv = num_piv;
	FILE *fp = fopen("node.b", "r");
	if (fp)
	{
		fclose(fp);
		remove("node.b");
	}
	BlockFile index("node.b", num_piv*te.get_size() + 4 * sizeof(int));

	int n_id = 0;
	for (int i = 1; i < size; )
	{
		int count = 0;
		for (int j = 0; j < num_piv; j++)
		{
			if (entries[i + j] != NULL)
			{
				count++;
			}
		}
		if (count != 0)
		{
			M_Node n;
			n.num_entries = count;
			n.entries = new M_Entry *[count];
			count = 0;
			for (int j = 0; j < num_piv; j++)
			{
				if (entries[i + j] != NULL)
				{
					n.level = entries[i + j]->level - 1;
					n.entries[count] = entries[i + j];
					if (entries[i + j]->num <= MAXNUM || entries[i + j]->level == MAXLEVEL)
					{
						n.entries[count]->ptr = -1;
						n.entries[count]->isleaf = true;
					}
					else
					{
						n_id++;
						n.entries[count]->ptr = n_id;
						n.entries[count]->isleaf = false;
					}
					entries[i + j] = NULL;
					count++;
				}
			}
			char* buf = new char[num_piv*te.get_size() + 4 * sizeof(int)];
			n.write_to_buffer(buf);
			index.append_block(buf);
			delete[] buf;
		}

		i += num_piv;
	}

	delete[] entries;
	for (int i = 0; i < o_num; i++)
		delete[] arraysort[i];
	delete[] arraysort;
}


void clear(vector<B_Node *> * inodes)
{
	for (int i = 0; i < inodes->size(); i++)
	{
		if ((*inodes)[i]->num_entries == 0)
		{
			inodes->erase(inodes->begin() + i);
			i--;
		}
	}
};

double PB_Tree::idist(double * q, double* mi, double* ma)
{
	double dist = 0;
	double temp;
	for (int i = 0; i < num_piv; i++)
	{
		if (q[i] - mi[i]<-0.000000001)
			temp = mi[i] - q[i];
		else if (q[i] - ma[i]>0.000000001)
			temp = q[i] - ma[i];
		else
			temp = 0;
		if (dist - temp < -0.000000001)
			dist = temp;
	}
	return dist;
}



// false: point is not in region [oqmin, oqmax]
// true: point is in region [oqmin, oqmax]
bool PB_Tree::inregion(double* point, double* oqmin, double* oqmax)
{
	for (int i = 0; i < num_piv; i++)
	{
		if (point[i] - oqmin[i]<-0.000000001 || point[i] - oqmax[i]>0.000000001)
			return false;
	}
	return true;
}


int PB_Tree::edist(int * query, int* o)
{
	int ed = 0;
	int temp;
	for (int i = 0; i < num_piv; i++)
	{
		temp = abs(query[i] - o[i]);
		if (temp > ed)
			ed = temp;
	}
	return ed;
}

int PB_Tree::edist(int * query, unsigned * o)
{
	int ed = 0;
	int temp;
	for (int i = 0; i < num_piv; i++)
	{
		temp = o[i];
		temp = abs(query[i] - temp);
		if (temp > ed)
			ed = temp;
	}
	return ed;
}


int PB_Tree::determine(int * min1, int * max1, int* min2, int* max2)
{
	int c1, c2, c3;
	c1 = c2 = c3 = 0;
	for (int i = 0; i < num_piv; i++)
	{
		if (min2[i] >= min1[i] && max2[i] <= max1[i])
			c1++;
		if (max2[i] < min1[i])
			return -1;
		if (min2[i] > max1[i])
			return -1;
	}
	if (c1 == num_piv)
		return 0;
	return 1;
}


Object* PB_Tree::getobject(int ptr)
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

vector<int> PB_Tree::rangequery(Object* q, double r)
{
	q->pd = new double[num_piv];
	double temp = MAXDIST;
	int* nearpivot = new int[MAXLEVEL];
	int* position = new int[num_piv];
	vector<TEntry> ve;
	for (int i = 0; i < num_piv; i++)
	{
		q->pd[i] = q->distance(ptable[i]);
		TEntry t;
		t.id = i;
		t.key = q->pd[i];
		ve.push_back(t);
	}
	sort(ve.begin(), ve.end());
	for (int i = 0; i < MAXLEVEL; i++)
	{
		nearpivot[i] = ve[i].id;
	}
	int count = 0;
	for (int i = 0; i < num_piv; i++)
	{
		position[ve[i].id] = count;
		count++;
	}

	bool* ispurned = new bool[MAXLEVEL];

	queue<M_Node*> queue;
	bplus->load_root();
	M_Node * node = new M_Node();
	node->init(bf, 0);
	queue.push(node);

	vector<int> * candi = new vector<int>;
	vector<int> answer;
	while (!queue.empty())
	{
		node = queue.front();
		queue.front() = NULL;
		queue.pop();

		for (int i = 0; i < node->num_entries; i++)
		{
			int near = 0;
			for (int j = 0; j < MAXLEVEL; j++)
				ispurned[j] = false;
			for (int k = 0; k < node->entries[i]->level - 1; k++)
			{
				ispurned[position[node->entries[i]->pivots[k]]] = true;
			}
			for (int k = 0; k < MAXLEVEL; k++)
			{
				if (!ispurned[k])
				{
					near = k;
					break;
				}
			}

			if (q->pd[node->entries[i]->pivots[node->entries[i]->level - 1]] - q->pd[nearpivot[near]] - 2 * r > 0.000000001)
				continue;

			bool prune = false;
			for (int k = 0; k < num_piv; k++)
			{
				if (q->pd[k] + r - node->entries[i]->min[k] < -0.0000001 || q->pd[k] - r - node->entries[i]->max[k] > 0.000000001)
				{
					prune = true;
					break;
				}
			}

			if (prune)
			{
				continue;
			}

			if (!node->entries[i]->isleaf)
			{
				M_Node* newnode = new M_Node();
				newnode->init(bf, node->entries[i]->ptr);
				queue.push(newnode);
				newnode = NULL;
			}
			else
			{
				bplus->root_ptr->find_interval(node->entries[i]->mkey, node->entries[i]->mxkey, q->pd, r, candi, &answer);
			}
		}
		delete node;
	}

	for (int i = 0; i < candi->size(); i++)
	{
		Object* o = getobject((*candi)[i]);
		if (q->distance(*o) - r < 0.000000001)
		{
			answer.push_back(o->id);
		}
		delete o;
	}

	delete candi;
	delete[] ispurned;
	delete[] position;
	delete[] nearpivot;

	return answer;
}


vector<int> PB_Tree::rangequery_orignal(Object* q, double r)

{
	q->pd = new double[num_piv];
	double temp = MAXDIST;
	int* nearpivot = new int[MAXLEVEL];
	int* position = new int[num_piv];
	vector<TEntry> ve;
	for (int i = 0; i < num_piv; i++)
	{
		q->pd[i] = q->distance(ptable[i]);
		TEntry t;
		t.id = i;
		t.key = q->pd[i];
		ve.push_back(t);
	}
	sort(ve.begin(), ve.end());
	for (int i = 0; i < MAXLEVEL; i++)
	{
		nearpivot[i] = ve[i].id;
	}
	int count = 0;
	for (int i = 0; i < num_piv; i++)
	{
		position[ve[i].id] = count;
		count++;
	}

	bool* ispurned = new bool[MAXLEVEL];

	queue<M_Node*> queue;
	bplus->load_root();
	M_Node * node = new M_Node();
	node->init(bf, 0);
	queue.push(node);

	vector<int> * candi = new vector<int>;
	vector<int> answer;
	while (!queue.empty())
	{
		node = queue.front();
		queue.front() = NULL;
		queue.pop();

		for (int i = 0; i < node->num_entries; i++)
		{
			int near = 0;
			for (int j = 0; j < MAXLEVEL; j++)
				ispurned[j] = false;
			for (int k = 0; k < node->entries[i]->level - 1; k++)
			{
				ispurned[position[node->entries[i]->pivots[k]]] = true;
			}
			for (int k = 0; k < MAXLEVEL; k++)
			{
				if (!ispurned[k])
				{
					near = k;
					break;
				}
			}

			if (q->pd[node->entries[i]->pivots[node->entries[i]->level - 1]] - q->pd[nearpivot[near]] - 2 * r > 0.000000001)
				continue;

			if (q->pd[nearpivot[0]] + r - node->entries[i]->minkey <-0.00000001 || q->pd[nearpivot[0]] - r - node->entries[i]->maxkey>0.000000001)
			{
				continue;
			}

			if (!node->entries[i]->isleaf)
			{
				M_Node* newnode = new M_Node();
				newnode->init(bf, node->entries[i]->ptr);
				queue.push(newnode);
				newnode = NULL;
			}
			else
			{
				bplus->root_ptr->find_interval_orignal(node->entries[i]->mkey, node->entries[i]->mxkey, q->pd, r, candi);
			}
		}
		delete node;
	}

	for (int i = 0; i < candi->size(); i++)
	{
		Object* o = getobject((*candi)[i]);
		if (q->distance(*o) - r < 0.000000001)
		{
			answer.push_back(o->id);
		}
		delete o;
	}

	delete candi;
	delete[] ispurned;
	delete[] position;
	delete[] nearpivot;

	return answer;
}


double PB_Tree::knn(Object * q, int k)
{
	double upper = MAXDIST;
	vector<TEntry> answer;
	q->pd = new double[num_piv];
	double temp = MAXDIST;
	int* nearpivot = new int[MAXLEVEL];
	int* position = new int[num_piv];
	vector<TEntry> ve;
	for (int i = 0; i < num_piv; i++)
	{
		q->pd[i] = q->distance(ptable[i]);
		TEntry t;
		t.id = i;
		t.key = q->pd[i];
		ve.push_back(t);
	}

	sort(ve.begin(), ve.end());
	for (int i = 0; i < MAXLEVEL; i++)
	{
		nearpivot[i] = ve[i].id;
	}

	int count = 0;
	for (int i = 0; i < num_piv; i++)
	{
		position[ve[i].id] = count;
		count++;
	}

	bool* ispurned = new bool[MAXLEVEL];

	bplus->load_root();
	KEntry te;
	te.id = te.key = 0;
	te.isobject = false;
	te.level = 0;
	priority_queue<KEntry> queue;
	queue.push(te);
	while (!queue.empty())
	{
		KEntry e = queue.top();
		queue.pop();
		if (e.key - upper > 0.0000001)
			break;
		if (e.isobject)
		{
			Object* o = getobject(e.id);
			double temp = q->distance(*o);
			if (temp - upper <0.0000001)
			{
				TEntry te;
				te.id = o->id;
				te.key = temp;
				answer.push_back(te);
				sort(answer.begin(), answer.end());
				if (answer.size() > k)
					answer.pop_back();
				if (answer.size() >= k)
				{
					upper = answer[k - 1].key;
				}
			}
			delete o;
			continue;
		}
		M_Node* node = new M_Node();
		node->init(bf, e.id);
		for (int i = 0; i < node->num_entries; i++)
		{
			bool flag = false;
			for (int x = 0; x < node->entries[i]->level; x++)
			{
				int near = 0;
				for (int j = 0; j < MAXLEVEL; j++)
					ispurned[j] = false;

				for (int k = 0; k < x; k++)
				{
					ispurned[position[node->entries[i]->pivots[k]]] = true;
				}

				for (int k = 0; k < x + 1; k++)
				{
					if (!ispurned[k])
					{
						near = k;
						break;
					}
				}

				if (q->pd[node->entries[i]->pivots[x]] - q->pd[nearpivot[near]] - 2 * upper > 0.0000001)
				{
					flag = true;
					break;
				}
			}
			if (flag)
				continue;

			double temp = idist(q->pd, node->entries[i]->min, node->entries[i]->max);
			if (temp - upper > 0.0000001)
				continue;

			if (node->entries[i]->ptr == -1)
			{
				bplus->root_ptr->find_kNN(node->entries[i]->mkey, node->entries[i]->mxkey, q->pd, upper, &queue);
			}
			else
			{
				KEntry te;
				te.id = node->entries[i]->ptr;
				te.key = temp;
				te.isobject = false;
				te.level = node->entries[i]->level;
				te.pivot = position[node->entries[i]->pivots[te.level - 1]];
				queue.push(te);
			}
		}

		delete node;
	}
	delete[] ispurned;
	delete[] position;
	delete[] nearpivot;
	delete[] q->pd;
	q->pd = NULL;
	return answer[k - 1].key;
}

double PB_Tree::knn_optimal(Object * q, int k)
{
	double upper = MAXDIST;
	vector<TEntry> answer;
	q->pd = new double[num_piv];
	double temp = MAXDIST;
	int* nearpivot = new int[MAXLEVEL];
	int* position = new int[num_piv];
	vector<TEntry> ve;
	for (int i = 0; i < num_piv; i++)
	{
		q->pd[i] = q->distance(ptable[i]);
		TEntry t;
		t.id = i;
		t.key = q->pd[i];
		ve.push_back(t);
	}
	sort(ve.begin(), ve.end());
	for (int i = 0; i < MAXLEVEL; i++)
	{
		nearpivot[i] = ve[i].id;
	}
	int count = 0;
	for (int i = 0; i < num_piv; i++)
	{
		position[ve[i].id] = count;
		count++;
	}

	bool* ispurned = new bool[MAXLEVEL];

	vector<TEntry> * candi = new vector<TEntry>();

	bplus->load_root();
	KEntry te;
	te.id = 0;
	te.key = 0;
	te.isobject = false;
	te.level = 0;
	priority_queue<KEntry> queue;
	queue.push(te);
	while (!queue.empty())
	{
		KEntry e = queue.top();
		queue.pop();
		if (te.key - upper >= -0.0000001)
			break;
		if (e.isobject)
		{
			Object* o = getobject(e.id);
			double temp = q->distance(*o);
			if (temp - upper <-0.0000001)
			{
				TEntry te;
				te.id = o->id;
				te.key = temp;
				answer.push_back(te);
				sort(answer.begin(), answer.end());
				if (answer.size() > k)
					answer.pop_back();
				if (answer.size() >= k &&upper - answer[k - 1].key > 0.0000001)
				{
					upper = answer[k - 1].key;
				}
			}
			delete o;
			continue;
		}
		M_Node* node = new M_Node();
		node->init(bf, e.id);
		for (int i = 0; i < node->num_entries; i++)
		{
			bool flag = false;
			for (int x = 0; x < node->entries[i]->level; x++)
			{
				int near = 0;
				for (int j = 0; j < MAXLEVEL; j++)
					ispurned[j] = false;

				for (int k = 0; k < x; k++)
				{
					ispurned[position[node->entries[i]->pivots[k]]] = true;
				}

				for (int k = 0; k < MAXLEVEL; k++)
				{
					if (!ispurned[k])
					{
						near = k;
						break;
					}
				}

				if (q->pd[node->entries[i]->pivots[x]] - q->pd[nearpivot[near]] - 2 * upper > 0.0000001)
				{
					flag = true;
					break;
				}
			}
			if (flag)
				continue;

			double temp = idist(q->pd, node->entries[i]->min, node->entries[i]->max);
			if (temp - upper > 0.0000001)
				continue;

			if (node->entries[i]->ptr == -1)
			{
				candi = new vector<TEntry>();
				bplus->root_ptr->find_kNN_range(node->entries[i]->mkey, node->entries[i]->mxkey, q->pd, upper, candi);
				for (int j = 0; j < candi->size(); j++)
				{
					if ((*candi)[i].key - upper > 0.0000001)
						continue;
					Object* o = getobject((*candi)[j].id);
					double temp = q->distance(*o);
					if (temp - upper < -0.0000001)
					{
						TEntry te;
						te.id = o->id;
						te.key = temp;
						answer.push_back(te);
						sort(answer.begin(), answer.end());
						if (answer.size() > k)
							answer.pop_back();
						if (answer.size() >= k &&upper - answer[k - 1].key > 0.0000001)
						{
							upper = answer[k - 1].key;
						}
					}
					delete o;
				}
				delete candi;
			}
			else
			{
				KEntry te;
				te.id = node->entries[i]->ptr;
				te.key = temp;
				te.isobject = false;
				te.level = node->entries[i]->level;
				te.pivot = position[node->entries[i]->pivots[te.level - 1]];
				queue.push(te);
			}
		}
		delete node;
	}
	return answer[k - 1].key;
}

double PB_Tree::knn_range(Object * q, int k)
{
	double upper = MAXDIST;
	vector<TEntry> answer;
	q->pd = new double[num_piv];
	double temp = MAXDIST;
	int* nearpivot = new int[MAXLEVEL];
	int* position = new int[num_piv];
	vector<TEntry> ve;
	for (int i = 0; i < num_piv; i++)
	{
		q->pd[i] = q->distance(ptable[i]);
		TEntry t;
		t.id = i;
		t.key = q->pd[i];
		ve.push_back(t);
	}
	sort(ve.begin(), ve.end());
	for (int i = 0; i < MAXLEVEL; i++)
	{
		nearpivot[i] = ve[i].id;
	}
	int count = 0;
	for (int i = 0; i < num_piv; i++)
	{
		position[ve[i].id] = count;
		count++;
	}

	bool* ispurned = new bool[MAXLEVEL];

	bplus->load_root();
	KEntry te;
	te.id = te.key = 0;
	te.isobject = false;
	queue<KEntry> queue;
	queue.push(te);
	int access;
	int p;
	vector<TEntry>* candi = new vector<TEntry>();
	while (!queue.empty())
	{
		KEntry e = queue.front();
		queue.pop();
		M_Node* node = new M_Node();
		node->init(bf, e.id);
		int min = num_piv;

		for (int i = 0; i < node->num_entries; i++)
		{
			if (position[node->entries[i]->pivots[node->entries[i]->level - 1]] < min&&node->entries[i]->num >= k)
			{
				min = position[node->entries[i]->pivots[node->entries[i]->level - 1]];
				p = i;
			}
		}
		if (node->entries[p]->ptr == -1)
		{
			access = e.id;
			bplus->root_ptr->find_kNN_range(node->entries[p]->mkey, node->entries[p]->mxkey, q->pd, upper, candi);
		}
		else
		{
			KEntry ke;
			ke.id = node->entries[p]->ptr;
			ke.key = 0;
			queue.push(ke);
		}
	}

	for (int i = 0; i < candi->size(); i++)
	{
		if ((*candi)[i].key - upper > 0.0000001)
			continue;
		Object* o = getobject((*candi)[i].id);
		double temp = q->distance(*o);
		if (temp - upper<-0.0000001)
		{
			TEntry te;
			te.id = o->id;
			te.key = temp;
			answer.push_back(te);
			sort(answer.begin(), answer.end());
			if (answer.size() > k)
				answer.pop_back();
			if (answer.size() >= k &&upper - answer[k - 1].key > 0.0000001)
			{
				upper = answer[k - 1].key;
			}
		}
		delete o;
	}
	delete candi;

	queue.push(te);
	while (!queue.empty())
	{
		KEntry e = queue.front();
		queue.pop();
		M_Node* node = new M_Node();
		node->init(bf, e.id);
		for (int i = 0; i < node->num_entries; i++)
		{
			int near = 0;
			for (int j = 0; j < MAXLEVEL; j++)
				ispurned[j] = false;
			for (int k = 0; k < node->entries[i]->level - 1; k++)
			{
				ispurned[position[node->entries[i]->pivots[k]]] = true;
			}
			for (int k = 0; k < MAXLEVEL; k++)
			{
				if (!ispurned[k])
				{
					near = k;
					break;
				}
			}

			if (q->pd[node->entries[i]->pivots[node->entries[i]->level - 1]] - q->pd[nearpivot[near]] - 2 * upper > 0.0000001)
				continue;

			double temp = idist(q->pd, node->entries[i]->min, node->entries[i]->max);
			if (temp - upper > 0.0000001)
				continue;
			if (q->pd[nearpivot[0]] + upper - node->entries[i]->minkey <-0.0000001 || q->pd[nearpivot[0]] - upper - node->entries[i]->maxkey>0.0000001)
			{
				continue;
			}

			if (node->entries[i]->ptr == -1)
			{
				if (e.id != access || p != i)
				{
					candi = new vector<TEntry>();
					bplus->root_ptr->find_kNN_range(node->entries[i]->mkey, node->entries[i]->mxkey, q->pd, upper, candi);
					for (int j = 0; j < candi->size(); j++)
					{
						if ((*candi)[i].key - upper > 0.0000001)
							continue;
						Object* o = getobject((*candi)[j].id);
						double temp = q->distance(*o);
						if (temp - upper<-0.0000001)
						{
							TEntry te;
							te.id = o->id;
							te.key = temp;
							answer.push_back(te);
							sort(answer.begin(), answer.end());
							if (answer.size() > k)
								answer.pop_back();
							if (answer.size() >= k &&upper - answer[k - 1].key > 0.0000001)
							{
								upper = answer[k - 1].key;
							}
						}
						delete o;
					}
					delete candi;
				}
			}
			else
			{
				KEntry te;
				te.id = node->entries[i]->ptr;
				te.key = temp;
				te.isobject = false;
				queue.push(te);
			}
		}
		delete node;
	}
	delete[] q->pd;
	q->pd = NULL;
	return answer[k - 1].key;
}

double PB_Tree::knn_orignal(Object * q, int k, double inc, int datasize)
{
	double r = inc;
	double upper = MAXDIST;
	vector<TEntry> answer;
	q->pd = new double[num_piv];
	double temp = MAXDIST;
	int* nearpivot = new int[MAXLEVEL];
	int* position = new int[num_piv];
	vector<TEntry> ve;
	for (int i = 0; i < num_piv; i++)
	{
		q->pd[i] = q->distance(ptable[i]);
		TEntry t;
		t.id = i;
		t.key = q->pd[i];
		ve.push_back(t);
	}

	bool* isaccess = new bool[datasize];
	for (int i = 0; i < datasize; i++)
		isaccess[i] = false;
	sort(ve.begin(), ve.end());
	for (int i = 0; i < MAXLEVEL; i++)
	{
		nearpivot[i] = ve[i].id;
	}

	int count = 0;
	for (int i = 0; i < num_piv; i++)
	{
		position[ve[i].id] = count;
		count++;
	}

	bool* ispurned = new bool[MAXLEVEL];

	bplus->load_root();


	while (r - upper - inc < 0.000001)
	{
		vector<int> candi;
		queue<M_Node*> queue;
		M_Node * node = new M_Node();
		node->init(bf, 0);
		queue.push(node);
		node = NULL;
		while (!queue.empty())
		{
			node = queue.front();
			queue.front() = NULL;
			queue.pop();
			for (int i = 0; i < node->num_entries; i++)
			{
				int near = 0;
				for (int j = 0; j < MAXLEVEL; j++)
					ispurned[j] = false;
				for (int k = 0; k < node->entries[i]->level - 1; k++)
				{
					ispurned[position[node->entries[i]->pivots[k]]] = true;
				}
				for (int k = 0; k < MAXLEVEL; k++)
				{
					if (!ispurned[k])
					{
						near = k;
						break;
					}
				}

				if (q->pd[node->entries[i]->pivots[node->entries[i]->level - 1]] - q->pd[nearpivot[near]] - 2 * r > 0.000000001)
				{
					continue;
				}

				if (q->pd[nearpivot[0]] + r - node->entries[i]->minkey <-0.00000001 || q->pd[nearpivot[0]] - r - node->entries[i]->maxkey>0.000000001)
				{
					continue;
				}

				if (!node->entries[i]->isleaf)
				{
					M_Node* newnode = new M_Node();
					newnode->init(bf, node->entries[i]->ptr);
					queue.push(newnode);
					newnode = NULL;
				}
				else
				{
					bplus->root_ptr->find_interval_orignal(node->entries[i]->mkey, node->entries[i]->mxkey, q->pd, r, &candi);
				}
			}

			delete node;
		}

		for (int x = 0; x < candi.size(); x++)
		{
			Object* o = getobject((candi)[x]);

			if (isaccess[o->id])
			{
				delete o;
				continue;
			}

			double temp = q->distance(*o);
			isaccess[o->id] = true;

			if (temp - upper < 0.000000001)
			{
				TEntry te;
				te.id = o->id;
				te.key = temp;
				answer.push_back(te);
				sort(answer.begin(), answer.end());
				if (answer.size() > k)
					answer.pop_back();
				if (answer.size() >= k && upper - answer[k - 1].key > 0.000000001)
				{
					upper = answer[k - 1].key;
				}
			}
			delete o;
		}
		candi.clear();
		r += inc;
	}
	delete[] isaccess;
	delete[] nearpivot;
	delete[] position;
	delete[] ispurned;
	delete[] q->pd;
	q->pd = NULL;

	return answer[k - 1].key;
}
