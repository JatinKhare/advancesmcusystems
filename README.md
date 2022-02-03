# advancesmcusystems

## 1. Setting up the environment

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
root@ultra96:/sys/firmware/devi sudi bash
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



### Testing
