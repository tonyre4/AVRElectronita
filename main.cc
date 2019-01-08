#define PATH /home/eansalab/atmega/electronita

#include <stdio.h>
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>


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
unsigned char sineSignal[50] = {0, 0, 31, 47, 63, 78, 93, 108, 122, 136, 149, 162, 174, 185, 196, 206, 215, 223, 230, 237, 242, 246, 250, 252, 254, 255, 254, 252, 250, 246, 242, 237, 230, 223, 215, 206, 196, 185, 174, 162, 149, 136, 122, 108, 93, 78, 63, 47, 0, 0};
//Numbers to divide sine signal
unsigned char expoSignal[33] = {7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1};
//number to calculate exponential
char expoCntr = 0;
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
void loop2();
void configureTimer2();
void Rly1State(bool turn);
void deactivatePWM();
void writeTREN();
void writeTENS();
void writeDIAD();
void writeRUSA();
void writeGALV(unsigned char shift); 
void writeEXPO();
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
void configureTimer1(unsigned int COM, unsigned char Prescaler);
void setFrequency(unsigned char f, bool act);
void countSeconds();
void intervalCheck();
void actTimer0();

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//#include </home/eansalab/atmega/electronita/interface.h>
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

//#include <LiquidCrystal_I2C.h>
//#include <Wire.h>
//LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <string.h> 

int Var1=1;   //Variable controlada por los botones OK y RETURN/BACK
int Var2=1;   //Variable controlada por los botones UP/+ y DOWN/-
int Var3=1;   //Variable controlada por los botones LEFT y RIGHT
unsigned int Modos=1;    //Indica que modo se debe imprimir en la pantalla y a que menu (función) de dicho modo se debe entrar
unsigned int Variables=1;    //Fluctua entre las variables a modificar
unsigned int Mensaje=1;    //Controla el switch principal, maneja lo que debe imprimirse
bool Testindicator=false;
bool t=false;   //Ayuda a reducir el rebote de la lectura de matriz

string Str1;    //Imprime los modos
string Str2;    //Imprime la variable a afectar
string Str3;    //Imprime los cambios en las varibales
//string Str4;
//string Str5;
bool toprint=true;    //Cuando ocurra un cambio que afecte a los valores impresos, da la orden de imprimir al estar en TRUE
unsigned char secondsPrintedP, secondsPrinted=0;

int timeM=10;   //Setea los minutos y fluctua entre 0 y 99 no pudiendo ser este menor a 0:30
int timeS=0;    //Setea los segundos y fluctua entre 0 y 30
int intervalos=5;   //Sete el tiempo que habra de intervalos
int frecuencia[6]={1, 5, 10, 50, 100, 200};   //Frecuencias disponibles para TENS
int Hz=0;   //Iterador de las frecuencias de TENS

//Func dec
void setup2() ;
void loop2() ;
void VerifyOut();
void menuGalvanica();
void menuTENS();
void menuTREN();
void menuRUSA();
void menuDiadinamica();
void menuExponencial();
void checktime();
void checkfrecuencia();
void checkintervalos();
void Aplicando();
void resetDef();


void setup2() {
  //Wire.begin();                             //Se inicia la interacción SCA y SCL
  //lcd.begin(16, 2);                         //"" la pantalla en 16 columnas y 2 filas
  ////lcd.backlight();                          //"" la iluminación de la pantalla
  ////lcd.init();
  //lcd.clear();                              //Limpia pantalla
  //lcd.setCursor(1,0);                       
  //lcd.print("Mensaje Inicio");             //Imprime mensaje de inicio y espera 2.5s antes de iniciar el programa
  delay(2500);

/*  pinMode(Out1, INPUT);                     //Salida de la matriz de botones, columna 0
  pinMode(Out2, INPUT);                     //Salida de la matriz de botones, columna 1
  pinMode(line1, OUTPUT);                   //Entrada de la matriz de botones, fila 0
  pinMode(line2, OUTPUT);                   //Entrada de la matriz de botones, fila 1
  pinMode(line3, OUTPUT);                   //Entrada de la matriz de botones, fila 2
  digitalWrite(line1, HIGH);                //Inicia todas las filas en HIGH y por ende, habilita la matriz
  digitalWrite(line2, HIGH);
  digitalWrite(line3, HIGH);*/
  DDRB  = 0x3C;  //PINES X,X,13,12row2,11row1,10row0,9col1,8col0   //0 INPUT   //1 OUTPUT
  PORTB = 0x1C;  //Pone en HIGH los pines 10, 11 y 12
}

void loop2() {
  //if(digitalRead(Out1)==HIGH || digitalRead(Out2)==HIGH) VerifyOut();   //Revisa si es presionado un botón y cuando sea así, verificara de que botón se trata
      //if(((PINB & B00000001) == B00000001)||((PINB & B00000010) == B00000010))VerifyOut();
  
  if(bPressed()){
    VerifyOut();
    
    waitFor(10);
    while(bPressed()) {
      //lcd.setCursor(0, 0);
      //lcd.print("hola5");
    }
  }

  if(Mensaje==1){   //Cuando el mensaje sea el inicial, solo tendra ponderancia el botón OK
/**/if(Var1>1){     //Si algún otro botón fuese presionado, su variable asignada no tendra influencia en los resultados siguientes
      Mensaje++;
      Var1=1;
      toprint=true;
    }
    if(Var1<1)Var1=1;
    if(Var2>1)Var2=1;
    if(Var2<1)Var2=1;
    if(Var3>1)Var3=1;
    if(Var3<1)Var3=1;
  }
  
  if(Mensaje==2){   //Cuando se encuentre dentro del menú de modos, los botones RETURN, LEFT y RIGHT estarán deshabilitados
    if(Var1>1){     //Entra al menú de variables
      Mensaje++;
      Var1=1;
      toprint=true;
    }
    if(Var2>1){   //Cambia de modo hacia arriba
      Modos++;
      Var2=1;
      toprint=true;
    }
    if(Var2<1){   //Cambia de modo hacia abajo
      Modos--;
      Var2=1;
      toprint=true;
    }
    if(Var1<1)Var1=1;
    if(Var3>1)Var3=1;
    if(Var3<1)Var3=1;
  }

  if(Mensaje==3){     //Cuando el mensaje sea 3, todos los botones tendran una función, aumentando o dismuyendo valores o saltando entre ellos para modificarlos
    if(Var3>1){       //Cambia de variable hacia adelante
      if(testMode){
        testMode = !testMode;
        resetSets();
      }
      Variables++;    
      Var3=1;
      toprint=true;
    }
    if(Var3<1){       //Cambia de variable hacia abajo
      if(testMode) {
        testMode = !testMode;
        resetSets();
      }
      Variables--;
      Var3=1;
      toprint=true;
    }
    if(Var1<1){       //Regresa al menú de modos
      Var1=1;
      Mensaje--;
      toprint=true;
      testMode=false;
      resetSets();
      resetDef();
    }
  }

  if(Modos>6) Modos=1;    //Restringe el switcheo de modos, ciclandolo entre los 6 existentes
  if(Modos<1) Modos=6;

  switch(Mensaje){
    case 1:   //Mensaje de bienvenida
      if(toprint){
        //lcd.clear(); //lcd.setCursor(2, 0);
        //lcd.print("Presione OK");
        //lcd.setCursor(2, 1);
        //lcd.print("para iniciar");
        toprint=false;
      }
      break;/////////////////////////////////////////////////////////////////////////////////////////////////
    case 2:   //Mensaje de modos
      switch(Modos){    //Usa el valor de Modos para declarar que imprimir
        case 1:
          Str1="-   Galvanica  +";
          break;
        case 2:
          Str1="-     TENS     +";
          break;
        case 3:
          Str1="-     TREN     +";
          break;
        case 4:
          Str1="-     RUSA     +";
          break;
        case 5:
          Str1="-  Diadinamica +";
          break;
        case 6:
          Str1="-  Exponencial +";
          break;
      }
      if(toprint){
        //lcd.clear();//lcd.setCursor(0,0);
        //lcd.print(Str1);
        //lcd.setCursor(0, 1);
        //lcd.print("OK p/ configurar");
        toprint=false;
      }
       break;/////////////////////////////////////////////////////////////////////////////////////////////////
    case 3:   //Mensajes para cambiar variables
      switch(Modos){    //Usa el valor de Modos para ejecutar la función de dicho Modo
        case 1:
          menuGalvanica();
          break;
        case 2:
          menuTENS();
          break;
        case 3:
          menuTREN();
          break;
        case 4:
          menuRUSA();
          break;
        case 5:
          menuDiadinamica();
          break;
        case 6:
          menuExponencial();
          break;
      }
      if(toprint){
        //lcd.clear(); //lcd.setCursor(0, 0);
        //lcd.print(Str2);
        //lcd.setCursor(0,1);
        //lcd.print(Str3);
        toprint=false;
      }
      break;/////////////////////////////////////////////////////////////////////////////////////////////////
    case 4:   //Mesajes mientras se aplica la terapia
      Aplicando();    //función de aplicación
      break;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VerifyOut(){

  PORTB = 0x04;
  startCount();
  waitFor(50);
  stopCount();
  //for(int i=0; i<11; i++){ t = !t;}   /*NO BORRAR AYUDA AL CORRECTO FUNCIONAMIENTO DE LA MATRIZ*/
  if(PINB & 0x01){
    Var1++;/*OK*/
    return;
  }
  if(PINB & 0x02){
    Var3++;/*RIGHT*/
    return;
  }
  
  PORTB = 0x08;
  startCount();
  waitFor(50);
  stopCount();
  //for(int i=0; i<11; i++){ t = !t;}   /*NO BORRAR AYUDA AL CORRECTO FUNCIONAMIENTO DE LA MATRIZ*/
  if(PINB & 0x01){
    Var1--;/*RETURN*/
    return;
  }
  if(PINB & 0x02){
    Var2++;/*UP +(PLUS)*/
    return;
  }
  
  PORTB = 0x10;
  startCount();
  waitFor(50);
  stopCount();
  //for(int i=0; i<11; i++){ t = !t;}   /*NO BORRAR AYUDA AL CORRECTO FUNCIONAMIENTO DE LA MATRIZ*/
  if(PINB & 0x01){
    Var3--;/*LEFT*/
    return;
  }
  if(PINB & 0x02){
    Var2--;/*DOWN -(MINUS)*/
    return;
  }
}

bool bPressed(){
  PORTB = 0x1C;
  waitFor(50);
  if (PINB & 0x03)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void menuGalvanica(){
  if(Variables<1) Variables=1;
  if(Variables>4) Variables=4;
  switch(Variables){
    case 1:
      Str2="Minutos";
      if(Var2>1){
        timeM++; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeM--; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string('<'+ string(timeM)+ " min> " + string(timeS) + " seg");
      break;
    case 2:
      Str2="Segundos";
      if(Var2>1){
        timeS=timeS+30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeS=timeS-30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string(string(timeM) + " min <" + string(timeS) + " seg>");
      break;
    case 3:
      if(Var1>1){
        Var1=1;
        testMode =! testMode;
        if(testMode){
          initGALV();
        }
        else{
          resetSets();
        }        
        toprint=true;
      }
      if(testMode){
        Str2="Test Activado";
        Str3="OK = Desactivar";
      }else {
        Str2="Test Desactivado";
        Str3="OK = Activar";
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
    case 4:
      Str2="Aplicar?";
      Str3="OK = Iniciar";
      if(Var1>1){
        Mensaje++;
        Var1=1;
        toprint=true;
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void menuTENS(){
  if(Variables<1) Variables=1;
  if(Variables>5) Variables=5;
  switch(Variables){
    case 1:
      Str2="Minutos";
      if(Var2>1){
        timeM++; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeM--; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string('<'+ string(timeM)+ " min> " + string(timeS) + " seg");
      break;
    case 2:
      Str2="Segundos";
      if(Var2>1){
        timeS=timeS+30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeS=timeS-30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string(string(timeM) + " min <" + string(timeS) + " seg>");
      break;
    case 3:
      Str2="Frecuencia";
      if(Var2>1){
        Hz++; checkfrecuencia();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        Hz--; checkfrecuencia();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string('<' + string(frecuencia[Hz]) + "Hz>");
      break;
    case 4:
      if(Var1>1){
        Var1=1;
        testMode =! testMode;
        if(testMode){
          currentTENSf = frecuencia[Hz];
          initTENS();
        }
        else{
          resetSets();
        }   
        toprint=true;
      }
      if(testMode){
        Str2="Test Activado";
        Str3="OK = Desactivar";
      }else {
        Str2="Test Desactivado";
        Str3="OK = Activar";
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
    case 5:
      Str2="Aplicar?";
      Str3="OK = Iniciar";
      if(Var1>1){
        Mensaje++;
        Var1=1;
        toprint=true;
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void menuTREN(){
  if(Variables<1) Variables=1;
  if(Variables>5) Variables=5;
  switch(Variables){
    case 1:
      Str2="Minutos";
      if(Var2>1){
        timeM++; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeM--; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string('<'+ string(timeM)+ " min> " + string(timeS) + " seg");
      break;
    case 2:
      Str2="Segundos";
      if(Var2>1){
        timeS=timeS+30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeS=timeS-30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string(string(timeM) + " min <" + string(timeS) + " seg>");
      break;
    case 3:
      Str2="Intervalos";
      if(Var2>1){
        intervalos++; checkintervalos();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        intervalos--; checkintervalos();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string('<' + string(intervalos) + "seg>");
      break;
    case 4:
      if(Var1>1){
        Var1=1;
        testMode =! testMode;
        if(testMode){
          initTREN();
        }
        else{
          resetSets();
        }  
        toprint=true;
      }
      if(testMode){
        Str2="Test Activado";
        Str3="OK = Desactivar";
      }else {
        Str2="Test Desactivado";
        Str3="OK = Activar";
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
    case 5:
      Str2="Aplicar?";
      Str3="OK = Iniciar";
      if(Var1>1){
        Mensaje++;
        Var1=1;
        toprint=true;
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void menuRUSA(){
  if(Variables<1) Variables=1;
  if(Variables>5) Variables=5;
  switch(Variables){
    case 1:
      Str2="Minutos";
      if(Var2>1){
        timeM++; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeM--; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string('<'+ string(timeM)+ " min> " + string(timeS) + " seg");
      break;
    case 2:
      Str2="Segundos";
      if(Var2>1){
        timeS=timeS+30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeS=timeS-30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string(string(timeM) + " min <" + string(timeS) + " seg>");
      break;
    case 3:
      Str2="Intervalos";
      if(Var2>1){
        intervalos++; checkintervalos();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        intervalos--; checkintervalos();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string('<' + string(intervalos) + "seg>");
      break;
    case 4:
      if(Var1>1){
        Var1=1;
        testMode =! testMode;
        if(testMode){
          initRUSA();
        }
        else{
          resetSets();
        }  
        toprint=true;
      }
      if(testMode){
        Str2="Test Activado";
        Str3="OK = Desactivar";
      }else {
        Str2="Test Desactivado";
        Str3="OK = Activar";
      }
      
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
    case 5:
      Str2="Aplicar?";
      Str3="OK = Iniciar";
      if(Var1>1){
        Mensaje++;
        Var1=1;
        toprint=true;
      }
      break;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void menuDiadinamica(){
  if(Variables<1) Variables=1;
  if(Variables>4) Variables=4;
  switch(Variables){
    case 1:
      Str2="Minutos";
      if(Var2>1){
        timeM++; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeM--; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string('<'+ string(timeM)+ " min> " + string(timeS) + " seg");
      break;
    case 2:
      Str2="Segundos";
      if(Var2>1){
        timeS=timeS+30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeS=timeS-30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string(string(timeM) + " min <" + string(timeS) + " seg>");
      break;
    case 3:
      if(Var1>1){
        Var1=1;
        testMode =! testMode;
        if(testMode){
          initDIAD();
        }
        else{
          resetSets();
        }  
        toprint=true;
      }
      if(testMode){
        Str2="Test Activado";
        Str3="OK = Desactivar";
      }else {
        Str2="Test Desactivado";
        Str3="OK = Activar";
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
    case 4:
      Str2="Aplicar?";
      Str3="OK = Iniciar";
      if(Var1>1){
        Mensaje++;
        Var1=1;
        toprint=true;
      }
      break;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void menuExponencial(){
  if(Variables<1) Variables=1;
  if(Variables>5) Variables=5;
  switch(Variables){
    case 1:
      Str2="Minutos";
      if(Var2>1){
        timeM++; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeM--; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string('<'+ string(timeM)+ " min> " + string(timeS) + " seg");
      break;
    case 2:
      Str2="Segundos";
      if(Var2>1){
        timeS=timeS+30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        timeS=timeS-30; checktime();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string(string(timeM) + " min <" + string(timeS) + " seg>");
      break;
    case 3:
      Str2="Intervalos";
      if(Var2>1){
        intervalos++; checkintervalos();
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        intervalos--; checkintervalos();
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      Str3=string('<' + string(intervalos) + "seg>");
      break;
    case 4:
      if(Var1>1){
        Var1=1;
        testMode =! testMode;
        if(testMode){
          initEXPO();
        }
        else{
          resetSets();
        }  
        toprint=true;
      }
      if(testMode){
        Str2="Test Activado";
        Str3="OK = Desactivar";
      }else {
        Str2="Test Desactivado";
        Str3="OK = Activar";
      }
      
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
    case 5:
      Str2="Aplicar?";
      Str3="OK = Iniciar";
      if(Var1>1){
        Mensaje++;
        Var1=1;
        toprint=true;
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void checktime(){
  if(timeM>99){ timeM=99;  timeS=30; }              //Mantiene el valor del tiempo debajo de 100 minutos     /
  if(timeM<=0 && timeS<=30){ timeM=0;  timeS=30; }  //Mantiene el valor del tiempo por encima de 0 minutos   /
  if(timeS>30){ timeM++;  timeS=0; }                //Mantiene el valor de los segundos entre 0 y 30,        /
  if(timeS<0){ timeM--;  timeS=30; }                //           restando o sumando 1 minuto de ser necesario/
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void checkfrecuencia(){
  if(Hz<=0) Hz=0;                                   //Mantiene el iterador dentro del rango del Array
  if(Hz>=5) Hz=5;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void checkintervalos(){
  if(intervalos>7) intervalos=7;                    //Mantiene el valor de los intervalos desde 1s hasta 7s
  if(intervalos<1) intervalos=1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Aplicando(){
  intervalSecs = intervalos; //Setting interval time
  testMode = false;
  therapyTime = 0;
  switch(Modos){
    case 1:
      initGALV();
      break;
    case 2:
      currentTENSf = frecuencia[Hz];
      initTENS();
      break;
    case 3:
      initTREN();
      break;
    case 4:
      initRUSA();
      break;
    case 5:
      initDIAD();
      break;
    case 6:
      initEXPO();
      break;
  }
  //bool m=true;
  //int secdelayfinish=0;
  unsigned int resttimeM=timeM;   //Variables volatiles que toman el valor del tiempo con X mins y Y segs
  unsigned int resttimeS=timeS;
  unsigned int secondsPrinted;
  unsigned int secdelayfinish=0;
  secondsPrinted=therapyTime;
  unsigned int secondsPrintedP=secondsPrinted;
  //Str4="Aplicando";
  //digitalWrite(line1, LOW); digitalWrite(line3, LOW);   //Se descativan la linea 1 y 3 para solo dejar habilitados los botones RETURN y UP
  //###PORTB = B00001000;   //Se descativan la linea 1 y 3 para solo dejar habilitados los botones RETURN y UP
  ////lcd.clear(); //lcd.setCursor(0, 0);
  ////lcd.print("Aplicando");
  while(true){
    ////Serial.println(string(string(therapyTime) + string(secondsPrinted)));
    //while(true){
      if(/*middleSec && m*/secondsPrinted != secondsPrintedP){
        /*m=false;*/secondsPrintedP = secondsPrinted;
        //if(digitalRead(Out1)==HIGH)break;   //Cuando se detecte un pulso en la columna1 (que este solo puede provenir de RETURN por lo anterior), se finalizará
        if((PINB & 0x01) == 0x01)break;   //Cuando se detecte un pulso en la columna1 (que este solo puede provenir de RETURN por lo anterior), se finalizará
        if(resttimeS==0){resttimeS=60; resttimeM--;}    //Cuando los segundos sean 0,  X-1 mins con 60 segundos
        resttimeS--;    //Se resta 1 segundo al tiempo total
        ////Serial.println(therapyTime);
        //lcd.clear(); //lcd.setCursor(0, 0);
        ////lcd.print(Str4);
        //lcd.print("Aplicando");
        //lcd.setCursor(0, 1); ////lcd.print("    "); //lcd.setCursor(0, 1);
        if(resttimeM<10){
          if(resttimeS<10){
            //lcd.print(string('0' +string(resttimeM) + ':' + '0' + string(resttimeS)));
          } else{//lcd.print('0' +string(string(resttimeM) + ':' + string(resttimeS)));}
        } else{
          if(resttimeS<10){
            //lcd.print(string(string(resttimeM) + ':' + '0' + string(resttimeS)));
          } else{//lcd.print(string(string(resttimeM) + ':' + string(resttimeS)));}
        }
      //}
    //while(true){
      ////Serial.println(secondsCntr);
      //Seconds();
      }
      //if(((PINB & B00000001) == B00000001) /*|| (secondsPrintedP == secondsPrinted /*|| (!middleSec&&!m))*/)break;
    //}
    secondsPrinted=therapyTime;
    /*m=true;*/
    //delay(1000);
    if((resttimeM==0 && resttimeS==0) ||((PINB & 0x01) == 0x01)) break;   //Cuando el tiempo llegue a 0:00 se detendra
  }
    //lcd.clear(); //lcd.setCursor(0,0);    //Cuando se termine la cuenta del tiempo se imprimira "Finalizado" y esperara 2.5segs
    //lcd.print("Finalizado");
    resetSets();
    int j=1;
    int until;
    for(int i=0; i<32000; i++){
      //while(true){
      //Serial.print(i-i); //No mover
      if(i == (4*j)){
        PORTD =(PIND | 0x10);
        j++;
        until = i + 1;
      }
      if(i == until){
        PORTD = (PIND & 0xEF);
      }
        /*if(/*middleSec && m*//*secondsPrinted != secondsPrintedP){
          /*m=false;*//*secondsPrintedP = secondsPrinted;
          secdelayfinish++;
        }
        //if(/*!middleSec&&!m*//*true)break;
      //}
      //m=true;
      secondsPrinted=therapyTime;
      if((secdelayfinish >=3) /*|| ((PINB & B00000001) == B00000001)*//*) break;*/
    }
    //delay(2500);
    Mensaje=1; Variables=1; Modos=1; timeS=0; timeM=10; Hz=0; intervalos=5; toprint=true; testMode=false;/*Resetean todos los valores*/
    Var1=1; Var2=1; Var3=1; /*digitalWrite(line1, HIGH); digitalWrite(line3, HIGH);*/ PORTB = 0x1C;/*Resetean todos los valores*/ 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void resetDef(){   //Resetea los valores de las variables a su valor inicial
  timeM=10;
  timeS=0;
  Variables=1;
  Hz=0;
  intervalos=5;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//END&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


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

int main(){
  //I/O declaration
  #ifndef DEBUGSIGNALS
  //setup2();
  #endif
  
  //Configuring timer for 1KHz
  configureTimer2();
  
  declareDDRC();
  declareDDRD();
  defaultSets();
  //Serial.begin(115200);
  
  
  //lcd.setCursor(0, 0);
  //lcd.print("Modo test"); 
  /**///initGALV();
  #ifdef DEBUGSIGNALS
  initEXPO();
  #endif
}

void loop(){
  #ifndef DEBUGSIGNALS
  //loop2();
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
    if(signalDirection != OFF) //If the direction is not pointing to disabled
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
    //expoCntr = 32;
  }
//  digitalWrite(13,middleSec); //Timing 1s
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
4 GALV - 200Hz
5 DIAD = 200Hz
*/
void setFrequency(unsigned char f, bool act){ //All real frequencies will be x100
  switch (f){
    case 1: //x100
      configureTimer1(624,  0x04);
      break;
    case 5: //x500
      configureTimer1(124,  0x04);
      break;
    case 10: //x1000
      configureTimer1(249,  0x03);
      break;
    case 35: //x3500
      configureTimer1(4570, 0x01);
      break;
    case 50: //x5000
      configureTimer1(49,   0x03);
      break;
    case 65: //x6500
      configureTimer1(2460, 0x01);
      break;   
    case 100: //x10000
      configureTimer1(24,   0x03);
      break;
    case 200: //x20000
      configureTimer1(99,   0x02);
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

void configureTimer1(unsigned int COM, unsigned char Prescaler){
  initTimer1();
  //CTC mode: pag 172
  TCCR1B |= (0 << WGM13) | (1 << WGM12) | (0 << WGM11) | (0 << WGM10);
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
  //Writing prescaler
  TCCR1B |= Prescaler; 
  activateTimer1(true);
}



void activateTimer1(bool act){ //This function activates TIMER1_COMPA_vect
  if(act)
    TIMSK1 |=  (1 << OCIE1A); //Activate interrupt
  else
    TIMSK1 &= ~(1 << OCIE1A); //Deactivate interrupt
}
///////////////////////////////
//Timer 1 configuration functions END
///////////////////////////////

/////////////////////////////////
//Timer 2 configuration functions
////Configures 1KHz interruption for timing
////Real freq 961.5 Hz
/////////////////////////////////
void initTimer2(){
  TCCR2A = 0x00;
  TCCR2B = 0x00;
  TIMSK2 = 0x00;
  TCNT2  = 0x00; //Counter is cleared
  OCR2A  = 0x14; //Compare Register for 1KHz
}

void configureTimer2(){ 
  initTimer2();
  //Enabling CTC mode pag 205
  TCCR2A |= (1 << WGM21);
  //CTC mode clear pag 203
  TCCR2A |= (1 << COM2A1); 
  //
  /*Prescaler: pag 207
   * 000 - Stopped
   * 001 - x1
   * 010 - x8
   * 011 - x32
   * 100 - x64
   * 101 - x128
   * 110 - x256
   * 111 - x1024
   */
  TCCR2B |= (1 << CS22) | (0 << CS21) | (1 << CS20); //Prescaling
  
  TIMSK2 = (1 << OCIE2A); //Enables A interrupt
}

void resetTimer(){
  mils = 0;
}
void actTimer(bool in){
  milsCnt = in;
}
void startCount(){
  resetTimer();
  milsCnt = true;
}

void waitFor(long time){
  while(true){
    if (mils>=time)
      break;
    //lcd.setCursor(0, 0); //NO QUITAR
  } 
}

void stopCount(){
  milsCnt = false;
}

ISR(TIMER2_COMPA_vect) {
   //if(milsCnt){
     mils++;
   //}
}

/////////////////////////////////
//Timer 2 configuration functions END
/////////////////////////////////




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
