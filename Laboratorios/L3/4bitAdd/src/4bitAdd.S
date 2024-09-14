// memory-mapped I/O addresses
# GPIO_SWs   = 0x80001400
# GPIO_LEDs  = 0x80001404
# GPIO_INOUT = 0x80001408

.globl main


main:
  li t0, 0x80001400   # base address of GPIO memory-mapped registers
  li t1, 0xFFFF       # set direction of GPIOs
                      # upper half = switches (inputs)  (=0)
                      # lower half = outputs (LEDs)     (=1)
  sw t1, 8(t0)        # GPIO_INOUT = 0xFFFF

repeat:
  lw   t1, 0(t0)      # read switches: t1 = GPIO_SWs
  srli t1, t1, 16     # Desplazamiento inicial
  andi t2,t1, 0x000F  # Mascara para extraer los 4 menos significativos
  srli t3, t1, 12     # Desplazamiento a los menos significativos
  add t4, t3, t2      # Suma binaria
  andi t4, t4, 0x001F # activando acarrero si es los 4 bits
  
  sw   t4, 4(t0)      # write value to LEDs: GPIO_LEDs = t1
  j    repeat         # repeat loop