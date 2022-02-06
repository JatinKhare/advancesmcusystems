# advancesmcusystems

### Contents
1. [Setting up the environment](#setting-up-the-environment)
2. [Lab 1](#lab-1)
3. [Setting up the Board](#setting-up-the-board)
4. [Testing](#testing)
5. [Code for setting the register values](#code-for-setting-the-register-values)

## Setting up the environment

```bash

jkhare@mario (/misc/scratch/jkhare) % cd ..
jkhare@mario (/misc/scratch) % ls
avenkkateshraj    dpinheiroleal  hbarclay  jkhare  jwu2             mlei   mwang3  sli4   vivado.jou  wkelly
dperaltavelazque  etaka          hychiang  jmehta  mbadrinarayanan  mshah  sdutta  tvema  vivado.log
jkhare@mario (/misc/scratch) % cd jkhare/
jkhare@mario (/misc/scratch/jkhare) % cd advancesmcusystems/
jkhare@mario (/misc/scratch/jkhare/advancesmcusystems) % mkdir vivado
jkhare@mario (/misc/scratch/jkhare/advancesmcusystems) % cd vivado


jkhare@mario (/misc/scratch/jkhare/advancesmcusystems/vivado) % cp /home/projects/courses/spring_22/ee382n-16775/arch/labs/BASELINE_ULTRA96_2018.3.tar.gz .
jkhare@mario (/misc/scratch/jkhare/advancesmcusystems/vivado) % # tar xvf BASELINE_ULTRA96_2018.3.tar.gz
jkhare@mario (/misc/scratch/jkhare/advancesmcusystems/vivado) % ls -1
BASELINE_ULTRA96_2018.3
BASELINE_ULTRA96_2018.3.tar.gz
CONSTRAINTS
ip_repo


jkhare@mario (/misc/scratch/jkhare/advancesmcusystems/vivado) % cd BASELINE_ULTRA96_2018.3/
jkhare@mario (/misc/scratch/jkhare/advancesmcusystems/vivado/BASELINE_ULTRA96_2018.3) % ls
ip_upgrade.log       ultra96v2_oob.ip_user_files  ultra96v2_oob.srcs  vivado_8015.backup.jou  vivado.log
ultra96v2_oob.cache  ultra96v2_oob.runs           ultra96v2_oob.tmp   vivado_8015.backup.log
ultra96v2_oob.hw     ultra96v2_oob.sim            ultra96v2_oob.xpr   vivado.jou

jkhare@mario (/misc/scratch/jkhare/advancesmcusystems/vivado/BASELINE_ULTRA96_2018.3) % module load xilinx/2018
jkhare@mario (/misc/scratch/jkhare/advancesmcusystems/vivado/BASELINE_ULTRA96_2018.3) % vivado &
[3] 23314
jkhare@mario (/misc/scratch/jkhare/advancesmcusystems/vivado/BASELINE_ULTRA96_2018.3) % 
****** Vivado v2018.3 (64-bit)
  **** SW Build 2405991 on Thu Dec  6 23:36:41 MST 2018
  **** IP Build 2404404 on Fri Dec  7 01:43:56 MST 2018
    ** Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.

start_gui

```

### When the GUI is open-
1. Open the project: **_ultra96v2_oob.xpr_**.
2. Open the block diagram of ultra96v2_oob.
3. 4 different heirarchical elements can be seen in dark blue- High_Speed_MEZZ, BD_CTL_GPIO, SYS_MGMT, Low_Speed_MEZZ.
4. **Address Editor** shows the address of all the components attached to the Processing System (PS). Each address is 40 bit long (hence XX_XXXX_XXXX).

zynq_ultra_ps_e_0 -> data -> 10 components.

axi_cdma_0 -> Data --> zynq_ultra_ps_e_0, zynq_ultra_ps_e_0 -> Excluded Address Segments -> 10 components.

5. 10 components are excluded from CDMA unit.
6. Confirm that you have the constraint file. This file tells the synthesis tool the timing and pin constraints for every I/O in the design. Check SOURCES window to confirm the ultra96v2_oob.xdc file.
The file has two important SET_PROPERTY  statements, both are needed for valid bit strean generation-

``` bash
set_property IOSTANDARD LVCMOS18 [get_ports ls_mezz_uart*]
# sets the voltage drive level for the pin (or pins)

set_property PACKAGE_PIN F8 [get_ports ls_mezz_uart0_rx]
#second assigns the signal to a particular pin on the FPGA.

```
7. Open the block diagram, and validate the design.

```cpp
// Validation successful. There are no errors or critical warnings in this design. 
```
8. Flow -> Generate Bitstrem -> Save the block design -> Synthesis and Implementation is out-of-date? -> Launch Runs.

9. Open reports under 'Design Runs' contain the Static Timing Analysis.

10. File -> Export -> Export Hardware -> include bit stream

### IP Integration

1. Tools -> create and package new IP.

2. Create a new AXI4 peripheral.

3. Name the Peripheral CUSTOM_GPIO

4. Add interfaces -> Lite, and number of Registers -> 16

5. IP Catalog -> AXI Peripheral -> CUSTOM_GPIO -> Right Click -> Edit in the Packager (opens a new project window)

6. Change the Verilog code as required. Package the IP, and add it to the Block.



## Lab 1

**generate the bitstream upto the point 10**

### Adding the BRAMs

1. Add the axi_bram_ctrl. 
- Number of BRAM interfaces = 1
- Make two instances of the axi_bram_ctrl

2. Add blk_mem_gen.
- Memory type = True Dual Port RAM
- Port A and Port B Options -> Write Depth = 8K Bytes.
- Disable _rsta_busy_ (as they are special pins used for diagnostics) by going to Other Options -> Untick the 'Enable Safty Circuit'.

3. Connect the Port A and Port B of the bram_ctrl to the blk_mem_gen.

4. Run Connection Automation.

5. We will see the S_AXI of axi_bram_ctrl_1 connected to M04_AXI of smartconnect_1 whos S02_AXI connects to M01_AXI of smartconnect_0. So we will remove both the wires, and connect the S_AXI of axi_bram_crtl_1 to M01_AXI of smartconnect_0.

6. Go to the address editor. 
[NOTE:] We can only edit the address of the components which are 'included'.
- The processing system (zynq_ultra) will have access to both the BRAM ports. We will make the zynq_ultra and axi_cdma, have one BRAM port each.
-  zynq_ultra will have the axi_bram_ctrl_0 and axi_cdma will have axi_bram_ctrl_1 included.
-  Make sure the addresses assigned are: axi_bram_ctrl_0: 0x00_A002_8000 and axi_bram_ctrl_1: 0x00_B002_8000.

7. Select all the three newly added components in the block, and create heirarchy naming it: BRAM_BLOCK.

8. Generate bitstream.

# Setting up the Board

## Enabling SSH access

1. sudo screen -L /dev/ttyUSB1 115200
2. wpa_passphrase SpectrumSetup-FE voyagegallon593
3. sudo ./wifi.sh sets up the wifi 
4. udhcpc: lease of **192.168.1.173** obtained, lease time 43200.

``` bash

#transfer system.bit and system.dtb files to the board.

```
5. Enable root access, and Mount the device to enable the boot access. 

``` bash
root@ultra96:/sys/firmware/devi sudo bash
root@ultra96: mount /dev/mmcblk0p1 BOOT/

Filesystem     1K-blocks     Used Available Use% Mounted on
/dev/root       29513292 15625940  12365100  56% /

```
``` bash

#copy system.bit and system.dtb files to /media/dev/.
root@ultra96:/media/BOOT# ls
bl31.bin   system.bit           u-boot.bin    uImage.7.19.21
bl31.elf   system.bit.2.2.2222  u-boot.elf    uImage.broken
BOOT.BIN   system.dtb           uboot.env     uImage.orig
boot.scr   system.dtb.2.2.2222  uEnv.txt      vmlinux
Image      system.dts           uEnv.txt.old  xxx.txt
image.ub   System.map.linux     uImage        zynqmp_fsbl.elf
pmufw.elf  system_orig.bit      uImage.5.4

```
6. fpgautil -b system.bit
7. Halt and restart the board. 
8. Check the device-tree at **/proc/device-tree/amba_pl@0/**-

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
# Testing

## Frequency Values

1. TEST #1: Vary CPU and FPGA [Frequency](#ps-frequency-table).
2. TEST #2: Vary only CPU [Frequency](#pl-frequency-table). 

The 5 frequnecy values for the CPU are-

I. 1499 MHz

II. 1333 MHz

III. 999 MHz

IV. 733 MHz

V. 416.6 MHz

Base frequency = 33.33 MHz

Hence the FBDIV and DIV2 values for each combination are given as

I. 1499 MHz, 1499/33.33 = 45 ---> Hence FBDIV = 45, DIV2 = 0

II. 1333 MHz, 1333/33.33 = 40 ---> Hence FBDIV = 40, DIV2 = 0

III. 999 MHz, 999/33.33 = 30 ---> Hence FBDIV = 30, DIV2 = 0

IV. 733 MHz, 733/33.33 = 22 ---> Hence FBDIV = 44, DIV2 = 2

V. 416.6 MHz, 416.6/33.33 = 12.5 ---> Hence FBDIV = 25, DIV2 = 1



## APLL_CTRL

|31|30|29|28| | |
|-|-|-|-|-|-|                      
|0|0|0|0|| **_0_**|

|27|26|25|24|||
|-|-|-|-|-|-|
|0|0|0|0||**_0_**|

|23|22|21|20|||
|-|-|-|-|-|-|
|0|0|0|0||**_0_**|

|19|18|17|16|||
|-|-|-|-|-|-|
|0|0|0|**0**||**_0_**|

|15|14|13|12|11|10|9|8||||
|-|-|-|-|-|-|-|-|-|-|-|
|0|**0**|**1**|**0**|**1**|**1**|**0**|**1**||**_2_**|**_D_**|

|7|6|5|4|||
|-|-|-|-|-|-|
|0|0|0|0||**_0_**|

|3|2|1|0|||
|-|-|-|-|-|-|
|**0**|0|0|**0**||**_0_**|

This makes the APLL_CTRL value as 0x0000_2D00

For the five frequency values, the register values should be set to the following-

I. 1499 MHz   = 0x0000_2D00 

II. 1333 MHz  = 0x0000_2800

III. 999 MHz  = 0x0000_1E00

IV. 733 MHz   = 0x0001_2C00

V. 416.6 MHz  = 0x0001_1900

|19|18|17|16|||
|-|-|-|-|-|-|
|0|0|0|**1**||**_1_**|


APLL_CFG values depend on the FBDIV factor. It is given as follows-

**45** -> 0xXXXX_XXX(7)0_0XXX_XXXX_XXX(10)0_XX(2)0X_XXX(4)0_XXXX(4)

LOCK_DLY(7) = 63

LOCK_CNT(10) = 825

LFHF(2) = 3

CP(4) = 3

RES(4) = 12

which gives us = 0x0111_1110_0110_0111_0010_1100_0110_1100 = 0x7E67_2C6C


**40** -> 0xXXXX_XXX(7)0_0XXX_XXXX_XXX(10)0_XX(2)0X_XXX(4)0_XXXX(4)

LOCK_DLY(7) = 63
 
LOCK_CNT(10) = 925

LFHF(2) = 3

CP(4) = 3

RES(4) = 12

which gives us = 0x0111_1110_0111_0011_1010_1100_0110_1100 = 0x7E73_AC6C


**30** -> 0xXXXX_XXX(7)0_0XXX_XXXX_XXX(10)0_XX(2)0X_XXX(4)0_XXXX(4)

LOCK_DLY(7) = 63
 
LOCK_CNT(10) = 1000

LFHF(2) = 3

CP(4) = 4

RES(4) = 6

which gives us = 0x0111_1110_0111_1101_0000_1100_1000_0110 = 0x7E7D_0C86


**44** -> 0xXXXX_XXX(7)0_0XXX_XXXX_XXX(10)0_XX(2)0X_XXX(4)0_XXXX(4)

LOCK_DLY(7) = 63

LOCK_CNT(10) = 850

LFHF(2) = 3

CP(4) = 3

RES(4) = 12

which gives us = 0x0111_1110_0110_1010_0100_1100_0110_1100 = 0x7E6A_4C6C


**35** -> 0xXXXX_XXX(7)0_0XXX_XXXX_XXX(10)0_XX(2)0X_XXX(4)0_XXXX(4)

LOCK_DLY(7) = 63
 
LOCK_CNT(10) = 1000

LFHF(2) = 3

CP(4) = 3

RES(4) = 10

which gives us = 0x0111_1110_0111_1101_0000_1100_0110_1010 = 0x7E7D_0C6A

# PS Frequency Table

|Frequency (MHz)|Factor| APLL_CTRL|APLL_CFG|
|-|-|-|-|
|1499|45 |0x0000_2D00|0x7E67_2C6C |
|1333|40 |0x0000_2800 | 0x7E73_AC6C|
|999|30 |0x0000_1E00 | 0x7E7D_0C86|
|733| 44| 0x0001_2C00| 0x7E6A_4C6C|
|416.6| 25| 0x0001_1900|0x7E7D_0C6A |


The 5 frequnecy values for the FPGA are-

I. 300 MHz

II. 250 MHz

III. 187.5 MHz

IV. 150 MHz

V. 100 MHz

Base frequency = 1500 MHz

Hence the DIV1 and DIV0 values for each combination are given as

I. 300 MHz, 1500/300 = 5 ---> Hence DIV1 = 1, DIV0 = 5

II. 250 MHz, 1500/250 = 6 ---> Hence DIV1 = 1, DIV0 = 6

III. 187.5 MHz, 1500/187.5 = 8 ---> Hence DIV1 = 1, DIV0 = 8

IV. 150 MHz, 1500/150 = 10 ---> Hence DIV1 = 1, DIV0 = 10

V. 100 MHz, 1500/100 = 15 ---> Hence DIV1 = 1, DIV0 = 15



## PL0_REF_CTRL

|31|30|29|28| | |
|-|-|-|-|-|-|                      
|0|0|0|0|| **_0_**|

|27|26|25|24|||
|-|-|-|-|-|-|
|0|0|0|0||**_0_**|

|23|22|21|20|19|18|17|16||||
|-|-|-|-|-|-|-|-|-|-|-|
|0|0|0|0|0|0|0|1||**_0_**|**_1_**|

|15|14|13|12|11|10|9|8||||
|-|-|-|-|-|-|-|-|-|-|-|
|0|0|0|0|0|1|1|0||**_0_**|**_6_**|


|7|6|5|4|||
|-|-|-|-|-|-|
|0|0|0|0||**_0_**|

|3|2|1|0|||
|-|-|-|-|-|-|
|**0**|0|0|**0**||**_0_**|


Hence, 

I. 300 MHz, 1500/300 = 5 ---> Hence DIV1 = 1, DIV0 = 5 ---> 0x0101_0500

II. 250 MHz, 1500/250 = 6 ---> Hence DIV1 = 1, DIV0 = 6 ---> 0x0101_0600

III. 187.5 MHz, 1500/187.5 = 8 ---> Hence DIV1 = 1, DIV0 = 8 ---> 0x0101_0800

IV. 150 MHz, 1500/150 = 10 ---> Hence DIV1 = 1, DIV0 = 10 ---> 0x0101_0A00

V. 100 MHz, 1500/100 = 15 ---> Hence DIV1 = 1, DIV0 = 15 ---> 0x0101_0F00


# PL Frequency Table
|Frequency (MHz)|DIV1| DIV0|PL0_REF_CTRL|
|-|-|-|-|
|300|1 |5|0x0101_0500 |
|250|1 |6 | 0x0101_0600|
|187.5|1 |8 | 0x0101_0800|
|150| 1| 10| 0x0101_0A00|
|100| 1| 15|0x0101_0F00 |

## Code for setting the register values

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>

#define PL0_REF_CTRL_ADD    0xFF5E0000

//register values for PS

#define PS_APLL_BASE        0xFD1A0000
#define APLL_CTRL_OFF       0x20
#define APLL_CFG_OFF        0x24
#define PLL_STATUS_OFF      0x44

#define PS_APLL_CTRL_1499   0x00002D00
#define PS_APLL_CTRL_1333   0x00002800
#define PS_APLL_CTRL_999    0x00001E00
#define PS_APLL_CTRL_733    0x00012C00
#define PS_APLL_CTRL_416_6  0x00011900

#define PS_APLL_CFG_1499    0x7E672C6C
#define PS_APLL_CFG_1333    0x7E73AC6C
#define PS_APLL_CFG_999     0x7E7D0C86
#define PS_APLL_CFG_733     0x7E6A4C6C
#define PS_APLL_CFG_416_6   0x7E7D0C6A

//register values for PL

#define PL_0_300            0x01010500
#define PL_0_250            0x01010600
#define PL_0_187_5          0x01010800
#define PL_0_150            0x01010A00
#define PL_0_100            0x01010F00

#define BRAM_ADD            0xA0028000
#define MAP_SIZE 	    131072UL
#define MAP_MASK	    (MAP_SIZE - 1)



void change_ps_freq(int dh){


    //changing PS Frequency
    
    int seed_ps= rand()%5;
    int g = 0;
    uint32_t *reg, *ps_clk_ctrl, *ps_clk_cfg, *ps_clk_status;

    reg = mmap(NULL, 
	       MAP_SIZE,
	       PROT_READ|PROT_WRITE,
	       MAP_SHARED, dh, PS_APLL_BASE  & ~MAP_MASK);
    
    ps_clk_ctrl = reg + (((PS_APLL_BASE + APLL_CTRL_OFF) & MAP_MASK) >> 2);

    ps_clk_cfg = reg + (((PS_APLL_BASE + APLL_CFG_OFF) & MAP_MASK) >> 2);

    ps_clk_status = reg + (((PS_APLL_BASE + PLL_STATUS_OFF) & MAP_MASK) >> 2);

    switch(seed_ps){

	    case 0:
		    //1499 MHz
		    //APLL_CTRL = 0x0000_2D00
		    //APLL_CFG  = 0x7E67_2C6C

		    //1. Program the new FIDIV, CLKOUT
		    *ps_clk_ctrl = PS_APLL_CTRL_1499;

		    unsigned int temp;
		    temp = *ps_clk_ctrl;
		    //2. Program the control data
		    *ps_clk_cfg  = PS_APLL_CFG_1499;
		    
		    unsigned int temp1;
		    temp1 = *ps_clk_cfg;
		    //3. Program the bypass -> APLL_CTRL[3] = 1;
		    *ps_clk_ctrl = (*ps_clk_ctrl) | (1<<3);

		    //4. Assert Reset -> APLL_CTRL[0] = 1;
		    *ps_clk_ctrl = (*ps_clk_ctrl) | 1;

		    //5. Deassert Reset -> APLL_CTRL[0] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFFE;

		    //6. Check for LOCK -> PLL_STATUS[0]
		    g = 0;
		    while((*ps_clk_status & 0x00000001) != 1){
		    	g++;
		    }
		    printf("waited %d loop cycles for STATUS = 1\n", g);

		    //7. Deassert the bypass -> APLL_CTRL[3] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFF7;

		    munmap(ps_clk_ctrl, 0x1000);

  		    printf("PS Frequncy changed to 1499 MHz\n");		    
		    break;
	    case 1:
		    //1333 MHz
		    //APLL_CTRL = 0x0000_2800
		    //APLL_CFG  = 0x7E73_AC6C


		    //1. Program the new FIDIV, CLKOUT
		    *ps_clk_ctrl = PS_APLL_CTRL_1333;

		    //2. Program the control data
		    *ps_clk_cfg  = PS_APLL_CFG_1333;

		    //3. Program the bypass -> APLL_CTRL[3] = 1;
		    *ps_clk_ctrl = (*ps_clk_ctrl) | (1<<3);

		    //4. Assert Reset -> APLL_CTRL[0] = 1;
		    *ps_clk_ctrl = (*ps_clk_ctrl) | 1;

		    //5. Deassert Reset -> APLL_CTRL[0] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFFE;

		    //6. Check for LOCK -> PLL_STATUS[0]
		    g = 0;
		    while((*ps_clk_status & 0x00000001) != 1){
		    	g++;
		    }
		    printf("waited %d loop cycles for STATUS = 1\n", g);

		    //7. Deassert the bypass -> APLL_CTRL[3] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFF7;

		    munmap(ps_clk_ctrl, 0x1000);

  		    printf("PS Frequncy changed to 1333 MHz\n");		    

		    break;
	    case 2:
		    //999 MHz
		    //APLL_CTRL = 0x0000_1E00
		    //APLL_CFG  = 0x7E7D_0C86


		    //1. Program the new FIDIV, CLKOUT
		    *ps_clk_ctrl = PS_APLL_CTRL_999;

		    //2. Program the control data
		    *ps_clk_cfg  = PS_APLL_CFG_999;

		    //3. Program the bypass -> APLL_CTRL[3] = 1;
		    *ps_clk_ctrl = (*ps_clk_ctrl) | (1<<3);

		    //4. Assert Reset -> APLL_CTRL[0] = 1;
		    *ps_clk_ctrl = (*ps_clk_ctrl) | 1;

		    //5. Deassert Reset -> APLL_CTRL[0] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFFE;

		    //6. Check for LOCK -> PLL_STATUS[0]
		    g = 0;
		    while((*ps_clk_status & 0x00000001) != 1){
		    	g++;
		    }
		    printf("waited %d loop cycles for STATUS = 1\n", g);

		    //7. Deassert the bypass -> APLL_CTRL[3] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFF7;

		    munmap(ps_clk_ctrl, 0x1000);


  		    printf("PS Frequncy changed to 999 MHz\n");		    
		    break;
	    case 3:
		    //733 MHz
		    //APLL_CTRL = 0x0001_2C00
		    //APLL_CFG  = 0x7E67_4C6C



		    //1. Program the new FIDIV, CLKOUT
		    *ps_clk_ctrl = PS_APLL_CTRL_733;

		    //2. Program the control data
		    *ps_clk_cfg  = PS_APLL_CFG_733;

		    //3. Program the bypass -> APLL_CTRL[3] = 1;
		    *ps_clk_ctrl = (*ps_clk_ctrl) | (1<<3);

		    //4. Assert Reset -> APLL_CTRL[0] = 1;
		    *ps_clk_ctrl = (*ps_clk_ctrl) | 1;

		    //5. Deassert Reset -> APLL_CTRL[0] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFFE;

		    //6. Check for LOCK -> PLL_STATUS[0]
		    g = 0;
		    while((*ps_clk_status & 0x00000001) != 1){
		    	g++;
		    }
		    printf("waited %d loop cycles for STATUS = 1\n", g);

		    //7. Deassert the bypass -> APLL_CTRL[3] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFF7;

		    munmap(ps_clk_ctrl, 0x1000);

  		    printf("PS Frequncy changed to 733 MHz\n");		    
		    break;
	    case 4:
		    //416.6 MHz
		    //APLL_CTRL = 0x0001_1900
		    //APLL_CFG  = 0x7E7D_0C6A


		    //1. Program the new FIDIV, CLKOUT
		    *ps_clk_ctrl = PS_APLL_CTRL_416_6;

		    //2. Program the control data
		    *ps_clk_cfg  = PS_APLL_CFG_416_6;

		    //3. Program the bypass -> APLL_CTRL[3] = 1;
		    *ps_clk_ctrl = (*ps_clk_ctrl) | (1<<3);

		    //4. Assert Reset -> APLL_CTRL[0] = 1;
		    *ps_clk_ctrl = (*ps_clk_ctrl) | 1;

		    //5. Deassert Reset -> APLL_CTRL[0] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFFE;

		    //6. Check for LOCK -> PLL_STATUS[0]
		    g = 0;
		    while((*ps_clk_status & 0x00000001) != 1){
		    	g++;
		    }
		    printf("waited %d loop cycles for STATUS = 1\n", g);

		    //7. Deassert the bypass -> APLL_CTRL[3] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFF7;

		    munmap(ps_clk_ctrl, 0x1000);

  		    printf("PS Frequncy changed to 416.6 MHz\n");		    
		    break;
    

    }
}
	
void change_pl_freq(int dh){

    //changing PL Frequency
    int seed_pl = rand() % 5;
    uint32_t *pl_clk_reg;
    uint32_t *pl0;
    pl_clk_reg = mmap(NULL, 
		     MAP_SIZE,
		     PROT_READ|PROT_WRITE,
		     MAP_SHARED, dh, PL0_REF_CTRL_ADD & ~MAP_MASK);
    pl0 = pl_clk_reg + (((PL0_REF_CTRL_ADD + 0xC0) & MAP_MASK) >> 2);
	            
    switch(seed_pl){


	    //address for PL0_REF_CTRL register = 0x00_FF5E_00C0;
	    case 0:
		    //300 MHz
		    //PL0_REF_CTRL = 0x0101_0500
 		    
		    *pl0 = PL_0_300;
			
		    unsigned int temp2;
		    temp2 = *pl0;
  		    printf("PL Frequncy changed to 300 MHz\n");		    
		    munmap(pl_clk_reg, 0x1000);

		    break;

	    case 1:
		    //250 MHz
		    //PL0_REF_CTRL = 0x0101_0600

		    *pl0 = PL_0_250;

		    munmap(pl_clk_reg, 0x1000);		  

  		    printf("PL Frequncy changed to 250 MHz\n");		    
		    break;
	    case 2:
		    //187.5 MHz
		    //PL0_REF_CTRL = 0x0101_0800

		    *pl0 = PL_0_187_5;

		    munmap(pl_clk_reg, 0x1000);	
	 	    
  		    printf("PL Frequncy changed to 187.5 MHz\n");		    
		    break;
	    case 3:
		    //150 MHz
		    //PL0_REF_CTRL = 0x0101_0A00

		    *pl0 = PL_0_150;

		    munmap(pl_clk_reg, 0x1000);	

  		    printf("PL Frequncy changed to 150 MHz\n");		    
		    break;
	    case 4:
		    //100 MHz
		    //PL0_REF_CTRL = 0x0101_0F00

		    *pl0 = PL_0_100;

		    munmap(pl_clk_reg, 0x1000);	

  		    printf("PL Frequncy changed to 100 MHz\n");		    
		    break;
    

    }
}

int main(int argc, char *argv[]) {

    volatile unsigned int *address;
    int seed_pl, seed_ps;
    int dh = open("/dev/mem", O_RDWR | O_SYNC); // Open /dev/mem which represents the whole physical memory
    
    if(dh == -1){
	    printf("Unable to open /dev/mem. Ensure if it exists.\n");
	    return -1;
    }

    if(argc>3){
	    printf("USAGE: ./test1 (yy words) (xx loops) \n");
	    return -1;
    }


    int xx = 1;   //default value of number of loops to run
    int yy = 2048;   //default value of number of words to test per loop

    if(argc == 2){
	    yy = strtoul(argv[1], 0, 0);   //taking number of words from the user
            printf("Changing the words/loop to yy = %d \n\n", yy);
    }
    if(argc == 3){ 
	    yy = strtoul(argv[1], 0, 0);   //taking number of words from the user
	    xx = strtoul(argv[2], 0, 0);   //taking number of loops from the user
            printf("Changing the number of loops to xx = %d\n", xx);
            printf("Changing the words/loop to yy = %d\n\n", yy);
    }

    

    //Generating random data and address
    uint32_t data;
    int addr_offset;    
    int LOOPS = xx;
    int count = 0;

    uint32_t* BRAM_virtual_address;
    srand(time(0));
    switch(argc){

    case 3:
	     
	    while(xx){

	    BRAM_virtual_address = mmap(NULL, 
						  MAP_SIZE, 
						  PROT_READ | PROT_WRITE, 
						  MAP_SHARED, 
						  dh, 
						  BRAM_ADD & ~MAP_MASK); // Memory map AXI Lite register block

	    for(int i = 0; i < yy; i++){
		    
		    change_ps_freq(dh);
		    change_pl_freq(dh);
		    
		    data = rand();
		    addr_offset = rand() % 2048;

		    address = BRAM_virtual_address + (((BRAM_ADD + addr_offset) & MAP_MASK) >>2);
		    
		    
		    *address = data;
		    
		    printf("Writing data: %d at address: 0x%.8x\n", *address, BRAM_ADD + addr_offset);

		    printf("randomization for loop %d completed!\n\n", data/*LOOPS - xx + 1*/);

			if(*address != data)
			{
			    printf("BRAM result = %d, random value written = %d at index = %d\n", *address, data, i);
			    printf("test failed!!\n");
			    munmap(BRAM_virtual_address, MAP_SIZE);
			    return -1;
			}
		    
	    }
		    xx--;
	    }
	    munmap(BRAM_virtual_address, MAP_SIZE);
   	    printf("Test passed: '%d' loops of '%d' 32-bit words\n", LOOPS, yy); 
	    break;
	default:
	    while(1){
		    
		    change_ps_freq(dh);
		    change_pl_freq(dh);
		    
		    BRAM_virtual_address = mmap(NULL, 
							  MAP_SIZE, 
							  PROT_READ | PROT_WRITE, 
							  MAP_SHARED, 
							  dh, 
							  BRAM_ADD & ~MAP_MASK); // Memory map AXI Lite register block
		    for(int i = 0; i < yy; i++){
		    data = rand();
		    addr_offset = rand() % 2048;

		    address = BRAM_virtual_address + (((BRAM_ADD + addr_offset) & MAP_MASK) >>2);
		    
		    
		    *address = data;
		    
		    printf("Writing data: %d at address: 0x%.8x\n", data, BRAM_ADD + addr_offset);

			unsigned int *reading_address = BRAM_virtual_address + (((BRAM_ADD + addr_offset) & MAP_MASK) >>2);
			if(*reading_address != data)
			{
			    printf("BRAM result = %d, random value written = %d on loop = %d\n", *reading_address, data, count);
			    printf("test failed!!\n");
			    munmap(BRAM_virtual_address, MAP_SIZE);
			    return -1;
			}
		    count++;
		    printf("Test passed: '%d' loops of '%d' 32-bit words\n\n", count, yy); 
	    }
	    }	  
	    munmap(BRAM_virtual_address, MAP_SIZE);
		    

    }


    return 0;
}
```
