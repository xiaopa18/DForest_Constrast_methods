# LIMS

Usage:` [dataid] [queryid] [pivotNumber] [num_clu] [dim] [r_num] [r1] [r2] ... [r_rnum] [knum] [k1] [k2]...[k_knum]`

Before running, please `makedir dataset[num_clu]`

Example:

 `mkdir dataset4 `

  `./main audio uniform1000 3 4 192 5 33055 39128 42210 44364 46798 5 10 20 30 40 50  `

- dataid: the id of dataset
- queryid: query distribution + the number of query point
- pivotNumber: the number of pivots
- num_clu: the number of cluster
- r_num: the number of r for rangequery

use `./calc_mem.exe  [num_clu]` to view how many storage space be used
