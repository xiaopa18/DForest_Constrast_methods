#include "object.h"
#include <stdio.h>
#include <memory.h>
#include <cstring>
#include "iopack/RAF.h"
#include "iopack/blk_file.h"



extern RAF* Obj_f;
extern RAF* Index_f;
extern Cache* Obj_c;
extern Cache* Index_c;

#include <math.h>
#include <sstream>
#include <iostream>

Object::Object()
{
	datad = NULL;
	size = NULL;
	datas = NULL;
	ansres = NULL;

	istravel = 0;
	dist = 0;
}
void Object::ini(string str) {
	stringstream sstr1(str);
//	cout << "we " << str << endl;
	string s1;
	float d1;
	sized = 0;
	sizes = 0;

	size = new long long[m * 2];
//	cout << "m " << m << endl;
	for (long long i = 0; i < m; i++) {
		switch (metricm[i]) {
			case 0:
			case 3:
			case 1:
			case 2:
			case 4:
			case 5:
				sstr1 >> s1;
			//	cout << "1 " << s1 << endl;
				sized ++;
				for (long long k = 0; s1[k]; k++) if (s1[k] == ',')sized++;
				break;
			case 6:
				sstr1 >> s1;
			//	cout << "2 " << s1 << endl;
				sizes += s1.length()+1;
				break;
			default:
				cout << "Object reading data error!!!! " << endl;
		}
	}
	datad = new float[sized];
	datas = new char[sizes];
	ansres = new float[m];
	//cout << "sized sizes " << sized << " " << sizes << endl;
	stringstream sstr2(str);
	sizes = 0;
	sized = 0;
	for (long long j = 0; j < m; j++) {
		switch (metricm[j]) {
		case 0:
		case 3:
		case 1:
		case 2:
		case 4:
		case 5:
		{
			size[j * 2] = sized;
			sstr2 >> s1;
			sized++;
			for (long long k = 0; s1[k]; k++)
				if (s1[k] == ',') {
					s1[k] = ' ';
					sized++;
				}
			size[j * 2+1] = sized;
			stringstream sstr3(s1);
			for (long long i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				sstr3 >> datad[i];
			}
			break;
		}
		case 6:
			size[j * 2] = sizes;
			sstr2 >> s1;
			sizes += s1.length()+1;
			size[j * 2+1] = sizes;
			for (long long i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				datas[i]=(char)s1[i- size[j * 2]];
			}
			break;
		}
		ansres[j] = -1;
	}
}
void Object::outnode()
{
	cout << " id " << id;
	cout << " data :";
	for (long long j = 0; j < m; j++) {
		switch (metricm[j]) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			for (long long i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				cout<<datad[i]<<",";
			}
			cout << " # ";
			break;
		case 6:
			for (long long i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				cout <<datas[i];
			}
			cout << " # ";
			break;
		}
	}
	cout << endl;
	/*
	cout<< " disres :";
	for (long long i = 0; i < m; i++)
		cout << " " << ansres[i];
	cout << endl<<endl;
	*/
}
Objectarr::Objectarr()
{
	n = 0;
	m = 0;
}
Objectarr::~Objectarr()
{
	if (arr != NULL)
		delete[] arr;
}

vector<string> readcsv(string name)
{
    ifstream inf(name,ios::in);
    if(!inf.is_open())
    {
        cout<<"open"<<name<<" is failed\n";
        exit(-1);
    }
    vector<string> res;
    string tmp;
    while(getline(inf,tmp)) res.push_back(tmp);
    return res;
}

void Objectarr::readfile(string filedata)
{
	ifstream in1;
	ofstream ou1, ou2, ou3;
	{
		char sk1[10000];
		sprintf(sk1, "%s", filedata.c_str());
		in1.open(sk1);

	}
	//in1.open("testmulti.txt");
	//getline(in1, str);
//	ou1.open("tmp.txt");
//	n = 20;
    vector<string> tmps=readcsv(filedata);
    n=tmps.size();m=1;
    cout<<n<<"\n";
	metricm = new long long[m];
	metricmaxdis = new float[m];
	for (long long i = 0; i < m; i++) metricm[i]=2;
	for (long long i = 0; i < m; i++) metricmaxdis[i]=1;
	arr = new Object * [n + 1];
	long long maxsize = 0;
	for (long long i = 0; i < n; i++) {
		//cout << "trying " << str << endl;
		arr[i] = new Object();
//		cout << "new finish " << str << endl;
		arr[i]->ini(tmps[i]);
		arr[i]->id = i;
		//arr[i]->outnode();
		maxsize = max(maxsize, arr[i]->getsize());
		//arr[i]->outnode();
	}
	metricmeandis=new float[m];
	for (long long j = 0; j < m; j++) {
		metricmeandis[j] = 0;
		for (long long i = 0; i < 1000; i++) {
			long long p = rand()%n;
			long long q = rand() % n;
			metricmeandis[j] += arr[p]->distance(arr[q], j);
		}
		metricmeandis[j]/= metricmaxdis[j]*1000;
	}
	cout << "Max size is "<<maxsize << endl;
	ou1.close();
}


float Object::distance(Object* other, long long  m)
{
	//cout<<m<<" "<<metricm[m]<<endl;
	//cout<<endl << "==========  distance " <<m<< endl;
	float dis = 0;
	compdists++;
	switch (metricm[m]) {
	case 0:
	case 1:
	case 4: {
		for (long long i = size[m * 2]; i < size[m * 2 + 1]; i++) {
			dis += abs(datad[i] - other->datad[i]);
		}
		break; }
	case 5: {
		float sa1 = 0, sa2 = 0, sa3 = 0;

		for (long long i = size[m * 2]; i < size[m * 2 + 1]; i++) {
			sa1 += datad[i] * datad[i];
			sa2 += other->datad[i] * other->datad[i];
			sa3 += datad[i] * other->datad[i];
		}
		sa1 = pow(sa1, 0.5);
		sa2 = pow(sa2, 0.5);
		if (sa1 * sa2 == 0) cout << "Error!!!" << endl;
		dis = sa3 / (sa1 * sa2);
		if (dis > 1) {
			dis = 0.99999999999999999;
		}
		dis = abs(acos(dis) * 180 / 3.1415926);
		//cout << "dis " << dis << endl;

		break; }
	case 2: {

		for (long long i = size[m * 2]; i < size[m * 2 + 1]; i++) {
			dis += abs(datad[i] - other->datad[i]) * abs(datad[i] - other->datad[i]);
		}
		//cout<<"asd"<<" "<<dis<<endl;
		dis = pow(dis, 0.5);
		break;
	}
	case 3: {
		long long st = size[m * 2];
		dis += trans(datad[st], other->datad[st]);
		if (size[m * 2 + 1] - st > 1) dis = dis * 24 * 3600 + trans2(datad[st + 1], other->datad[st + 1]);
		dis = abs(dis);
		break;
	}

	case 6: {
		long long n = 0;
		long long m = 0;
		for (long long i = 0; datas[i]; i++) n++;
		for (long long i = 0; other->datas[i]; i++) m++;
		if (n == 0) return m;
		if (m == 0) return n;
		long long** table1 = new long long* [n + 1];
		for (long long i = 0; i < n + 1; i++)
			table1[i] = new long long[m + 1];
		for (long long i = 0; i <= n; i++) table1[i][0] = i;
		for (long long j = 0; j <= m; j++) table1[0][j] = j;
		for (long long i = 1; i <= n; i++) {
			for (long long j = 1; j <= m; j++) {
				long long cost = (datas[i - 1] == other->datas[j - 1]) ? 0 : 1;	// ith character of s, jth character of t
				table1[i][j] = 1 + MIN(table1[i - 1][j], table1[i][j - 1]);
				table1[i][j] = MIN(table1[i - 1][j - 1] + cost, table1[i][j]);
			}
		}
		dis = table1[n][m];
		for (long long i = 0; i < n + 1; i++)
			delete[] table1[i];
		delete[]table1;
		table1 = NULL;
		break;
	}
	}

	return dis * 1.0 / metricmaxdis[m];
}



Object::Object(Object* o)
{
	char* buf = new char[o->getsize()+2];
	o->write_to_buffer(buf);
	read_from_buffer(buf);
	delete []buf;

}
Object::~Object()
{

	if (ansres != NULL)
		delete[]ansres;
	if (datad != NULL)
		delete[] datad;
	if (datas != NULL)
		delete[] datas;
	if (size != NULL)
		delete[] size;
	datad = NULL;
	ansres = NULL;
	datas = NULL;
	size = NULL;

}
long long Object::getsize()
{
	//return size * sizeof(float) + 2 * sizeof(long long);

	long long i = 0;
	i+= sizeof(id);
	i += sizeof(sized);
	i += sizeof(sizes);
//	i += sizeof(dist);
//	i += sizeof(istravel);
	i += sized * sizeof(float);
	i += sizes * sizeof(char);
//	i += m * sizeof(float);
	i += 2*m * sizeof(long long);

	return i;
}
long long Object::read_from_buffer(char* buffer)
{
	long long i;
	memcpy(&id, buffer, sizeof(id));
	i = sizeof(id);

	memcpy(&sized, &buffer[i], sizeof(sized));
	i += sizeof(sized);

	memcpy(&sizes, &buffer[i], sizeof(sizes));
	i += sizeof(sizes);

//	ansres = new float[m];
//	memcpy(ansres, &buffer[i], m * sizeof(float));
//	i += m * sizeof(float);

	size = new long long[2*m];
	memcpy(size, &buffer[i], 2*m * sizeof(long long));
	i += m*2 * sizeof(long long);

	datad = new float[sized];
	memcpy(datad, &buffer[i], sized * sizeof(float));
	i += sized * sizeof(float);
//	cout << "start i " << i << endl;

	datas = new char[sizes];
	memcpy(datas, &buffer[i], sizes * sizeof(char));
	i += sizes * sizeof(char);
	//cout << "end i " << i <<" "<<getsize()<< endl;
	return i;
}
long long Object::write_to_buffer(char* buffer)
{
	long long i;

	memcpy(buffer, &id, sizeof(id));
	i = sizeof(id);

	memcpy(&buffer[i], &sized, sizeof(sized));
	i += sizeof(sized);

	memcpy(&buffer[i], &sizes, sizeof(sizes));
	i += sizeof(sizes);

//	memcpy(&buffer[i], ansres, m * sizeof(float));
//	i += m * sizeof(float);

	memcpy(&buffer[i], size, 2*m * sizeof(long long));
	i += 2*m * sizeof(long long);

	memcpy(&buffer[i], datad, sized * sizeof(float));
	i += sized * sizeof(float);

	memcpy(&buffer[i], datas, sizes * sizeof(char));
	i += sizes * sizeof(char);
	//cout << "end i " << i << " " << getsize() << endl;
	return i;
}




















