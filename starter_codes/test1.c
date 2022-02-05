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

//registervalues for PL

#define PL_0_300            0x01010500
#define PL_0_250            0x01010600
#define PL_0_187_5          0x01010800
#define PL_0_150            0x01010A00
#define PL_0_100            0x01010F00

#define BRAM_ADD            0xA0028000
#define MAP_SIZE 	    4096UL
#define MAP_MASK	    (MAP_SIZE - 1)
		
int main(int argc, char *argv[]) {

    volatile unsigned int *address;
    unsigned int value[6]; 
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
    int yy = 5;   //default value of number of words to test per loop

    if(argc == 2){
	    yy = strtoul(argv[1], 0, 0);   //taking number of words from the user
            printf("Changing the words/loop to yy = %d \n", yy);
    }
    if(argc == 3){ 
	    yy = strtoul(argv[1], 0, 0);   //taking number of words from the user
	    xx = strtoul(argv[2], 0, 0);   //taking number of loops from the user
            printf("Changing the number of loops to xx = %d\n", xx);
            printf("Changing the words/loop to yy = %d\n", yy);
    }

    
    //changing PS Frequency
    
    seed_ps= 0/*rand() % 5*/;
    int g = 0;
    uint32_t *reg, *ps_clk_ctrl, *ps_clk_cfg, *ps_clk_status;

    reg = mmap(NULL, 
	       0x1000,
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
		    printf("g = %d\n", g);

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
		    printf("g = %d\n", g);

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
		    printf("g = %d\n", g);

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
		    printf("g = %d\n", g);

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
		    printf("g = %d\n", g);

		    //7. Deassert the bypass -> APLL_CTRL[3] = 0;
		    *ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFF7;

		    munmap(ps_clk_ctrl, 0x1000);

  		    printf("PS Frequncy changed to 416.6 MHz\n");		    
		    break;
    

    }

    //changing PL Frequency
    seed_pl = rand() % 5;
    seed_pl = 0;
    uint32_t *pl_clk_reg;
    uint32_t *pl0;
    pl_clk_reg = mmap(NULL, 
		     0x1000,
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
    //Generating random data and address
    int r_data[yy];
    int r_addr_offset[yy];    
    int LOOPS = xx;


    switch(argc){

	    case 3:
		    while(xx){
			    for(int i = 0; i < yy; i++){
				    r_data[i] = rand();
				    r_addr_offset[i] = rand() % 4097;
				    printf("Writing data: %d at address: 0x%.8x\n", r_data[i], BRAM_ADD + r_addr_offset[i]);
			    }	    
			    printf("randomization for loop %d completed!\n", LOOPS - xx + 1);
			    xx--;
		    }
		    
		    break;

            default:
		    while(xx){
			    xx--;
		    }
		    break;

    }

    uint32_t* BRAM_virtual_address = mmap(NULL, 
                                          4096, 
                                          PROT_READ | PROT_WRITE, 
                                          MAP_SHARED, 
                                          dh, 
                                          BRAM_ADD & ~MAP_MASK); // Memory map AXI Lite register block
    printf("memory writing\n");  
    int offset = 0;
    for(int i = 0; i < 6; i++){

	    printf("Writing at: 0x%.8x", BRAM_ADD + offset);
	    address = BRAM_virtual_address + (((BRAM_ADD + offset) & MAP_MASK) >>2);
            value[i] = 7;
	    printf(", the data %d, ", value[i]);
	    *address = value[i];
	    printf("Written: %d\n", *address);
	    offset = i + 4;
	    
    }

    offset = 0;
    for(int i=0; i<6; i++)
    {
	unsigned int *reading_address = BRAM_virtual_address + (((BRAM_ADD + offset) & MAP_MASK) >>2);
        if(*reading_address != value[i])
        {
            printf("BRAM result = %d, random value written = %d at offset = %d\n", *reading_address, value[i], i);
            printf("test failed!!\n");
            munmap(BRAM_virtual_address,4096);
            return -1;
        }
	offset = i + 4;
    }
    printf("test passed!!\n");
    munmap(BRAM_virtual_address,4096);
    return 0;
}
