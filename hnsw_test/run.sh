# Ms=(512 4096 8192 16384)
# #Ms=(32 64 128 256)
# ef=(1000 4000 8000 12000 16000 20000)
# dataid=audio
# queryid=uniform1000
# for m in ${Ms[@]}
# do
#    for e in ${ef[@]}
#    do
#    	./main $dataid $queryid $m $e
#    done
# done

# Ms=(8192 16384)
# #Ms=(32 64 128 256)
# ef=(1000)
# dataid=glove
# queryid=uniform1000
# for m in ${Ms[@]}
# do
#    for e in ${ef[@]}
#    do
#    	./main $dataid $queryid $m $e
#    done
# done


Ms=(4096 8192 16384)
#Ms=(32 64 128 256)
ef=(1000)
dataid=deep1M
queryid=uniform1000
for m in ${Ms[@]}
do
   for e in ${ef[@]}
   do
   	./main $dataid $queryid $m $e
   done
done

