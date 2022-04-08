 
# Lab 3: 
```
Name: Jatin Khare
UT EID: jk47976
Email: jatinkhare@utexas.edu
```


# Contents
1. [How to run the code](#how-to-run-the-code)
2. [Starting with Lab 3](#starting-with-lab-2)
3. [Setting up the Board](#setting-up-the-board)
4. [Setting the Frequency](#setting-the-frequency)
5. [Useful information for writing the code](#useful-information-for-writing-the-code)
6. [Codes](#codes)


## How to run the code

First, run the following command in the specified directories to setup the environment:

```bash

/your_path/lab3/kernal_modules# source insert.sh       #this makes the kernel modules and inserts them in the kernel

/your_path/lab3/codes# source bitstream_lab3.bit       #to insert the bit file

/your_path/lab3/codes# make clean
/your_path/lab3/codes# make


```

Once you are in the lab3/codes directory and  have the .c files (test1.c, test2.c) in the same directory as the Makefile, do the following:

``` bash
$ make clean
$ make 
```
This will generate the corresponding object files and executables for all the three test cases. 

### ./test1

USAGE:

```bash

./test1 (n PS Freq) (m PL Freq) (Loops)

```
The test runs (input) transfers (1024 words each, to and fro from OCM->BRAM)

n and m values determine the PS-PL frequency combinations 

n: 0 (1499 MHz), 1 (999 MHz), 3 (416.6 MHz), -1 (default PS frequency = 1499 MHz)

m: 0 (250 MHz), 1 (187.5 MHz), 3 (100 MHz), -1 (default PL frequency = 250 MHz)

(Loops): -1 (Default value = 500), maximum input value allowed = 10000

```bash
root@ultra96:~/labs/advmcu_codes/advancesmcusystems/lab3/codes# ./test1 0 0 
Default loop number = 500
Setting PS Freq. to 1499 MHz
Setting PL Freq. to 250 MHz

DMA's OCM/BRAM traffic tests with 500 and 1024 words successful!!!

For OCM to BRAM:
Minimum Latency:    465
Maximum Latency:    588
Average Latency:    467.000000
Standard Deviation: 31.000000
Number of samples:  500

For BRAM to OCM:
Minimum Latency:    474
Maximum Latency:    494
Average Latency:    475.000000
Standard Deviation: 9.000000
Number of samples:  500
Total number of Interrupts for to-and-fro transfer: 1000

```

## Starting with Lab 3

## When the GUI is open-
1. Open the project from lab2.
2. Change the widths and burst sizes of the different blocks.
3. Generate the bitstreams and analyse the performance improvements.


## Understanding the data flow in the given design

Before we go on with changing the bus width of various components in the design, we need to know how does the data really flows. The following image shows the path the data follows while we transfer content to and fro  from OCM to BRAM.

<img src="images\design.png" width="1000" />
<hr style="border:2px solid gray"> </hr>

The using the red colored path the CDMA reads the content of the OCM using the smartconnect0, and writes the data to the BRAM via the green path. 


## The different combinations possible

Based on the data flow discussed above, following are the components and their respective properties that have been explored in this lab.


1. **Zynq Unit**
 - Master bus width and Master ports types
 - Slave bus width and Slave ports types

2. **CDMA Unit**
 - Bit Width
 - Burst Size
 - CDMA store and forward option

3. **BRAM Controller**
 - Bit Width

4. **Synthesis Frequency**
 - The maximum possible (without any timing violations)


## (1) Zynq Unit

### Bit Width
The bus widths possible for the master and slave interface of the PS are **32, 64, and 128**. We have been using the width size 128 since lab 1 and hence make no change to it whatsoever. The 128 bit bus width for the zynq master and slave buses is the maximum we can make them upto.

### Ports 
Following is the screenshot from showing the various port options available for the master and slave interfaces. 


<img src="images\ports.png" width="400" />
<hr style="border:2px solid gray"> </hr>

We explore the three combination out of it, i.e. slave port being LPD (same as lab2), High Performace (HP), and Accelerator Coherancy Port (ACP).


## (2) CDMA Unit

### Bus Widths
The bus width options available for CDMA are 32, 64, 128, 256, 512, 1024. The lab2 configuration had 32 bus bit width. Here in this lab we explore multiple combinations starting from 128 to 256, 512, and 1024 bits. 

### Burst size
The burst size gets auto set when we change the bus width, but for the sake of consistency, the burst size for all the bust widths has been set to 32 bits.

### CDMA store and forward
As we can see in the screenshot below form Vivado, we can enable/disable the 'Store and Forward' option in the CDMA. This option has also been explored. 

<img src="images\cdma_vivado.png" width="600" />
<hr style="border:2px solid gray"> </hr>

## (3) BRAM Controller

### Bus Width
The bus width options available for BRAM are 32, 64, 128, 256, 512, 1024. The lab2 configuration had 32 bus bit width. Here in this lab we explore multiple combinations starting from 128 to 256, 512, and 1024 bits. 

## (4) Synthesis Frequnecy

The PL synthesis frequency was set to 100 MHz for lab2. But as the aim for lab3 is to push the design through all the limits and extract the best performance out of it, we can go for the maximum synthesis frequency possible (without a negative slack). This can be done by changing the divisor values in the corresponding registers.
 
# Setting up the Board

1. sudo screen -L /dev/ttyUSB1 115200
2. transfer system.bit and system.dtb files to the board.
8. Check the device-tree.

``` bash
root@ultra96:/proc/device-tree/amba_pl@0# ll
total 0
drwxr-xr-x 18 root root  0 Apr  8 13:51  ./
drwxr-xr-x 44 root root  0 Dec 10 03:15  ../
-r--r--r--  1 root root  4 Apr  8 13:51 '#address-cells'
drwxr-xr-x  2 root root  0 Apr  8 13:51 'axi_bram_ctrl@b0020000'/
drwxr-xr-x  2 root root  0 Apr  8 13:51 'axi_bram_ctrl@b0030000'/
drwxr-xr-x  2 root root  0 Apr  8 13:51 'CAPTURE_TIMER@a0030000'/
-r--r--r--  1 root root 11 Apr  8 13:51  compatible
drwxr-xr-x  3 root root  0 Apr  8 13:51 'dma@b0000000'/
drwxr-xr-x  2 root root  0 Apr  8 13:51 'gpio@a0020000'/
drwxr-xr-x  2 root root  0 Apr  8 13:51 'gpio@a0021000'/
drwxr-xr-x  2 root root  0 Apr  8 13:51 'gpio@a0022000'/
drwxr-xr-x  2 root root  0 Apr  8 13:51 'gpio@a0025000'/
-r--r--r--  1 root root  8 Apr  8 13:51  name
drwxr-xr-x  2 root root  0 Apr  8 13:51 'PERIPHERAL@ff380000'/
drwxr-xr-x  2 root root  0 Apr  8 13:51 'PERIPHERAL@ff990000'/
drwxr-xr-x  2 root root  0 Apr  8 13:51 'PWM_w_Int@a0023000'/
drwxr-xr-x  2 root root  0 Apr  8 13:51 'PWM_w_Int@a0024000'/
-r--r--r--  1 root root  0 Apr  8 13:51  ranges
drwxr-xr-x  2 root root  0 Apr  8 13:51 'serial@a0000000'/
drwxr-xr-x  2 root root  0 Apr  8 13:51 'serial@a0010000'/
-r--r--r--  1 root root  4 Apr  8 13:51 '#size-cells'
drwxr-xr-x  2 root root  0 Apr  8 13:51 'system_management_wiz@a0026000'/

```

# Setting the Frequency

## Frequency Values

Vary the frequencies using the method from Lab 1 [README.md](https://github.com/JatinKhare/advancesmcusystems/blob/main/lab1/README.md)

# Interrupt handling

## Writing and inserting a kernel module

Follow the same steps from the lab2 for this. 

## Codes

1. [test1.c](codes/test1.c)
 
## Performance Analysis

Let us talk numbers. The following table shows the latency for the different configurations tested for the lab3.

(Note: the transfer latency numbers are for PS freq: 1499 MHz and PL freq: Synthesis Freq)
(All the bus widths are in bits)
|S. No.|Zynq-CDMA Width|Slave Port|CDMA-BRAM width|CDMA Burst Size|CDMA S & F|Synthesis Frquency|Capture Timer Count|Transfer Time (micro sec)|
|------|---------|---------|---------|---------|---------|---------|---------|---------|
|1. |    128 |LPD|    32   |     32   |**enabled**|  250 MHz | 1295    |  5.18|
|2. |    128 |LPD|    **128**   |     32   |enabled|  250 MHz |   690  |  2.76|
|3. |    128 |LPD|    **256**   |     32   |enabled|  250 MHz |   480  |  1.92|
|4. |    128 |LPD|    **512**   |     32   |enabled|  250 MHz |   470  |  1.88|
|5. |    128 |LPD|    **1024**   |     32   |enabled|  250 MHz |   468  | 1.872 |
|6. |    128 |**HP0**|    1024   |     32   |enabled|  250 MHz |   436  |  1.744|
|7. |    128 |**ACP**|    1024   |     32   |enabled|  250 MHz |   443  |  1.772|
|8. |    128 |LPD|    1024   |     32   |**disabled**|  250 MHz |   431  |  1.724|

## Observations

1. **Effect of increasing the Bus Width**: As we increase the width from 32 to 1024, we can see that there is improvment in the performance, but the rate of improvement decreases with increase in the bus width. The maximum change is from 32 to 128, and then it becomes nearly constant with very small improvements from 256 bits onwards. The following graph depicts this result. (Red dot reperesnts the configuration when CDMA S & F is disabled, where for all the black dots, it is enabled)

<img src="images\plot.png" width="500" />
<hr style="border:2px solid gray"> </hr>

2. **Effect of Burst size**: As we know that the CDMA transfers the data in bursts and after each burst, there is exchange of few ack signals in the AXI protocol. Hence, the less we have the burst size the more will be the signals, which will degrade the performace. As a result,  the burst size has been kept as constant 32 bits throughout the experiment. (Why 32? Because when we set the bus width for the CDMA to 1024 bits, the maximum we can go for the burst size is 32, and hence for lower widths, even if it was possible to have a larger burst size, for the sake of the analysis I have stick to the 32 bit burst size.

3. **CDMA S & F**: This enables the internal buffer in the AXI CDMA. One would except improvement after using the internal buffer, but this is not observed in the results is. The performace does down, which can be seene with the results from (7) and (8). This can be explained by the argument that it might be consuming some extra time in storing the values before it transfers the big chunk of the collected data.

4. **BRAM controller**: We have one BRAM and 2 BRAM controllers. The bram_ctrl_0 is connected to the  Zynq via a smartconnect. The bram_ctrl_1 is connected to the CDMA. So, the bus width of the bram_ctrl_1 will change as the CDMA bus width changes. We can set the bus width of the other bram_ctrl_0 to any number (as long as the bus width ratio of (bram_ctrl_0):(bram_ctrl_1) > 1:4).


5. **Ports**: All the three buses perform nearly equally. The ACP gives comparatively the worst performance of all. The reason might be that the data being transferedf is more than the cache size of the ACP.

Let us answer the questions now:

**1. Determine which busses to maximize between the PS-PL.
a. Explain how and why?**
```bash
Zynq-CDMA bus width = 128 (max possible)
CDMA-BRAM bus width = 1024, Burst Size = 32

How: Double click on each component and select the frequency from the drop down options. 
Why: On the similar lines as discissed in the point 2 of 'Observations', we want to minimize 
the to-and-from AXI signals and trnasfer a lot of data at one transaction so that we can speed 
up the process.
```
**2. If necessary, change the SmartConnect configuration.
a. Explain why?**
```bash
No changes made to the smartconnect
```
**3. Determine which busses to maximize in the PL to the point where there are no timing
issues.
a. Explain how and why?**
```bash
Even after we increase the bus width to the maximum of the options available, we do not get any timing issues.
```

**4. Determine which busses to maximize in the PL to the point where there are still some
FPGA resources available (i.e., 3)**
```bash
```

**5.Maximize the PL synthesis clock frequency in Vivado to the point where there are no
timing issues.
a. Explain how and why?**
```bash
How: Double click on the Zynq block and go to the PL clock frequency option. We can change the divisor values and change the frequency. It was observed that the maximum we can go on increasing the frequency is 250 MHz. Beyond that we start getting timing violations. 
Why: The BRAM blocks are scattered all over the place on the chip. As for the transfer, the data needs to travel to all these scattered BRAMS, we can only increase the PL frequency to a certain extent, after which we run into negative slack values. 
```


**6. Synthesize your new Vivado schematic and generate a new bit file.**
```bash
-done-
```

**7. Download the new DTB from here
a. We need a new DTB because the size of the BRAM memory grew from 8K to 64K
b. Convert the DTB to a DTS and confirm that the new address map is correct.
c. Change the compatible statement to match what you have in your kernel
module. Convert the DTS back to a DTB and rewrite it back to the BOOT sector**
```bash
-done-
```


**8. Run Lab2 Test 1 using the new PS and PL configurations to determine the baseline
performance improvements.
a. Graph the performance improvements versus what you got in Lab2.
b. If there are performance improvements explain where they are coming from. If
not explain why there was no improvement. You may need to generate 
additional instrumentation blocks to help determine what is going on.**

(A) a and b: Find the graph and explaination in the [Observations](#observations) section above.

**9. Determine if should now modify your application software and kernel module to take
advantage of the new hardware configuration.
a. Graph the performance improvements with the new software changes.
b. If there are performance improvements explain where they are coming from. If
not explain why there was no improvement. You may need to generate
additional instrumentation blocks to help determine what is going on.**

```bash
Do not think we need to any modifications to the software side as of now.
```



