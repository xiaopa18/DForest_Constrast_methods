# dataid=audio
# distribut=uniform
# q_num=1000
# r=(46798	60475	67635	76067	86026)
# pvnums=(100)
# for pvnum in ${pvnums[@]}
# do
	# rs="${#r[@]} ${r[@]}"
	# ./main $dataid $distribut$q_num $pvnum -1 $rs
# done

dataid=notre
distribut=uniform
q_num=1000
r=(263	302	319	338	358)
pvnums=(300)
for pvnum in ${pvnums[@]}
do
	rs="${#r[@]} ${r[@]}"
	./main $dataid $distribut$q_num $pvnum -1 $rs
done

dataid=enron
distribut=uniform
q_num=1000
r=(54	59	62	66	71)
pvnums=(100)
for pvnum in ${pvnums[@]}
do
	rs="${#r[@]} ${r[@]}"
	./main $dataid $distribut$q_num $pvnum -1 $rs
done

dataid=nuswide
distribut=uniform
q_num=1000
r=(24.5764	26.7535	27.7856	28.9165	30.2023)
pvnums=(100)
for pvnum in ${pvnums[@]}
do
	rs="${#r[@]} ${r[@]}"
	./main $dataid $distribut$q_num $pvnum -1 $rs
done

dataid=sift
distribut=uniform
q_num=1000
r=(244	287	307	329	353)
pvnums=(300)
for pvnum in ${pvnums[@]}
do
	rs="${#r[@]} ${r[@]}"
	./main $dataid $distribut$q_num $pvnum -1 $rs
done

dataid=trevi
distribut=uniform
q_num=1000
r=(1517	1839	1997	2175	2380)
pvnums=(100)
for pvnum in ${pvnums[@]}
do
	rs="${#r[@]} ${r[@]}"
	./main $dataid $distribut$q_num $pvnum -1 $rs
done

dataid=sun
distribut=uniform
q_num=1000
r=(76279	89849	95663	101897	108819)
pvnums=(100)
for pvnum in ${pvnums[@]}
do
	rs="${#r[@]} ${r[@]}"
	./main $dataid $distribut$q_num $pvnum -1 $rs
done

# dataid=deep1M
# distribut=uniform
# q_num=1000
# r=(0.564587	0.626831	0.656971	0.677176	0.69897)
# pvnums=(100 300 500 700 1000)
# for pvnum in ${pvnums[@]}
# do
	# rs="${#r[@]} ${r[@]}"
	# ./main $dataid $distribut$q_num $pvnum -1 $rs
# done