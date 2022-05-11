 
# Project ADV E MCU: 
```
Name: Jatin Khare
UT EID: jk47976
Email: jatinkhare@utexas.edu

Name: Abhijjith Venkkateshraj
UT EID: av36677
Email: abhijjith@utexas.edu
```
## How to run the code

First, run the following command in the specified directories to setup the environment:

```bash

/your_path/project_sniffer/kernal_modules# source insert.sh       #this makes the kernel modules and inserts them in the kernel

/your_path/project_sniffer/codes# fpgautil -b project_bit_streams/<name-of-the-bit-file>.bit        #to insert the bit file

/your_path/project_sniffer/codes# make clean
/your_path/project_sniffer/codes# make


```

Once you are in the project_sniffer/codes directory and have the .c files (test1.c) in the same directory as the Makefile, do the following:

``` 
$ make clean
$ make
$ ./test1 
& ./sniffer_test1

```

## Running test1.c before sniffer_test1.c is must as it initializes all the counts before the sniffer and start probing.

This will generate the corresponding object files and executables for the test case.


### ./test1

USAGE:

```bash

./test1

```
### ./sniffer_test1

USAGE:

```bash

./test1

```
