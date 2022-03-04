#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <sys/mman.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/version.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>


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
#define CAPTIME_DEV_PATH    "/dev/captimer_int"
#define NUM_MEASUREMENTS 10000
#define STAT_MEASUREMENTS 1000
//#define PRINT_COUNT

void sigio_signal_handler(int signo);
volatile int rc;
static volatile sig_atomic_t sigio_signal_processed = 0;
volatile int sigio_signal_count = 0;
int captime_dev_fd  = -1;
int det_int;
sigset_t signal_mask, signal_mask_old, signal_mask_most;
unsigned long intr_latency_measurements[NUM_MEASUREMENTS];
unsigned long min_latency_measurements[STAT_MEASUREMENTS];
unsigned long max_latency_measurements[STAT_MEASUREMENTS];
void compute_interrupt_latency_stats(
                unsigned long   *min_latency_p,
                unsigned long   *max_latency_p,
                double          *average_latency_p,
                double          *std_deviation_p);

void save_file(long unsigned *array, int size);

int xx = 10000;

uint32_t *slv_reg_base, *slv_reg0, *slv_reg1, *slv_reg2, *slv_reg3;

void m_unmap_ctrl_c(int sig_num){

	munmap(slv_reg_base, MAP_SIZE);
	printf("\n\nmunmap() done!\nNow terminating the process with grace...\n\n");
	kill(0,SIGKILL);
}


void change_ps_freq(int dh, int n){

	//changing PS Frequency

	int g = 0;
	uint32_t *reg, *ps_clk_ctrl, *ps_clk_cfg, *ps_clk_status;

	reg = mmap(NULL,
			MAP_SIZE_F,
			PROT_READ|PROT_WRITE,
			MAP_SHARED, dh, PS_APLL_BASE  & ~MAP_MASK_F);

	ps_clk_ctrl = reg + (((PS_APLL_BASE + APLL_CTRL_OFF) & MAP_MASK) >> 2);

	ps_clk_cfg = reg + (((PS_APLL_BASE + APLL_CFG_OFF) & MAP_MASK) >> 2);

	ps_clk_status = reg + (((PS_APLL_BASE + PLL_STATUS_OFF) & MAP_MASK) >> 2);

	switch(n){

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
#ifdef PRINT_COUNT
			printf("waited %d loop cycles for STATUS = 1\n", g);
#endif

			//7. Deassert the bypass -> APLL_CTRL[3] = 0;
			*ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFF7;

			munmap(ps_clk_ctrl, MAP_SIZE_F);

#ifdef PRINT_COUNT
			printf("PS Frequncy changed to 1499 MHz\n");
#endif
			break;
		case 1:
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
#ifdef PRINT_COUNT
			printf("waited %d loop cycles for STATUS = 1\n", g);
#endif

			//7. Deassert the bypass -> APLL_CTRL[3] = 0;
			*ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFF7;

			munmap(ps_clk_ctrl, MAP_SIZE_F);


#ifdef PRINT_COUNT
			printf("PS Frequncy changed to 999 MHz\n");
#endif
			break;
		case 2:
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
#ifdef PRINT_COUNT
			printf("waited %d loop cycles for STATUS = 1\n", g);
#endif

			//7. Deassert the bypass -> APLL_CTRL[3] = 0;
			*ps_clk_ctrl = (*ps_clk_ctrl) & 0xFFFFFFF7;

			munmap(ps_clk_ctrl, MAP_SIZE_F);

#ifdef PRINT_COUNT
			printf("PS Frequncy changed to 416.6 MHz\n");
#endif
			break;

		default:
			break;
	}
}


void change_pl_freq(int dh, int m){

	//changing PL Frequency
	uint32_t *pl_clk_reg;
	uint32_t *pl0;
	pl_clk_reg = mmap(NULL,
			MAP_SIZE_F,
			PROT_READ|PROT_WRITE,
			MAP_SHARED, dh, PL0_REF_CTRL_ADD & ~MAP_MASK_F);
	pl0 = pl_clk_reg + (((PL0_REF_CTRL_ADD + 0xC0) & MAP_MASK) >> 2);

	switch(m){


		//address for PL0_REF_CTRL register = 0x00_FF5E_00C0;
		case 0:
			//300 MHz
			//PL0_REF_CTRL = 0x0101_0500

			*pl0 = PL_0_300;

#ifdef PRINT_COUNT
			printf("PL Frequncy changed to 300 MHz\n");
#endif
			munmap(pl_clk_reg, MAP_SIZE_F);

			break;

		case 1:
			//187.5 MHz
			//PL0_REF_CTRL = 0x0101_0800

			*pl0 = PL_0_187_5;

			munmap(pl_clk_reg, MAP_SIZE_F);

#ifdef PRINT_COUNT
			printf("PL Frequncy changed to 187.5 MHz\n");
#endif
			break;
		case 2:
			//100 MHz
			//PL0_REF_CTRL = 0x0101_0F00

			*pl0 = PL_0_100;

			munmap(pl_clk_reg, MAP_SIZE_F);

#ifdef PRINT_COUNT
			printf("PL Frequncy changed to 100 MHz\n");
#endif
			break;
		default:
			break;


	}
}


int count1, count2;

void sigio_signal_handler(int signo){

	//assert(signo == SIGIO);   // Confirm correct signal #
	//printf("sigio_signal_handler called (signo=%d)\n", signo);
	//*slv_reg1 = *slv_reg1 & 0x0;
	*slv_reg1 = *slv_reg1 & 0xFFFFFFFC;
	sigio_signal_count ++;
	det_int = 1;
}



int main(int argc, char *argv[]) {
	det_int = 0;
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

	if(argc>4){
                printf("USAGE: ./test1 (n PS Freq) (m PL Freq) (Loops)\n");
                return -1;
        }

        int n = -1, m = -1;
        if(argc == 1){
                printf("Default loop number = 10000\n");
        }
        if(argc == 2){
                n = strtoul(argv[1], 0, 0);   //taking number for PS Freq from the user
                printf("Default loop number = 10000\n");
        }
        if(argc == 3){
                n = strtoul(argv[1], 0, 0);   //taking number for PS Freq from the user
                m = strtoul(argv[2], 0, 0);   //taking number for PL Freq from the user
                printf("Default loop number = 10000\n");
        }
        if(argc == 4){
                n = strtoul(argv[1], 0, 0);   //taking number for PS Freq from the user
                m = strtoul(argv[2], 0, 0);   //taking number for PL Freq from the user
                xx = strtoul(argv[3], 0, 0);   //taking number for Loops Freq from the user
                printf("Setting loop number = %d\n", xx);
        }
        if(n==0)
                printf("Setting PS Freq. to 1499 MHz\n");
        else if(n==1)
                printf("Setting PS Freq. to 999 MHz\n");
        else if(n==2)
                printf("Setting PS Freq. to 416.6 MHz\n");

        else if(n>2||n==-1)
                printf("PS Frequency: Enter number 0, 1, and 2 for setting PS Freq. to 1499 MHz, 999 MHz, and 416.6 MHz respectively.\nFor now, setting it to 1499 MHz..\n");

  //    signal(SIGINT, m_unmap_ctrl_c);
        int dh = open("/dev/mem", O_RDWR | O_SYNC); // Open /dev/mem which represents the whole physical memory
        if(m==0)
                printf("Setting PL Freq. to 300 MHz\n\n");
        else if(m==1)
                printf("Setting PL Freq. to 187.5 MHz\n\n");
        else if(m==2)
                printf("Setting PL Freq. to 100 MHz\n\n");
        else if(m>2||m==-1)
                printf("PL Frequency: Enter number 0, 1, and 2 for setting PL Freq. to 300 MHz, 187.5 MHz, and 100 MHz respectively.\nFor now, setting it to 300 MHz..\n\n");





	if(dh == -1){
		printf("Unable to open /dev/mem. Ensure if it exists.\n");
		return -1;
	}

	//signal(SIGINT, m_unmap_ctrl_c);
	slv_reg_base = mmap(NULL,
			MAP_SIZE,
			PROT_READ|PROT_WRITE,
			MAP_SHARED, dh, SLV_REG_BASE  & ~MAP_MASK);

	slv_reg0 = slv_reg_base + (((SLV_REG_BASE + SLV_REG_0_OFF) & MAP_MASK) >> 2);
	slv_reg1 = slv_reg_base + (((SLV_REG_BASE + SLV_REG_1_OFF) & MAP_MASK) >> 2);
	slv_reg2 = slv_reg_base + (((SLV_REG_BASE + SLV_REG_2_OFF) & MAP_MASK) >> 2);
	slv_reg3 = slv_reg_base + (((SLV_REG_BASE + SLV_REG_3_OFF) & MAP_MASK) >> 2);
	change_ps_freq(dh, n);
	change_pl_freq(dh, m);
		int sample = 0;
       //for(int j = 0; j< STAT_MEASUREMENTS; j++){	
	for(int i = 0;i<xx;i++){ 
		*slv_reg1 = *slv_reg1 & 0xFFFFFFFC;

		*slv_reg1 = *slv_reg1 | 0x3;
		count1 = *slv_reg2;
		while(!det_int){
			;
		} 

		det_int = 0;

		count2 = *slv_reg2;
		intr_latency_measurements[i] = count2 - count1;
	}

	//	for(int i=0;i<xx;i++){
	//		printf("%d ", intr_latency_measurements[i]);
	//	}
	//	printf("\n\n");


	unsigned long   min_latency;
	unsigned long   max_latency;
	double          average_latency;
	double          std_deviation;

	compute_interrupt_latency_stats(
			&min_latency,
			&max_latency,
			&average_latency,
			&std_deviation);
		min_latency_measurements[sample] = min_latency;
		max_latency_measurements[sample] = max_latency;
		sample ++;
	printf("Minimum Latency:    %lu\n"
			"Maximum Latency:    %lu\n"
			"Average Latency:    %f\n"
			"Standard Deviation: %f\n"
			"Number of samples:  %d\n",
			min_latency,
			max_latency,
			average_latency,
			std_deviation,
			xx 
	      );
	printf("Number of Interrupts: %d\n", sigio_signal_count); 
       save_file(intr_latency_measurements, xx);
      // }
/*		for(int i=0;i<STAT_MEASUREMENTS;i++){
			printf("%d ", min_latency_measurements[i]);
		}
		for(int i=0;i<STAT_MEASUREMENTS;i++){
			printf("%d ", max_latency_measurements[i]);
		}
	//	printf("\n\n");*/
//	save_file(min_latency_measurements, STAT_MEASUREMENTS);
//	save_file(max_latency_measurements, STAT_MEASUREMENTS);
	return 0;

}
unsigned long int_sqrt(unsigned long n){
	unsigned long root = 0;
	unsigned long bit;
	unsigned long trial;

	bit = (n >= 0x10000) ? 1<<30 : 1<<14;
	do
	{
		trial = root+bit;
		if (n >= trial)
		{
			n -= trial;
			root = trial+bit;
		}
		root >>= 1;
		bit >>= 2;
	} while (bit);
	return root;
}

void compute_interrupt_latency_stats(
		unsigned long   *min_latency_p,
		unsigned long   *max_latency_p,
		double          *average_latency_p,
		double          *std_deviation_p){
	int i;
	unsigned long   val;
	unsigned long   min = ULONG_MAX;
	unsigned long   max = 0;
	unsigned long   sum = 0;
	unsigned long   sum_squares = 0;

	for (i = 0; i < xx; i ++) {
		val = intr_latency_measurements[i];

		if (val < min) {
			min = val;
		}

		if (val > max) {
			max = val;
		}

		sum += val;
		sum_squares += val * val;
	}

	*min_latency_p = min;
	*max_latency_p = max;

	unsigned long average = (unsigned long)sum / xx;

	unsigned long std_deviation = int_sqrt((sum_squares / xx) -
			(average * average));


	*average_latency_p = average;
	*std_deviation_p = std_deviation;
}
int flag = 1;
void save_file(long unsigned *array, int size){

FILE *fp;

char *filename = "data.csv"; 

fp = fopen(filename,"w+");

for(int i=0;i<size;i++){

    fprintf(fp,",%ld ",array[i]);
    }

fclose(fp);
if(flag){
	rename("data.csv", "data_min.csv");
	flag = 0;
}
}
