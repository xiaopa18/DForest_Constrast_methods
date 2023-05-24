#include<iostream>
#include<cstring>
#include<algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <ftw.h>
using namespace std;
long long int totalDirectorySize;
int sumDirectory(const char *fpath, const struct stat *sb, int typeflag)
{
	totalDirectorySize += sb->st_size;
	return 0;
}

long long int GetDirectorySize(const char* dir)
{
	//totalDirectorySize = 0;

	if (!dir || access(dir, R_OK)) {
		return -1;
	}

	if (ftw(dir, &sumDirectory, 1)) {
		perror("ftw");
		return -2;
	}

	return totalDirectorySize;
}

int main(int argc,char **argv)
{
    string num_clu=string(argv[1]);
	string tmp="./dataset"+num_clu;
    GetDirectorySize("./data");
    GetDirectorySize(tmp.c_str());
    cout<<1.0*totalDirectorySize/1024/1024<<"MB\n";
    return 0;
}
