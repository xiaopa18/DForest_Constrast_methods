dataid=glove
distribut=uniform
q_num=1000
r=(6.039538	6.458204	6.656926	6.878966	7.135468)
k=()
pvnums=(15)
bss=(4096)
for pvnum in ${pvnums[@]}
do
	./hfi $dataid $pvnum
	for bs in ${bss[@]}
	do
		rs="${#r[@]} ${r[@]} ${#k[@]} ${k[@]}"
		./main $dataid $distribut$q_num $pvnum $bs $rs
		./calc_mem
	done
done

# dataid=sift10M
# distribut=uniform
# q_num=1000
# r=(269	335	363	388.195123	412.872988)
# k=()
# pvnums=(5)
# bss=(20480)
# for pvnum in ${pvnums[@]}
# do
# 	./hfi $dataid $pvnum
# 	for bs in ${bss[@]}
# 	do
# 		rs="${#r[@]} ${r[@]} ${#k[@]} ${k[@]}"
# 		./main $dataid $distribut$q_num $pvnum $bs $rs
# 		./calc_mem
# 	done
# done

# dataid=audio
# distribut=uniform
# q_num=1000
# r=(33055	39128	42210	44364	46798)
# k=(10 20 30 40 50)
# pvnums=(2)
# bss=(4096)
# for pvnum in ${pvnums[@]}
# do
# 	./hfi.exe $dataid $pvnum
# 	for bs in ${bss[@]}
# 	do
# 		rs="${#r[@]} ${r[@]} ${#k[@]} ${k[@]}"
# 		./main.exe $dataid $distribut$q_num $pvnum $bs $rs
# 		./calc_mem
# 	done
# done

# dataid=enron
# distribut=uniform
# q_num=1000
# r=(16	48	51	52	54)
# pvnums=(5 10 15 20 25)
# bss=(20480)
# for pvnum in ${pvnums[@]}
# do
	# ./hfi.exe $dataid $pvnum
	# for bs in ${bss[@]}
	# do
		# rs="${#r[@]} ${r[@]}"
		# ./main $dataid $distribut$q_num $pvnum $bs $rs
		# ./calc_mem.exe
	# done
# done

# dataid=nuswide
# distribut=uniform
# q_num=1000
# r=(21	22.7596	23.5372	24.0416	24.5764)
# pvnums=(5 10 15 20 25)
# bss=(20480)
# for pvnum in ${pvnums[@]}
# do
	# ./hfi.exe $dataid $pvnum
	# for bs in ${bss[@]}
	# do
		# rs="${#r[@]} ${r[@]}"
		# ./main $dataid $distribut$q_num $pvnum $bs $rs
		# ./calc_mem.exe
	# done
# done

# dataid=sift
# distribut=uniform
# q_num=1000
# r=(185	213	227	235	244)
# pvnums=(5 10 15 20 25)
# bss=(20480)
# for pvnum in ${pvnums[@]}
# do
	# ./hfi.exe $dataid $pvnum
	# for bs in ${bss[@]}
	# do
		# rs="${#r[@]} ${r[@]}"
		# ./main $dataid $distribut$q_num $pvnum $bs $rs
		# ./calc_mem.exe
	# done
# done

# dataid=trevi
# distribut=uniform
# q_num=1000
# r=(1197	1337	1409	1460	1517)
# pvnums=(5 10 15)
# bss=(20480)
# for pvnum in ${pvnums[@]}
# do
	# ./hfi.exe $dataid $pvnum
	# for bs in ${bss[@]}
	# do
		# rs="${#r[@]} ${r[@]}"
		# ./main $dataid $distribut$q_num $pvnum $bs $rs
		# ./calc_mem.exe
	# done
# done

# dataid=sun
# distribut=uniform
# q_num=1000
# r=(60305	67636	71221	73648	76279)
# pvnums=(5 10 15)
# bss=(4096)
# for pvnum in ${pvnums[@]}
# do
	# ./hfi.exe $dataid $pvnum
	# for bs in ${bss[@]}
	# do
		# rs="${#r[@]} ${r[@]}"
		# ./main $dataid $distribut$q_num $pvnum $bs $rs
		# ./calc_mem.exe
	# done
# done

# dataid=deep1M
# distribut=uniform
# q_num=1000
# r=(0.564587	0.626831	0.656971	0.677176	0.69897)
# pvnums=(3)
# bss=(20480)
# for pvnum in ${pvnums[@]}
# do
# 	./hfi.exe $dataid $pvnum
# 	for bs in ${bss[@]}
# 	do
# 		rs="${#r[@]} ${r[@]}"
# 		./main $dataid $distribut$q_num $pvnum $bs $rs
# 		./calc_mem.exe
# 	done
# done

# dataid=notre
# distribut=uniform
# q_num=1000
# r=(195	230	244	253	263)
# pvnums=(5 10 15)
# bss=(20480)
# for pvnum in ${pvnums[@]}
# do
	# ./hfi.exe $dataid $pvnum
	# for bs in ${bss[@]}
	# do
		# rs="${#r[@]} ${r[@]}"
		# ./main $dataid $distribut$q_num $pvnum $bs $rs
		# ./calc_mem.exe
	# done
# done
