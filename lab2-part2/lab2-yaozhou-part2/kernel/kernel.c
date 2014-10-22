/*****************************************************
*	Kernel.c: Kernel main(entry) function             *
*	Author: Yao Zhou <yaozhou@andrew.cmu.edu>	      *
*	Date:	10/20/2014								  *
 *****************************************************/

#include <bits/fileno.h>
#include <bits/errno.h>
#include <bits/swi.h>
#include <exports.h>

#define NULL 0

extern unsigned SWI_Handler();
extern int setup(int argc, char* argv[]);
extern ssize_t read(int fd, void *buf, size_t count);
extern ssize_t write(int fd, const void *buf, size_t count);
extern void exit(int status);

void SWI_Handler_C(unsigned swi_no, unsigned* regs) {

	char* buf = NULL; // a simple pointer
	int exit_no = -1; // used to store exit code
	int fd = -1;	  // file descriptor
	int bytes = 0;	  // store the return number from read() or write()

	switch(swi_no) {
		case EXIT_SWI: {
			// get exit code
			exit_no = regs[0];
			// exit!
			exit(exit_no);
		}
		case READ_SWI: {
			// get parameters
			fd = regs[0];
			buf = (char*)regs[1];
			bytes = regs[2];

			// call my read function
			regs[0] = read(fd,buf,bytes);
			return;
		}
		case WRITE_SWI: {
			// get parameters
			fd = regs[0];
			buf = (char*)regs[1];
			bytes = regs[2];

			// call my write function
			regs[0] = write(fd,buf,bytes);
			return;
		}
		default: {
			// unimplemented SWI or invalid SWI number
			printf("Invalid SWI number: %d\n", swi_no);
            exit(0x0badc0de);
		}
	}
	return;
}

void install_MySWIHandler(unsigned new_S_addr, unsigned *SWI_vec_addr, 
    unsigned *instr1, unsigned *instr2) {
	unsigned vec_instr, op, sign, offset, SWI_Handler_Addr;

    // Get SWI Handler instrucntion in the vector table
    vec_instr = *SWI_vec_addr;
    sign = vec_instr >> 23 & 1;

    // Get the operator part of LDR instruction
    op = vec_instr & 0xFFFFF000;

    // Check whether instruction at 0x08 is in format of LDR pc, [pc, #offset]
    if(op != (unsigned)0xe59FF000 && op != (unsigned)0xE51FF000) {
        printf("Unrecognized Instruction\n");
        exit(0x0BADC0E);
    }

    // Get the offset part of the LDR instruction
    offset = vec_instr & 0xFFF;
    if( sign == 0) {
        offset = offset * -1;
    }

    // Get the address of SWI Handler
    SWI_Handler_Addr = *(unsigned *)((unsigned)SWI_vec_addr + offset + 0x8);

    // store the original two instructions for recovery
    *instr1 = *(unsigned *)SWI_Handler_Addr;
    *instr2 = *(unsigned *)(SWI_Handler_Addr + 4);

    // Install our SWI handler
    *(unsigned *)SWI_Handler_Addr = 0xE51FF004;
    *(unsigned *)(SWI_Handler_Addr + 4) = new_S_addr;

}

void recover_Handler(unsigned *SWI_vec_addr, unsigned instr1, unsigned instr2) {

	unsigned vec_instr, sign, offset, SWI_Handler_Addr;

    // Get SWI Handler instrucntion in the vector table
    vec_instr = *SWI_vec_addr;
    sign = vec_instr >> 23 & 1;

    // Get the offset part of the LDR instruction
    offset = vec_instr & 0xFFF;
    if( sign == 0) {
        offset = offset * -1;
    }

    // Get the address of SWI Handler
    SWI_Handler_Addr = *(unsigned *)((unsigned)SWI_vec_addr + offset + 0x8);

    // Recover two instructions
    *(unsigned *)SWI_Handler_Addr = instr1;
    *(unsigned *)(SWI_Handler_Addr + 4) = instr2;
}

int main(int argc, char *argv[]) {
	/* Needs more code. */
    unsigned instr1, instr2;
    int exit_no = 0;

	// load vector table
    unsigned *SWI_vec_Addr = (unsigned *)0x08;
    // Address of new SWI handler
    unsigned my_SWI_Handler = (unsigned)&SWI_Handler;

    //install my SWI handler
    install_MySWIHandler(my_SWI_Handler, SWI_vec_Addr, &instr1, &instr2);

    // change to user mode and run user program
    exit_no = setup(argc, argv);

    //recover original SWI Handler
    recover_Handler(SWI_vec_Addr, instr1, instr2);

	return exit_no;
}
