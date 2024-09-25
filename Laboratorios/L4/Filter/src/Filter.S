.globl main

.data
A: .word 48, 64, 56, 80, 96, 48 # Se define arreglo de pruebas.

.equ N, 6 # Se define tamaño maximo del arreglo B

.bss
B: .space 4*N # Se reserva 6 espacios alineados a bloques de 4
              # bytes.

.text

MyFilter:

    # Definición de la subrutina filtro. Inicialmente abre espacio en el stack
    # para todos los registros que utiliza ella, independientemente si son 
    # utilizados en el bloque de codigo principal.

    addi sp, sp, -16
    sw t0, 0(sp)
    sw t1, 4(sp)
    sw t2, 8(sp)
    sw t3, 12(sp)

    # Trae los valores de los registros tipicamente utilizados para pasar
    # argumentos a funciones.

    mv t0, a2
    mv t1, a3

    andi t2, t0, 0x000F  # aisla el byte menos significativo del primer
                         # valor el cual es cero unicamente para multiplos
                         # de 16.

    beq t2, zero, si_es_multiplo_de_16 # Salta si el BMS es cero.

asigna0:

    # Si no es multiplo de 16, no se evalua la segunda condición, ya que
    # no cumple con lo solicitado y se devuelve un cero.
    mv a0, zero
    j fin_MyFilter # Salta a fin.

si_es_multiplo_de_16:
    li t3, 1              # Si si es multiplo de 16, se evalua la segunda condición
    sub t1, t1, t3        # donde se resta uno al segundo valor, y se prueba si es
    bge t1, t0, asigna1   # un numero mayor o igual que el primer valor, realizando
                          # la comprobación si es mayor o estricto, con las
                          # instrucciones disponibles por el set.

    j asigna0             # si no cumple, se asigna cero

asigna1:
    li a0, 1              # si cumple todas las condiciones, se retorna un 1.

fin_MyFilter:
    
    lw t0, 0(sp)
    lw t1, 4(sp)
    lw t2, 8(sp)
    lw t3, 12(sp)

    addi sp, sp, 16
    # cierra el espacio para el stack, devolviendo el contexto del programa principal
    # a su estado original.
    ret



main:

    li t0, 0x0 # i
    li t1, 0x0 # j 
    li t2, N   # Tamaño maximo de B.
    li t3, 1
    sub t2, t2, t3 # resta un valor al tamaño maximo de B
for:

    la t3, A       # Carga dirección base del arreglo A
    li t4, 4       # Carga valor de alineamiento de memoria
    mul t4, t4, t0 # i * 4
    add a2, t4, t3  # A + 4 * i
    lw a3, 4(a2)  # A[i+1]
    lw a2, 0(a2)  # A[i]

    # Moviendo preventivamente los resultados para no cargarlos dos veces
    # ya que pueden ser utilizados mas adelante, y no hace falta gastar
    # tiempo de procesamiento obteniendo los valores de nuevo.
    mv t3, a3
    mv t4, a2

    jal MyFilter  # Llamado a sub rutina que realiza el filtrado

    beq a0, zero, no_asigna_a_b # Salta si el valor no es multiplo de 16
                                # o el segundo no es mayor que el primero.

    add t3, t3, t4 # Si el primer valor es multiplo de 16 y el segundo
    addi t3, t3, 2 # es mayor que el primero, suma ambos valores y el
                   # numero 2.

    la t5, B       # Realiza el proceso de calculo de la dirección del 
    li t4, 4       # arreglo de resultado para guardar el valor calculado,
    mul t4, t4, t1 # tomando en cuenta la memoria alineada a 4 bytes.

    add t5, t4, t5
    sw t3, 0(t5)   # Incrementa el contador de B unicamente.
    addi t1, t1, 1
no_asigna_a_b:
    
    # incrementa el contador de A unicamente.
    addi t0, t0, 1

    bne t0, t2, for # salta si no ha llegado al fin.
.end
