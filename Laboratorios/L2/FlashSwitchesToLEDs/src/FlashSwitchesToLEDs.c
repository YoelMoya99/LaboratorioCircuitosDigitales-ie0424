/*
Direcciónes de memoria de los LEDs, switches, y el registro de
direcciónamiento de datos para entrada y/o salida de 
estos.
*/

#define GPIO_SWs    0x80001400
#define GPIO_LEDs   0x80001404
#define GPIO_INOUT  0x80001408

/*
Macros utilizados para lectura y escritura de las posiciones de
memoria utilizadas por los perifericos.
*/

#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_GPIO(dir, value) { (*(volatile unsigned *)dir) = (value); }

void delay(int timer);

int main ( void )
{
    /*
    Estructuras de datos globales utilizadas en el programa 
    */

    int En_Value=0xFFFF, switches_value;
    int timer = 2500000;


    WRITE_GPIO(GPIO_INOUT, En_Value);

    while (1) { 

        /*
        Lectura, desplazamiento y escritura de los valores encontrados
        en los switches, a los valores de los LEDs.
        */ 
        
        switches_value = READ_GPIO(GPIO_SWs);   // read value on switches
        switches_value = switches_value >> 16;  // shift into lower 16 bits
        WRITE_GPIO(GPIO_LEDs, switches_value);  // display switch value on LEDs
    
        delay(timer); // Tiempo de espera entre escrituras    

        WRITE_GPIO(GPIO_LEDs, 0x0000); // Apagado de los leds

        delay(timer);  // Tiempo de espera entre escrituras  

    }
    return(0);
}

void delay(int timer){

    /*
    Implementación del retardo temporal entre escrituras de patrón de LEDs.
    */

    static int j, i; // Enteros estaticos para evitar optimizaciónes del compilador.
    for (i = 0; i < timer; i++){
        j++; // Tarea de relleno para evitar optimizaciones del compilador.
    } 
}