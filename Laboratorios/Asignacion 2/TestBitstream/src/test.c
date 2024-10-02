// memory-mapped I/O addresses
#define GPIO_LED_RED 0x80001240
#define GPIO_LED_BLUE 0x80001240
#define GPIO_LED_GREEN 0x800012C0
#define GPIO_Boton 0x80001800
#define GPIO_INOUT 0x80001408

#define LRC 0x80001248
#define HRC 0x80001244
#define CTRL 0x8000124C

#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_GPIO(dir, value) {(*(volatile unsigned *)dir) = (value);}



int main (void)
{

WRITE_GPIO(CTRL, 1)
WRITE_GPIO(LRC, 0x80);
WRITE_GPIO(HRC, 0x40);

}

