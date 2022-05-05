read varname

pm 0xa0028004 -1
pm 0xa0028004 $varname

dm 0xa0028004 15
./test1 0 0 1 1024
dm 0xa0028004 15
