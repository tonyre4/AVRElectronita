////////////////////////////////////////
//Function declaration
void interfaceloop();
void VerifyOut();
void Aplicando();
int  bPressed();
void resetDef();
void StrW(char *wr, int shift , char *in);
int ValStr(int v, int shift, char *wr);
void menuINTER(int m);
void menuTREN();
void menuTENS();
void menuDIAD();
void menuRUSA();
void menuGALV();
void menuGALVDIAD(int m);
void menuEXPO();
void checktime();
void checkfrecuencia();
void checkintervalos();

/////////////////////////////////////////
//Variable declaration
int Var1=1;   //Variable controlada por los botones OK y RETURN/BACK
int Var2=1;   //Variable controlada por los botones UP/+ y DOWN/-
int Var3=1;   //Variable controlada por los botones LEFT y RIGHT
unsigned int Modos=1;    //Indica que modo se debe imprimir en la pantalla y a que menu (función) de dicho modo se debe entrar
unsigned int Variables=1;    //Fluctua entre las variables a modificar
unsigned int Mensaje=1;    //Controla el switch principal, maneja lo que debe imprimirse
bool Testindicator=false;
bool t=false;   //Ayuda a reducir el rebote de la lectura de matriz

bool toprint=true;    //Cuando ocurra un cambio que afecte a los valores impresos, da la orden de imprimir al estar en TRUE
unsigned char secondsPrintedP, secondsPrinted=0;

int timeM=10;   //Setea los minutos y fluctua entre 0 y 99 no pudiendo ser este menor a 0:30
int timeS=0;    //Setea los segundos y fluctua entre 0 y 30
int intervalos=5;   //Sete el tiempo que habra de intervalos
const int frecuencia[6]={1, 5, 10, 50, 100, 200};   //Frecuencias disponibles para TENS
int Hz=0;   //Iterador de las frecuencias de TENS

//Charing stuff
char Str2[16]; 
char Str3[16];
char *Str2p;
char *Str3p;


/////////////////////////////////////////////
//Buttons routines
void VerifyOut(){
  PORTB = 0x04;
  _delay_ms(1);
  if(PINB & 0x01){
    Var1++;/*OK*/
    return;
  }
  if(PINB & 0x02){
    Var3++;/*RIGHT*/
    return;
  }
  PORTB = 0x08;
  _delay_ms(1);
  if(PINB & 0x01){
    Var1--;/*RETURN*/
    return;
  }
  if(PINB & 0x02){
    Var2++;/*UP +(PLUS)*/
    return;
  }
  PORTB = 0x10;
  _delay_ms(1);
  if(PINB & 0x01){
    Var3--;/*LEFT*/
    return;
  }
  if(PINB & 0x02){
    Var2--;/*DOWN -(MINUS)*/
    return;
  }
}

int bPressed(){
  PORTB = 0x1C;
  _delay_ms(1);
  if (PINB & 0x03){
    return 1;
  }
  else{
    return 0;
  }
}


//////////////////////////////////////////////////
//LCD stuff handler
void resetDef(){   //Resetea los valores de las variables a su valor inicial
  timeM=10;
  timeS=0;
  Variables=1;
  Hz=0;
  intervalos=5;
}

int ValStr(int v, int shift, char *wr){
  int i = 0;
  int c;
  if(v>=100){
    c = v/100;
    wr[shift+i]= ('0' + c);
    i++;
  }
  if(v>=10){
    c = (v%100)/10;
    wr[shift+i]= ('0' + c);
    i++;
  }
  c = v%10;
  wr[shift+i]= ('0' + c);
  wr[shift+i+1] = '\0';
  return i+1;
}

void StrW(char *wr, int shift , char *in){
  int i;
  //for(i=0;in[i]!='\0';i++){
  //  wr[i+shift] = in[i];
  //}
  
  while(1){
    if(in[i]!='\0'){
      wr[i+shift] = in[i];
      i++;
    }
    else{
      wr[i+shift] = '\0';
      break;
    }
  }
  //i++;
  //wr[i] = '\0';
}


void interfaceloop() { 
  if(bPressed()){
    VerifyOut();
    _delay_ms(10);
    while(bPressed()) {
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
        Lcd4_Clear();
        Lcd4_Set_Cursor(1, 1);
        Lcd4_Write_String("Presione OK");
        Lcd4_Set_Cursor(2, 2);
        Lcd4_Write_String("para iniciar");
        toprint=false;
        //initVars();
      }
      break;/////////////////////////////////////////////////////////////////////////////////////////////////
    case 2:   //Mensaje de modos
      if(toprint){
        Lcd4_Clear();
        Lcd4_Set_Cursor(1, 0);
        switch(Modos){    //Usa el valor de Modos para declarar que imprimir
          case 1:
            Lcd4_Write_String("-  GALVANICA   +");
            break;
          case 2:
            Lcd4_Write_String("-    TENS      +");
            break;
          case 3:
            Lcd4_Write_String("-    TREN      +");
            break;
          case 4:
            Lcd4_Write_String("-    RUSA      +");
            break;
          case 5:
            Lcd4_Write_String("- DIADINAMICA  +");
            break;
          case 6:
            Lcd4_Write_String("- EXPONENCIAL  +");
            break;
      }
        Lcd4_Set_Cursor(2, 0);
        Lcd4_Write_String("OK p/ configurar");
        toprint=false;
      }
       break;/////////////////////////////////////////////////////////////////////////////////////////////////
    case 3:   //Mensajes para cambiar variables
      switch(Modos){    //Usa el valor de Modos para ejecutar la función de dicho Modo
        case 1:
          menuGALV();
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
          menuDIAD();
          break;
        case 6:
          menuEXPO();
          break;
      }
      if(toprint){
        Lcd4_Clear();
        Lcd4_Write_String(Str2p);
        Lcd4_Set_Cursor(2,0);
        Lcd4_Write_String(Str3p);
        toprint=false;
      }
      break;/////////////////////////////////////////////////////////////////////////////////////////////////
    case 4:   //Mesajes mientras se aplica la terapia
      Aplicando();    //función de aplicación
      break;
  }
}

///////////////////////////////////
//MENUS
void menuGALV(){
  menuGALVDIAD(1);
}

void menuDIAD(){
  menuGALVDIAD(0);
}

void menuGALVDIAD(int m){
  if(Variables<1) Variables=1;
  if(Variables>4) Variables=4;
  switch(Variables){
    case 1:{
      StrW(Str2p,0,"Minutos");
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
      Str3[0] = '<';
      int sh = 1;
      sh += ValStr(timeM,1,Str3p);
      StrW(Str3p, sh ," min> ");
      sh += 6;
      sh += ValStr(timeS,sh,Str3p);
      StrW(Str3p, sh ," seg");
      break;}
    case 2:{
      StrW(Str2p,0,"Segundos");
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
      int sh = 0;
      sh += ValStr(timeM,0,Str3p);
      StrW(Str3p, sh ," min <");
      sh += 6;
      sh += ValStr(timeS,sh,Str3p);
      StrW(Str3p, sh ," seg>");
      break;}
    case 3:{
      if(Var1>1){
        Var1=1;
        testMode =! testMode;
        if(testMode){
          if(m==1){
            initGALV();
          }
          else{
            initDIAD();
          }
        }
        else{
          resetSets();
        }        
        toprint=true;
      }
      if(testMode){
       StrW(Str2p, 0, "Test Activado");
       StrW(Str3p, 0, "OK = Desactivar");
      }else {
       StrW(Str2p, 0, "Test Desactivado");
       StrW(Str3p, 0, "OK = Activar");
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;}
    case 4:
       StrW(Str2p, 0, "Aplicar?");
       StrW(Str3p, 0, "OK = Iniciar");
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
    case 1:{
      StrW(Str2p,0,"Minutos");
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
      Str3[0] = '<';
      int sh = 1;
      sh += ValStr(timeM,1,Str3p);
      StrW(Str3p, sh ," min> ");
      sh += 6;
      sh += ValStr(timeS,sh,Str3p);
      StrW(Str3p, sh ," seg");
      break;}
    case 2:{
      StrW(Str2p,0,"Segundos");
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
      int sh = 0;
      sh += ValStr(timeM,0,Str3p);
      StrW(Str3p, sh ," min <");
      sh += 6;
      sh += ValStr(timeS,sh,Str3p);
      StrW(Str3p, sh ," seg>");
      break;}
    case 3:{
      StrW(Str2p,0,"Frecuencia");
      if(Var2>1){
        Hz++; checkfrecuencia();
        currentTENSf = frecuencia[Hz];
        Var2=1;
        toprint=true;
      }
      if(Var2<1){
        Hz--; checkfrecuencia();
        currentTENSf = frecuencia[Hz];
        Var2=1;
        toprint=true;
      }
      if(Var1>1) Var1=1; 
      int sh = 1;
      StrW(Str3p, 0 ,"<");
      sh += ValStr(currentTENSf,sh,Str3p);
      StrW(Str3p, sh ," Hz>");
      break;}
    case 4:
      if(Var1>1){
        Var1=1;
        testMode =! testMode;
        if(testMode){
          initTENS();
        }
        else{
          resetSets();
        }   
        toprint=true;
      }
      if(testMode){
       StrW(Str2p, 0, "Test Activado");
       StrW(Str3p, 0, "OK = Desactivar");
      }else {
       StrW(Str2p, 0, "Test Desactivado");
       StrW(Str3p, 0, "OK = Activar");
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
    case 5:
       StrW(Str2p, 0, "Aplicar?");
       StrW(Str3p, 0, "OK = Iniciar");
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
  menuINTER(0);
}

void menuINTER(int m){
  if(Variables<1) Variables=1;
  if(Variables>5) Variables=5;
  switch(Variables){
    case 1:{
      StrW(Str2p,0,"Minutos");
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
      Str3[0] = '<';
      int sh = 1;
      sh += ValStr(timeM,1,Str3p);
      StrW(Str3p, sh ," min> ");
      sh += 6;
      sh += ValStr(timeS,sh,Str3p);
      StrW(Str3p, sh ," seg");
      break;}
    case 2:{
      StrW(Str2p,0,"Segundos");
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
      int sh = 0;
      sh += ValStr(timeM,0,Str3p);
      StrW(Str3p, sh ," min <");
      sh += 6;
      sh += ValStr(timeS,sh,Str3p);
      StrW(Str3p, sh ," seg>");
      break;}
    case 3:{
      StrW(Str2p,0,"Segundos");
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
      int sh = 1;
      StrW(Str3p, 0 ,"<");
      sh += ValStr(intervalos,sh,Str3p);
      StrW(Str3p, sh ," seg>");
      break;}
    case 4:
      if(Var1>1){
        Var1=1;
        testMode =! testMode;
        if(testMode){
          switch(m){
            case 0:
              initTREN();
              break;
            case 1:
              initRUSA();
              break;
            case 2:
              initEXPO();
              break;
          }
        }
        else{
          resetSets();
        }  
        toprint=true;
      }
      if(testMode){
       StrW(Str2p, 0, "Test Activado");
       StrW(Str3p, 0, "OK = Desactivar");
      }else {
       StrW(Str2p, 0, "Test Desactivado");
       StrW(Str3p, 0, "OK = Activar");
      }
      if(Var2>1)Var2=1;
      if(Var2<1)Var2=1;
      break;
    case 5:
       StrW(Str2p, 0, "Aplicar?");
       StrW(Str3p, 0, "OK = Iniciar");
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
  menuINTER(1);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void menuEXPO(){
  menuINTER(2);
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

///////////////////////////////////
//MENU de aplicacion
void Aplicando(){
  Lcd4_Clear();
  Lcd4_Set_Cursor(1, 0);
  Lcd4_Write_String("Aplicando");
  intervalSecs = intervalos; //Setting interval time
  testMode = false;
  therapyTime = 0;
  switch(Modos){
    case 1:
      initGALV();
      break;
    case 2:
      //currentTENSf = frecuencia[Hz];
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
  int resttimeM=timeM;   //Variables volatiles que toman el valor del tiempo con X mins y Y segs
  int resttimeS=timeS;
  int secondsPrinted;
  int secdelayfinish=0;
  secondsPrinted=therapyTime;
  int secondsPrintedP=secondsPrinted;

  activateTimer1(true);

  while(1){
      if(secondsPrinted != secondsPrintedP){
        secondsPrintedP = secondsPrinted;
        if((PINB & 0x01) == 0x01){
          break;   //Cuando se detecte un pulso en la columna1 (que este solo puede provenir de RETURN por lo anterior), se finalizará
        }
        if(resttimeS==0){
          resttimeS=60;
          resttimeM--;
        }    //Cuando los segundos sean 0,  X-1 mins con 60 segundos
        
        resttimeS--;    //Se resta 1 segundo al tiempo total
        
        int sh = 0;
        
        if(resttimeM<10){
            StrW(Str3p, 0, "0");
            sh++;
        }
        sh += ValStr(resttimeM, sh, Str3p);
        StrW(Str3p, sh, ":");
        sh++;
        
        if(resttimeS<10){
            StrW(Str3p, sh, "0");
            sh++;
        }
        sh += ValStr(resttimeS, sh, Str3p);
        Lcd4_Set_Cursor(2, 0);
        Lcd4_Write_String(Str3p);
     }
    secondsPrinted=therapyTime;
    if((resttimeM==0 && resttimeS==0) ||((PINB & 0x01) == 0x01)) {
    	break;   //Cuando el tiempo llegue a 0:00 se detendra
    }
  }
 
  //Lcd4_Clear(); 
  Lcd4_Set_Cursor(1,0);    //Cuando se termine la cuenta del tiempo se imprimira "Finalizado" y esperara 2.5segs
  Lcd4_Write_String("Finalizado");
  resetSets();
  int j=1;
  int until;
  for(int i=0; i<2000; i++){
    _delay_ms(1);
    if(i == (4*j)){
      PORTD =(PIND | 0x10);
      j++;
      until = i + 1;
    }
    if(i == until){
      PORTD = (PIND & 0xEF);
    }
  }
  Mensaje=1; Variables=1; Modos=1; timeS=0; timeM=10; Hz=0; intervalos=5; toprint=true; testMode=false;/*Resetean todos los valores*/
  Var1=1; Var2=1; Var3=1;
}
