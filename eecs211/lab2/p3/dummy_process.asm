.ORIG x4000
LD R1, L2
TRAP xFF
L2: .FILL x0001
L4: BRnzp L4
.END