.ORIG 	x3000	

IF_STATEMENT
	LD 		R5,		b
	LD 		R3,		Array
	LD  	R4, 	a
	LDI  	R6, 	inst1
	NOT 	R6, 	R6
	ADD 	R6, 	R6,		#1
	ADD 	R4, 	R4,		R6
    BRzp 	CHECK_2  
    STR		R5, 	R3,		#0
CHECK_2
	LD  	R4, 	a
	LDI  	R6, 	inst2
	NOT 	R6, 	R6
	ADD 	R6, 	R6,		#1
	ADD 	R4, 	R4,		R6
    BRzp 	CHECK_3   
    STR		R5, 	R3,		#1  
CHECK_3
	LD  	R4, 	a
	LDI  	R6, 	inst3
	NOT 	R6, 	R6
	ADD 	R6, 	R6,		#1
	ADD 	R4, 	R4,		R6
    BRzp 	CHECK_4   
    STR		R5, 	R3,		#2  
CHECK_4
	LD  	R4, 	a
	LDI  	R6, 	inst4
	NOT 	R6, 	R6
	ADD 	R6, 	R6,		#1
	ADD 	R4, 	R4,		R6
    BRzp 	CHECK_5  
    STR		R5, 	R3,		#3 
CHECK_5
	LD  	R4, 	a
	LDI  	R6, 	inst5
	NOT 	R6, 	R6
	ADD 	R6, 	R6,		#1
	ADD 	R4, 	R4,		R6
    BRzp 	TRAP_HALT
    STR		R5, 	R3,		#4 
TRAP_HALT
	; set mem protection unit to 1

	LD      R2,     MR_MPU
    LD      R3,     b
    STR     R3,     R2,     #0

	TRAP 	x25
a 		.FILL	#0
b 		.FILL	#1
Array	.FILL	x0200
inst1	.FILL	x4000 
inst2	.FILL	x5000 
inst3	.FILL	x6000 
inst4	.FILL	x7000 
inst5	.FILL	x8000 
MR_MPU 	.FILL	xFE06 
.END
