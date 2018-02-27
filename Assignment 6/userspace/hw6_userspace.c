/* 
 Aaron Chan
 ECE373 (Spring 2017)
 Assignment #6 Userspace Program

 This program will read from the node file to get
 an integer indicating index of head and tail for 
 the receive descriptor
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
    unsigned int head_tail;

    // Open file made with mknod
    fd = open("/dev/hw6_interrupts",O_RDWR);
    if(fd < 0)
	{
		fprintf(stderr, "Error opening file.\n");
		return -1;
	}

    // Get head and tail index
	ret = read(fd,&head_tail,0);
	if(ret < 0)
		fprintf(stderr, "Error reading head and tail\n");
	
	printf("HEAD: %d\nTAIL: %d\n",(head_tail >> 16)&0xFFFF, (head_tail & 0xFFFF));	
    
    // Done with file. Close.
    close(fd);
	printf("End of program.\n\n");
	return 0;	
	
}
