.ORIG 	x100
TRAP_HALT
    ; set ME_SM to one
    TRAP        xFF                ; dump mem and reg
    LD          R3,                 Scheduler_A
    JMP         R3
Scheduler_A     .FILL       x300
.END