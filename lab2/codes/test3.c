
//
//
/**********************************************************************************************/
/*                                            |      |                                        */
/*                      OCM1 (0xFFFC0000) --- | CDMA | --- BRAM (0xB0028000)                  */
/*                                            |      |                                        */
/*                                                                                            */
/*                                            |      |                                        */
/*                      BRAM (0xB0028000) --- | CDMA | --- OCM2 (0xFFFC2000)                  */
/*                                            |      |                                        */
/*                                                                                            */
/**********************************************************************************************/


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
#include <signal.h>

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
#define TAILDESC_PNTR       0x10	   //Tail Description Pointer
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
//DMA Set

uint32_t *slv_reg_base, *slv_reg0, *slv_reg1, *slv_reg2, *slv_reg3;
unsigned int dma_set(unsigned int* dma_virtual_address, int offset, unsigned int value) {
    dma_virtual_address[offset>>2] = value;
    return 0;
}

//DMA Get

unsigned int dma_get(unsigned int* dma_virtual_address, int offset) {
    return dma_virtual_address[offset>>2];
}

//CDMA Sync

int cdma_sync(unsigned int* dma_virtual_address) {
    unsigned int status = dma_get(dma_virtual_address, CDMASR);
    if( (status&0x40) != 0)
    {
        //unsigned int desc = dma_get(dma_virtual_address, CURDESC_PNTR);
        //printf("error address : %X\n", desc);
    }
    while(!(status & 1<<1)){
        status = dma_get(dma_virtual_address, CDMASR);
    }
    return 0;
}

void memdump(void* virtual_address, int byte_count) {
    char *p = virtual_address;
    int offset;
    for (offset = 0; offset < byte_count; offset++) {
        printf("%02x", p[offset]);
        if (offset % 4 == 3) { printf(" "); }
    }
    printf("\n");
}

//Transfer from OCM to BRAM

void transfer(unsigned int *cdma_virtual_address, int length){
    
    //set TE and capture_complete
    printf("Transfering data from OCM to BRAM\n");	
    dma_set(cdma_virtual_address, DA, BRAM); // Write destination address
    dma_set(cdma_virtual_address, SA, OCM_1); // Write source address
    dma_set(cdma_virtual_address, BTT, length*4);
    cdma_sync(cdma_virtual_address);
}

void reset_TE(){
   //set the TE = 1
   *slv_reg1 = *slv_reg1 & 0xFFFFFFFD; 
}
void set_TE(){
   //set the TE = 1
   *slv_reg1 = *slv_reg1 | 2; 
}

int capture_complete(){
	int count_value;
   //check for the capture_complete flag
   if((*slv_reg0 >> 4) & 1){
	count_value = *slv_reg2;
	reset_TE();
	return count_value;
   }
   return -1;

}
//Transfer from BRAM to OCM

void transfer_back(unsigned int *cdma_virtual_address, int length){

    printf("Transfering data from BRAM to OCM\n");	
    dma_set(cdma_virtual_address, DA, OCM_2); // Write destination address
    dma_set(cdma_virtual_address, SA, BRAM); // Write source address
    dma_set(cdma_virtual_address, BTT, length*4);
    cdma_sync(cdma_virtual_address);
}

uint32_t* cdma_virtual_address;
uint32_t* BRAM_virtual_address;
void m_unmap_ctrl_c(int sig_num){

        munmap(cdma_virtual_address, MAP_SIZE);
        munmap(BRAM_virtual_address, MAP_SIZE);
	printf("\n\nmunmap() done!\nNow terminating the process with grace...\n\n");
	kill(0,SIGKILL);
}


void change_ps_freq(int dh){


    //changing PS Frequency

    int seed_ps= rand()%5;
    int g = 0;
    uint32_t *reg, *ps_clk_ctrl, *ps_clk_cfg, *ps_clk_status;

    reg = mmap(NULL,
	       MAP_SIZE_F,
	       PROT_READ|PROT_WRITE,
	       MAP_SHARED, dh, PS_APLL_BASE  & ~MAP_MASK_F);

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

		    //2. Program the control data
		    *ps_clk_cfg  = PS_APLL_CFG_1499;

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

		    munmap(ps_clk_ctrl, MAP_SIZE_F);

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

		    munmap(ps_clk_ctrl, MAP_SIZE_F);

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

		    munmap(ps_clk_ctrl, MAP_SIZE_F);


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

		    munmap(ps_clk_ctrl, MAP_SIZE_F);

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

		    munmap(ps_clk_ctrl, MAP_SIZE_F);

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
		     MAP_SIZE_F,
		     PROT_READ|PROT_WRITE,
		     MAP_SHARED, dh, PL0_REF_CTRL_ADD & ~MAP_MASK_F);
    pl0 = pl_clk_reg + (((PL0_REF_CTRL_ADD + 0xC0) & MAP_MASK) >> 2);

    switch(seed_pl){


	    //address for PL0_REF_CTRL register = 0x00_FF5E_00C0;
	    case 0:
		    //300 MHz
		    //PL0_REF_CTRL = 0x0101_0500

		    *pl0 = PL_0_300;

  		    printf("PL Frequncy changed to 300 MHz\n");
		    munmap(pl_clk_reg, MAP_SIZE_F);

		    break;

	    case 1:
		    //250 MHz
		    //PL0_REF_CTRL = 0x0101_0600

		    *pl0 = PL_0_250;

		    munmap(pl_clk_reg, MAP_SIZE_F);

  		    printf("PL Frequncy changed to 250 MHz\n");
		    break;
	    case 2:
		    //187.5 MHz
		    //PL0_REF_CTRL = 0x0101_0800

		    *pl0 = PL_0_187_5;

		    munmap(pl_clk_reg, MAP_SIZE_F);

  		    printf("PL Frequncy changed to 187.5 MHz\n");
		    break;
	    case 3:
		    //150 MHz
		    //PL0_REF_CTRL = 0x0101_0A00

		    *pl0 = PL_0_150;

		    munmap(pl_clk_reg, MAP_SIZE_F);

  		    printf("PL Frequncy changed to 150 MHz\n");
		    break;
	    case 4:
		    //100 MHz
		    //PL0_REF_CTRL = 0x0101_0F00

		    *pl0 = PL_0_100;

		    munmap(pl_clk_reg, MAP_SIZE_F);

  		    printf("PL Frequncy changed to 100 MHz\n");
		    break;


    }
}


int main(int argc, char *argv[]) {

    signal(SIGINT, m_unmap_ctrl_c);
    int dh = open("/dev/mem", O_RDWR | O_SYNC); // Open /dev/mem which represents the whole physical memory
    
    if(dh == -1){
	    printf("Unable to open /dev/mem. Ensure if it exists.\n");
	    return -1;
    }

    if(argc!=3 && argc!=1){
	    printf("USAGE: ./test1 (xx loops) (yy words), and pass 0 to run with default values\n");
	    return -1;
    }


    int xx = 0;   //default value of number of loops to run
    int yy = 1024;   //default value of number of words to test per loop
    int sw = 1;
    if(argc == 2){
	    xx = strtoul(argv[1], 0, 0);   //taking number of words from the user
            printf("Changing the words/loop to yy = %d \n\n", yy);
    }
    if(argc == 3){ 
	    xx = strtoul(argv[1], 0, 0);   //taking number of words from the user
	    yy = strtoul(argv[2], 0, 0);   //taking number of loops from the user
            printf("Changing the number of loops to xx = %d\n", xx);
            printf("Changing the words/loop to yy = %d\n\n", yy);
	    if(yy == 0)
		    yy = 1024;
    }
    if(xx == 0)
	    sw = 0;

    
    uint32_t* ocm_1 = mmap(NULL, 
			   OCM_MAP_SIZE, 
			   PROT_READ | PROT_WRITE, 
			   MAP_SHARED, 
			   dh, 
			   OCM_1 & ~OCM_MAP_MASK);

    uint32_t* ocm_2 = mmap(NULL, 
			   OCM_MAP_SIZE, 
			   PROT_READ | PROT_WRITE, 
			   MAP_SHARED, 
			   dh, 
			   OCM_2 & ~OCM_MAP_MASK);
   

    slv_reg_base = mmap(NULL,
               MAP_SIZE,
               PROT_READ|PROT_WRITE,
               MAP_SHARED, dh, SLV_REG_BASE  & ~MAP_MASK);

    slv_reg0 = slv_reg_base + (((SLV_REG_BASE + SLV_REG_0_OFF) & MAP_MASK) >> 2);
    slv_reg1 = slv_reg_base + (((SLV_REG_BASE + SLV_REG_1_OFF) & MAP_MASK) >> 2);
    slv_reg2 = slv_reg_base + (((SLV_REG_BASE + SLV_REG_2_OFF) & MAP_MASK) >> 2);
    slv_reg3 = slv_reg_base + (((SLV_REG_BASE + SLV_REG_3_OFF) & MAP_MASK) >> 2);
    printf("slv_reg0 = 0x%.8x \n", *slv_reg0);
    printf("slv_reg1 = 0x%.8x \n", *slv_reg1);
    printf("slv_reg2 = 0x%.8x \n", *slv_reg2);
    printf("slv_reg3 = 0x%.8x \n", *slv_reg3);
    //Generating random data and address
    int count = 0;
    int LOOPS = xx;
    srand(time(0));
    switch(sw){
	    case 1:
		    cdma_virtual_address = mmap(NULL, 
							  MAP_SIZE, 
							  PROT_READ | PROT_WRITE, 
							  MAP_SHARED, 
							  dh, 
							  CDMA & ~MAP_MASK); // Memory map AXI Lite register block
		    BRAM_virtual_address = mmap(NULL, 
							  MAP_SIZE, 
							  PROT_READ | PROT_WRITE, 
							  MAP_SHARED, 
							  dh, 
							  BRAM & ~MAP_MASK); // Memory map AXI Lite register block
	    while(xx){
		    //change_ps_freq(dh);
		    //change_pl_freq(dh);
		    uint32_t data[yy];
		    srand(time(0));
		    for(int i = 0; i < yy; i++){
			    data[i] = rand();
		    }


		    for(int i=0; i<yy; i++)
			ocm_1[i] = data[i];
		    
		    // RESET DMA
		    dma_set(cdma_virtual_address, CDMACR, 0x04);
		   
		    //struct timeval start, end;
		    //printf("Source memory block:      "); memdump(cdma_virtual_address, yy);
		    //printf("Destination memory block: "); memdump(BRAM_virtual_address, yy);

		    set_TE();
		    transfer(cdma_virtual_address, yy);
		    printf("slv_reg0_at = 0x%.8x \n", *slv_reg0);
		    printf("slv_reg1_at = 0x%.8x \n", *slv_reg1);
		    printf("slv_reg2_at = 0x%.8x \n", *slv_reg2);
		    printf("slv_reg3_at = 0x%.8x \n", *slv_reg3);
		    int val, i=0;
		    reset_TE();
		    //while(capture_complete()==-1){
			//    i++;//waiting for the transfer to complete
		    //}

		    printf("slv_reg0_at = 0x%.8x \n", *slv_reg0);
		    printf("slv_reg1_at = 0x%.8x \n", *slv_reg1);
		    printf("slv_reg2_at = 0x%.8x \n", *slv_reg2);
		    printf("slv_reg3_at = 0x%.8x \n", *slv_reg3);
		    val = *slv_reg2;
		    printf("Count = %d, i = %d\n", val, i);
		    printf("OCM to BRAM: Transfer of %d words successful!\n\n", yy);

		    transfer_back(cdma_virtual_address, yy);
		    printf("BRAM to OCM: Transfer of %d words successful!\n\n", yy);
		 
		    //for(int i = 0; i<yy; i++){
		    //	    printf("ocm1[%d] = %d, ocm2[%d] = %d\n", i, ocm_1[i], i, ocm_2[i]);
		    //}
		    
		    for(int i=0; i<yy; i++)
		    {
			if(ocm_2[i] != ocm_1[i])
			{
			    printf("\nTest failed!!\n");
			    printf("OCM2 result: %d and OCM1 result is %d for the element %d\n", ocm_2[i], ocm_1[i], i+1);
			    munmap(ocm_1, OCM_MAP_SIZE);
			    munmap(ocm_2, OCM_MAP_SIZE);
			    munmap(cdma_virtual_address, MAP_SIZE);
			    munmap(BRAM_virtual_address, MAP_SIZE);
			    return -1;
			}
		    }
		   
		    printf("\nDMA's OCM/BRAM traffic test loop %d with %d words successful!!!\n", LOOPS - xx + 1, yy);
		    xx--;
		   
	    }
		    munmap(ocm_1, OCM_MAP_SIZE);
		    munmap(ocm_2, OCM_MAP_SIZE);
		    munmap(cdma_virtual_address, MAP_SIZE);
		    munmap(BRAM_virtual_address, MAP_SIZE);
		    break;

	    case 0:
	    while(1){
		    cdma_virtual_address = mmap(NULL, 
							  MAP_SIZE, 
							  PROT_READ | PROT_WRITE, 
							  MAP_SHARED, 
							  dh, 
							  CDMA & ~MAP_MASK); // Memory map AXI Lite register block
		    BRAM_virtual_address = mmap(NULL, 
							  MAP_SIZE, 
							  PROT_READ | PROT_WRITE, 
							  MAP_SHARED, 
							  dh, 
							  BRAM & ~MAP_MASK); // Memory map AXI Lite register block
		    uint32_t data[yy];
		    srand(time(0));
		    for(int i = 0; i < yy; i++){
			    data[i] = rand();
		    }


		    for(int i=0; i<yy; i++)
			ocm_1[i] = data[i];
		    
		    // RESET DMA
		    dma_set(cdma_virtual_address, CDMACR, 0x04);
		   
		    //struct timeval start, end;
		    //printf("Source memory block:      "); memdump(cdma_virtual_address, yy);
		    //printf("Destination memory block: "); memdump(BRAM_virtual_address, yy);
		    transfer(cdma_virtual_address, yy);
		    printf("OCM to BRAM: Transfer of %d words successful!\n\n", yy);

		    transfer_back(cdma_virtual_address, yy);
		    printf("BRAM to OCM: Transfer of %d words successful!\n\n", yy);
		 
		    //for(int i = 0; i<yy; i++){
		    //	    printf("ocm1[%d] = %d, ocm2[%d] = %d\n", i, ocm_1[i], i, ocm_2[i]);
		    //}
		    
		    for(int i=0; i<yy; i++)
		    {
			if(ocm_2[i] != ocm_1[i])
			{
			    printf("\nTest failed!!\n");
			    printf("OCM2 result: %d and OCM1 result is %d for the element %d\n", ocm_2[i], ocm_1[i], i+1);
			    munmap(ocm_1, OCM_MAP_SIZE);
			    munmap(ocm_2, OCM_MAP_SIZE);
			    munmap(cdma_virtual_address, MAP_SIZE);
			    munmap(BRAM_virtual_address, MAP_SIZE);
			    return -1;
			}
		    }
		    count++;
		    printf("\nDMA's OCM/BRAM traffic test loop %d with %d words successful!!!\n", count, yy);
		    munmap(cdma_virtual_address, MAP_SIZE);
		    munmap(BRAM_virtual_address, MAP_SIZE);
		   
	    }
		    munmap(ocm_1, OCM_MAP_SIZE);
		    munmap(ocm_2, OCM_MAP_SIZE);
		    break;

    }

    return 0;
}
