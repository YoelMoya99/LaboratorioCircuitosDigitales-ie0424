

// Letras posibles de colcar
#define A 0x08
#define B 0xE0  
#define C 0xB1
#define D 0xC2  
#define E 0xB0
#define F 0xB8
#define G 0xA0
#define H 0x48
#define I 0x4F
#define J 0xC7
// k
#define L 0xF1
// M
#define N 0xEA
#define O 0x81
#define P 0x18
// Q
// R
#define S 0x24
// T
#define U 0xC1
// V
// W
// X
#define Y 0x44
#define _ 0xFF // Espacio
// Z

// Numeros
#define n0 0x81
#define n1 0xCF
#define n2 0x92
#define n3 0x86
#define n4 0xCC
#define n5 0xA4
#define n6 0xA0
#define n7 0x8F
#define n8 0x80
#define n9 0x84




// 7 segmentos
#define SEG7_RIGHT 0x80001038
#define SEG7_LEFT 0x8000103C

// Escribir display por display
#define SEG_1 0x80001038 
#define SEG_2 0x80001039
#define SEG_3 0x8000103A
#define SEG_4 0x8000103B
#define SEG_5 0x8000103C
#define SEG_6 0x8000103D
#define SEG_7 0x8000103E
#define SEG_8 0x8000103F


// ----------- Encabezados funciones ------------
// Escritura y lectura de direcciones 
#define READ_MEMORY(dir) (*(volatile unsigned *)dir)
#define WRITE_MEMORY(dir, value) {(*(volatile unsigned *)dir) = (value);}



void delay(int timer);
int timer = 5000000;

// Función para escribir un carácter en un segmento
void writeCharToSegment(unsigned int segment, unsigned char character) {
    WRITE_MEMORY(segment, character);
}





// Función para escribir una palabra en los segmentos (de izquierda a derecha)
void writeWordToDisplay(const char *word) {
    const unsigned int segments[] = {SEG_1, SEG_2, SEG_3, SEG_4, SEG_5, SEG_6, SEG_7, SEG_8};
    int wordLength = 0;

    // Calcular la longitud de la palabra
    while (word[wordLength] != '\0') {
        wordLength++;
    }

    // Invertir el orden de escritura para que SEG_1 sea el más a la izquierda
    int i = wordLength - 1;
    int j = 0;

    while (i >= 0 && j < 8) {
        unsigned char character;

        // Mapear el carácter a su valor en 7 segmentos
        switch (word[i]) {
            case 'A': character = A; break;
            case 'C': character = C; break;
            case 'D': character = D; break;
            case 'E': character = E; break;
            case 'F': character = F; break;
            case 'G': character = G; break;
            case 'H': character = H; break;
            case 'I': character = I; break;
            case 'J': character = J; break;
            case 'L': character = L; break;
            case 'N': character = N; break;
            case 'O': character = O; break;
            case 'P': character = P; break;
            case 'S': character = S; break;
            case 'U': character = U; break;
            case 'Y': character = Y; break;
            case '_': character = _; break;

            case '0': character = n0; break;
            case '1': character = n1; break;
            case '2': character = n2; break;
            case '3': character = n3; break;
            case '4': character = n4; break;
            case '5': character = n5; break;
            case '6': character = n6; break;
            case '7': character = n7; break;
            case '8': character = n8; break;
            case '9': character = n9; break;

            default:  character = 0xFF; // Apagar segmento si carácter no reconocido
        }

        // Escribir el carácter en el segmento correspondiente
        writeCharToSegment(segments[j], character);
        i--;
        j++;
    }

    // Apagar los segmentos restantes
    for (; j < 8; j++) {
        writeCharToSegment(segments[j], 0xFF); // Apagar segmento
    }
}


int main() {

    writeWordToDisplay("YOEL"); 
    delay(timer);
    writeWordToDisplay("YOEL_"); 
    delay(timer);
    writeWordToDisplay("YOEL__"); 
    delay(timer);
    writeWordToDisplay("YOEL__1");
    delay(timer);
    writeWordToDisplay("YOEL__2");
    delay(timer);
    writeWordToDisplay("YOEL__3");
    delay(timer);
    writeWordToDisplay("YOEL__4");
    delay(timer);
    writeWordToDisplay("YOEL__5");
    delay(timer);
    writeWordToDisplay("YOEL__6");
    delay(timer);
    writeWordToDisplay("YOEL__7");
    delay(timer);
    writeWordToDisplay("YOEL__8");
    delay(timer);
    writeWordToDisplay("YOEL__9");
    delay(timer);
    writeWordToDisplay("YOEL__N");
    delay(timer);
    writeWordToDisplay("YOEL__GUAP");
    delay(timer);
    writeWordToDisplay("YOEL__GUAPO_");
    delay(timer);
    writeWordToDisplay("YOEL__GUAPO__");
    delay(timer);
    writeWordToDisplay("________");
    delay(timer);
    writeWordToDisplay("GUAPO__");
    delay(timer);
    writeWordToDisplay("________");
    delay(timer);
    writeWordToDisplay("GUAPO__");
    delay(timer);


    while (1) {
        
        
    }
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
