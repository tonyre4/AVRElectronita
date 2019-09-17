
#include <stdio.h>
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>


//debug Flags

#define FIX

//#define DEBUGSIGNALS

//int x = false;
///////////////
//HIM variables
////////////////
//#include <./LiquidCrystal.h>
//const unsigned char rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
///////////////
//HIM variables END
///////////////


///////////////////
//Timer 2 variables
///////////////////
bool milsCnt = false;
unsigned int long mils = 0;
///////////////////
//Timer 2 variables END
///////////////////

/////////////////////////////
//Signal generation Variables
/////////////////////////////
//Therapy types
#define TENS 0
#define TREN 1
#define RUSA 2
#define EXPO 3
#define GALV 4
#define DIAD 5
//Signal direction
#define OFF 0
#define UP 1
#define DOWN 2
//Numbers to generate sine signal
const unsigned char sineSignal[50] = {0, 0, 31, 47, 63, 78, 93, 108, 122, 136, 149, 162, 174, 185, 196, 206, 215, 223, 230, 237, 242, 246, 250, 252, 254, 255, 254, 252, 250, 246, 242, 237, 230, 223, 215, 206, 196, 185, 174, 162, 149, 136, 122, 108, 93, 78, 63, 47, 0, 0};
//Numbers to divide sine signal
const unsigned char expoSignal[33] = {7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1};
//number to calculate exponential
unsigned int expoCntr = 0;
//Sample counter
unsigned char sampleCntr = 0;
//Register used to write samples 
unsigned char regSample = 0;
//Bool to know the side of the output signal / true - Positive edge / false - Negative edge
bool sideEdge = false;
//Signal type register
unsigned char signalType = GALV; //Galv as default
//Period counter (to count seconds in activation or deactivation)
unsigned int periodCntr = 0;
//Seconds counter to activate or deactivate signal
unsigned char secondsCntr = 0;
//This points to which freq the machine is bringing
unsigned char workingFreq = 200; //Galv freq as default
//Amount of time in active/nonactive oscilation (seconds)
unsigned char intervalSecs = 5;
//This bool is to know if secondsCntr should be counting (for interval switching)
bool intervalCount = false;
//This variable denotes if the output is enabled or not /true - active / false - deactive
bool signalState = false; //Signal by default is deactivated
//This will be true during therapy
bool therapyState = false; //Obviously this should be false by default
//This will store time of terapy (seconds)
unsigned int therapyTime = 0;
//This will store the signal direction
unsigned char signalDirection = OFF;
//This bool will make
bool periodCnt = false;
//This will store the configured freq for TENS
unsigned char currentTENSf = 1; //Default is 1
//To show if test mode is enabled
bool testMode = false; //default is false
//Variable to know /-true is in the first half of a second /-false is in the second half of a second
bool middleSec = false; //default is false
//Var to write in Expo signal (shift value)
int expWrite = 0;
/////////////////////////////
//Signal generation Variables END
/////////////////////////////

/*/////////
//Pins used
///////////
BitPort:Description:BoardPin

PORTB 0-5:
0: Col 0 : 8 IN
1: Col 1 : 9 IN
2: Row 0 : 10 OUT
3: Row 1 : 11 OUT
4: Row 2 : 12 OUT
5: 
6: Xtal1 
7: Xtal2

//Buttons MUXed
//Buttons where made has a matrix to reduce 7 wires to 5
| Row Col ->
v  0       1
0  OK      Left
1  Cancel  Up
2  Right   Down

//Buttons Normal
// 6 wires for buttons and Vcc
0 Ok
1 Cancel
2 Right
3 Left
4 Up
5 Down

PORTC 0-5:
0: LCD RS :A0 
1: LCD E  :A1
2: LCD d4 :A2
3: LCD d5 :A3
4: LCD d6 :A3
5: LCD d7 :A5
6: RESET : Reset  IN

PORTD 7-0:
0: Programacion 
1: Programacion
2:
3:
4: Relay2 signal :4
5: PWM1 - Negative edge :5 OUT
6: PWM2 - Positive edge :6 OUT
7: Relay1 signal :7 OUT
*/

/*Timers used:
Timer 0: For generate PWM signals 
Default should be: 
  -x8 Prescaler
  -Fast PWM mode

Timer 1: For change PWM registers
This timer doesnt have any default config.
  - This timer should be changed all times
*/
//Function declarations

void defaultSets();
void resetSets();
void loop();
void declareDDRC();
void declareDDRD();
void configureTimer2();
void Rly1State(bool turn);
void deactivatePWM();
void writeTREN();
void writeTENS();
void writeDIAD();
void writeRUSA();
void writeGALV(unsigned char shift); 
void writeEXPO();
void initTREN();
void initTENS();
void initDIAD();
void initRUSA();
void initGALV();
void initEXPO();
void configTREN();
void configTENS();
void configDIAD();
void configRUSA();
void configGALV();
void configEXPO();
void activateUpOutput();
void activateDwOutput();
void deactivateOutput();
void activateTimer1(bool act);
void configureTimer1(int COM, char Prescaler);
void setFrequency(int f, bool act);
void countSeconds();
void intervalCheck();
void actTimer0();

///////////////////////////////////////////
//INTERFACE CODE///////////////////////////
///////////////////////////////////////////
#define D4 eS_PORTC2
#define D5 eS_PORTC3
#define D6 eS_PORTC4
#define D7 eS_PORTC5
#define RS eS_PORTC0
#define EN eS_PORTC1

#include "lcd.h"
#include "interface.h"



//Func dec
void setup2() ;
void waitFor(int time);
void startCount();
void stopCount();
void LCDsetup();
void initVars();

void initVars(){
//////////
//HMI vars
//////////
Var1=1;   //Variable controlada por los botones OK y RETURN/BACK
Var2=1;   //Variable controlada por los botones UP/+ y DOWN/-
Var3=1;   //Variable controlada por los botones LEFT y RIGHT
Modos=1;    //Indica que modo se debe imprimir en la pantalla y a que menu (función) de dicho modo se debe entrar
Variables=1;    //Fluctua entre las variables a modificar
Mensaje=1;    //Controla el switch principal, maneja lo que debe imprimirse
Testindicator=false;
t=false;   //Ayuda a reducir el rebote de la lectura de matriz

toprint=true;    //Cuando ocurra un cambio que afecte a los valores impresos, da la orden de imprimir al estar en TRUE
secondsPrintedP=0; 
secondsPrinted=0;

timeM=10;   //Setea los minutos y fluctua entre 0 y 99 no pudiendo ser este menor a 0:30
timeS=0;    //Setea los segundos y fluctua entre 0 y 30
intervalos=5;   //Sete el tiempo que habra de intervalos
Hz=0;   //Iterador de las frecuencias de TENS

////////
//Signals vars
////////
signalType = GALV; //Galv as default
periodCntr = 0;
sampleCntr = 0;
regSample = 0;
secondsCntr = 0;
workingFreq = 200; //Galv freq as default
intervalSecs = 5;
therapyTime = 0;
signalDirection = OFF;
currentTENSf = 1; //Default is 1
milsCnt = false;
expoCntr = 0;
sideEdge = false;
intervalCount = false;
signalState = false; //Signal by default is deactivated
therapyState = false; //Obviously this should be false by default
periodCnt = false;
testMode = false; //default is false
middleSec = false; //default is false
expWrite = 0;
}

void setup2() {
  LCDsetup();
  DDRB  = 0x1C;  //PINES X,X,13,12row2,11row1,10row0,9col1,8col0   //0 INPUT   //1 OUTPUT
  PORTB = 0x1C;  //Pone en HIGH los pines 10, 11 y 12
  Str2p = &Str2[0];
  Str3p = &Str3[0];
}

void LCDsetup(){
  DDRC = 0xFF;
  Lcd4_Init();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////


///////////
//Pins used
//////////
char dTCCR0A;
char dTCCR0B ;
char dTIMSK0;
char dTCCR1A;
char dTCCR1B;
char dTIMSK1 ;
void defaultSets(){
  dTCCR0A = TCCR0A;
  dTCCR0B = TCCR0B;
  dTIMSK0 = TIMSK0;
  dTCCR1A = TCCR1A;
  dTCCR1B = TCCR1B;
  dTIMSK1 = TIMSK1;
}

void resetSets(){
  deactivateOutput();
  TCCR0A = dTCCR0A;
  TCCR0B = dTCCR0B;
  TIMSK0 = dTIMSK0;
  TCCR1A = dTCCR1A;
  TCCR1B = dTCCR1B;
  TIMSK1 = dTIMSK1;
  Rly1State(false);
}


//#define DEBUGSIGNALS

void PMes(int n);
void PMes(int n){
  Lcd4_Clear();
  Lcd4_Set_Cursor(1,0);
  Lcd4_Write_String("Pass");
  Lcd4_Set_Cursor(2,0);
  ValStr(n,0,Str2p);
  Lcd4_Write_String(Str2p);
}


#define DEBUGSIGNALS
#include "testers.h"


int main(){
  declareDDRC();
  declareDDRD();
  defaultSets();
  //I/O declaration
  setup2();
  
  #ifdef DEBUGSIGNALS
  Lcd4_Clear();
  _delay_ms(1000);

  //Here code to debug//
  //testTENS();
  Lcd4_Clear();
  Lcd4_Set_Cursor(1,0);
  Lcd4_Write_String("Turning relay on"); 
  _delay_ms(2000);
  Lcd4_Clear();
  Lcd4_Set_Cursor(1,0);
  DDRD |= 0x80;
  Rly1State(true);
  testTENS();
  Lcd4_Write_String("It should be on"); 

  while(1){ 
  }
  #else
  
  //Configuring timer for 1KHz
  //configureTimer2();
  
  
  Lcd4_Clear();
  Lcd4_Set_Cursor(1,1);
  Lcd4_Write_String("Inicio");
  _delay_ms(500);
  Lcd4_Clear();


  while(1){
    interfaceloop();
  }
  #endif
}

ISR(TIMER1_COMPA_vect) {
  if(++sampleCntr == 50){ //this part will move sample Cntr and check if the last value was reached
    sampleCntr = 0;
    //digitalWrite(13,x); //for trigger
    //x = !x;
    //This part is to know if the period will be counted
    if(periodCnt){
      periodCnt = false;
    }
    else{
     periodCntr++;
     periodCnt = true;
    }
  }

  if(signalState){ //This part will output the signal if enabled
    switch (signalType){
      case TENS:
        writeTENS();
        intervalCheck();
        break;
      case TREN:
        writeTREN();
        intervalCheck();
        break;
      case RUSA:
        writeRUSA();
        intervalCheck();
        break;
      case EXPO:
        writeEXPO();
        intervalCheck();
        break;
      case GALV:
        writeGALV(0);
        break;
      case DIAD:
        writeDIAD();
        break;
    }
  }
  else{ //If the signal should be disable this part will kill the signal
    intervalCheck();
    //if(signalDirection != OFF) //If the direction is not pointing to disabled
      deactivateOutput(); //This will kill the output
  }
  countSeconds(); //timing stuff
}

//////////////////
//Timing functions
//////////////////
void countSeconds(){ //This function manage some timing things
  if(workingFreq == periodCntr){
    periodCntr = 0;
    middleSec = false; //reseting middle time
    //expoCntr = 0;
    if(intervalCount && !testMode){ //This is for managing interval counting. Should be disabled for GALV, DIAD and so on.
      secondsCntr++; //This var is to manage intervals
    }
    therapyTime++; //How much seconds the program has been running
  }
  if(workingFreq/2 == periodCntr){
    middleSec = true; //to know the middle time
  }
}

void intervalCheck(){ //To enable and disable output for RUSA and EXPO
  if(testMode){
    secondsCntr = 0;
  }
  if(secondsCntr == intervalSecs){
    secondsCntr = 0;
    signalState = !signalState;
  }
}
//////////////////
//Timing functions END
//////////////////


////////////////////////
//Initializing functions
////////////////////////
void declareDDRC(){
  DDRC = 0x00;
  DDRC |= 0x3F; //= 0x3F //For LCD driving
}
void declareDDRD(){ //PWM outputs are controlled
  DDRD = 0x00;
  DDRD |= 0x90; //This is for Rly1 and Rly2 outputs
}
////////////////////////
//Initializing functions END
////////////////////////

/////////////////////////////
//Signal Generation Functions
/////////////////////////////
/*
enum Name Freqs
0 TENS - 1,5,10,50,100,200 Hz
1 Tren = 35Hz
2 RUSA = 50Hz
3 EXPO = 65Hz
4 GALV = 0Hz << DC
5 DIAD = 200Hz
*/
void setFrequency(int f, bool act){ //All real frequencies will be x100
  switch (f){
    case 1: //x100
      configureTimer1(624,  4);//0x04);
      break;
    case 5: //x500
      configureTimer1(124,  4);//0x04);
      break;
    case 10: //x1000
      configureTimer1(249,  3);//0x03);
      break;
    case 35: //x3500
      configureTimer1(4570, 1);//0x01);
      break;
    case 50: //x5000
      configureTimer1(49,   3);//0x03);
      break;
    case 65: //x6500
      configureTimer1(2460, 1);//0x01);
      break;   
    case 100: //x10000
      configureTimer1(24,   3);//0x03);
      break;
    case 200: //x20000
      configureTimer1(99,   2);//0x02);
      //configureTimer1(89,   0x02);  //timing fix
      break;          
  }
  workingFreq = f; //This variable stores the current freq, so there is stored
  activateTimer1(act);
}


//To turn on relay
void Rly1State(bool turn){
  if (turn){
    PORTD |=   0x80;
  }
  else{
    PORTD &= ~(0x80);
  }
}

//&&&&&&
//EXPO
//&&&&&&
void initEXPO(){
  configEXPO();
  expoCntr = 0;
  signalType = EXPO;
  actTimer0();
  signalState = true; //For activate outputs
  setFrequency(65,true); //Activating timers
  Rly1State(true);
}
void configEXPO(){
  configGALV();
  intervalCount = true;
}

void writeEXPO(){
  if(sampleCntr == 0 && periodCnt){
    if(secondsCntr==0 && !middleSec){
      expoCntr++;
      if(expoCntr>32) expoCntr = 32;
        expWrite = expoSignal[expoCntr];
    } 
  else{
      if(secondsCntr==intervalSecs-1 && middleSec){
        expoCntr--;
        if(expoCntr< 0) expoCntr = 0;
          expWrite = expoSignal[expoCntr];
      }
      else{
        expWrite = 0;
      }
    }
  
  }
  
  writeGALV(expWrite);
}
//&&&&&&&&&&&&&
//&&&&&&&&&&&&&

//&&&&&&
//TREN
//&&&&&&
void initTREN(){
  configTREN();
  signalType = TREN;
  actTimer0();
  signalState = true; //For activate outputs
  setFrequency(35,true); //Activating timers
  Rly1State(true);
}
void configTREN(){
  intervalCount = true;
  configGALV();
}
void writeTREN(){
  writeGALV(0);
}
//&&&&&&&&&&&&&
//&&&&&&&&&&&&&

//&&&&&&
//RUSA
//&&&&&&
void initRUSA(){
  configRUSA();
  signalType = RUSA;
  actTimer0();
  signalState = true; //For activate outputs
  setFrequency(50,true); //Activating timers
  Rly1State(true);
}
void configRUSA(){
  intervalCount = true;
  configGALV();
}
void writeRUSA(){
  writeGALV(0);
}
//&&&&&&&&&&&&&
//&&&&&&&&&&&&&

//&&&&&&
//TENS
//&&&&&&
void initTENS(){
  configTENS();
  signalType = TENS;
  actTimer0();
  signalState = true; //For activate outputs
  setFrequency(currentTENSf,true); //Activating timers
  Rly1State(true);
}
void configTENS(){ //Init config for TENS
  configGALV();
  deactivatePWM(); //Deactivating PWM pins
  DDRD |= 0x60; //Activate output
}
void writeTENS(){
  //ValStr(sampleCntr, 0, Str2p);
  //Lcd4_Set_Cursor(1, 1);
  //Lcd4_Write_String(Str2p);
  if(sampleCntr == 0){
    sideEdge = !sideEdge;
  }
  switch(sampleCntr){
    case 0:
      if(sideEdge){
        activateUpOutput();
      }
      else{
        activateDwOutput();
      }
      //Aunque son los dos bits uno se declara como entrada para
      //no sacar nada
      PORTD |= 0x60;
    break;
    case 4:
      deactivateOutput();
    break;
  }
}
//&&&&&&&&&&&&&
//&&&&&&&&&&&&&

//&&&&&&&
//GALV
//&&&&&&&
void initGALV(){
  configGALV();
  signalType = GALV;
  actTimer0();
  signalState = true; //For activate outputs
  setFrequency(200,true); //Activating timers
  Rly1State(true);
}
void configGALV(){ // Init config for GALV
  deactivateOutput();
  sideEdge = true;
}
void writeGALV(unsigned char shift){
  if (signalType == GALV){
    deactivatePWM();
    //activateDwOutput();
    PORTD |= 0x40;
  }
  else{
  regSample = sineSignal[sampleCntr];  //taking the register from sineSignal
  if (regSample == 0){  //if the reg is 0 then deactivate output
    if(signalDirection != OFF){
      sideEdge = !sideEdge; //Change the direction of the signal
    }
    deactivateOutput();
  }
  else{
    regSample = (regSample >> shift);
    if(sideEdge){
      OCR0A = regSample; //Writes register for PWM
      if(signalDirection != UP){ //If UP direction is not active activate it
        activateUpOutput();
      }
    }
    else{
      OCR0B = regSample; //Writes register for PWM
      if(signalDirection != DOWN){ //If DOWN direction is not active activate it
        activateDwOutput();
      }
    } 
  }
  }
}
//&&&&&&&&&&&&&
//&&&&&&&&&&&&&

//&&&&&&
//DIAD
//&&&&&&
void initDIAD(){
  configDIAD();
  signalType = DIAD;
  actTimer0();
  signalState = true; //For activate outputs
  setFrequency(200,true); //Activating timers
  Rly1State(true);
}

void configDIAD(){ // Init config for DIAD
  configGALV();
}
void writeDIAD(){
  regSample = sineSignal[sampleCntr]; //taking the register from sineSignal
  if (regSample == 0){ //if the reg is 0 then deactivate output
    if(signalDirection != OFF){
      deactivateOutput();
    }
  }
  else{
    OCR0A = regSample; //Writes register for PWM
    if(signalDirection != UP){ //If UP direction is not active activate it
      activateUpOutput();
    }
  }
}
//&&&&&&&&&&&&&
//&&&&&&&&&&&&&

void deactivateOutput(){
  DDRD &= ~(0x60);
  PORTD &= ~(0x60);
  signalDirection = OFF;
}
void activateUpOutput(){ //OCR0A
  deactivateOutput();
  DDRD |= 0x40;
  signalDirection = UP;
}
void activateDwOutput(){ //OCR0B
  deactivateOutput();
  DDRD |= 0x20;
  signalDirection = DOWN;
}

/////////////////////////////
//Signal Generation Functions END
/////////////////////////////

////////////////////////
//Timer 0 configurations
////////////////////////
void initTimer0(){
  TCCR0A = 0x00;
  TCCR0B = 0x00;
  TIMSK0 = 0x00;
  TCNT0  = 0x00; //Counter is cleared
  OCR0A  = 0x00; //This guy is used when CTC mode is enabled, so is useless here
  OCR0B  = 0x00; //This guy is used when CTC mode is enabled, so is useless here
}

//Configures PWM signals
void configureTimer0(){
  //Enabling fastpwm mode for timer 0: pag 140
  TCCR0A |= (1 << WGM01 ) | (1 << WGM00 );
  //PWM in non-inverting mode: pag 139
  TCCR0A |= (1 << COM0A1) | (0 << COM0A0);
  TCCR0A |= (1 << COM0B1) | (0 << COM0B0);
  //
  /*Prescaler: pag 142
   * 000 - Stopped
   * 001 - x1
   * 010 - x8
   * 011 - x64
   * 100 - x256
   * 101 - x1024
   */
  TCCR0B |= (0 << CS02) | (1 << CS01) | (0 << CS00); //Prescaling
}

void deactivatePWM(){
  //Normal port operation: pag 139
  TCCR0A &= ~((1 << COM0A1) | (0 << COM0A0));
  TCCR0A &= ~((1 << COM0B1) | (0 << COM0B0));
}
////////////////////////
//Timer 0 configurations END
////////////////////////


/////////////////////////////////
//Timer 1 configuration functions
/////////////////////////////////
void initTimer1(){
  TCCR1A = 0x00;
  TCCR1B = 0x00;
  TCNT1  = 0x00;  //Counter is cleared
  TIMSK1 = 0x00;

}

void configureTimer1(int COM, char Prescaler){
  initTimer1();
  //Writing prescaler
  TCCR1B  = Prescaler;
  //CTC mode: pag 172
  TCCR1B |= 0x08;
  /*Prescaler: pag 173
   * 000 - Stopped
   * 001 - x1
   * 010 - x8
   * 011 - x64
   * 100 - x256
   * 101 - x1024
   */
  //Interrupt Compare register
  OCR1A = COM;
  //TCCR1B = 0x08; 
  activateTimer1(true);
}

void activateTimer1(bool act){ //This function activates TIMER1_COMPA_vect
  if(act){
    TIMSK1 |=  (1 << OCIE1A); //Activate interrupt
    sei(); // VERY IMPORTANT
  }
  else{
    TIMSK1 &= ~(1 << OCIE1A); //Deactivate interrupt
    cli(); //VERY IMPORTANT
  }
}
///////////////////////////////
//Timer 1 configuration functions END
///////////////////////////////


/////////////////
//Debug functions
/////////////////
void testPWMs(){ //This function is only to check if signals are being generated, and you could check frequency in the scope 
  DDRD  |= 0x60; //Enabling outputs
  actTimer0();
  OCR0A = 128;
  OCR0B = 128;
}


void actTimer0(){
  initTimer0();
  configureTimer0();
}
/////////////////
//Debug functions END
/////////////////
