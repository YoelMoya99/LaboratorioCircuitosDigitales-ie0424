.globl main

.data
A: .word 48, 64, 56, 80, 96, 48

.equ N, 6

.bss
B: .space 4*N

.text

MyFilter:

    addi sp, sp, -16
    sw t0, 0(sp)
    sw t1, 4(sp)
    sw t2, 8(sp)
    sw t3, 12(sp)

    # abre espacio para el stack

    mv t0, a2
    mv t1, a3

    andi t2, t0, 0x000F

    beq t2, zero, si_es_multiplo_de_16

asigna0:

    mv a0, zero
    j fin_MyFilter

si_es_multiplo_de_16:
    li t3, 1
    sub t1, t1, t3
    bge t1, t0, asigna1

    j asigna0

asigna1:
    li a0, 1

fin_MyFilter:
    
    lw t0, 0(sp)
    lw t1, 4(sp)
    lw t2, 8(sp)
    lw t3, 12(sp)

    addi sp, sp, 16
    # cierra el espacio para el stack
    ret



main:

    li t0, 0x0 # i
    li t1, 0x0 # j 
    li t2, N 
    li t3, 1
    sub t2, t2, t3
for:

    la t3, A 
    li t4, 4
    mul t4, t4, t0 # i * 4
    add a2, t4, t3  # A + 4 * i
    lw a3, 4(a2)  # A[i+1]
    lw a2, 0(a2)  # A[i]

    # Moviendo preventivamente los resultados para no cargarlos dos veces
    mv t3, a3
    mv t4, a2

    jal MyFilter

    beq a0, zero, no_asigna_a_b

    add t3, t3, t4
    addi t3, t3, 2

    la t5, B 
    li t4, 4
    mul t4, t4, t1

    add t5, t4, t5
    sw t3, 0(t5)
    addi t1, t1, 1
no_asigna_a_b:
    
    # incrementando contadores y evaluando la condicion
    addi t0, t0, 1

    bne t0, t2, for
.end