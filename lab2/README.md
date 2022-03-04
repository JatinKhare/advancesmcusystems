 
# Lab 2: 
```
Name: Jatin Khare
UT EID: jk47976
Email: jatinkhare@utexas.edu
```


# Contents
1. [How to run the code](#how-to-run-the-code)
2. [Starting with Lab 2](#starting-with-lab-2)
3. [Setting up the Board](#setting-up-the-board)
4. [Setting the Frequency](#setting-the-frequency)
5. [Useful information for writing the code](#useful-information-for-writing-the-code)
6. [Codes](#codes)


## How to run the code

Once you have the .c files (test1.c, test2.c) in the same directory as the Makefile, do the following:

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
m: 0 (300 MHz), 1 (187.5 MHz), 3 (100 MHz), -1 (default PL frequency = 300 MHz)
(Loops): -1 (Default value = 500), maximum input value allowed = 10000
```bash
root@ultra96:~/labs/advmcu_codes/advancesmcusystems/lab2/codes# ./test1 2 -1 200
Setting loop number = 200
Setting PS Freq. to 416.6 MHz
PL Frequency: Enter number 0, 1, and 2 for setting PL Freq. to 300 MHz, 187.5 MHz, and 100 MHz respectively.
For now, setting it to 300 MHz..

DMA's OCM/BRAM traffic tests with 200 and 1024 words successful!!!

For OCM to BRAM:
Minimum Latency:    6601
Maximum Latency:    24878
Average Latency:    7721.000000
Standard Deviation: 1509.000000
Number of samples:  200

For BRAM to OCM:
Minimum Latency:    4869
Maximum Latency:    14799
Average Latency:    6263.000000
Standard Deviation: 1077.000000
Number of samples:  200
Total number of Interrupts for to-and-fro transfer: 400

```
### ./test2

USAGE: 

```bash

./test2 (n PS Freq) (m PL Freq) (Loops)

```
The test runs (input) transfers (1024 words each, to and fro from OCM->BRAM)

n and m values determine the PS-PL frequency combinations 

n: 0 (1499 MHz), 1 (999 MHz), 3 (416.6 MHz), -1 (default PS frequency = 1499 MHz)
m: 0 (300 MHz), 1 (187.5 MHz), 3 (100 MHz), -1 (default PL frequency = 300 MHz)
(Loops): -1 (Default value = 10000), maximum input value allowed = 10000

```bash
root@ultra96:~/labs/advmcu_codes/advancesmcusystems/lab2/codes# ./test2 0 2 1000
Setting loop number = 1000
Setting PS Freq. to 1499 MHz
Setting PL Freq. to 100 MHz

Minimum Latency:    452
Maximum Latency:    295891
Average Latency:    832.000000
Standard Deviation: 9343.000000
Number of samples:  1000
Number of Interrupts: 1000

```
## Starting with Lab 2

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

## When the GUI is open-
1. Open the project from lab1.
2. Create a new IP block for capture timer.
3. Integrate the new IP into the pre-existing block diagram and make the connections.
4. Generate the bitstream.


### IP Integration

1. Tools -> create and package new IP.

2. Create a new AXI4 peripheral.

3. Name the Peripheral CAPTURE_TIMER

4. Add interfaces -> Lite, and number of Registers -> 4

5. IP Catalog -> AXI Peripheral -> CAPTURE_TIMER -> Right Click -> Edit in the Packager (opens a new project window)

6. Design the state machine required for this project, find the verilog modules [here](verilog_codes/). Simulation of the module written can be performed by forcing constant values/clock cycles to the module ports. 

``` verilog
//Split view of register bits
slv_reg0 <= {16'hBEAD, 8'h0, timer_enable, 1'b0, 1'b0, capture_complete, 1'b0, 1'b0, 1'b0, capture_gate};
slv_reg1[31:2] <= {16'hFEED, 14'h0};
slv_reg2 <= Cap_Timer_Out[31:0];
slv_reg3 <= {28'h5555_CAB, 1'b0, current_state[2:0]};
```
``` verilog
//State assignment
parameter RESET = 3'b000,
parameter LOAD  = 3'b001,
parameter COUNT = 3'b010,
parameter WAIT  = 3'b011,
parameter IDLE  = 3'b100,
```

```bash 
error: [xsim 43-3268] logical library nameshould not contain white space, new line, /, \, = or . error: [xsim 43-3217] capture_timer_v1_0_vlog.prj (line 2): incorrect project file syntax. correct syntax is one of: vhdl <worklib> <file>, verilog <worklib> <file> [<file> ...] [[-d <macro>] ...] [[-i <include>] ...], or nosort. presence of nosort on a line of its own disables file order sorting. ~ ~ ~
```
Above mentioned error is a simulation bug (atleast with Vivado 2018.3), which can be fixed by running the following tcl command:

```bash
set_property library xil_defaultlib [get_files]
```
7. Repackage the IP and do not forget to Upgrade IP, and rerun the design.

8. Flow -> Generate Bitstrem -> Save the block design -> Synthesis and Implementation is out-of-date? -> Launch Runs.

9. Open reports under 'Design Runs' contain the Static Timing Analysis.

10. File -> Export -> Export Hardware -> include bit stream


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
8. Check the device-tree at **/proc/device-tree/amba_pl@0/ls**:

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
9. Once the bit file is loaded in the hardware, check for the debug register values

``` bash
root@ultra96:/proc/device-tree/amba_pl@0# dm 0xa0030000
0xa0030000 = 0xbead0000
root@ultra96:/proc/device-tree/amba_pl@0# dm 0xa0030004
0xa0030004 = 0xfeed0000
root@ultra96:/proc/device-tree/amba_pl@0# dm 0xa0030008
0xa0030008 = 0x00000000
root@ultra96:/proc/device-tree/amba_pl@0# dm 0xa003000c
0xa003000c = 0x5555cab0
```


# Setting the Frequency

## Frequency Values

Vary the frequencies using the method from Lab 1 [README.md](advancesmcusystems/lab1/README.md)

# Interrupt handling

## Writing and inserting a kernel module

1. Run the following script inside the kernel_modules directory-

```bash
#!/bin/sh

make clean
make

rm /dev/cdma_int
mknod /dev/cdma_int c 241 0

#rmmod cdma_int.ko
insmod cdma_int.ko

rm /dev/captimer_int
mknod /dev/captimer_int c 240 0

#rmmod captimer_int.ko
insmod captimer_int.ko

```

2. The mknod will make a device node inside the /dev/ and the insmod will insert the kernel module to the /proc/interrupts.
3. The 240 and 241 are the major number for the driver, and the 'c' stands for the character device.
4. The .ko is the object file for the kernel modules and can be formed using the Makefile provided.
5. We can check for the interrupt number to which the linux maps our hardware interrupts 240 and 241 to the application end by the following command-

``` bash
root@ultra96:~/labs/advmcu_codes/advancesmcusystems/lab2# more /proc/interrupts 
           CPU0       CPU1       CPU2       CPU3       
  3:      55746      74969      30077     172018     GICv2  30 Level     arch_timer
  6:          0          0          0          0     GICv2  67 Level     ff9905c0.mailbox
  7:          0          0          0          0     GICv2 175 Level     arm-pmu
  8:          0          0          0          0     GICv2 176 Level     arm-pmu
  9:          0          0          0          0     GICv2 177 Level     arm-pmu
 10:          0          0          0          0     GICv2 178 Level     arm-pmu
 12:          0          0          0          0     GICv2 156 Level     zynqmp-dma
 13:          0          0          0          0     GICv2 157 Level     zynqmp-dma
 14:          0          0          0          0     GICv2 158 Level     zynqmp-dma
 15:          0          0          0          0     GICv2 159 Level     zynqmp-dma
 16:          0          0          0          0     GICv2 160 Level     zynqmp-dma
 17:          0          0          0          0     GICv2 161 Level     zynqmp-dma
 18:          0          0          0          0     GICv2 162 Level     zynqmp-dma
 19:          0          0          0          0     GICv2 163 Level     zynqmp-dma
 20:          0          0          0          0     GICv2 164 Level     Mali_GP_MMU, Mali_GP, Mali_PP0_MMU, Mali_PP0, Mali_PP1_MMU, Mali_PP1
 21:          0          0          0          0     GICv2 109 Level     zynqmp-dma
 22:          0          0          0          0     GICv2 110 Level     zynqmp-dma
 23:          0          0          0          0     GICv2 111 Level     zynqmp-dma
 24:          0          0          0          0     GICv2 112 Level     zynqmp-dma
 25:          0          0          0          0     GICv2 113 Level     zynqmp-dma
 26:          0          0          0          0     GICv2 114 Level     zynqmp-dma
 27:          0          0          0          0     GICv2 115 Level     zynqmp-dma
 28:          0          0          0          0     GICv2 116 Level     zynqmp-dma
 31:          5          0          0          0     GICv2  50 Level     cdns-i2c
 32:          0          0          0          0     GICv2  42 Level     ff960000.memory-controller
 33:          0          0          0          0     GICv2  57 Level     axi-pmon
 34:          0          0          0          0     GICv2  58 Level     ffa60000.rtc
 35:          0          0          0          0     GICv2  59 Level     ffa60000.rtc
 36:      31292          0          0          0     GICv2  80 Level     mmc0
 37:     706998          0          0          0     GICv2  81 Level     mmc1
 38:          0          0          0          0     GICv2  51 Level     ff040000.spi
 39:          0          0          0          0     GICv2  52 Level     ff050000.spi
 41:        535          0          0          0     GICv2  54 Level     xuartps
 43:          0          0          0          0     GICv2  84 Edge      ff150000.watchdog
 44:          0          0          0          0     GICv2  88 Level     ams-irq
 45:          0          0          0          0     GICv2 154 Level     fd4c0000.dma
 46:          0          0          0          0     GICv2 151 Level     fd4a0000.zynqmp-display
 47:          0          0          0          0     GICv2  61 Level     ff9a0100.zynqmp_r5_rproc
 48:      75006          0          0          0     GICv2 124 Edge      captime_interrupt
 51:       9408          0          0          0     GICv2 123 Edge      cdma_interrupt
 71:         87          0          0          0     GICv2 102 Level     xhci-hcd:usb1
 75:          0          0          0          0  zynq-gpio  23 Edge      sw4
IPI0:     24773     281618      55990      96623       Rescheduling interrupts
IPI1:        18         11         15         20       Function call interrupts
IPI2:         0          0          0          0       CPU stop interrupts
IPI3:         0          0          0          0       CPU stop (for crash dump) interrupts
IPI4:     33637      34042      41189       9056       Timer broadcast interrupts
IPI5:       180          0          0          0       IRQ work interrupts
IPI6:         0          0          0          0       CPU wake-up interrupts
Err:          0

```
6. The kernel module needs to have the exact name of the '.compatible' as in the DTB.

7. Extract the DTS from the DTB using:

``` bash
$ dtc -I dtb -O dts system.dtb

```
``` bash
	Capture_Timer@a0030000 {
			clock-names = "s00_axi_aclk";
			clocks = <0x3 0x47>;
			compatible = "xlnx,Capture-Timer-1.0";
			interrupt-names = "interrupt_out";
			interrupt-parent = <0x4>;
			interrupts = <0x0 0x5c 0x4>;
			reg = <0x0 0xa0030000 0x0 0x10000>;
			xlnx,s00-axi-addr-width = <0x6>;
			xlnx,s00-axi-data-width = <0x20>;
		};


dma@b0000000 {
			#dma-cells = <0x1>;
			clock-names = "m_axi_aclk", "s_axi_lite_aclk";
			clocks = <0x3 0x47 0x3 0x47>;
			compatible = "xlnx,cdma_int";
			interrupt-names = "cdma_introut";
			interrupt-parent = <0x4>;
			interrupts = <0x0 0x5b 0x4>;
			reg = <0x0 0xb0000000 0x0 0x1000>;
			xlnx,addrwidth = <0x28>;

			dma-channel@b0000000 {
				compatible = "xlnx,axi-cdma-channel";
				interrupts = <0x0 0x5b 0x4>;
				xlnx,datawidth = <0x20>;
				xlnx,device-id = <0x0>;
				xlnx,max-burst-len = <0x20>;
			};

```

6. Now the SIGIO needs to be handled, which can be done using the signal_handler as follows

```c

void sigio_signal_handler(int signo){
	
	
	/* user code */
	sigio_signal_count ++;
	det_int = 1;
}

	struct sigaction sig_action;
	memset(&sig_action, 0, sizeof sig_action);
	sig_action.sa_handler = sigio_signal_handler;

	//Block all signals while our signal handler is executing:
	(void)sigfillset(&sig_action.sa_mask);

	rc = sigaction(SIGIO, &sig_action, NULL);

	if (rc == -1) {
		perror("sigaction() failed");
		return -1;
	}

	//Open the device file

	captime_dev_fd = open(CAPTIME_DEV_PATH, O_RDWR);
	if(captime_dev_fd == -1)    {
		perror("open() of " CAPTIME_DEV_PATH " failed");
		return -1;
	}

	//Set our process to receive SIGIO signals from the GPIO device:

	rc = fcntl(captime_dev_fd, F_SETOWN, getpid());

	if (rc == -1) {
		perror("fcntl() SETOWN failed\n");
		return -1;
	}

	//Enable reception of SIGIO signals for the captime_dev_fd descriptor

	int fd_flags = fcntl(captime_dev_fd, F_GETFL);
	rc = fcntl(captime_dev_fd, F_SETFL, fd_flags | O_ASYNC);

	if (rc == -1) {
		perror("fcntl() SETFL failed\n");
		return -1;
	}
```	

## Useful information for writing the code 

1. Saving the values in .csv file

``` c
void save_file(long unsigned *array){

FILE *fp;

char *filename = "data.csv";

fp = fopen(filename,"w+");

fprintf(fp,"Sample Cycles");

for(int i=0;i<xx;i++){

    fprintf(fp,",%ld ",array[i]);
    }

fclose(fp);
}

```
2. Useful constant values:

```c

#define CDMA                0xB0000000
#define BRAM                0xB0028000
#define OCM_1               0xFFFC0000
#define OCM_2               0xFFFC2000

#define SLV_REG_BASE        0xA0030000
#define SLV_REG_0_OFF       0x00000000
#define SLV_REG_1_OFF       0x00000004
#define SLV_REG_2_OFF       0x00000008
#define SLV_REG_3_OFF       0x0000000c

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

#define CDMACR              0x00           //CDMA Control
#define CDMASR              0x04           //Status
#define CURDESC_PNTR        0x08           //Current Descriptor Pointer
#define CURDESC_PNTR_MSB    0x0C           //Current Description Pointer MSB
#define TAILDESC_PNTR       0x10           //Tail Description Pointer
#define TAILDESC_PNTR_MSB   0x14           //Tail Description Pointer MSB
#define SA                  0x18           //Source Address
#define SA_MSB              0x1C           //Source Address MSB
#define DA                  0x20           //Destination Address
#define DA_MSB              0x24           //Destination Address MSB
#define BTT                 0x28           //Bytes to Transfer
#define OCM_MAP_SIZE        131072UL
#define OCM_MAP_MASK        (OCM_MAP_SIZE - 1)
#define MAP_SIZE            4096UL
#define MAP_MASK            (MAP_SIZE - 1)
#define MAP_SIZE_F          4096UL
#define MAP_MASK_F          (MAP_SIZE_F - 1)
#define CDMA_DEV_PATH       "/dev/cdma_int"
#define HIGHEST_MEAS_NUMBER         10000
//#define PRINT_COUNT

```
## Codes

1. [test1.c](codes/test1.c)
2. [test2.c](codes/test2.c)
  
