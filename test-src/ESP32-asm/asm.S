.global getMatrix

; int assemblyAdd(a2, a3)
; should return a mapper
; here is a good approach :D https://stackoverflow.com/questions/11979788/arm-assembly-loop

getMatrix:      entry   a1, 48
                mov     r0, zero,       ;save initializer
                mov     r1, a2,         ;save number of necessary iterations
                mov     a2, 1           ;initialize result
                .loop
                add     r0, r0, #1      ;increment by one
                cmp     r0, r1          ;check if below limit
                mov     r3, r3, lsl 1   ;do some work 
                ble     loop            ;loop if not finished
                retw.n
