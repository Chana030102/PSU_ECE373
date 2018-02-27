/* 
 Aaron Chan
 ECE373 (Spring 2017)
 Assignment #3 Userspace Program

 This C program writes to a node file to change the
 state of LED0 on the Atom Box

 We will imitate a LED blink by turning it on, pause
 for 2 seconds, then turn it off. The program will also
 read the value stored at the LED control register.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
	int ret, fd;
	int readval;
    int turn_on = 78, turn_off = 15;

    // Open file made with mknod
    fd = open("/dev/hw3_pci",O_RDWR);
    if(fd < 0)
	{
		fprintf(stderr, "Error opening file.\n");
		return -1;
	}

    // Read value from LED register
	ret = read(fd,&readval,0);
	if(ret < 0)
		fprintf(stderr, "Error reading led_val\n");
	printf("Current led_val: %d\n", readval);

    // Write to LED and turn on
    ret = write(fd,&turn_on,sizeof(turn_on));
	if(ret < 0)
	{
		fprintf(stderr, "Error writing to file.\n");
		return -1;
	}

    // Read LED register again. 
    ret = read(fd,&readval,0);
    printf("Current led_val: %d\n", readval);
    sleep(2);
    
    // Write to LED and turn off
    ret = write(fd,&turn_off,sizeof(turn_off));

    // Done with file. Close.
    close(fd);
	printf("End of program.\n\n");
	return 0;	
	
}
