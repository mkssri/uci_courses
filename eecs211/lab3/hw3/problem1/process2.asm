.ORIG x5000
AND R1, R1, #0 ; Clearing the R1 Register by doing bitwise AND
ADD R1, R1, #2   ; Doing R1&2 and filling it in R1
HALT
.END