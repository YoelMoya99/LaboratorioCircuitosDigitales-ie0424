//Direcciones de memoria para encender el PTC asociado
#define PTCBase 0x80001200
#define  LRC 0x80001208
#define  HRC 0x80001204
#define CTRL 0x8000120C

// 7 segmentos
#define SEG7_ENABLE 0x80001038
#define SEG7_DATA 0x8000103C

//Valores para determinar el ciclo de trabajo de los leds
#define HRC_Value 0x2FAF080
#define LRC_Value 0x2FAF080

// Banderas
#define INT 0x40
#define INTERRUPT_START 0x21

// Escritura y lectura de direcciones 
#define READ_MEMORY(dir) (*(volatile unsigned *)dir)
#define WRITE_MEMORY(dir, value) {(*(volatile unsigned *)dir) = (value);}

int main() {

    unsigned int Segundos = 0;
    WRITE_MEMORY(SEG7_ENABLE, 0xF8); // Habilita 3 displays
    WRITE_MEMORY(SEG7_DATA, Segundos); //Inicializa en cero el display
    WRITE_MEMORY(LRC, LRC_Value); // una interrupcion no deseada
    WRITE_MEMORY(HRC, HRC_Value); // Colocando en ambos el mismo valor para evitar
    WRITE_MEMORY(CTRL, INTERRUPT_START);     // Habilitando interrupciones y contador, asi como
    
    while (1) {
        if (READ_MEMORY(CTRL) & INT){
            WRITE_MEMORY(CTRL, INTERRUPT_START);     // Habilitando interrupciones y contador, asi como
            Segundos++;
            WRITE_MEMORY(SEG7_DATA, Segundos);
        }
    }
    
}