#define GPIO_SWs    0x80001400
#define GPIO_LEDs   0x80001404
#define GPIO_INOUT  0x80001408

#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_GPIO(dir, value) { (*(volatile unsigned *)dir) = (value); }

void delay(int timer);

int main ( void ){

    int En_Value=0xFFFF;
    int timer = 5000000;
    int LEDs_value = 0x0001;

    WRITE_GPIO(GPIO_INOUT, En_Value)

    while (1){
        
        if (LEDs_value == 0xFFFF)
        {
            LEDs_value = 0x0001;
            WRITE_GPIO(GPIO_LEDs, LEDs_value);
            delay(timer);
        }

        while ((LEDs_value & 0x8000) == 0)
        {
            LEDs_value = LEDs_value << 1;
            WRITE_GPIO(GPIO_LEDs, LEDs_value);
            delay(timer);
        }

        while ((LEDs_value & 0x0001) == 0)
        {
            LEDs_value = LEDs_value >> 1;
            WRITE_GPIO(GPIO_LEDs, LEDs_value);
            delay(timer);
        }

        LEDs_value = LEDs_value << 1;
        LEDs_value++;
        WRITE_GPIO(GPIO_LEDs, LEDs_value);
        delay(timer);

    }
         
    return(0);
}

void delay(int timer){
    static int j, i;
    for (i = 0; i < timer; i++){
        j++;
    } 
}