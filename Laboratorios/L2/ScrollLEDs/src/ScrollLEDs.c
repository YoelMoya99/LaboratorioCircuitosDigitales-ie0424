/*
Direcciónes de memoria de los LEDs, y el registro de
direcciónamiento de datos para entrada y/o salida de 
estos.
*/

#define GPIO_LEDs   0x80001404
#define GPIO_INOUT  0x80001408

/*
Macros utilizados para lectura y escritura de las posiciones de
memoria utilizadas por los perifericos.
*/

#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_GPIO(dir, value) { (*(volatile unsigned *)dir) = (value); }

void delay(int timer);

int main ( void ){

    /*
    Estructuras de datos globales utilizadas en el programa 
    */

    int En_Value=0xFFFF; // Valor inicialización para I/O.
    int timer = 2500000; // Valor de iteraciónes para generar retardo.
    int LEDs_value = 0x0001; // Valor inicial de los LEDs.

    WRITE_GPIO(GPIO_INOUT, En_Value)

    // Ciclo principal.
    while (1){

        // Verificación de llegada al final del patrón de desplazamiento
        // de los LEDs.
        if (LEDs_value == 0xFFFF)
        {
            LEDs_value = 0x0001;
            WRITE_GPIO(GPIO_LEDs, LEDs_value);
            delay(timer);
        }

        // Ciclo de desplazamiento hacia la izquierda de un unico patrón de
        // LEDs.
        while ((LEDs_value & 0x8000) == 0)
        {
            /*
            Se aísla el bit mas significativo del patron a ser desplazado,
            con el objetivo de evaluar si el desplazamiento ya ha llegado 
            hasta el final. De no ser asi, se continua desplazando. 
            */
            LEDs_value = LEDs_value << 1;      // desplaza 1 posición.
            WRITE_GPIO(GPIO_LEDs, LEDs_value); // Escribe a LEDs.
            delay(timer);                      // Tiempo de espera para la proxima escritura
        }
        
        // Ciclo de desplazamiento hacia la derecha de un unico patrón de
        // LEDs.
        while ((LEDs_value & 0x0001) == 0)
        {
            /*
            Se aísla el bit menos significativo del patron a ser desplazado,
            con el objetivo de evaluar si el desplazamiento ya ha llegado 
            hasta el final. De no ser asi, se continua desplazando. 
            */
            LEDs_value = LEDs_value >> 1;      // desplaza 1 posición.
            WRITE_GPIO(GPIO_LEDs, LEDs_value); // Escribe a LEDs.
            delay(timer);                      // Tiempo de espera para la proxima escritura
        }

        /*
        Incremento del patrón de los leds en una posición.
        */
        LEDs_value = LEDs_value << 1; // desplaza una posición hacia la izquierda
        LEDs_value++; // Aumenta en uno la cantidad de LEDs a desplazar
        WRITE_GPIO(GPIO_LEDs, LEDs_value); // Escribe a LEDs.
        delay(timer); // Tiempo de espera para la proxima escritura.

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