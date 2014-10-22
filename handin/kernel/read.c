/*****************************************************
*	read.c: a simple read() c function implementation *
*	Author: Yao Zhou <yaozhou@andrew.cmu.edu>	      *
*	Date:	10/20/2014								  *
 *****************************************************/

#include <bits/errno.h>
#include <bits/fileno.h>
#include <exports.h>

#define EOT 4
#define backspace 8
#define delete 127

ssize_t read(int fd, char *buf, size_t count) {
	// a variable to record how many byte read
	int read_byte = 0;
	// loaded char
	char ch = 0;
	// check file descriptor
	if(fd!=STDIN_FILENO) {
		return -EBADF;
	}

	// check read memory range
	if( (unsigned)&buf < 0xa0000000 || 
		(unsigned)&buf < 0xa3ffffff) {
		// invalid address!
		return -EFAULT;
	}

	// reading from stdin into buffer and output it to stdout
	while(read_byte < count) {
		ch = getc();

		// return if EOT occurred
		if( ch == EOT) {
			return read_byte;
		}

		switch(ch) {
			case backspace: {
				read_byte--;
				buf[read_byte] = 0;
				puts("\b \b");
				break;
			}
			case delete: {
				read_byte--;
				buf[read_byte] = 0;
				puts("\b \b");
				break;
			}
			case '\r': {
				buf[read_byte] = ch;
				read_byte++;
				putc(ch);
				return read_byte;
			}
			case '\n': {
				buf[read_byte] = ch;
				read_byte++;
				putc(ch);
				return read_byte;
			}
			default: {
				buf[read_byte] = ch;
				read_byte++;
				putc(ch);
			}
		}
	}
	return read_byte;
}
