#include<iostream>
#include<cstring>
#include<algorithm>
#include <sys/stat.h>

using namespace std;

int main()
{
    struct stat info;
    int suc=stat("./SFIndex-Block1.raf",&info);
    cout<<suc<<" ";
    double mem_use=1.0*info.st_size/1024/1024;
    suc=stat("SFObj-Block1.raf",&info);
    mem_use+=1.0*info.st_size/1024/1024;
    cout<<suc<<" "<<mem_use<<"MB\n";
    return 0;
}
