/******************************************************
*	read.c: a simple write() c function implementation *
*	Author: Yao Zhou <yaozhou@andrew.cmu.edu>	       *
*	Date:	10/20/2014								   *
 ******************************************************/

#include <bits/errno.h>
#include <exports.h>
#include <bits/fileno.h>

ssize_t write(int fd, char *buf, size_t count) {

	// a variable to record how many byte already wrote
	int byte_write = 0;
	// check file descriptor
	if(fd != STDOUT_FILENO) {
		return -EBADF;
	}

	// check dest memory range
	if(((unsigned)&buf > 0xa0000000 && (unsigned)&buf < 0xa3ffffff) || 
		((unsigned)&buf > 0x00000000 && (unsigned)&buf < 0x00ffffff) ) {
		//valid address
		while(byte_write < count) {
			if(buf[byte_write] == '\0') {
				// the end of string
				return byte_write;
			}
			else {
				putc(buf[byte_write]);
				byte_write++;
			}
		}
	}else {
		// invalid address, return error!
		return -EFAULT;
	}
	return byte_write;
}
