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

# When the GUI is open-
open the project: *_ultra96v2_oob.xpr_*





