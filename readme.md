##  All Contrast methods of DForest

The storage files for our dataset are all in CSV format and are located in the path `/data_set/dataid/dataid.csv` . If your dataset format is different from ours, you can either write a custom input functions to replace the functions of our code or convert the file format to CSV. In our CSV files, each row represents a vector, which corresponds to a data point in the dataset. For example, the storage format of a dataset with $n$ points and $d$ dimensions is as follows:
$$
p_{11},p_{12},p_{13},\cdots,p_{1d}\\
p_{21},p_{22},p_{23},\cdots,p_{2d}\\
\vdots \quad \vdots\quad \vdots\quad  \vdots\\
p_{n1},p_{n2},p_{n3},\cdots,p_{nd}\\
$$


We are using ` CodeBlocks 20.04 or makefile with GCC 8.1.0` to compile those C++ code, with the following compilation parameters: `-O3 -Wall -ffast-math`.

- Original link ：
- BB-Tree：https://hu.berlin/bbtree
- DESIRE：https://github.com/ZJU-DAILY/DESIRE
- EPT：https://github.com/ZJU-DAILY/Metric_Index
- GNAT：https://github.com/ZJU-DAILY/Metric_Index
- LIMS：https://github.com/learned-index/LIMS
- M-index：https://github.com/ZJU-DAILY/Metric_Index
- Mtree：https://github.com/erdavila/M-Tree
- OmniR-Tree：https://github.com/ZJU-DAILY/Metric_Index
- SAT：https://github.com/ZJU-DAILY/Metric_Index
- SPBTree：https://github.com/ZJU-DAILY/Metric_Index