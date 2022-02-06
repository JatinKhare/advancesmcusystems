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

#define CDMA                0xB0000000
#define BRAM                0xB0028000
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
#define OCM_MAP_SIZE        65536UL
#define MAP_SIZE            65536UL
#define MAP_MASK            (MAP_SIZE - 1)

//DMA Set

unsigned int dma_set(unsigned int* dma_virtual_address, int offset, unsigned int value) {
    dma_virtual_address[offset>>2] = value;
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
        unsigned int desc = dma_get(dma_virtual_address, CURDESC_PNTR);
        printf("error address : %X\n", desc);
    }
    while(!(status & 1<<1)){
        status = dma_get(dma_virtual_address, CDMASR);
    }
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

int main() {
    int dh = open("/dev/mem", O_RDWR | O_SYNC); // Open /dev/mem which represents the whole physical memory
    uint32_t* cdma_virtual_address = mmap(NULL, 
                                          MAP_SIZE, 
                                          PROT_READ | PROT_WRITE, 
                                          MAP_SHARED, 
                                          dh, 
                                          CDMA & ~MAP_MASK); // Memory map AXI Lite register block
    uint32_t* BRAM_virtual_address = mmap(NULL, 
                                          MAP_SIZE, 
                                          PROT_READ | PROT_WRITE, 
                                          MAP_SHARED, 
                                          dh, 
                                          BRAM & ~MAP_MASK); // Memory map AXI Lite register block
    int yy = 1028;
    uint32_t data[yy];
    srand(time(0));
    for(int i = 0; i < yy; i++){
	    data[i] = rand();
    }

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
            munmap(BRAM_virtual_address,MAP_SIZE);
            return -1;
        }
    }
   
    printf("\nDMA's OCM/BRAM traffic test with %d words successful!!!\n", yy);
    munmap(ocm_1, OCM_MAP_SIZE);
    munmap(ocm_2, OCM_MAP_SIZE);
    munmap(cdma_virtual_address, MAP_SIZE);
    munmap(BRAM_virtual_address,MAP_SIZE);
   
    return 0;
}
