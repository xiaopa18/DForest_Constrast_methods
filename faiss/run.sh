dataid='audio'
python3 ./main.py $dataid Flat
echo ${dataid}
./calc_mem

dataid='deep1M'
python3 ./main.py $dataid Flat
echo ${dataid}
./calc_mem

dataid='enron'
python3 ./main.py $dataid Flat
echo ${dataid}
./calc_mem

dataid='notre'
python3 ./main.py $dataid Flat
echo ${dataid}
./calc_mem

dataid='nuswide'
python3 ./main.py $dataid Flat
echo ${dataid}
./calc_mem

dataid='sun'
python3 ./main.py $dataid Flat
echo ${dataid}
./calc_mem

dataid='trevi'
python3 ./main.py $dataid Flat
echo ${dataid}
./calc_mem

dataid='sift10M'
python3 ./main.py $dataid Flat
echo ${dataid}
./calc_mem

dataid='glove'
python3 ./main.py $dataid Flat
echo ${dataid}
./calc_mem


