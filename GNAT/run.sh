dataid=sift10M
distribut=uniform
q_num=1000
r=(388.195123	412.872988	438.956845	467.977108	502.686021)
pvnums=(5 10 15 20 25)
k=(10 20 30 40 50)
for pvnum in ${pvnums[@]}
do
	rs="${#r[@]} ${r[@]}"
	ks="${#k[@]} ${k[@]}"
	./main $dataid $distribut$q_num $pvnum $rs $ks
done

# dataid=glove
# distribut=uniform
# q_num=1000
# r=(8.376503	8.729799	9.131738	9.622691	10.29212)
# pvnums=(5 10 15 20 25)
# k=(10 20 30 40 50)
# for pvnum in ${pvnums[@]}
# do
# 	rs="${#r[@]} ${r[@]}"
# 	ks="${#k[@]} ${k[@]}"
# 	./main $dataid $distribut$q_num $pvnum $rs $ks
# done

# dataid=audio
# distribut=uniform
# q_num=1000
# r=(46798	60475	67635	76067	86026)
# pvnums=(5 10 15 20 25)
# k=(10 20 30 40 50)
# for pvnum in ${pvnums[@]}
# do
# 	ks="${#k[@]} ${k[@]}"
# 	./main $dataid $distribut$q_num $pvnum 0 $ks
# done

# dataid=notre
# distribut=uniform
# q_num=1000
# r=(263	302	319	338	358)
# pvnums=(5 10 15 20 25)
# k=(10 20 30 40 50)
# for pvnum in ${pvnums[@]}
# do
# 	ks="${#k[@]} ${k[@]}"
# 	./main $dataid $distribut$q_num $pvnum 0 $ks
# done

# dataid=enron
# distribut=uniform
# q_num=1000
# r=(54	59	62	66	71)
# pvnums=(5 10 15 20 25)
# k=(10 20 30 40 50)
# for pvnum in ${pvnums[@]}
# do
# 	ks="${#k[@]} ${k[@]}"
# 	./main $dataid $distribut$q_num $pvnum 0 $ks
# done

# dataid=nuswide
# distribut=uniform
# q_num=1000
# r=(24.5764	26.7535	27.7856	28.9165	30.2023)
# pvnums=(5 10 15 20 25)
# k=(10 20 30 40 50)
# for pvnum in ${pvnums[@]}
# do
# 	ks="${#k[@]} ${k[@]}"
# 	./main $dataid $distribut$q_num $pvnum 0 $ks
# done

# dataid=sift
# distribut=uniform
# q_num=1000
# r=(244	287	307	329	353)
# pvnums=(5 10 15 20 25)
# k=(10 20 30 40 50)
# for pvnum in ${pvnums[@]}
# do
# 	ks="${#k[@]} ${k[@]}"
# 	./main $dataid $distribut$q_num $pvnum 0 $ks
# done

# dataid=trevi
# distribut=uniform
# q_num=1000
# r=(1517	1839	1997	2175	2380)
# pvnums=(5 10 15 20 25)
# k=(10 20 30 40 50)
# for pvnum in ${pvnums[@]}
# do
# 	ks="${#k[@]} ${k[@]}"
# 	./main $dataid $distribut$q_num $pvnum 0 $ks
# done

# dataid=sun
# distribut=uniform
# q_num=1000
# r=(76279	89849	95663	101897	108819)
# pvnums=(5 10 15 20 25)
# k=(10 20 30 40 50)
# for pvnum in ${pvnums[@]}
# do
# 	ks="${#k[@]} ${k[@]}"
# 	./main $dataid $distribut$q_num $pvnum 0 $ks
# done

# dataid=deep1M
# distribut=uniform
# q_num=1000
# r=(0.564587	0.626831	0.656971	0.677176	0.69897)
# pvnums=(5 10 15 20 25)
# k=(10 20 30 40 50)
# for pvnum in ${pvnums[@]}
# do
# 	ks="${#k[@]} ${k[@]}"
# 	./main $dataid $distribut$q_num $pvnum 0 $ks
# done
