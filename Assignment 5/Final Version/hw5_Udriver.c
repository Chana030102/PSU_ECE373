/*
 * Aaron Chan
 * EC373 (Spring 2017)
 * Assignment #5 
 *
 * Userspace Driver
 * This program will do the following:
 * - Save current LEDCTL value and print for user to read
 * - Turn both green LEDs on for 2 seconds
 * - Turn all LEDs off for 2 seconds
 * - Loop 5 times and turn each LED on for 1 second
 * - Restore LEDCTL to initial value
 * - Read and print contents of Good Packets Received statistics register
 * - Cleanup and exit
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

#define MEM_SIZE (1 << 17) // 128K memory size at Region 0 
#define REGION 0xfebe0000

#define LEDCTL 0x0E00   // LED Control register offset
#define GPRC 0x4074     // Good Packets Received Count register offset
#define TPR  0x40D0     // Total Packets Received register

#define LED 0x4E
#define LED1 0x4E00
#define LED2 0x4E0000
#define BOTH_GREEN 0x4E4E00
#define ALL_OFF 0x0F0F0F
#define LED_OFF 0x0F

// Prototype
int update_LED(uint32_t write_val, void* hw_addr); // Used to write to LEDCTL

int main(void)
{
    uint32_t init_val; // Store initial LEDCTL value
    uint32_t read_val; // Use to read Good Packets Received reg
    int fd;       // For opening file
    int loop = 0; // Loop condition
    void *hw_addr; // Store BAR for use

    // Open file
    fd = open("/dev/mem", O_RDWR);
    if(fd < 0) 
    {
        fprintf(stderr,"Error opening file\n");
        return -1;
    }
    
    // Map the BAR to work with
    hw_addr = mmap(NULL, MEM_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, REGION);

    // Save initial value and print
    init_val = *((uint32_t*)(hw_addr + LEDCTL));
    fprintf(stdout, "Initial Value: %x\n", init_val);

    
    update_LED(BOTH_GREEN, hw_addr); // Turn on both green for 2 seconds
    sleep(2);
    update_LED(ALL_OFF,hw_addr); // Turn off all for 2 seconds
    sleep(2);

    // Loop 5 times turning each LED on individually for 1 second
    for(loop = 0;loop < 5; ++loop)
    {
        update_LED(LED,hw_addr);        // LED0 
        sleep(1);
        update_LED(LED1,hw_addr); // LED1 
        sleep(1);
        update_LED(LED2,hw_addr); // LED2 
        sleep(1);
    }
    
    // Restore LEDCTL to initial value
    update_LED(init_val, hw_addr);
    
    // Read and print contents of GPRC
    fprintf(stdout,"Good Packets Received: %x\n", *(uint32_t*)(hw_addr + GPRC));

    // Read and print contents of TPR
    fprintf(stdout,"Total Packets Received: %x\n",*((uint32_t*)(hw_addr + TPR)));

    // Clean up
    munmap(hw_addr, MEM_SIZE);      // Delete mapping
    close(fd);			    // Close file

    printf("Program closed...\n");
    return 0;
}

// Write in a passed value to LEDCTL
int update_LED(uint32_t write_val, void* hw_addr)
{
     *((uint32_t*)(hw_addr + LEDCTL)) = write_val; 
    if( *((uint32_t*)(hw_addr + LEDCTL))!= write_val)
    {
        fprintf(stderr,"Failed to write value: %x\n", write_val);
        return -1;
    }
    return 0;
}
