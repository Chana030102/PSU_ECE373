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

#define LEDCTL 0x00E00 // LED Control register offset
#define REGION 0xfebe0000 
#define LED0 0x4E
#define LED_OFF 0x0F

int main(void)
{
    uint32_t init_val; // Store initial LEDCTL value
    uint32_t read_val; // Use to read LEDCTL value
    uint32_t write_val; 
    int fd;       // For opening file
    void *hw_addr; // Store BAR for use

    fd = open("/dev/mem", O_RDWR);
    if(fd < 0) 
    {
        fprintf(stderr,"Error opening file\n");
        return -1;
    }
    
    // Map the BAR to work with
    hw_addr = mmap(NULL, (int)MEM_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, (unsigned long)REGION);

 
    fprintf(stdout, "Address mapped: %p\n", hw_addr);
    fprintf(stdout, "Address with offset: %p\n", (hw_addr + LEDCTL)); 

    // Save initial value and print
    init_val = *((uint32_t*)hw_addr + LEDCTL);
    fprintf(stdout, "Initial Value: %x\n", init_val);
    *((uint32_t*)(hw_addr + LEDCTL)) = LED0;
     read_val = *((uint32_t*)(hw_addr + LEDCTL));
     fprintf(stdout, "Stored Value: %x\n", read_val);
    sleep(1);

    fprintf(stdout, "Size of stored value: %lx\n",sizeof(*(hw_addr + LEDCTL)));
    fprintf(stdout, "Size of stored value: %lx\n",sizeof(unsigned int));   
    sleep(1);

    write_val = (init_val & 0xffffff00) | LED0;
    *((uint32_t*)(hw_addr + LEDCTL)) = write_val;    // Modify value at register to turn on LED0
    read_val = *((uint32_t*)(hw_addr + LEDCTL)); // Read value to verify change
    fprintf(stdout, "Value at address: %x\n", read_val);

    sleep(1);
    *((uint32_t*)(hw_addr + LEDCTL)) = LED_OFF;  // Modify value at register to turn off LED0
    read_val = *((uint32_t*)(hw_addr + LEDCTL)); // Read value to verify change
    fprintf(stdout, "Value at address: %x\n", read_val);
    sleep(1);

    // Clean up
    *((uint32_t*)(hw_addr + LEDCTL)) = init_val; // Restore initial state of registers
    read_val = *((uint32_t*)(hw_addr + LEDCTL)); // Read value to verify change
    fprintf(stdout, "Verify restored initial value: %x\n", read_val);
    
    munmap(hw_addr, MEM_SIZE);      // Delete mapping
    close(fd);			    // Close file

    return 0;
}
