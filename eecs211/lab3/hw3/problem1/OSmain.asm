.ORIG 	x3000	
SET_STAT_1
	LD 		R5,		b
	LD 		R3,		Array 
    STR		R5, 	R3,		#0
CHECK_2
	LD 		R5,		b
	LD 		R3,		Array 
    STR		R5, 	R3,		#1
CHECK_3
	LD 		R5,		b
	LD 		R3,		Array 
    STR		R5, 	R3,		#2 
CHECK_4
	LD 		R5,		b
	LD 		R3,		Array 
    STR		R5, 	R3,		#3
CHECK_5
	LD 		R5,		b
	LD 		R3,		Array 
    STR		R5, 	R3,		#4 
TRAP_HALT
	TRAP 	x25
a 		.FILL	#0
b 		.FILL	#1
Array	.FILL	x0200
.END