.ORIG x8000
AND R1, R1, #0 ; Clearing the R1 Register by doing bitwise AND
ADD R1, R1, #5   ; Doing R1&5 and filling it in R1
HALT
.END