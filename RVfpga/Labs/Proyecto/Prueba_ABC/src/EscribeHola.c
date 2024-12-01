//Direcciones de memoria para encender el PTC asociado
#define I_SA 0x4FFF2408
#define Y_HI 0x44FF484F

// 7 segmentos
#define SEG7_RIGHT 0x80001038
#define SEG7_LEFT 0x8000103C

// Escritura y lectura de direcciones 
#define READ_MEMORY(dir) (*(volatile unsigned *)dir)
#define WRITE_MEMORY(dir, value) {(*(volatile unsigned *)dir) = (value);}

int main() {

    while (1) {
        WRITE_MEMORY(SEG7_LEFT,I_SA);     // Habilitando interrupciones y contador, asi como
        WRITE_MEMORY(SEG7_RIGHT,Y_HI);
    }
}
    
