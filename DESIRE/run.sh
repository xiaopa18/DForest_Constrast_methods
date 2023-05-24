dataid=audio
distribut=uniform
q_num=1000
r=(46798	60475	67635	76067	86026)
k=(10 20 30 40 50)
rs="${#r[@]} ${r[@]}"
ks="${#k[@]} ${k[@]}"
./main $dataid $distribut$q_num  $rs $ks
./calc_mem.exe

dataid=enron
distribut=uniform
q_num=1000
r=(54	59	62	66	71)
k=(10 20 30 40 50)
rs="${#r[@]} ${r[@]}"
ks="${#k[@]} ${k[@]}"
./main $dataid $distribut$q_num  $rs $ks
./calc_mem.exe

dataid=nuswide
distribut=uniform
q_num=1000
r=(24.5764	26.7535	27.7856	28.9165	30.2023)
k=(10 20 30 40 50)
rs="${#r[@]} ${r[@]}"
ks="${#k[@]} ${k[@]}"
./main $dataid $distribut$q_num  $rs $ks
./calc_mem.exe

dataid=sift
distribut=uniform
q_num=1000
r=(244	287	307	329	353)
k=(10 20 30 40 50)
rs="${#r[@]} ${r[@]}"
ks="${#k[@]} ${k[@]}"
./main $dataid $distribut$q_num  $rs $ks
./calc_mem.exe

dataid=notre
distribut=uniform
q_num=1000
r=(263	302	319	338	358)
k=(10 20 30 40 50)
rs="${#r[@]} ${r[@]}"
ks="${#k[@]} ${k[@]}"
./main $dataid $distribut$q_num  $rs $ks
./calc_mem.exe

dataid=trevi
distribut=uniform
q_num=1000
r=(1517	1839	1997	2175	2380)
k=(10 20 30 40 50)
rs="${#r[@]} ${r[@]}"
ks="${#k[@]} ${k[@]}"
./main $dataid $distribut$q_num  $rs $ks
./calc_mem.exe

dataid=sun
distribut=uniform
q_num=1000
r=(76279	89849	95663	101897	108819)
k=(10 20 30 40 50)
rs="${#r[@]} ${r[@]}"
ks="${#k[@]} ${k[@]}"
./main $dataid $distribut$q_num  $rs $ks
./calc_mem.exe

dataid=deep1M
distribut=uniform
q_num=1000
r=(0.69897	0.80634	0.855226	0.906499	0.960435)
k=(10 20 30 40 50)
rs="${#r[@]} ${r[@]}"
ks="${#k[@]} ${k[@]}"
./main $dataid $distribut$q_num  $rs $ks
./calc_mem.exe