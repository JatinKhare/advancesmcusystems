//Testing of the DMA traffic to/from the OCM and BRAM
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
#define BRAM                0xB0030000
#define OCM_1               0xFFFC0000
#define OCM_2               0xFFFC2000

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

//DMA Set

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
    
    printf("Transfering data from OCM to BRAM\n");	
    dma_set(cdma_virtual_address, DA, BRAM); // Write destination address
    dma_set(cdma_virtual_address, SA, OCM_1); // Write source address
    dma_set(cdma_virtual_address, BTT, length*4);
    cdma_sync(cdma_virtual_address);
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
			   OCM_1);

    uint32_t* ocm_2 = mmap(NULL, 
			   OCM_MAP_SIZE, 
			   PROT_READ | PROT_WRITE, 
			   MAP_SHARED, 
			   dh, 
			   OCM_2);

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
		    uint32_t data[yy];
		    srand(time(0));
		    for(int i=0; i<yy; i++)
		    {
		     printf("ocm1[%d] = x%.8x, ocm2[%d] = x%.8x\n", i, ocm_1[i], i, ocm_2[i]);
		    }
		    printf("\n");
		    for(int i = 0; i < yy; i++){
			    data[i] = rand();
			    printf("data[%d] = x%.8x\n", i, data[i]);
		    }

		    printf("\n");

		    for(int i=0; i<yy; i++)
			*(ocm_1 + i) = data[i];
		    
		    for(int i=0; i<yy; i++)
		    {
		     printf("ocm1 = %p: ocm1[%d] = x%.8x, ocm2 = %p: ocm2[%d] = x%.8x\n",ocm_1+i, i, *(ocm_1+i), ocm_2+i, i, *(ocm_2 + i));
		    }
		    printf("\n");
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
		     printf("ocm1[%d] = x%.8x, ocm2[%d] = x%.8x\n", i, ocm_1[i], i, ocm_2[i]);

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
