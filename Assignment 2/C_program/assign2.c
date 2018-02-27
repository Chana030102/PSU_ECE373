/* 
 Aaron Chan
 ECE373 (Spring 2017)
 Assignment #2 C program

*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX 10

int main(void)
{
	int ret, fd;
	int readval;
	char to_write[MAX];

	fd = open("/dev/mycdev",O_RDWR);
	
	if(fd < 0)
	{
		fprintf(stderr, "Error opening file.\n");
		return -1;
	}

	ret = read(fd,&readval,sizeof(int));
	if(ret < 0)
		fprintf(stderr, "Error reading syscall_val\n");
	printf("Current syscall_val: %d\n", readval);

	printf("Enter an integer to change syscall_val: ");
	scanf("%s", to_write);
	printf("Passing the following to driver for conversion: %s\n",to_write);
	ret = write(fd,to_write,sizeof(to_write));
	
	if(ret < 0)
	{
		fprintf(stderr, "Error writing to file.\n");
		return -1;
	}
	
	printf("End of program.\n\n");
	return 0;	
	
}
