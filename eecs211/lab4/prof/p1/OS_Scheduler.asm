.ORIG   x300

CHECK_RUNNING_TOP
        LD      R0,     zero        
        ST      R0,     j
CHECK_RUNNING_NEXT
        ; check running
        LD      R5,     two
        LD      R3,     S_Array
        LD      R4,     j
        ADD     R3,     R3,     R4
        LDR     R3,     R3,     #0 
        NOT     R3,     R3          
        ADD     R3,     R3,     #1  
        ADD     R5,     R5,     R3

        BRnp CHECK_IF_NEXT_INST_RUNNING
        LD      R5,     zero
        LD      R3,     S_Array
        LD      R4,     j
        ADD     R3,     R3,     R4
        STR     R5 ,    R3,     #0  
        BR      DO_TOP_READY
        ; do sthg - set the value to 0, run the next inst

CHECK_IF_NEXT_INST_RUNNING
        LD      R0,     j          ;
        ADD     R5,     R0,     #1
        ST      R5,     j
        LD      R6,     ARRAYSIZE   ;  
        NOT     R6,     R6          ; 
        ADD     R6,     R6,     #1
        ADD     R0,     R0,     R6  ; 
        BRn     CHECK_RUNNING_NEXT  ; 



DO_TOP_READY  
        ; check if reddy
        LD      R5,     one
        LD      R3,     S_Array
        LD      R4,     i
        ADD     R3,     R3,     R4
        LDR     R3,     R3,     #0 
        NOT     R3,     R3          
        ADD     R3,     R3,     #1  
        ADD     R5,     R5,     R3 
        BRnp CHECK_RUNNING
        ; do sthg - run this program, set flag running maybe check if there are other programs running and change flag to 1
        LD      R5,     two
        LD      R3,     S_Array
        LD      R4,     i
        ADD     R3,     R3,     R4
        STR     R5 ,    R3,     #0  
        
        RUN_1st
            LD  R0, i           
            LD  R6, zero           
            NOT R6, R6          
            ADD R6, R6, #1      
            ADD R0, R0, R6      
            BRnp RUN_2nd  
            ; dump and set ME_SM to zero
            LD      R0,     i           ; i++
            ADD     R5,     R0,     #1
            ST      R5,     i

            LD      R4,     MR_SM
            LD      R5,     zero
            STR     R5,     R4,     #0
            LD      R3,     inst1
            TRAP    xFF
            JMP     R3 

        RUN_2nd                
            LD  R0, i           
            LD  R6, one           
            NOT R6, R6          
            ADD R6, R6, #1      
            ADD R0, R0, R6      
            BRnp RUN_3rd  
            ; dump and set ME_SM to zero
            LD      R0,     i           ; i++
            ADD     R5,     R0,     #1
            ST      R5,     i

            LD      R4,     MR_SM
            LD      R5,     zero
            STR     R5,     R4,     #0
            LD      R3,     inst2
            TRAP    xFF
            JMP     R3

        RUN_3rd                
            LD  R0, i           
            LD  R6, two           
            NOT R6, R6          
            ADD R6, R6, #1      
            ADD R0, R0, R6      
            BRnp RUN_4th  
            ; dump and set ME_SM to zero
            LD      R0,     i           ; i++
            ADD     R5,     R0,     #1
            ST      R5,     i

            LD      R4,     MR_SM
            LD      R5,     zero
            STR     R5,     R4,     #0
            LD      R3,     inst3
            TRAP    xFF
            JMP     R3

        RUN_4th                
            LD  R0, i           
            LD  R6, three           
            NOT R6, R6          
            ADD R6, R6, #1      
            ADD R0, R0, R6      
            BRnp RUN_5th  
            ; dump and set ME_SM to zero
            LD      R0,     i           ; i++
            ADD     R5,     R0,     #1
            ST      R5,     i

            LD      R4,     MR_SM
            LD      R5,     zero
            STR     R5,     R4,     #0
            LD      R3,     inst4
            TRAP    xFF
            JMP     R3

        RUN_5th                
            LD  R0, i           
            LD  R6, four           
            NOT R6, R6          
            ADD R6, R6, #1      
            ADD R0, R0, R6      
            BRnp FINAL  
            ; dump and set ME_SM to zero
            LD      R0,     i           ; i++
            ADD     R5,     R0,     #1
            ST      R5,     i
            
            LD      R4,     MR_SM
            LD      R5,     zero
            STR     R5,     R4,     #0
            LD      R3,     inst5
            TRAP    xFF
            JMP     R3 

CHECK_RUNNING
        ; check running
        LD      R5,     two
        LD      R3,     S_Array
        LD      R4,     i
        ADD     R3,     R3,     R4
        LDR     R3,     R3,     #0 
        NOT     R3,     R3          
        ADD     R3,     R3,     #1  
        ADD     R5,     R5,     R3

        BRnp CHECK_NEXT_INST
        LD      R5,     zero
        LD      R3,     S_Array
        LD      R4,     i
        ADD     R3,     R3,     R4
        STR     R5 ,    R3,     #0  
        ; do sthg - set the value to 0, run the next inst

CHECK_NEXT_INST
        LD      R0,     i           ;
        ADD     R5,     R0,     #1
        ST      R5,     i
        LD      R6,     ARRAYSIZE   ;  
        NOT     R6,     R6          ; 
        ADD     R6,     R6,     #1
        ADD     R0,     R0,     R6  ; 
        BRn     DO_TOP_READY              ; 

 FINAL
        LD      R6,     zero
        ST      R6,     i
        ADD     R6,     R6,     #0
        BR     CHECK_RUNNING_TOP

i         .BLKW     1
j         .BLKW     1
zero      .FILL     #0
one       .FILL     x01
two       .FILL     x02
three     .FILL     x03
four      .FILL     x04
ARRAYSIZE .FILL     #5 
S_Array   .FILL     x0200
inst1 .FILL   x4000 
inst2 .FILL   x5000 
inst3 .FILL   x6000 
inst4 .FILL   x7000 
inst5 .FILL   x8000
MR_SM .FILL   xFE04 
        .END