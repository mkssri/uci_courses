.ORIG x7000
AND R1, R1, #0 ; Clearing the R1 Register by doing bitwise AND
ADD R1, R1, #4   ; Doing R1&4 and filling it in R1
HALT
.END