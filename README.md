# advancesmcusystems

## How to run the code

Once you have the .c files (test1.c, test2.c, and test3.c) in the same directory as the Makefile, do the following:

``` bash
$ make clean
$ make 
```
This will generate the corresponding object files and executables for all the three test cases. 

### ./test1

USAGE: ./test1 (xx loops) (yy words), where pass 0 for default values.

default value for loops: INF

default value for words: 2048

### ./test2

USAGE: ./test2 (xx loops) (yy words), where pass 0 for default values.

default value for loops: INF

default value for words: 4096

### ./test3

USAGE: ./test3
no arguments needed to pass.


# Contents
1. [Setting up the environment](#setting-up-the-environment)
2. [Lab 1](#lab-1)
3. [Setting up the Board](#setting-up-the-board)
4. [Setting the Frequency](#setting-the-frequency)
5. [Useful information for writing the code](#useful-information-for-writing-the-code)
6. [Codes](#codes)


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
2. wpa_passphrase SpectrumSetup-FE PASSWORD
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
# Setting the Frequency

## Frequency Values

1. TEST #1: Vary CPU and [FPGA Frequency](#pl-frequency-table).
2. TEST #2: Vary only [CPU Frequency](#ps-frequency-table). 

# PS Frequency

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

## APLL_CFG

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

# PL Frequency

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

## Useful information for writing the code 

#test1

1. PL0_REF_CTRL_ADD         0xFF5E0000
2. PS_APLL_BASE             0xFD1A0000, with an offset of 0x20, 0x24, 0x44 for CTRL, CFG, STATUS.
3. BRAM_ADD                 0xA0028000

#test2

1. PL0_REF_CTRL_ADD         0xFF5E0000
2. PS_APLL_BASE             0xFD1A0000, with an offset of 0x20, 0x24, 0x44 for CTRL, CFG, STATUS.
3. OCM_ADD                  0xFFFC000

#test3

1.                       OCM1 (0xFFFC0000) --- |      | --- BRAM (0xB0028000)                  
                                               |      |                                        
                                               | CDMA |                                      
                                               |      |                                        
                         BRAM (0xB0028000) --- |      | --- OCM2 (0xFFFC2000)                 

2. 
```c

#define CDMACR              0x00           //CDMA Control
#define CDMASR              0x04           //Status
#define CURDESC_PNTR        0x08           //Current Descriptor Pointer
#define CURDESC_PNTR_MSB    0x0C           //Current Description Pointer MSB
#define TAILDESC_PNTR       0x10	   //Tail Description Pointer
#define TAILDESC_PNTR_MSB   0x14           //Tail Description Pointer MSB
#define SA                  0x18           //Source Address
#define SA_MSB              0x1C           //Source Address MSB
#define DA                  0x20           //Destination Address
#define DA_MSB              0x24           //Destination Address MSB
#define BTT                 0x28           //Bytes to Transfer
#define OCM_MAP_SIZE        65536UL
#define MAP_SIZE            65536UL
#define MAP_MASK            (MAP_SIZE - 1)

```

## Codes

1. [test1.c](codes/test1.c)
2. [test2.c](codes/test2.c)
3. [test3.c](codes/test3.c)
