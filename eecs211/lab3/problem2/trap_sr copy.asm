    .ORIG x0100
    


    TRAP xFF 

    AND R5, R5, #0
    STR R5, R3, #0


SCHEDULER:

    LD R2, OS_ST      ;;Load start of OS into R2
    ADD R2, R2, #10        ;;Add 18 to address to reach address status of process P1
    ADD R2, R2, #8
    LDR R3, R2, #0        ;; Address where P1's status is
    LDR R4, R3, #0        ;; Get process status from address 
    ADD R4, R4, #-1 
    BRz JUMP_TO_P1

    LD R2, OS_ST      ;;Load start of OS into R2
    ADD R2, R2, #10        ;;Add 19 to address to reach status of process P2
    ADD R2, R2, #9
    LDR R3, R2, #0        ;; Load what is in memory at R2(address of status of P2) to R3
    LDR R4, R3, #0        ;; Get process status from address 
    ADD R4, R4, #-1 
    BRz JUMP_TO_P2

    LD R2, OS_ST      ;;Load start of OS into R2
    ADD R2, R2, #10        ;;Add 20 to address to reach status of process P3
    ADD R2, R2, #10
    LDR R3, R2, #0        ;; Load what is in memory at R2(address of status of P3) to R3
    LDR R4, R3, #0        ;; Get process status from address 
    ADD R4, R4, #-1 
    BRz JUMP_TO_P3

    LD R2, OS_ST      ;;Load start of OS into R2
    ADD R2, R2, #11        ;;Add 21 to address to reach status of process P4
    ADD R2, R2, #10
    LDR R3, R2, #0        ;; Load what is in memory at R2(address of status of P4) to R3
    LDR R4, R3, #0        ;; Get process status from address 
    ADD R4, R4, #-1 
    BRz JUMP_TO_P4

    LD R2, OS_ST      ;;Load start of OS into R2
    ADD R2, R2, #12        ;;Add 22 to address to reach status of process P5
    ADD R2, R2, #10
    LDR R3, R2, #0        ;; Load what is in memory at R2(address of status of P5) to R3
    LDR R4, R3, #0        ;; Get process status from address 
    ADD R4, R4, #-1 
    BRz JUMP_TO_P5

    ;;TRAP xFF
    BRnzp SCHEDULER    ;;Loop back to start of scheduler
    


JUMP_TO_P1:
    ;; Change status of P1 to 2
    ADD R4, R4, #2      ;;0 should be there in register, add 2 to switch state of process to running
    STR R4, R3, #0      ;; store value back into memory

    ;;Load address where P1's code is to jump to that
    LD R4, OS_ST
    LDR R4, R4, #13 ;; address of P1

    TRAP xFF

    ;;Set MR_S to zero
    AND R0, R0, #0
    STI R0, MR_S

    JMP R4

JUMP_TO_P2:
    ;; Change status of P1 to 2
    ADD R4, R4, #2      ;;0 should be there in register, add 2 to switch state of process to running
    STR R4, R3, #0      ;; store value back into memory

    ;;Load address where P2's code is to jump to that
    LD R4, OS_ST
    LDR R4, R4, #14   ;; address of P2
    
    TRAP xFF

    ;;Set MR_S to zero
    AND R0, R0, #0
    STI R0, MR_S

    JMP R4

JUMP_TO_P3:
    ;; Change status of P1 to 2
    ADD R4, R4, #2      ;;0 should be there in register, add 2 to switch state of process to running
    STR R4, R3, #0      ;; store value back into memory

    ;;Load address where P3's code is to jump to that
    LD R4, OS_ST
    LDR R4, R4, #15    ;; address of P3

    TRAP xFF

    ;;Set MR_S to zero
    AND R0, R0, #0
    STI R0, MR_S

    JMP R4

JUMP_TO_P4:
    ;; Change status of P1 to 2
    ADD R4, R4, #2      ;;0 should be there in register, add 2 to switch state of process to running
    STR R4, R3, #0      ;; store value back into memory

    ;;Load address where P4's code is to jump to that
    LD R4, OS_ST
    ADD R4, R4, #10
    LDR R4, R4, #6    ;; address of P4

    TRAP xFF

    ;;Set MR_S to zero
    AND R0, R0, #0
    STI R0, MR_S
    
    JMP R4

JUMP_TO_P5:
    ;; Change status of P1 to 2
    ADD R4, R4, #2      ;;0 should be there in register, add 2 to switch state of process to running
    STR R4, R3, #0      ;; store value back into memory

    ;;Load address where P5's code is to jump to that
    LD R4, OS_ST
    ADD R4, R4, #10
    LDR R4, R4, #7     ;; address of P5
    
    TRAP xFF

    ;;Set MR_S to zero
    AND R0, R0, #0
    STI R0, MR_S

    JMP R4

OS_ST: .FILL x3000
MR_S:    .FILL xFE04
.END