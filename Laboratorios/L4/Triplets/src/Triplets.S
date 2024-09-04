# memory-mapped I/O addresses
# GPIO_SWs   = 0x80001400
# GPIO_LEDs  = 0x80001404
# GPIO_INOUT = 0x80001408


.globl main

.data
A: .word 7,3,25,-6,5,-8,10,22,69

.equ N, 3

.bss
B: .space 4*N

.text

res_triplet:
    addi sp, sp, -20  # apartando registros
    sw t0, 0(sp)
    sw t1, 4(sp)
    sw t2, 8(sp)
    sw t3, 12(sp)
    sw t4, 16(sp)

    # calculando dir de j
    li t0, 4
    mul a3, a3,t0

    add t0, a2, a3  # t0 = V + pos
    lw t1, 0(t0)
    lw t2, 4(t0)
    lw t3, 8(t0)

    add t4, t1,t2
    add t4, t4,t3

    li t3, 0x8000  # Mascara para tomar solo el bit mas significativo
    li t5, 16
    sll t3, t3,t5 # 1000 0000  .... 0000
    and t3, t3, t4 # Aplicando mascara

    beq t3, zero, Es_posi 
    # Si es negativo invetimos
    neg a0, t4

    j Fin_res_triplet # Restornamos

Es_posi:
    # Si es positivo dejelo asi
    mv a0, t4 


Fin_res_triplet:
    lw t0, 0(sp)
    lw t1, 4(sp)
    lw t2, 8(sp)
    lw t3, 12(sp)
    lw t4, 16(sp)
    addi sp, sp, 20  # Retornando pila
    ret


main:


    li t0, 0  # i
    li t3, 0  # J
    li t4, N  # N
For1:
    
    la a2, A        # a2 hasta a7 son registros para pasar argumentos a funcion
    mv a3, t3
    jal res_triplet  # res_triplet(A,j)
    li t1, 4
    mul t1, t0, t1   # 4 * i
    la t2, B         # Treyendo el puntero de N 
    add t2,t2,t1     # t2 = B + 4*i

    sw a0, 0(t2)     # a0 y a1 son resgitros de retorno de las funciones
    addi t3,t3,3     # t3 + 3
    addi t0,t0,1    # i++
    bne t0, t4, For1 # hasta que no se recorra todo quedese
    

.end


