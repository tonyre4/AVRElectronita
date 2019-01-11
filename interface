//#include <LiquidCrystal_I2C.h>
//#include <Wire.h>
//LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <avr/io.h>
#include <string.h> 

int Var1=1;   //Variable controlada por los botones OK y RETURN/BACK
int Var2=1;   //Variable controlada por los botones UP/+ y DOWN/-
int Var3=1;   //Variable controlada por los botones LEFT y RIGHT
unsigned int Modos=1;    //Indica que modo se debe imprimir en la pantalla y a que menu (función) de dicho modo se debe entrar
unsigned int Variables=1;    //Fluctua entre las variables a modificar
unsigned int Mensaje=1;    //Controla el switch principal, maneja lo que debe imprimirse
bool Testindicator=false;
bool t=false;   //Ayuda a reducir el rebote de la lectura de matriz

String Str1;    //Imprime los modos
String Str2;    //Imprime la variable a afectar
String Str3;    //Imprime los cambios en las varibales
//String Str4;
//String Str5;
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
      Str3=String('<'+ String(timeM)+ " min> " + String(timeS) + " seg");
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
      Str3=String(String(timeM) + " min <" + String(timeS) + " seg>");
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
      Str3=String('<'+ String(timeM)+ " min> " + String(timeS) + " seg");
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
      Str3=String(String(timeM) + " min <" + String(timeS) + " seg>");
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
      Str3=String('<' + String(frecuencia[Hz]) + "Hz>");
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
      Str3=String('<'+ String(timeM)+ " min> " + String(timeS) + " seg");
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
      Str3=String(String(timeM) + " min <" + String(timeS) + " seg>");
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
      Str3=String('<' + String(intervalos) + "seg>");
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
      Str3=String('<'+ String(timeM)+ " min> " + String(timeS) + " seg");
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
      Str3=String(String(timeM) + " min <" + String(timeS) + " seg>");
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
      Str3=String('<' + String(intervalos) + "seg>");
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
      Str3=String('<'+ String(timeM)+ " min> " + String(timeS) + " seg");
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
      Str3=String(String(timeM) + " min <" + String(timeS) + " seg>");
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
      Str3=String('<'+ String(timeM)+ " min> " + String(timeS) + " seg");
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
      Str3=String(String(timeM) + " min <" + String(timeS) + " seg>");
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
      Str3=String('<' + String(intervalos) + "seg>");
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
    ////Serial.println(String(String(therapyTime) + String(secondsPrinted)));
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
            //lcd.print(String('0' +String(resttimeM) + ':' + '0' + String(resttimeS)));
          } else{//lcd.print('0' +String(String(resttimeM) + ':' + String(resttimeS)));}
        } else{
          if(resttimeS<10){
            //lcd.print(String(String(resttimeM) + ':' + '0' + String(resttimeS)));
          } else{//lcd.print(String(String(resttimeM) + ':' + String(resttimeS)));}
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
