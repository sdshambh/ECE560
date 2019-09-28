#include "integer.h"
#include <MKL25Z4.h>
#include "spi_io.h"
#include "sd_io.h"
#include "LEDs.h"
#include "debug.h"
#include "cmsis_os2.h"

#define NUM_SECTORS_TO_READ (100)

SD_DEV dev[1];          // SD device descriptor
uint8_t buffer[512];    // Buffer for SD read or write data
osThreadId_t tid_Makework;
osThreadId_t tid_testSD;
uint32_t tick_freq;
uint32_t idle_before=0;
uint32_t idle_after=0;
uint32_t time_diff=0;
 uint32_t idle_counter=0;
uint32_t init_before=0;
uint32_t init_after=0;
uint32_t init_time_diff=0;
void Thread_Makework(){
	while(1)
	{
	static int n=2;
	static int done = 0;
	static double my_pi=3.0;
	double term, prev_pi=0.0;
	
	// Nilakantha Series to approximate pi
	if (!done) {
		term = 4.0/(n*(n+1.0)*(n+2.0));
		if (n&4) { // is multiple of four
			term = -term;
		}
		prev_pi = my_pi;
		my_pi += term;
		if (my_pi != prev_pi) {
			n += 2;
		} else {
			// Done with approximating pi
			done = 1; 
		}
	}	
}
}

void Error_Handler(void) {
	Control_RGB_LEDs(1, 0, 0); // Light red LED
	while (1)
		;
}

void Thread_Test_SD(void) {
	 uint32_t total=0;
	static int k=0;
	static uint32_t FinalDiff=0;
	
		 
    
	//idle_after=idle_counter;
	//time_diff=idle_after-idle_before;
		//total=total+time_diff;
	// Write test data to given block (sector_num) in flash. 
	// Read it back, compute simple checksum to confirm it is correct.
	int i;
	DWORD sector_num = 0, read_sector_count=0; 
	uint32_t sum=0;
	SDRESULTS res;
	//	static char err_color_code = 0; // xxxxxRGB
	init_before = idle_counter;
	if (SD_Init(dev) != SD_OK) {
		Error_Handler(); // Initialization error
	}
init_after = idle_counter;
	init_time_diff = init_after - init_before;
	Control_RGB_LEDs(0, 1, 1); // Cyan: initialized OK
	while (1) {
//		for (; k<100; k++  )
//		{
//		idle_before = idle_counter;
//		osDelay(2000);
//		idle_after = idle_counter;
//		time_diff = idle_after - idle_before;
//		total = total + time_diff;
//			//break;
//		
//		if(k==100)
//		{
//			FinalDiff=total;
//			break;
//		}
		//idle_before=idle_counter; 
		//osDelay(2000);
		idle_before = idle_counter;
		//osDelay(2000);
		idle_after = idle_counter;
		
		time_diff = idle_after - idle_before;
		for (read_sector_count=0; read_sector_count < NUM_SECTORS_TO_READ; read_sector_count++) {
			// erase buffer
			for (i=0; i<SD_BLK_SIZE; i++)
				buffer[i] = 0;
			// perform SD card read
			res = SD_Read(dev, (void *)buffer, sector_num, 0, 512);	
			if (res != SD_OK) { // Was read was OK?
				Error_Handler(); // Read error
			} else {
				Control_RGB_LEDs(0, 0, 1); // Blue: Read OK
			}
			sector_num++; // Advance to next sector
		}
		// erase buffer
		for (i=0; i<SD_BLK_SIZE; i++)
			buffer[i] = 0;
		// Load sample data into buffer
		*(uint64_t *)(&buffer[0]) = 0xFEEDDC0D;
		*(uint64_t *)(&buffer[508]) = 0xACE0FC0D;
		// SD card write to sector_num
		res = SD_Write(dev, (void *) buffer, sector_num);
		if (res != SD_OK) { // Was write completed OK?
			Error_Handler(); // Write error
		} 
		Control_RGB_LEDs(1, 0, 1); // Magenta: Write OK
		// erase buffer
		for (i=0; i<SD_BLK_SIZE; i++)
			buffer[i] = 0;
		// request SD card read to verify contents written correctly
		res = SD_Read(dev, (void *)buffer, sector_num, 0, 512);	
		if (res != SD_OK) { // Was verify read OK?
			Error_Handler(); // Verify read error
		} 
		Control_RGB_LEDs(0, 0, 1); // Blue: Verify read OK
		for (i = 0, sum = 0; i < SD_BLK_SIZE; i++)
			sum += buffer[i];		// Compute checksum
		if (sum != 0x0569) {
			Error_Handler(); // Checksum error
		} 
		Control_RGB_LEDs(1, 1, 1); // White: Checksum OK
	} 
}
	

int main(void) {
	Init_Debug_Signals();
	Init_RGB_LEDs();
	Control_RGB_LEDs(1,1,0);	// Yellow - starting up

	osKernelInitialize();
	tick_freq = osKernelGetTickFreq();
	tid_testSD = osThreadNew(Thread_Test_SD, NULL, NULL);
	//tid_Makework = osThreadNew(Thread_Makework, NULL, NULL);
	osKernelStart();
	while(1)
		;
}
