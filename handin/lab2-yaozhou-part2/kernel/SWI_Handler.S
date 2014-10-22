@ SWI_Handler.S : Wrapper before calling SWI_Handler_C func
@ Anthor: Yao Zhou <yaozhou@andrew.cmu.edu>
@ Date:	10/20/2014	

.extern SWI_Handler_C
.global SWI_Handler

SWI_Handler:
				STMFD	sp!, {r1-r12,lr}			@ store registers and link register
				MOV		r1,	sp						@ get paramater list address
				LDR 	r0, [lr,#-4]				@ load SWI instruction
				BIC		r0, #0xff000000				@ get SWI number
				B 		SWI_Handler_C				@ call C SWI function
				LDMFD	sp!, {r1-r12,lr}			@ restore regsiters and link register
				MOV		pc, lr						@ return from SWL SWI_Handler