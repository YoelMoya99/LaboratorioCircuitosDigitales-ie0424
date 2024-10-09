// memory-mapped I/O addresses

//Direcciones de memoria para encender el LED azul con el PTC asociado
#define GPIO_LED_BLUE 0x80001280
#define  LRC_B 0x80001288
#define  HRC_B 0x80001284
#define CTRL_B 0x8000128C

//Direcciones de memoria para encender el LED verde con el PTC asociado
#define GPIO_LED_GREEN 0x800012C0
#define  LRC_G 0x800012C8
#define  HRC_G 0x800012C4
#define CTRL_G 0x800012CC

//Direcciones de memoria para encender el LED rojo con el PTC asociado
#define GPIO_LED_RED_Base 0x80001240
#define  LRC_R 0x80001248
#define  HRC_R 0x80001244
#define CTRL_R 0x8000124C

// Direcciones de boton y de gpio para INOUT
#define GPIO_Boton 0x80001800
#define GPIO_INOUT 0x80001408
#define GPIO_SWs   0x80001400


// Escritura y lectura de direcciones 
#define READ_MEMORY(dir) (*(volatile unsigned *)dir)
#define WRITE_MEMORY(dir, value) {(*(volatile unsigned *)dir) = (value);}

//Valores para determinar el ciclo de trabajo de los leds
#define HRC_IN_Value 0x40
#define LRC_IN_Value 0x40
#define PERCENT_INC  0x0C
#define OE           0x08


int main (void)
{

WRITE_MEMORY(CTRL_R, 1);
WRITE_MEMORY(HRC_R, HRC_IN_Value);
WRITE_MEMORY(LRC_R, HRC_IN_Value);

WRITE_MEMORY(CTRL_G, 1);
WRITE_MEMORY(HRC_G, HRC_IN_Value);
WRITE_MEMORY(LRC_G, HRC_IN_Value);

WRITE_MEMORY(CTRL_B, 1);
WRITE_MEMORY(HRC_B, HRC_IN_Value);
WRITE_MEMORY(LRC_B, HRC_IN_Value);


WRITE_MEMORY(GPIO_INOUT, 0xFFFF);

int Switches, temp_green, temp_blue;

while (1)
{
    Switches = READ_MEMORY(GPIO_SWs);
    Switches = Switches >> 16;

    switch (Switches & 0x1F)
    {
    case 0x1:
        WRITE_MEMORY(LRC_R, LRC_IN_Value + PERCENT_INC);
        break;
    case 0x3:
        WRITE_MEMORY(LRC_R, LRC_IN_Value + 2*PERCENT_INC);
        break;
    case 0x7:
        WRITE_MEMORY(LRC_R, LRC_IN_Value + 3*PERCENT_INC);
        break;
    case 0xF:
        WRITE_MEMORY(LRC_R, LRC_IN_Value + 4*PERCENT_INC);
        break;
    case 0x1F:
        WRITE_MEMORY(LRC_R, LRC_IN_Value + 5*PERCENT_INC);
        break;
    default:
        WRITE_MEMORY(LRC_R, LRC_IN_Value);
        //WRITE_MEMORY(CTRL_R, 0);
        break;
    } 

    temp_blue = Switches >> 5;

    switch (temp_blue & 0x1F)
    {
    case 0x1:
        WRITE_MEMORY(LRC_B, LRC_IN_Value + PERCENT_INC);
        break;
    case 0x3:
        WRITE_MEMORY(LRC_B, LRC_IN_Value + 2*PERCENT_INC);
        break;
    case 0x7:
        WRITE_MEMORY(LRC_B, LRC_IN_Value + 3*PERCENT_INC);
        break;
    case 0xF:
        WRITE_MEMORY(LRC_B, LRC_IN_Value + 4*PERCENT_INC);
        break;
    case 0x1F:
        WRITE_MEMORY(LRC_B, LRC_IN_Value + 5*PERCENT_INC);
        break;
    default:
        WRITE_MEMORY(LRC_B, LRC_IN_Value);
        //WRITE_MEMORY(CTRL_B, 0);
        break;
    } 

    temp_green = Switches >> 10;


    switch (temp_green & 0x1F)
    {
    case 0x1:
        WRITE_MEMORY(LRC_G, LRC_IN_Value + PERCENT_INC);
        break;
    case 0x3:
        WRITE_MEMORY(LRC_G, LRC_IN_Value + 2*PERCENT_INC);
        break;
    case 0x7:
        WRITE_MEMORY(LRC_G, LRC_IN_Value + 3*PERCENT_INC);
        break;
    case 0xF:
        WRITE_MEMORY(LRC_G, LRC_IN_Value + 4*PERCENT_INC);
        break;
    case 0x1F:
        WRITE_MEMORY(LRC_G, LRC_IN_Value + 5*PERCENT_INC);
        break;
    default:
        WRITE_MEMORY(LRC_G, LRC_IN_Value);
        //WRITE_MEMORY(CTRL_G, 0);
        break;
    } 

}


}

