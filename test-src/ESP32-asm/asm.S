.global getMatrix

getMatrix:  entry a1, 48
            movi a5, 0          # init increment
            mov a6, a2          # save number of necessary iterations
            movi a3, 1          # init final result 0000 0001

loop:
            addi a5, a5, 1      # increment by one
            slli a3, a3, 1      # do some work 
            bne a5, a6, loop    # branch to loop if not equal
            retw.n
