 
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

/your_path/lab2/kernal_modules# source insert.sh       #this makes the kernel modules and inserts them in the kernel

/your_path/lab2/codes# source bitstream_lab3.bit       #to insert the bit file

/your_path/lab2/codes# make clean
/your_path/lab2/codes# make


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
root@ultra96:~/labs/advmcu_codes/advancesmcusystems/lab2/codes# ./test1 2 -1 200

 **COPY**

## Starting with Lab 3

```bash

jkhare@mario (/misc/scratch/jkhare) % cd ..
jkhare@mario (/misc/scratch) % ls
avenkkateshraj    dpinheiroleal  hbarclay  jkhare  jwu2             mlei   mwang3  sli4   vivado.jou  wkelly
dperaltavelazque  etaka          hychiang  jmehta  mbadrinarayanan  mshah  sdutta  tvema  vivado.log
jkhare@mario (/misc/scratch) % cd jkhare/
jkhare@mario (/misc/scratch/jkhare) % cd advancesmcusystems/
jkhare@mario (/misc/scratch/jkhare/advancesmcusystems) % module load xilinx/2018
jkhare@mario (/misc/scratch/jkhare/advancesmcusystems) % vivado &
[3] 23314
jkhare@mario (/misc/scratch/jkhare/advancesmcusystems) % 
****** Vivado v2018.3 (64-bit)
  **** SW Build 2405991 on Thu Dec  6 23:36:41 MST 2018
  **** IP Build 2404404 on Fri Dec  7 01:43:56 MST 2018
    ** Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.

start_gui

```

## When the GUI is open-
1. Open the project from lab2.
2. Change the different widths and burst sizes of the different blocks.
3. Generate the bitstreams.


## Understanding the data flow in the given design

Before we go on with changing the bit width of various components in the design, we need to know how does the data flow in the design. The image shows the path the data follows while we transfer content from OCM to BRAM and vise-versa.
The red colored path is the one by which the CDMA reads the content of the OCM, and then using the smartconnect, writes the data to the BRAM via the green path. 


## The different combinations possible

Based on the path of data flow discussed above, following are the components and their respective properties that have been explored in this lab.


1. Zynq unit
 - Master bit width and Master ports 
 - Slave bit width and Slave ports

2. CDMA unit
 - Bit Width
 - Burst Size
 - CDMA store and forward option

3. BRAM Controller
 - Bit Width

4. Synthesis frequency
 - the maximum possible (without any negative slack)


## Zynq Unit

Bit Width: The bus widths possible for the master and slave interface of the PS are **32, 64, and 128**. We have been using the width size 128 since lab 1 and hence make no change to it whatsoever. The 128 bit width for the zynq master and slave buses is the maximum we can make them upto.

Ports: Following is the screenshot from showing the various port options available for the master and slave interfaces. 


<img src="images\ports.png" width="1000" />
<hr style="border:2px solid gray"> </hr>

We explore the three combination out of it, i.e. slave port being LPD (same as lab2), High Performace (HP), and ACP (Acceleration Co-herancy Port)


## CDMA Unit

Bus Widths: The bus width options available for CDMA are 32, 64, 128, 256, 512, 1024. The lab2 configuration had 32 bit width. Here in this lab we explore multiple combinations starting from 128 to 256, 512, and 1024 bits. 

Burst size: The burst size gets auto set when we change the bus width, but for the sake of consistency, the burst size for all the bust widths has been set to 32 bits.

CDMA store and forward: As we can see in the screenshot below form Vivado, we can enable/disable the 'Store and Forward' option in the CDMA. This option has also been explored. 


<img src="images\cdma.png" width="1000" />
<hr style="border:2px solid gray"> </hr>

## BRAM Controller

Bus Width: The bus width options available for BRAM are 32, 64, 128, 256, 512, 1024. The lab2 configuration had 32 bit width. Here in this lab we explore multiple combinations starting from 128 to 256, 512, and 1024 bits. 

## Synthesis Frequnecy

The PL synthesis frequency was set to 100 MHz for lab2. But as the aim for lab3 is to push the design through all the limits and extract the best performance out of it, we can go for the maximum synthesis frequency possible (without a negative slack). This can be done by changing the divisor values in the corresponding registers.
 
# Setting up the Board

1. sudo screen -L /dev/ttyUSB1 115200
2. wpa_passphrase SpectrumSetup-FE PASSWORD
3. sudo ./wifi.sh sets up the wifi 
```bash
#udhcpc: lease of **XXX.XXX.X.XXX** obtained, lease time 43200.
```
4. transfer system.bit and system.dtb files to the board.
5. Enable root access, and Mount the device to enable the boot access. 

``` bash
root@ultra96:/sys/firmware/devi sudo bash
root@ultra96: mount /dev/mmchblk0p1 BOOT/

Filesystem     1K-blocks     Used Available Use% Mounted on
/dev/root       29513292 15625940  12365100  56% /

```
6. fpgautil -b system.bit
7. Halt and restart the board. 
8. Check the device-tree by doing ls at **/proc/device-tree/amba_pl@0/**:

``` bash
total 0
drwxr-xr-x 15 root root  0 Feb  3 01:57  ./
drwxr-xr-x 44 root root  0 Feb  3 01:57  ../
-r--r--r--  1 root root  4 Feb  3 01:57 '#address-cells'
drwxr-xr-x  2 root root  0 Feb  3 01:57 'axi_bram_ctrl@a0028000'/
drwxr-xr-x  2 root root  0 Feb  3 01:57 'Capture_Timer@a0030000'/
-r--r--r--  1 root root 11 Feb  3 01:57  compatible
drwxr-xr-x  3 root root  0 Feb  3 01:57 'dma@b0000000'/
drwxr-xr-x  2 root root  0 Feb  3 01:57 'gpio@a0020000'/
drwxr-xr-x  2 root root  0 Feb  3 01:57 'gpio@a0021000'/
drwxr-xr-x  2 root root  0 Feb  3 01:57 'gpio@a0022000'/
drwxr-xr-x  2 root root  0 Feb  3 01:57 'gpio@a0025000'/
-r--r--r--  1 root root  8 Feb  3 01:57  name
drwxr-xr-x  2 root root  0 Feb  3 01:57 'PWM_w_Int@a0023000'/
drwxr-xr-x  2 root root  0 Feb  3 01:57 'PWM_w_Int@a0024000'/
-r--r--r--  1 root root  0 Feb  3 01:57  ranges
drwxr-xr-x  2 root root  0 Feb  3 01:57 'serial@a0000000'/
drwxr-xr-x  2 root root  0 Feb  3 01:57 'serial@a0010000'/
-r--r--r--  1 root root  4 Feb  3 01:57 '#size-cells'
drwxr-xr-x  2 root root  0 Feb  3 01:57 'system_management_wiz@a0026000'/
drwxr-xr-x  2 root root  0 Feb  3 01:57  zyxclmm_drm/

```


# Setting the Frequency

## Frequency Values

Vary the frequencies using the method from Lab 1 [README.md](https://github.com/JatinKhare/advancesmcusystems/blob/main/lab1/README.md)

# Interrupt handling

## Writing and inserting a kernel module

Follow the same steps from the lab2 for this. 

## Codes

1. [test1.c](codes/test1.c)
2. [test2.c](codes/test2.c)
 
## Performance Analysis

Let us talk numbers: 

The following table shows the latency for the different configurations tested for the lab3.
(Note: the transfer latency numbers are for PS freq: 1499 MHz and PL freq: Synthesis Freq)
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

Observations:

1. Effect of increasing the Bit Width: As we increase the width from 32 to 1024, we can see that there is improvment in the performance, but the rate of improvement decreases with increase in the bit width. It is the max change from 32 to 128, then becomes nearly constant with very small changes from 256 bits onwards.

2. Effect of Burst size: As we know that the CDMA transfers the data in bursts and after each burst, there is exchange of few ack signals in the AXI protocol. Hence, the less we have the burst size the more will be the signals, which will degrade the performace. Hence we have kept the burst size as 32 throughout the experiment. (Why 32? Because when we set the bus width for the CDMA to 1024 bits, the maximum we can go for the burst size is 32, and hence for lower widths, even if it was possible to have a larger burst size, for the sake of the analysis, we stick to the 32 bit burst size.

3. CDMA S & F: This enables the internal buffer in the AXI CDMA. But one would except improvement after using the internal buffer, but this is not the real result is. The performace does down, which can be compared eith the results from (7) and (8)

4. BRAM controller: We have one BRAM and 2 BRAM controllers. The bram_ctrl_0 is connected to the  Zynq via a smartconnect. The bram_ctrl_1 is connected to the CDMA. So the bus width of the bram_ctrl_1 will change with the CDMA width change. We can set the bus width of the bram_ctrl_0 to any number, as long as the width(bram_ctrl_0):width(bram_ctrl_1).


5. Ports:


Let us answer the questions now:

1. Determine which busses to maximize between the PS-PL.
a. Explain how and why?
2. If necessary, change the SmartConnect configuration.
a. Explain why?
3. Determine which busses to maximize in the PL to the point where there are no timing
issues.
a. Explain how and why?
4. Determine which busses to maximize in the PL to the point where there are still some
FPGA resources available (i.e., 3)
5. Maximize the PL synthesis clock frequency in Vivado to the point where there are no
timing issues.
a. Explain how and why?
6. Synthesize your new Vivado schematic and generate a new bit file.
7. Download the new DTB from here
a. We need a new DTB because the size of the BRAM memory grew from 8K to 64K
b. Convert the DTB to a DTS and confirm that the new address map is correct.
c. Change the compatible statement to match what you have in your kernel
module. Convert the DTS back to a DTB and rewrite it back to the BOOT sector
8. Run Lab_2 Test 1 using the new PS and PL configurations to determine the baseline
performance improvements.
a. Graph the performance improvements versus what you got in Lab_2.
b. If there are performance improvements explain where they are coming from. If
not explain why there was no improvement. You may need to generate
additional instrumentation blocks to help determine what is going on.
9. Determine if should now modify your application software and kernel module to take
advantage of the new hardware configuration.
a. Graph the performance improvements with the new software changes.
b. If there are performance improvements explain where they are coming from. If
not explain why there was no improvement. You may need to generate
additional instrumentation blocks to help determine what is going on.




