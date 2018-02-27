/* 
 Aaron Chan
 ECE373 (Spring 2017)
 Assignment #4 Userspace Program

 This program will read from the node file to get
 an integer indicating the blink rate of the LED.
 It can also write an integer to the module to change
 the blink rate as well.
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
    int new_rate;

    // Open file made with mknod
    fd = open("/dev/hw4_led",O_RDWR);
    if(fd < 0)
	{
		fprintf(stderr, "Error opening file.\n");
		return -1;
	}

    // Get blink rate of LED
	ret = read(fd,&readval,0);
	if(ret < 0)
		fprintf(stderr, "Error reading blink_rate\n");
	printf("Current blink rate: %d\n", readval);

    // Write to change blink rate of LED
    printf("Enter a new blink rate for LED0: ");
    scanf("%d",&new_rate);
    printf("You wrote %d\n",new_rate);
    ret = write(fd,&new_rate,sizeof(new_rate));
	if(ret < 0)
	{
		fprintf(stderr, "Error writing to file.\n");
		return -1;
	}

    // Read LED register again to verify the change
    ret = read(fd,&readval,0);
    printf("Verify new blink rate: %d\n", readval);
    
    // Done with file. Close.
    close(fd);
	printf("End of program.\n\n");
	return 0;	
	
}
