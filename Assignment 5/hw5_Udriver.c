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

#define LEDCTL 0x0E00 // LED Control register offset
#define REGION0 0xfebe0000 
#define LED0 0x4E4E4E
#define LED_OFF 

int main(void)
{
    uint32_t init_val; // Store initial LEDCTL value
    int fd;       // For opening file
    void *hw_addr; // Store BAR for use

    fd = open("/dev/mem", O_RDWR);
    if(fd < 0) 
    {
        fprintf(stderr,"Error opening file\n");
        return -1;
    }
    
    // Get BAR to work with
    hw_addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, REGION0);

    fprintf(stdout, "Address mapped: %p\n", hw_addr);
    
    // Save initial value
    init_val =  *((uint32_t*)hw_addr + LEDCTL);
    fprintf(stdout, "Value at address: %x\n", init_val);

    *((uint32_t*)hw_addr + LEDCTL) |= LED0;
    
    fprintf(stdout, "Value at address: %x\n", init_val);
    munmap(hw_addr, MEM_SIZE); 

    return 0;
}
