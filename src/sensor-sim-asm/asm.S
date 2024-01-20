.global getMask

getMask:  entry a1, 48
            movi a5, 0          # init increment
            mov a6, a2          # save number of necessary iterations
            movi a2, 1          # init final result

loop: 
            addi a5, a5, 1      # increment by one
            beq a5, a6, end     # branch to end if equal
            slli a2, a2, 1      # left shif by 1, i.e. 0001 -> 0010
            j loop              # loop

end:
            retw.n
