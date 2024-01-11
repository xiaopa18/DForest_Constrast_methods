import os
import time
import faiss
import numpy as np
import sys
import psutil
from read_bin import *
# 获取当前 Python 进程的 PID
pid = os.getpid()
# 获取当前 Python 进程对象
process = psutil.Process(pid)
# 获取当前 Python 进程占用的内存
dir='../data_set/'
dataid='audio'
queryid='uniform1000'
file_name='../data_set/audio/knn_10.data'
ks = [i*10 for i in range(1,6)]

idx='Flat'
argv = sys.argv
if len(argv)!=1:
    dataid=argv[1]
    idx=argv[2]

# prepare for dataset
dim = -1  # dimension
data = []
with open(dir+dataid+'/'+dataid+'.csv', 'r') as file:
    data_csv = file.readlines()
for da in data_csv:
    d = [d for d in da.split(',')]
    if dim == -1:
        for i in range(len(d)):
            if isFloat(d[i]):
                dim=i + 1
    d = d[:dim]
    d = [float(d) for d in d]
    data.append(d)
data=np.array(data)
query = []
with open(dir+dataid+'/'+dataid+'_'+queryid+'.csv', 'r') as file:
    data_csv = file.readlines()
for da in data_csv:
    d = [d for d in da.split(',')]
    d = d[:dim]
    d = [ float(d) for d in d]
    query.append([d])
query=np.array(query)
print(f'read {dataid} over')
m=dim
# build the index
if idx=='Flat':
    #index = faiss.IndexIVFFlat(faiss.IndexFlatL2(dim), dim, 100)
    # index = faiss.IndexFlatL2(dim)\
    index = faiss.IndexIVFPQ(faiss.IndexFlatL2(dim), dim, 100, m, 8)
else:
    index = faiss.IndexIVFPQ(faiss.IndexFlatL2(dim), dim, int(argv[3]), int(argv[4]), int(argv[5]))
construct_time=0.0
mem_use=-process.memory_info().rss
start_time = time.time()
index.train(data)
end_time = time.time()
construct_time+=end_time - start_time
start_time = time.time()
for i in data:
    index.add(np.array([i]))
end_time = time.time()
mem_use+=process.memory_info().rss
construct_time+=end_time - start_time
print(f'construction time: {construct_time}s, memory used:{mem_use/1024/1024}MB')
faiss.write_index(index,'./index.bin')
exit(0)
nprobes=[1,5,10,30,50,75,100]
ouf = open('./record.csv','a+')
# searching
for k in ks:
    for ns in nprobes:
        index.nprobe = ns
        file_name=dir+dataid+f'/knn_{k}.data'
        knn_exact = read_bin(file_name,k)
        tot_time=0.0
        avg_recall=0.0
        for i in range(len(query)):
            start_time = time.time()
            distances, indices = index.search(query[i], k)
            end_time = time.time()
            tot_time+=(end_time-start_time)
            avg_recall+=recall(knn_exact[i],indices[0])
        ouf.write(f'{dataid},{queryid},faiss,IVFPQ,ns={ns} 8bits m={m},{construct_time}s,{mem_use/1024/1024}MB,{k},{avg_recall/len(query)*100},{tot_time*1000/len(query)}ms\n')
        print(f'{dataid},{queryid},faiss,IVFPQ,ns={ns} 8bits m={m},{construct_time}s,{mem_use/1024/1024}MB,{k},{avg_recall/len(query)*100},{tot_time*1000/len(query)}ms\n')
    # ouf.write(f'{dataid},{queryid},faiss,IVFFlat,ns={ns},{construct_time}s,{mem_use/1024/1024}MB,{k},{avg_recall/len(query)*100},{tot_time*1000/len(query)}ms\n')
    # print(f'{dataid},{queryid},faiss,IVFFlat,ns={ns},{construct_time}s,{mem_use/1024/1024}MB,{k},{avg_recall/len(query)*100},{tot_time*1000/len(query)}ms\n')
    

