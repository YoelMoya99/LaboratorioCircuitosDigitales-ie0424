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
    li t0, SPER
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
    li t0, 0xDB
    li t1, SegEn_ADDR
    sb t0,0(t1) 

    # La pantalla de siete segmentos incia en ceros
    li t0, SegDig_ADDR
    li t1, 0x0
    sw t1, 0(t0)

    jal spiInit

InicializacionDeNuevaSerieDeTransacciones:
    li a3, 0x3
    li a2, 0x8

IniciaUnaTransaccion:
    jal spiCSUp
    li a0, 0x0B
    jal spiSendGetData
    mv a0, t2
    jal spiSendGetData
    li a0, 0xFF
    jal spiSendGetData
    # Retorna el valor por a1
    jal spiCSDown

    li t0, 0x3
    beq a3, t0, LlevamosUnValor
    li t0, 0x2
    beq a3, t0, LlevamosDosValores
    li t0, 0x1
    beq a3, t0, LlevamosTresValores
    beq zero,zero,MostrarValoresYReiniciar

LlevamosUnValor:
    mv a1, a6
    j DecrementarValores

LlevamosDosValores:
    mv a1, a5
    j DecrementarValores

LlevamosTresValores:
    mv a1, a4
    j DecrementarValores

MostrarValoresYReiniciar:
    li t1, 0xFF # Mascara para limpiar
    li t0, 0x18 # 24 en hex
    and a6, a6, t1  # limpia
    sll a6, a6, t0 # Desplaza
    li t0, 0xC # 12 en hexadecimal
    and a5, a5, t1 # Limpia
    sll a5, a5, t0 # Desplaza
    and a4, a4, t1 # limpia
    or a4, a4, a5
    or a4, a4, a6
    
    li t0, SegDig_ADDR
    sw a4, 0(t0)

    j InicializacionDeNuevaSerieDeTransacciones

DecrementarValores:
    li t0, 0x1
    sub a3, a3, t0
    add a2, a2, t0
    j IniciaUnaTransaccion

.end

# ---------------- Fin bloque de codigo principal -------------------
