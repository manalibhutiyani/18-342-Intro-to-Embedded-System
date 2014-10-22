@setup: Used to change to user mode. create user stack and push argc&argv
@ Author: Yao Zhou <yaozhou@andrew.cmu.edu>
@ Date:	10/20/2014

.global setup

#include <bits/swi.h>

setup:
		STMFD sp!, {r1-r12, lr}				@ store bank registers and link regsiter
		MOV r2, #0xA0000004					@ load user stack address
		STR sp, [r2]  						@ set stack pointer to user stack

		MRS	r2, cpsr						@ load cpsr
		AND r2, r2, #0xFFFFFF00				@ get last two bytes
		ORR r2, r2, #0xD0 					@ mask IRQ&FIQ
		MSR cpsr, r2						@ write new mode code back to cpsr

		MOV sp, #0xA3000000					@ create a downward stack for user
		MOV	r2, #0   						@ add 0 to the top of stack(0 argus)
		STMFD sp!, {r2} 						

		MOV r2, r0							@ load U-Boot argc

.push_argv:
		SUBS r2, r2, #1   					@ get the number of rest arguments
		LDRGE r3, [r1, r2, LSL #2] 			@ load parameter to register
		STMGEFD sp!, {r3}					@ push it into user stack
		BGE .push_argv						@ if not finished, jump back to .push_argv

.push_argc:
		MOV r2, r0
		STMFD sp!, {r0}						@ push argc to user stack
		MOV	r2, #0xA2000000					@ get the address of loaded user program
		BLX r2   							@ jum to loaded user program
		SWI EXIT_SWI						@ exit user program, call sys call exit

