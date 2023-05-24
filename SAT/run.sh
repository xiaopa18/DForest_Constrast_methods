dataid=audio
distribut=uniform
q_num=1000
r=(46798	60475	67635	76067	86026)
rs="${#r[@]} ${r[@]}"
k=(10 20 30 40 50)
ks="${#k[@]} ${k[@]}}"
./main $dataid $distribut$q_num  0 $ks

dataid=notre 
distribut=uniform
q_num=1000
r=(263	302	319	338	358)
rs="${#r[@]} ${r[@]}"
k=(10 20 30 40 50)
ks="${#k[@]} ${k[@]}}"
./main $dataid $distribut$q_num  0 $ks

dataid=enron
distribut=uniform
q_num=1000
r=(54	59	62	66	71)
rs="${#r[@]} ${r[@]}"
k=(10 20 30 40 50)
ks="${#k[@]} ${k[@]}}"
./main $dataid $distribut$q_num  0 $ks

dataid=nuswide
distribut=uniform
q_num=1000
r=(24.5764	26.7535	27.7856	28.9165	30.2023)
rs="${#r[@]} ${r[@]}"
k=(10 20 30 40 50)
ks="${#k[@]} ${k[@]}}"
./main $dataid $distribut$q_num  0 $ks

dataid=sift
distribut=uniform
q_num=1000
r=(244	287	307	329	353)
rs="${#r[@]} ${r[@]}"
k=(10 20 30 40 50)
ks="${#k[@]} ${k[@]}}"
./main $dataid $distribut$q_num  0 $ks

dataid=trevi
distribut=uniform
q_num=1000
r=(1517	1839	1997	2175	2380)
rs="${#r[@]} ${r[@]}"
k=(10 20 30 40 50)
ks="${#k[@]} ${k[@]}}"
./main $dataid $distribut$q_num  0 $ks

dataid=sun
distribut=uniform
q_num=1000
r=(76279	89849	95663	101897	108819)
rs="${#r[@]} ${r[@]}"
k=(10 20 30 40 50)
ks="${#k[@]} ${k[@]}}"
./main $dataid $distribut$q_num  0 $ks

dataid=deep1M
distribut=uniform
q_num=1000
r=(0.564587	0.626831	0.656971	0.677176	0.69897)
rs="${#r[@]} ${r[@]}"
k=(10 20 30 40 50)
ks="${#k[@]} ${k[@]}}"
./main $dataid $distribut$q_num  0 $ks