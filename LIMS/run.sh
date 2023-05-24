dataid=audio
distribut=uniform
dim=192
q_num=1000
r=(33055	39128	42210	44364	46798)
pvnums=(2 4 6 )
clus=(10 20 30)
for pvnum in ${pvnums[@]}
do
	for clu in ${clus[@]}
	do
		rs="${#r[@]} ${r[@]}"
		rm -r './data' './dataset'${clu}
		mkdir 'data' './dataset'${clu}
		./main.exe $dataid $distribut$q_num $pvnum $clu $dim $rs
		./calc_mem.exe $clu
	done
done

dataid=nuswide
distribut=uniform
dim=500
q_num=1000
r=(21	22.7596	23.5372	24.0416	24.5764)
pvnums=(3 5 10 15)
clus=(2 3 4)
for pvnum in ${pvnums[@]}
do
	for clu in ${clus[@]}
	do
		rs="${#r[@]} ${r[@]}"
		rm -r './data' './dataset'${clu}
		mkdir 'data' './dataset'${clu}
		./main.exe $dataid $distribut$q_num $pvnum $clu $dim $rs
		./calc_mem.exe $clu
	done
done

dataid=notre
distribut=uniform
dim=128
q_num=1000
r=(195	230	244	253	263)
pvnums=(2 4 6 )
clus=(10 20 30)
for pvnum in ${pvnums[@]}
do
	for clu in ${clus[@]}
	do
		rs="${#r[@]} ${r[@]}"
		rm -r './data' './dataset'${clu}
		mkdir 'data' './dataset'${clu}
		./main.exe $dataid $distribut$q_num $pvnum $clu $dim $rs
		./calc_mem.exe $clu
	done
done


dataid=sift
distribut=uniform
dim=128
q_num=1000
r=(185	213	227	235	244)
pvnums=(2 4 6 )
clus=(10 20 30)
for pvnum in ${pvnums[@]}
do
	for clu in ${clus[@]}
	do
		rs="${#r[@]} ${r[@]}"
		rm -r './data' './dataset'${clu}
		mkdir 'data' './dataset'${clu}
		./main.exe $dataid $distribut$q_num $pvnum $clu $dim $rs
		./calc_mem.exe $clu
	done
done

dataid=sun
distribut=uniform
dim=512
q_num=1000
r=(60305	67636	71221	73648	76279)
pvnums=(2 4 6)
clus=(2 3 4)
for pvnum in ${pvnums[@]}
do
	for clu in ${clus[@]}
	do
		rs="${#r[@]} ${r[@]}"
		rm -r './data' './dataset'${clu}
		mkdir 'data' './dataset'${clu}
		./main.exe $dataid $distribut$q_num $pvnum $clu $dim $rs
		./calc_mem.exe $clu
	done
done

dataid=deep1M
distribut=uniform
dim=256
q_num=1000
r=(0.564587	0.626831	0.656971	0.677176	0.69897)
pvnums=(2 4 6 )
clus=(10 20 30)
for pvnum in ${pvnums[@]}
do
	for clu in ${clus[@]}
	do
		rs="${#r[@]} ${r[@]}"
		rm -r './data' './dataset'${clu}
		mkdir 'data' './dataset'${clu}
		./main.exe $dataid $distribut$q_num $pvnum $clu $dim $rs
		./calc_mem.exe $clu
	done
done
