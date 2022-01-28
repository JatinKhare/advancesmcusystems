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

6. 	
