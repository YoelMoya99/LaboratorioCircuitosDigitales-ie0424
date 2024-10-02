// memory-mapped I/O addresses
#define GPIO_LEDs  0x80001404
#define GPIO_Boton 0x80001800
#define GPIO_INOUT 0x80001408

#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_LEDS(dir, value) {(*(volatile unsigned *)dir) = (value);}



int main (void)
{


// Valores iniciales
int enable_value = 0xFFFF;
unsigned int DELAY = 3000000;
unsigned int boton_value = 0;
int posicion_Led = 0x00000001;  // Posicion inicial del led (solo uno encendido).

WRITE_LEDS(GPIO_INOUT, enable_value);

static int i,j,k;

while(1){
    // Desplazamieno de los leds a la izquierda
    for( i = 0; i < (16); i++)
    {
        boton_value = READ_GPIO(GPIO_Boton);
        WRITE_LEDS(GPIO_LEDs, posicion_Led);       
        posicion_Led = posicion_Led + 1;            // Se desplazando a la derecha

        // Si el valor de los botones es igual a BTNC cambiamos la velocidad
        if (boton_value & 0x1){
            if (DELAY <= 50){
                DELAY = 3000000;
            }
            DELAY = DELAY - 500000;
        }

        // Si se lee BTNU reiniciamos el timer y el valor inicial de leds
        else if (boton_value & 0x2){
        DELAY = 3000000;
        posicion_Led = 0x00000001;  // retornando posicion inicial
        }

        //Implementacion del retardo
        for (k = 0; k < DELAY; k++){
            j++;
        }
    }
}
}