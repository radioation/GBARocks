    .text
    .align 2
    .arm
    .global my_add
    .type my_add, %function

@ int myadd(int a, int b)
@   
@ > * r0 to r3: Argument values passed to a subroutine and results returned from a subroutine.
@ > If the type of value returned is too large to fit in r0 to r3, or
@ > whose size cannot be determined statically at compile time, then the
@ > caller must allocate space for that value at run time, and pass a
@ > pointer to that space in r0.
@   result goes out via r0
my_add:
    add     r0, r0, r1     @ r0 = r0 + r1 (stores in r0)
    bx      lr             @ br: Branch and eXchange  
                           # lr : link register (also r14.  stores return address 
