#include "psp_api.h"
#include "bsp_external_interrupts.h"
#include "psp_ext_interrupts_eh1.h"
#include "bsp_timer.h"
#include "bsp_printf.h"

// ---------------------------------------------------------------------------
//          Direcciones: DEFINES DEL MODULO DE 7 SEG extendido
// ---------------------------------------------------------------------------

#define SegDig_RIGHT   0x80001038
#define SegDig_LEFT    0x8000103C

// Escribir display por display
#define SEG_1 0x80001038 
#define SEG_2 0x80001039
#define SEG_3 0x8000103A
#define SEG_4 0x8000103B
#define SEG_5 0x8000103C
#define SEG_6 0x8000103D
#define SEG_7 0x8000103E
#define SEG_8 0x8000103F

// ---------------------------------------------------------------------------
//          Direcciones: DEFINES DEL MODULO GPIO (GPIO normal)
// ---------------------------------------------------------------------------

#define GPIO_SWs        0x80001400
#define GPIO_LEDs       0x80001404
#define GPIO_INOUT      0x80001408
#define RGPIO_INTE      0x8000140C
#define RGPIO_PTRIG     0x80001410
#define RGPIO_CTRL      0x80001418
#define RGPIO_INTS      0x8000141C

// ---------------------------------------------------------------------------
//                  Direcciones: DEFINES DEL MODULO PTC 
// ---------------------------------------------------------------------------

#define RPTC_CNTR       0x80001200
#define RPTC_HRC        0x80001204
#define RPTC_LRC        0x80001208
#define RPTC_CTRL       0x8000120c
#define Select_INT      0x80001018

// ---------------------------------------------------------------------------
//                 Direcciones: DEFINES DE LOS BOTONES (GPIO2)
// ---------------------------------------------------------------------------

#define GPIO_BOTON        0x80001800
#define RGPIO_BOTON_INTE  0x8000180C  
#define RGPIO_BOTON_PTRIG 0x80001810  
#define RGPIO_BOTON_CTRL  0x80001818   
#define RGPIO_BOTON_INTS  0x8000181C 

// ---------------------------------------------------------------------------
//                 Valores: DEFINES DE LOS BOTONES (GPIO2)
// ---------------------------------------------------------------------------

#define INT             0x40
#define INTERRUPT_START 0x21
#define DEFAULT_LH_RC   50000  // 0xC350 AJUSTADO PARA 1mS
#define MAX_LH_RC       0x5F5E100 
#define MIN_LH_RC       0x3B9ACA

// -------------------------------------------------------------------------
//                 VALORES PARA EL LED TRICOLOR
// ------------------------------------------------------------------------

// Direcciones de memoria para encender el LED azul con el PTC asociado
#define GPIO_LED_BLUE 0x80001280
#define  LRC_B        0x80001288
#define  HRC_B        0x80001284
#define CTRL_B        0x8000128C

// Direcciones de memoria para encender el LED verde con el PTC asociado
#define GPIO_LED_GREEN 0x800012C0
#define  LRC_G         0x800012C8
#define  HRC_G         0x800012C4
#define CTRL_G         0x800012CC

// Direcciones de memoria para encender el LED rojo con el PTC asociado
#define GPIO_LED_RED_Base 0x80001240
#define  LRC_R            0x80001248
#define  HRC_R            0x80001244
#define CTRL_R            0x8000124C

// Valores para determinar el ciclo de trabajo de los leds
#define HRC_ON_VALUE     0xAFC8  // 45000
#define RC_DEFAULT_VALUE  0xC350 // 50000
#define OE                0x08



// ---------------------------------------------------------------------------
//                 ESTRUCTURAS DE DATOS GLOBALES
// ---------------------------------------------------------------------------

static unsigned int timer_parpadeo = 0;   // Variable para delay

// ---------------- tablas de padre tiempo -----------------------------------
static unsigned int TimerLedTestigo = 0;


// Maquina LED testigo
static void (*EstPresLedTestigo)(void);
void ME_LedTestigo(void);
void Est1_LedTestigo(void);
void Est2_LedTestigo(void);
void Est3_LedTestigo(void);


// Maquina TOPOS
static unsigned int TimerTopos = 0;
static void (*EstPresTopos)(void);
void ME_Topos(void);
void Est1_Topos(void);
void Est2_Topos(void);
void Est3_Topos(void);
void Est4_Topos(void);
void Est5_Topos(void);





// Función para escribir un carácter en un segmento
void writeCharToSegment(unsigned int segment, unsigned char character) {
    M_PSP_WRITE_REGISTER_32(segment, character);
}

// ---------------- tablas de 7 segmentos -----------------------------------

// Tabla de mensajes y tabla de numeros

// ---------------- Letras posibles de colcar -------------------------------

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

// --------------------- Numeros ----------------------------
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


// --------------------- Combi topos ----------------------------------------

#define C1 0x01
#define C2 0x08
#define C3 0x80


static unsigned char Contador = '0';

// ---------------------------------------------------------------------------
//            SUBRUTINAS DE INTERRUPCIONES
// ---------------------------------------------------------------------------



extern D_PSP_DATA_SECTION D_PSP_ALIGNED(1024) pspInterruptHandler_t G_Ext_Interrupt_Handlers[8];

void DefaultInitialization(void){
  u32_t uiSourceId;

  /* Register interrupt vector */
  pspInterruptsSetVectorTableAddress(&M_PSP_VECT_TABLE);

  /* Set external-interrupts vector-table address in MEIVT CSR */
  pspExternalInterruptSetVectorTableAddress(G_Ext_Interrupt_Handlers);

  /* Put the Generation-Register in its initial state (no external interrupts are generated) */
  bspInitializeGenerationRegister(D_PSP_EXT_INT_ACTIVE_HIGH);

  for (uiSourceId = D_BSP_FIRST_IRQ_NUM; uiSourceId <= D_BSP_LAST_IRQ_NUM; uiSourceId++)
  {
    /* Make sure the external-interrupt triggers are cleared */
    bspClearExtInterrupt(uiSourceId);
  }

  /* Set Standard priority order */
  pspExtInterruptSetPriorityOrder(D_PSP_EXT_INT_STANDARD_PRIORITY);

  /* Set interrupts threshold to minimal (== all interrupts should be served) */
  pspExtInterruptsSetThreshold(M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

  /* Set the nesting priority threshold to minimal (== all interrupts should be served) */
  pspExtInterruptsSetNestingPriorityThreshold(M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);
}


void ExternalIntLine_Initialization(u32_t uiSourceId, u32_t priority, pspInterruptHandler_t pTestIsr){
  /* Set Gateway Interrupt type (Level) */
  pspExtInterruptSetType(uiSourceId, D_PSP_EXT_INT_LEVEL_TRIG_TYPE);

  /* Set gateway Polarity (Active high) */
  pspExtInterruptSetPolarity(uiSourceId, D_PSP_EXT_INT_ACTIVE_HIGH);

  /* Clear the gateway */
  pspExtInterruptClearPendingInt(uiSourceId);

  /* Set IRQ4 priority */
  pspExtInterruptSetPriority(uiSourceId, priority);
    
  /* Enable IRQ4 interrupts in the PIC */
  pspExternalInterruptEnableNumber(uiSourceId);

  /* Register ISR */
  G_Ext_Interrupt_Handlers[uiSourceId] = pTestIsr;
}

void PTC_Initialization(void){
  
  M_PSP_WRITE_REGISTER_32(RPTC_LRC, DEFAULT_LH_RC); // una interrupcion no deseada
  M_PSP_WRITE_REGISTER_32(RPTC_HRC, DEFAULT_LH_RC); // Colocando en ambos el mismo valor para evitar
  M_PSP_WRITE_REGISTER_32(RPTC_CTRL, INTERRUPT_START);     // Habilitando interrupciones y contador, asi como

}

void Padre_Tiempo_PTC_ISR(void){
    // Funcion de interrupciones PTC A.K.A. Padre Tiempo

    M_PSP_WRITE_REGISTER_32(RPTC_CTRL, INTERRUPT_START);  // Borando interrupt PTC
    
    if (TimerLedTestigo != 0){TimerLedTestigo--;} 
    if (TimerTopos != 0){TimerTopos--;} 
    if (timer_parpadeo!= 0){timer_parpadeo--;} 
    
    bspClearExtInterrupt(3);                             // Borrando interrpcion externa
}



// ---------------------------------------------------------------------------
//            FUNCION DE IMPLEMENTACION DE ESCRITURA
// ---------------------------------------------------------------------------

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
            case 'B': character = B; break;
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



// ==========================================================
//          MAQUINA DE ESTADOS LED TESTIGO
// ==========================================================

void ME_LedTestigo(void){ (*EstPresLedTestigo)(); }

void Est1_LedTestigo(void){

    if (TimerLedTestigo == 0){
    //  writeWordToDisplay("CONSOLA_"); 
        M_PSP_WRITE_REGISTER_32(HRC_R, HRC_ON_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_G, RC_DEFAULT_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_B, RC_DEFAULT_VALUE);
        TimerLedTestigo = 1000;
        EstPresLedTestigo = &Est2_LedTestigo; 
    }
}

void Est2_LedTestigo(void){

    if (TimerLedTestigo == 0){
    //  writeWordToDisplay("LAB_DIGI"); 
        M_PSP_WRITE_REGISTER_32(HRC_R, RC_DEFAULT_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_G, HRC_ON_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_B, RC_DEFAULT_VALUE);
        TimerLedTestigo = 1000;
        EstPresLedTestigo = &Est3_LedTestigo;
    }
}

void Est3_LedTestigo(void){

    if (TimerLedTestigo == 0){
     // writeWordToDisplay("CONSOLA_"); 
        M_PSP_WRITE_REGISTER_32(HRC_R, RC_DEFAULT_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_G, RC_DEFAULT_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_B, HRC_ON_VALUE);
        TimerLedTestigo = 1000;
        EstPresLedTestigo = &Est1_LedTestigo;
    }
}

// ==========================================================
//          MAQUINA DE ESTADOS TOPOS
// ==========================================================

void ME_Topos(void){ (*EstPresTopos)(); }

void Est1_Topos(void){
    
        if ((M_PSP_READ_REGISTER_32(GPIO_BOTON) & 0x2) != 0){
          EstPresTopos = &Est2_Topos;

        }
        else{EstPresTopos = &Est1_Topos;}
       
}

void Est2_Topos(void){

        writeWordToDisplay("DALE");
  
        TimerTopos = 5000;
        Contador = '0';
        EstPresTopos = &Est3_Topos;
    
}

void Est3_Topos(void){

    if (TimerTopos == 0){

        
        writeWordToDisplay(Contador);

        // Comenzando juego

        M_PSP_WRITE_REGISTER_32(GPIO_LEDs, C1);
        
        TimerTopos = 5000;
        EstPresTopos = &Est4_Topos;
    }
}


void Est4_Topos(void){

    if (TimerTopos == 0){
 
       EstPresTopos = &Est5_Topos;

    }
    else if((M_PSP_READ_REGISTER_32(GPIO_LEDs) & 0xFFFF) == ((M_PSP_READ_REGISTER_32(GPIO_SWs) >> 16) & 0xFFFF)){
        Contador++;
    }
}

void Est5_Topos(void){

    writeWordToDisplay("LOSE");
    M_PSP_WRITE_REGISTER_32(GPIO_LEDs, 0);
    EstPresTopos = &Est1_Topos;
}




int main(void)
{


// ---------------------------------------------------------------------------
//                    INICIALIZACIÓN DE HARDWARE
// ---------------------------------------------------------------------------

// ------------------- interrupciones del ptc -------------------------------

  /* INITIALIZE THE INTERRUPT SYSTEM */
  DefaultInitialization();                            /* Default initialization */
  pspExtInterruptsSetThreshold(5);                    /* Set interrupts threshold to 5 */

  ExternalIntLine_Initialization(3, 6, Padre_Tiempo_PTC_ISR);
  M_PSP_WRITE_REGISTER_32(Select_INT, 0x2);           /* Connects the irq_PTC_enable line to the interrupt line. Basically does an Interrupt enable on bit 0 for gpio and bit 1 for ptc */

  /* INITIALIZE THE PERIPHERALS */
  PTC_Initialization();

  /* ENABLE INTERRUPTS */
  pspInterruptsEnable();                              /* Enable all interrupts in mstatus CSR */
  M_PSP_SET_CSR(D_PSP_MIE_NUM, D_PSP_MIE_MEIE_MASK);  /* Enable external interrupts in mie CSR */

// ------------------ Led tri color ---------------------------------------

    M_PSP_WRITE_REGISTER_32(CTRL_R, 1);
    M_PSP_WRITE_REGISTER_32(LRC_R, RC_DEFAULT_VALUE);
    
    M_PSP_WRITE_REGISTER_32(CTRL_G, 1);
    M_PSP_WRITE_REGISTER_32(LRC_G, RC_DEFAULT_VALUE);
    
    M_PSP_WRITE_REGISTER_32(CTRL_B, 1);
    M_PSP_WRITE_REGISTER_32(LRC_B, RC_DEFAULT_VALUE);
    

// ------------------ Pantalla de 7 segmentos -----------------------------

// Escribiendo mensaje inicial
writeWordToDisplay("CONSOLA_"); 

// ------------------ gpio y gpio2 ----------------------------------------
 
  M_PSP_WRITE_REGISTER_32(GPIO_INOUT, 0xFFFF);

  EstPresLedTestigo = &Est1_LedTestigo;
  EstPresTopos = &Est1_Topos;


  while (1) {
    /* SECUENCIADOR DE MAQUINAS DE ESTADO */
    ME_LedTestigo();
    ME_Topos();

  }

}

