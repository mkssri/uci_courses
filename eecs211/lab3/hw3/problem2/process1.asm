.ORIG x4000
AND R1, R1, #0 ; Clearing the R1 Register by doing bitwise AND
ADD R1, R1, #1   ; Doing R1&1 and filling it in R1
YIELD
ADD R1, R1, #2
HALT
.END