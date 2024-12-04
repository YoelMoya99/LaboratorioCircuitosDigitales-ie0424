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

// ---------------- Variables de estado de led testigo -----------------------------------
static unsigned int TimerLedTestigo = 0;
static void (*EstPresLedTestigo)(void);

void ME_LedTestigo(void);
void Est1_LedTestigo(void);
void Est2_LedTestigo(void);
void Est3_LedTestigo(void);

// --------------- Variables de estdo de botones pulsadores ----------------------------

static void (*EstPresBotones[5])(void);   // Arreglo de estados
static int Boton_i = 0;                   // Variable de dimensiones

void ME_Botones(void);
void Est1_Botones(void);
void Est2_Botones(void);
void Est3_Botones(void);
void Est4_Botones(void);

static unsigned int Botones_Msk[5] = {0x01,0x02,0x04,0x08,0x10};
static unsigned int TimerSupReb[5] = {0,0,0,0,0};
static unsigned int TimerShortPress[5] = {0,0,0,0,0};
static unsigned int TimerLongPress[5] = {0,0,0,0,0};

#define tSupReb 10
#define tShortP 250
#define tLongP  3000


static unsigned int BanderasSP = 0;
static unsigned int BanderasLP = 0;

// ----------------- Variables de estado de juego topos --------------------

static unsigned int TimerTopo   = 0;
static unsigned int TimerSMSTran = 0;

static void (*EstPresTopos)(void);

void ME_JuegoTopos(void);
void Est1_JuegoTopos(void);
void Est2_JuegoTopos(void);
void Est3_JuegoTopos(void);
void Est4_JuegoTopos(void);
void Est5_JuegoTopos(void);
void Est6_JuegoTopos(void);

static const char SmsPlayTopo[10][9] = {"PLAY___0", "PLAY___1", "PLAY___2", "PLAY___3", "PLAY___4", "PLAY___5", "PLAY___6", "PLAY___7", "PLAY___8", "PLAY___9"};
static unsigned int TopoPatron [10] = {0x1111, 0x8888, 0x000F, 0x9911, 0xDDDD, 0x9876, 0x7722, 0xFFF7, 0x6565, 0x9210};
static unsigned int tTimerTopo [10] = {20000, 19000, 18000, 17000, 16000, 15000, 14000, 13000, 12000, 11000};

#define CURRENT_GAME_TOPOS 0
#define CURRENT_GAME_LEDS 1
#define CURRENT_GAME_IDLE 2

static unsigned int CurrentGame;
static unsigned int Contador = 0;


// ---------------- maquina de estados de Idle ------------------------------

static unsigned int TimerIdle   = 0;

static void (*EstPresIdle)(void);

void ME_Idle(void);
void Est1_Idle(void);
void Est2_Idle(void);
void Est3_Idle(void);

// ----------------- maquina de estados LEDS game -------------------------

static unsigned int TimerTenisLED   = 0;

static void (*EstPresTenisLED)(void);

void ME_JuegoTenisLED(void);
void Est2_JuegoTenisLED(void);
void Est1_JuegoTenisLED(void);
void Est3_JuegoTenisLED(void);
void Est4_JuegoTenisLED(void);
void Est5_JuegoTenisLED(void);
void Est6_JuegoTenisLED(void);
void Est7_JuegoTenisLED(void);
void Est8_JuegoTenisLED(void);

static unsigned int tTimerTenisLED [10] = {500, 400, 300, 200, 100, 90, 80, 70, 50, 20};


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

    for (int i = 0; i < 5; i++){
        if (TimerSupReb[i] != 0){TimerSupReb[i]--;} 
    }

    for (int i = 0; i < 5; i++){
        if (TimerShortPress[i] != 0){TimerShortPress[i]--;} 
    }

    for (int i = 0; i < 5; i++){
        if (TimerLongPress[i] != 0){TimerLongPress[i]--;} 
    }

    if (TimerTopo != 0){TimerTopo--;} 
    if (TimerSMSTran != 0){TimerSMSTran--;} 
    if (TimerIdle != 0){TimerIdle--;} 
    if (TimerTenisLED != 0){TimerTenisLED--;} 



    bspClearExtInterrupt(3);                             // Borrando interrpcion externa
}


// ---------------------------------------------------------------------------
//            FUNCION DE IMPLEMENTACION DE ESCRITURA
// ---------------------------------------------------------------------------

// Función para escribir un carácter en un segmento
void writeCharToSegment(unsigned int segment, unsigned char character) {
    M_PSP_WRITE_REGISTER_32(segment, character);
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
        M_PSP_WRITE_REGISTER_32(HRC_R, HRC_ON_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_G, RC_DEFAULT_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_B, RC_DEFAULT_VALUE);
        TimerLedTestigo = 1000;
        EstPresLedTestigo = &Est2_LedTestigo; 
    }
}

void Est2_LedTestigo(void){

    if (TimerLedTestigo == 0){
        M_PSP_WRITE_REGISTER_32(HRC_R, RC_DEFAULT_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_G, HRC_ON_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_B, RC_DEFAULT_VALUE);
        TimerLedTestigo = 1000;
        EstPresLedTestigo = &Est3_LedTestigo;
    }
}

void Est3_LedTestigo(void){

    if (TimerLedTestigo == 0){
        M_PSP_WRITE_REGISTER_32(HRC_R, RC_DEFAULT_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_G, RC_DEFAULT_VALUE);
        M_PSP_WRITE_REGISTER_32(HRC_B, HRC_ON_VALUE);
        TimerLedTestigo = 1000;
        EstPresLedTestigo = &Est1_LedTestigo;
    }
}


// =================================================================
//                PUSH BUTTONS
// =================================================================

void ME_Botones(void){
    (*EstPresBotones[Boton_i])();
 
    if (Boton_i == 4){ Boton_i = 0;}
    else {Boton_i++;}
}

void Est1_Botones(void){

    if ((M_PSP_READ_REGISTER_32(GPIO_BOTON) & Botones_Msk[Boton_i]) != 0){

        TimerSupReb[Boton_i]     = tSupReb;
        TimerShortPress[Boton_i] = tShortP; 
        TimerLongPress[Boton_i]  = tLongP; 

        EstPresBotones[Boton_i] = &Est2_Botones;
    }
}

void Est2_Botones(void){

    if (TimerSupReb[Boton_i] == 0){
        
        if ((M_PSP_READ_REGISTER_32(GPIO_BOTON) & Botones_Msk[Boton_i]) != 0){
            EstPresBotones[Boton_i] = &Est3_Botones;
        }
        else {
            EstPresBotones[Boton_i] = &Est1_Botones;
        }
    }

}
void Est3_Botones(void){

    if (TimerShortPress[Boton_i] == 0){
        
        if ((M_PSP_READ_REGISTER_32(GPIO_BOTON) & Botones_Msk[Boton_i]) != 0){
            EstPresBotones[Boton_i] = &Est4_Botones;
        }
        else {
            BanderasSP = BanderasSP | Botones_Msk[Boton_i];
            EstPresBotones[Boton_i] = &Est1_Botones;
        }
    }
}

void Est4_Botones(void){

    if (TimerLongPress[Boton_i] == 0){
        
        if ((M_PSP_READ_REGISTER_32(GPIO_BOTON) & Botones_Msk[Boton_i]) == 0){
            BanderasLP = BanderasLP | Botones_Msk[Boton_i];
            EstPresBotones[Boton_i] = &Est1_Botones;
        }
    }
    else {
         if ((M_PSP_READ_REGISTER_32(GPIO_BOTON) & Botones_Msk[Boton_i]) == 0){
            EstPresBotones[Boton_i] = &Est1_Botones;
            BanderasSP = BanderasSP | Botones_Msk[Boton_i];
        }
    }
}

void ME_LED_Botones(void){
    unsigned int temp2 = BanderasSP & 0x13;
    if (temp2 != 0) {
        M_PSP_WRITE_REGISTER_32(GPIO_LEDs, BanderasSP);
    }
    if (BanderasLP != 0) {

        if (BanderasLP & 0x2) {CurrentGame=CURRENT_GAME_TOPOS;}
        else if (BanderasLP & 0x10) {CurrentGame=CURRENT_GAME_LEDS;}
        else if (BanderasLP & 0x1) {CurrentGame=CURRENT_GAME_IDLE;}

        int temp = BanderasSP ^ BanderasLP;
        M_PSP_WRITE_REGISTER_32(GPIO_LEDs, temp);
        BanderasSP = 0;
        BanderasLP = 0;
    }
}



// =================================================================
//                JUEGO UNO TOPOS
// =================================================================

void ME_JuegoTopos(void){ (*EstPresTopos)(); }

void Est1_JuegoTopos(void){

    if (CurrentGame == CURRENT_GAME_TOPOS){
   
	    writeWordToDisplay("BEGIN_1_"); 
        TimerSMSTran = 5000;  
  	    EstPresTopos = &Est2_JuegoTopos; 
    }
    else {
       EstPresTopos = &Est1_JuegoTopos;
    }

}

void Est2_JuegoTopos(void){

    if (CurrentGame == CURRENT_GAME_TOPOS){
       
        if (TimerSMSTran == 0){
      
	    Contador = 0;
	    EstPresTopos = &Est3_JuegoTopos;
        } 
   
    }
    else {
       EstPresTopos = &Est1_JuegoTopos;
    }
}

void Est3_JuegoTopos(void){

    if (CurrentGame == CURRENT_GAME_TOPOS){
       
	    TimerTopo = tTimerTopo[Contador];
    	M_PSP_WRITE_REGISTER_32(GPIO_LEDs, TopoPatron[Contador]);
	    writeWordToDisplay(SmsPlayTopo[Contador]); 
    
	    EstPresTopos = &Est4_JuegoTopos;
   
    }
    else {
       EstPresTopos = &Est1_JuegoTopos;
    }

}

void Est4_JuegoTopos(void){

    if (CurrentGame == CURRENT_GAME_TOPOS){
       
	    if (TimerTopo > 0) {
	        unsigned int temp = M_PSP_READ_REGISTER_32(GPIO_SWs);
    	    temp = temp >> 16;
            unsigned int temp2 = M_PSP_READ_REGISTER_32(GPIO_LEDs);
            temp2 = temp2 & 0xFFFF;
	    
	        if ((temp ^ temp2) == 0){
	   
	            Contador = Contador + 1;
	            if (Contador >= 10) {
		            EstPresTopos = &Est5_JuegoTopos;	
		            }
		        else {
	                EstPresTopos = &Est3_JuegoTopos;	
		        }	
	        }	    
	    }
	    else {
	        EstPresTopos = &Est6_JuegoTopos;
	    }
    }
    else {
       EstPresTopos = &Est1_JuegoTopos;
    }
}


void Est5_JuegoTopos(void){

    if (CurrentGame == CURRENT_GAME_TOPOS){
    
	    writeWordToDisplay("__YAY___"); 
    	M_PSP_WRITE_REGISTER_32(GPIO_LEDs, 0xFFFF);
	    TimerSMSTran = 1000;
	    EstPresTopos = &Est2_JuegoTopos;
    }
    else {
       EstPresTopos = &Est1_JuegoTopos;
    }
}


void Est6_JuegoTopos(void){

    if (CurrentGame == CURRENT_GAME_TOPOS){
    
	writeWordToDisplay("__LOSE__"); 
    	M_PSP_WRITE_REGISTER_32(GPIO_LEDs, 0xAAAA);
	TimerSMSTran = 3000;
	EstPresTopos = &Est2_JuegoTopos;
    }
    else {
       EstPresTopos = &Est1_JuegoTopos;
    }
}

// =================================================================
//                JUEGO IDLE
// =================================================================

void ME_Idle(void){ (*EstPresIdle)(); }

void Est1_Idle(void){

     if (CurrentGame == CURRENT_GAME_IDLE){

        if (TimerIdle == 0){ 
    
	        writeWordToDisplay("CONSOLA_"); 
	        TimerIdle = 1000;
	        EstPresIdle = &Est2_Idle;
            unsigned int temp = M_PSP_READ_REGISTER_32(GPIO_LEDs);
            M_PSP_WRITE_REGISTER_32(GPIO_LEDs, ~temp);
        }
    }
}

void Est2_Idle(void){

     if (CurrentGame == CURRENT_GAME_IDLE){

        if (TimerIdle == 0){ 
    
	        writeWordToDisplay("_JUEGOS_"); 
	        TimerIdle = 1000;
	        EstPresIdle = &Est3_Idle;
        }
    }
    else {
        EstPresIdle = &Est1_Idle;
    }
}

void Est3_Idle(void){

     if (CurrentGame == CURRENT_GAME_IDLE){

        if (TimerIdle == 0){ 
    
	        writeWordToDisplay("EN_IDLE_"); 
	        TimerIdle = 1000;
	        EstPresIdle = &Est1_Idle;
        }
    }
    else {
        EstPresIdle = &Est1_Idle;
    }
}

//  =========================================================================
//              JUEGO TENIS LED
// ==========================================================================

void ME_JuegoTenisLED(void){ (*EstPresTenisLED)(); }

void Est1_JuegoTenisLED(void){

    if (CurrentGame == CURRENT_GAME_LEDS){
   
	    writeWordToDisplay("BEGIN_2_"); 
        TimerSMSTran = 5000;  
  	    EstPresTenisLED = &Est2_JuegoTenisLED; 
    }
    else {
       EstPresTenisLED = &Est1_JuegoTenisLED;
    }

}

void Est2_JuegoTenisLED(void){

    if (CurrentGame == CURRENT_GAME_LEDS){
       
        if (TimerSMSTran == 0){
      
	    Contador = 0;
        writeWordToDisplay("___GO___");
        M_PSP_WRITE_REGISTER_32(GPIO_LEDs, 0x1); 
        TimerTenisLED = tTimerTenisLED[Contador];
	    EstPresTenisLED = &Est3_JuegoTenisLED;
        } 
   
    }
    else {
       EstPresTenisLED = &Est1_JuegoTenisLED;
    }
}

void Est3_JuegoTenisLED(void){

    if (CurrentGame == CURRENT_GAME_LEDS){
       
        if (TimerTenisLED == 0) {

            unsigned int temp = M_PSP_READ_REGISTER_32(GPIO_LEDs);
            temp = temp << 1;
            M_PSP_WRITE_REGISTER_32(GPIO_LEDs, temp);
            if ((temp & 0x8000) == 0x8000){
                BanderasSP = BanderasSP & 0xFB;
                EstPresTenisLED = Est4_JuegoTenisLED;
            }
            TimerTenisLED = tTimerTenisLED[Contador];
        }
   
    }
    else {
       EstPresTenisLED = &Est1_JuegoTenisLED;
    }

}

void Est4_JuegoTenisLED(void){

    if (CurrentGame == CURRENT_GAME_LEDS){
       
	    if (TimerTenisLED != 0){
            if ((BanderasSP & 0x4) == 0x4) {
                TimerTenisLED = 0;
                BanderasSP = BanderasSP ^ 0x4;
                EstPresTenisLED = Est5_JuegoTenisLED;
                Contador++;
            }
        }
        else {
            EstPresTenisLED = Est7_JuegoTenisLED;
        }
    }
    else {
       EstPresTenisLED = &Est1_JuegoTenisLED;
    }
}

void Est5_JuegoTenisLED(void){

    if (CurrentGame == CURRENT_GAME_LEDS){
       
        if (TimerTenisLED == 0) {

            unsigned int temp = M_PSP_READ_REGISTER_32(GPIO_LEDs);
            temp = temp >> 1;
            M_PSP_WRITE_REGISTER_32(GPIO_LEDs, temp);
            if ((temp & 0x1) == 0x1){
                EstPresTenisLED = Est6_JuegoTenisLED;
                BanderasSP = BanderasSP & 0xF7;
            }
            TimerTenisLED = tTimerTenisLED[Contador];
        }
   
    }
    else {
       EstPresTenisLED = &Est1_JuegoTenisLED;
    }

}

void Est6_JuegoTenisLED(void){

    if (CurrentGame == CURRENT_GAME_LEDS){
       
	    if (TimerTenisLED != 0){
            if ((BanderasSP & 0x8) == 0x8) {
                TimerTenisLED = 0;
                BanderasSP = BanderasSP ^ 0x8;
                EstPresTenisLED = Est3_JuegoTenisLED;
                Contador++;
            }
        }
        else {
            EstPresTenisLED = Est8_JuegoTenisLED;
        }
    }
    else {
       EstPresTenisLED = &Est1_JuegoTenisLED;
    }
}

void Est7_JuegoTenisLED(void){

    if (CurrentGame == CURRENT_GAME_LEDS){
    
	writeWordToDisplay("LOSE____"); 
	TimerSMSTran = 3000;
	EstPresTenisLED = &Est2_JuegoTenisLED;
    }
    else {
       EstPresTenisLED = &Est1_JuegoTenisLED;
    }
}


void Est8_JuegoTenisLED(void){

    if (CurrentGame == CURRENT_GAME_LEDS){
    
	writeWordToDisplay("____LOSE"); 
	TimerSMSTran = 3000;
	EstPresTenisLED = &Est2_JuegoTenisLED;
    }
    else {
       EstPresTenisLED = &Est1_JuegoTenisLED;
    }
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

// ------------------ gpio y gpio2 ----------------------------------------
 
  M_PSP_WRITE_REGISTER_32(GPIO_INOUT, 0xFFFF);

  EstPresLedTestigo = &Est1_LedTestigo;
  EstPresBotones[0] = &Est1_Botones;
  EstPresBotones[1] = &Est1_Botones;
  EstPresBotones[2] = &Est1_Botones;
  EstPresBotones[3] = &Est1_Botones;
  EstPresBotones[4] = &Est1_Botones;
  EstPresTopos = &Est1_JuegoTopos;
  EstPresIdle = &Est1_Idle;
  EstPresTenisLED = &Est1_JuegoTenisLED;

  CurrentGame = CURRENT_GAME_IDLE;

  while (1) {
    /* SECUENCIADOR DE MAQUINAS DE ESTADO */
    ME_LedTestigo();
    ME_Botones();
    ME_Botones();
    ME_Botones();
    ME_Botones();
    ME_Botones();
    ME_LED_Botones();
    ME_JuegoTopos();
    ME_Idle();
    ME_JuegoTenisLED();
  }

}

