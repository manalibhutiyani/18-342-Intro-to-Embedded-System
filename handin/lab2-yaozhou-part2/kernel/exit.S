@ exit.S:	exit swi 
@ Anthor: 	Yao Zhou <yaozhou@andrew.cmu.edu>
@ Date:		10/20/2014 

.global exit

exit:
		MOV 	r1, #0xa0000004		@ load stack pointer addr
		LDR		sp, [r1] 			@ restore stack pointer
		LDMFD	sp!, {r1-r12,lr}		@ restore all registers
		MOV		pc, lr				@ return
