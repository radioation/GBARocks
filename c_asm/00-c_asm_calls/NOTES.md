# Calling Convention NOTES
##  callling convention 
[here](https://en.wikipedia.org/wiki/Calling_convention#ARM_(A32))

> ARM (A32)
> The standard 32-bit ARM calling convention allocates the 16 general-purpose registers as:
> 
> * r15: Program counter (as per the instruction set specification).
> * r14: Link register. The BL instruction, used in a subroutine call, stores the return address in this register.
> * r13: Stack pointer. The Push/Pop instructions in "Thumb" operating mode use this register only.
> * r12: Intra-Procedure-call scratch register.
> * r4 to r11: Local variables.
> * r0 to r3: Argument values passed to a subroutine and results returned from a subroutine.
> If the type of value returned is too large to fit in r0 to r3, or
> whose size cannot be determined statically at compile time, then the
> caller must allocate space for that value at run time, and pass a
> pointer to that space in r0.
> 

So a simple add function would take two arguments (r0 and r1) and store
the return value in r0 (as it's returning an int)

```s
    .text
    .align 2
    .arm
    .global my_add
    .type my_add, %function
my_add:
    add     r0, r0, r1     @ r0 = r0 + r1 (stores in r0)
    bx      lr             @ br: Branch and eXchange  
                           # lr : link register (also r14.  stores return address 
```

The `bx lr` returns us to the caller. lr (A.K.A. r14) has the address we're returning to



Start you assembly file with: 

* `.arm` : gells gas to generate ARM and not Thumb code?
* `.text` : Tells as to assemble the following statements onto the end of the
  text subsection numbered subsection, which is an absolute expression. If
  subsection is omitted, subsection number zero is used.  `.global` `.globl` :
* `.global` makes the symbol visible to ld. This lets
  us link to it from C
* `.align 2`   Align to word boundary

GNU Assembler [docs](https://sourceware.org/binutils/docs/as/)

## C and ASM examples

A simple example from [patater](https://www.patater.com/gbaguy/gba/ch4.htm)
```s
.arm
.text
.global main
main:
	mov r0, #0x4000000  @ the usual set up routine
	mov r1, #0x400   @ 0x403 is BG 2 enable, and mode 3.
	add r1, r1, #3
	strh r1, [r0]   @ the memory I/O value we're setting is actually 16bits, let's not mess 
			@ something else up by writting 32.
	
	mov r0, #0x6000000  @ address of VRAM

...


```

[tonc says](https://www.coranac.com/tonc/text/asm.htm)
> The most important one is ‘.code n’, where n is 32 or 16 for ARM or
> THUMB code respectively. You can also use the more descriptive .arm
> and thumb directives, which do the same thing. T

```s
@ ARM function definition
@ void m5_plot_arm(int x, int y, u16 clr)
    .align 2                    @ Align to word boundary
    .arm                        @ This is ARM code
    .global m5_plot_arm         @ This makes it a real symbol
    .type m5_plot_arm STT_FUNC  @ Declare m5_plot_arm to be a function.
m5_plot_arm:                    @ Start of function definition
    add     r1, r1, lsl #2
    add     r0, r1, lsl #5
    ldr     r1,=vid_page
    ldr     r1, [r1]
    mov     r0, r0, lsl #1
    strh    r2, [r1, r0]
    bx      lr
```


