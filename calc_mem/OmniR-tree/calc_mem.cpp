#include<iostream>
#include<cstring>
#include<algorithm>
#include <sys\stat.h>

using namespace std;

int main()
{
    struct _stat info;
    int suc=_stat("./index.raf",&info);
    double mem_use=1.0*info.st_size/1024/1024;
    suc=_stat("./index",&info);
    mem_use+=1.0*info.st_size/1024/1024;
    cout<<suc<<" "<<mem_use<<"MB\n";
    return 0;
}
