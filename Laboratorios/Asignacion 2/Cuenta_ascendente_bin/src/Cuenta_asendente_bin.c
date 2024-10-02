// memory-mapped I/O addresses
#define GPIO_LEDs 0x80001404
#define GPIO_Boton 0x80001800
#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_LEDS(dir, value) {(*(volatile unsigned *)dir) = (value);}



int main (void)
{

// Valores iniciales
unsigned int DELAY = 2000000;
unsigned int boton_value = 0;
int posicion_Led = 0x00008000;  // Posicion inicial del led (solo uno encendido).

while(1){
    // Desplazamieno de los leds a la izquierda
    for(int i = 0; i < (16); i++)
    {
        boton_value = READ_GPIO(GPIO_Boton);
        WRITE_LEDS(GPIO_LEDs, posicion_Led);       
        posicion_Led = posicion_Led >> 1;            // Se desplazando a la derecha

        // Si el valor de los botones es igual a BTNC cambiamos la velocidad
        if (boton_value & 0x1){
        DELAY = DELAY - 5000;
        }
        // Si se lee BTNU reiniciamos el timer
        else if (boton_value & 0x2){
        DELAY = 2000000;
        }
        for (int k = 0; k < DELAY; k++){}
    }
    posicion_Led = 0x00008000;  // retornando posicion inicial
}
return (0);
}