#define GPIO_SWs    0x80001400
#define GPIO_LEDs   0x80001404
#define GPIO_INOUT  0x80001408
#define GPIO_boton  0x80001800 

#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_GPIO(dir, value) { (*(volatile unsigned *)dir) = (value); }

int main ( void )
{
    int En_Value=0xFFFF, boton_value;

    WRITE_GPIO(GPIO_INOUT, En_Value);

    while (1) { 
        boton_value = READ_GPIO(GPIO_boton);
        //switches_value = switches_value >> 16;
        WRITE_GPIO(GPIO_LEDs, boton_value);
    }

    return(0);
}
