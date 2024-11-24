# Valores del modulo SPI

#define SPCR 0x80001100
#define SPSR 0x80001108
#define SPDR 0x80001110
#define SPER 0x80001118
#define SPCS 0x80001120

# Valores del modulo de 7 segmentos

#define SegEn_ADDR      0x80001038
#define SegDig_ADDR     0x8000103C

.globl main

.text
    # --------------------- Inicio de sub rutinas -----------------------
spiInit:
    li t1, SPCR
    li t0, 0x53
    sb t0, 0(t1)
    li t1, SPER
    li t0, 0x02
    sb t0, 0(t1)
ret

spiCS:
    li t0, SPCS
    sb a0, 0(t0)
ret

spiCSUp:
    li a0, 0x00
    j spiCS

spiCSDown:
    li a0, 0xFF
    j spiCS

spiSendGetData:
internalSpiClearIF:
    li t1, SPCR
    lb t0, 0(t1)
    ori t0,t0,0x80
    sb t0, 0(t1)
internalSpiActualSend:
    li t0, SPDR
    sb a0, 0(t0)
internalSpiTestIF:
    li t1, SPSR
    lb t0, 0(t1)
    andi t0,t0,0x80
    li t1,0x80
    bne t0, t1,internalSpiTestIF
internalSpiReadData:
    li t0, SPDR
    lb a1, 0(t0)
ret

# ------------------ Bloque de codigo principal -------------------
main:

    # Inicializa la pantalla de 7 segmentos con valores intercalados
    # para cada uno de los digitos a mostrar
    li t0, 0x24
    li t1, SegEn_ADDR
    sb t0,0(t1) 

    # La pantalla de siete segmentos incia en ceros
    li t0, SegDig_ADDR
    li t1, 0x0
    sw t1, 0(t0)

    jal spiInit

InicializacionDeNuevaSerieDeTransacciones:

    jal spiCSUp
    li a0, 0x0B
    jal spiSendGetData
    li a0, 0x08
    jal spiSendGetData
    li a0, 0xFF
    jal spiSendGetData
    # Retorna el valor por a1
    jal spiCSDown

    li t0, 0xFFFF
delay1:
    li t1, 1
    sub t0, t0, t1
    bne zero, t0, delay1

    // Aislando el byte de resultado para X
    li t0, 0xFF
    and a2, a1, t0

    jal spiCSUp
    li a0, 0x0B
    jal spiSendGetData
    li a0, 0x09
    jal spiSendGetData
    li a0, 0xFF
    jal spiSendGetData
    # Retorna el valor por a1
    jal spiCSDown

    li t0, 0xFFFF
delay2:
    li t1, 1
    sub t0, t0, t1
    bne zero, t0, delay2

    // Aislando el byte de resultado para Y
    li t0, 0xFF
    and a3, a1, t0

    jal spiCSUp
    li a0, 0x0B
    jal spiSendGetData
    li a0, 0x10
    jal spiSendGetData
    li a0, 0xFF
    jal spiSendGetData
    # Retorna el valor por a1
    jal spiCSDown

    li t0, 0xFFFF
delay3:
    li t1, 1
    sub t0, t0, t1
    bne zero, t0, delay3

    // Aislando el byte de resultado para Z
    li t0, 0xFF
    and a4, a1, t0

    // Para escribir a los 7 segmentos tenemos que escribir
    // un word, entonces hay que unificar todas las lecturas
    // en un registro

    slli a2, a2, 24
    slli a3, a3, 12
    
    add a2, a2, a3
    add a2, a2, a4  
 
    li t0, SegDig_ADDR
    sw a2, 0(t0)

    li t0, 0xFFFF
delay:
    li t1, 1
    sub t0, t0, t1
    bne zero, t0, delay

    j InicializacionDeNuevaSerieDeTransacciones

.end

# ---------------- Fin bloque de codigo principal -------------------
