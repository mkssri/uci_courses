    .ORIG x0100
    
    TRAP xFF ;;dump memory with trap at start of trap subroutine

    ;; PASS
    ;; Change status of Process to 0
    AND R2, R2, #0 ; Bitwise add is performed on R2 and 0 and result stored in R2. (R2=0)
    STR R2, R4, #0 ; The value of R2 is stored in R4+0. (R4=0)


SCHEDULER:

    ;; PROCESS 1
    AND R2, R2, #0 ; Bitwise add is performed on R2 and 0 and result stored in R2. (R2=0)
    LD R2,P1_STATUS ; Load the value at P1_STATUS in to R2 (R2=1)
    LDR R0,R2,#0 ; Load R0 with value of (R2-0), 
    ADD R0, R0, #-1
    BRz JUMP_TO_PROCESS1
    

    ;; PROCESS 2
    AND R2, R2, #0
    LD R2,P2_STATUS
    LDR R0,R2,#0
    ADD R0, R0, #-1    
    BRz JUMP_TO_PROCESS2


    ;; PROCESS 3
    AND R2, R2, #0
    LD R2,P3_STATUS
    LDR R0,R2,#0
    ADD R0, R0, #-1
    BRz JUMP_TO_PROCESS3


    ;; PROCESS 4
    AND R2, R2, #0
    LD R2,P4_STATUS
    LDR R0,R2,#0
    ADD R0, R0, #-1
    BRz JUMP_TO_PROCESS4


    ;; PROCESS 5
    AND R2, R2, #0
    LD R2,P5_STATUS
    LDR R0,R2,#0
    ADD R0, R0, #-1
    BRz JUMP_TO_PROCESS5


    TRAP xFF


    Loop:
    BRnzp Loop

JUMP_TO_PROCESS1:
    ;; Change status of P1 to 2
    AND R2, R2, #0
    ADD R2, R2, #2
    ST R2, P1_STATUS


    AND R4, R4, #0
    LD R4, P1_STATUS

    AND R3, R3, #0
    LD R3, P1_ADDR_LABEL

    TRAP xFF

    AND R5, R5, #0
    STI R5, MR_SM

    JMP R3


JUMP_TO_PROCESS2:
    ;; Change status of P1 to 2
    AND R2, R2, #0
    ADD R2, R2, #2
    ST R2, P2_STATUS


    AND R4, R4, #0
    LD R4, P2_STATUS

    AND R3, R3, #0
    LD R3, P2_ADDR_LABEL

    TRAP xFF

    AND R5, R5, #0
    STI R5, MR_SM

    JMP R3



JUMP_TO_PROCESS3:
    ;; Change status of P1 to 2
    AND R2, R2, #0
    ADD R2, R2, #2
    ST R2, P1_STATUS


    AND R4, R4, #0
    LD R4, P3_STATUS

    AND R3, R3, #0
    LD R3, P3_ADDR_LABEL

    TRAP xFF

    AND R5, R5, #0
    STI R5, MR_SM

    JMP R3


JUMP_TO_PROCESS4:
    ;; Change status of P1 to 2
    AND R2, R2, #0
    ADD R2, R2, #2
    ST R2, P1_STATUS


    AND R4, R4, #0
    LD R4, P4_STATUS

    AND R3, R3, #0
    LD R3, P4_ADDR_LABEL

    TRAP xFF

    AND R5, R5, #0
    STI R5, MR_SM

    JMP R3


JUMP_TO_PROCESS5:
    ;; Change status of P1 to 2
    AND R2, R2, #0
    ADD R2, R2, #2
    ST R2, P1_STATUS


    AND R4, R4, #0
    LD R4, P5_STATUS

    AND R3, R3, #0
    LD R3, P5_ADDR_LABEL

    TRAP xFF

    AND R5, R5, #0
    STI R5, MR_SM

    JMP R3


MR_SM: .FILL xFE04
MR_MPU: .FILL xFE06

P1_STATUS: .FILL x0200
P2_STATUS: .FILL x0201
P3_STATUS: .FILL x0202
P4_STATUS: .FILL x0203
P5_STATUS: .FILL x0204

P1_ADDR_LABEL: .FILL x4000
P2_ADDR_LABEL: .FILL x5000
P3_ADDR_LABEL: .FILL x6000
P4_ADDR_LABEL: .FILL x7000
P5_ADDR_LABEL: .FILL x8000

.END