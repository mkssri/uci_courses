    .ORIG x0400
    
    TRAP xFF ;;dump memory with trap at start of trap subroutine


    ;; RESUME
    AND R2, R2, #0
    AND R2, R2, #1
    STR R2, R4, #0


SCHEDULER:

    ;; PROCESS 1
    AND R0, R0, #0
    LDR R0,R1,#0
    ADD R0, R0, #-5
    BRz JUMP_TO_PROCESS1
    

    ;; PROCESS 2
    AND R0, R0, #0
    LDR R0,R1,#0
    ADD R0, R0, #-1    
    BRz JUMP_TO_PROCESS2


    ;; PROCESS 3
    AND R0, R0, #0
    LDR R0,R1,#0
    ADD R0, R0, #-2
    BRz JUMP_TO_PROCESS3


    ;; PROCESS 4
    AND R0, R0, #0
    LDR R0,R1,#0
    ADD R0, R0, #-3
    BRz JUMP_TO_PROCESS4


    ;; PROCESS 5
    AND R2, R2, #0
    LD R2,P5_STATUS
    LDR R0,R2,#0
    ADD R0, R0, #-4
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
    LD R3, P1_RESUME_ADDR_LABEL
    TRAP xFF
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
    JMP R3



P1_STATUS: .FILL x0200
P2_STATUS: .FILL x0201
P3_STATUS: .FILL x0202
P4_STATUS: .FILL x0203
P5_STATUS: .FILL x0204


P1_RESUME_ADDR_LABEL: .FILL x4003
P2_RESUME_ADDR_LABEL: .FILL x5003
P3_RESUME_ADDR_LABEL: .FILL x6003
P4_RESUME_ADDR_LABEL: .FILL x7003
P5_RESUME_ADDR_LABEL: .FILL x8003

.END