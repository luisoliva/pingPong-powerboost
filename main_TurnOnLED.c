/*
 * File:   main_TurnOnLED.c
 * Author: luisoliva
 *
 * Created on August 17, 2018, 4:06 PM
 */
// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
     // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = HSPLL_HS  // Oscillator Selection bits (HS oscillator, PLL enabled (HSPLL))
#pragma config FCMEN = OFF 
#pragma config BORV = 3// Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))

// CONFIG3H
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)

// CONFIG5L

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)


#include "config.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//bandera que se prende cuando el jugador realiza el saque 
int startGame=0;
int serve=0;
int count=0;
float speed=500;
int flagOnGame=0;
int i=0;
int breakon=0;
int loser=0;
int playerTurn=0;
int ofensor=0;
int disableInterrupt=1;
////LCD BUFFERS////
unsigned char P1buffer[]={0};
unsigned char P2buffer[]={0};
unsigned char P3buffer[]={0};
unsigned char P4buffer[]={0};
unsigned char VelBuffer[]={0};
unsigned char TimeBuffer[]={0};

int P1points=0;
int P2points=0;
int P3points=0;
int P4points=0;
int velocity=1;
int winner=0;
int addScore=0;
int endGame=0;


int tmr1Overflow;
int firstPulse=1; 
int powerBoost=0;
int timeCount=0;
//counter vars
int decNum=0;

unsigned char rESP[20];
int contador=0;

///LCD FUNCTIONS///
void initLCD();
unsigned char BusyLCD();
void writeCommand(unsigned char); //unsigned char byte value (0-255)
void writeData(unsigned char);
////INITIALIZE LCD////
void initLCD(){ 
	//Inicializar LED 8 bits
	//1
	LATD = LOW; //puertos de datos en bajo
	//2
	TRISD = OUTPUT; //puertos de datos salida
	//3
	//Señales de control como salida.
	RS = OUTPUT; //RS salida TRISRS
	RW = OUTPUT; //R/W salida TRISRW
    E = OUTPUT;
	//4
	//Señales de control en bajo.
	RS = LOW; //PORT
	RW = LOW; //PORT
    E = LOW; 
	//5
	//Espera al menos 15 ms para que se energice el LCD.
    __delay_ms(15);
	//Delay10KTCYx(18); //import delay.h
	//6
	//8-bit, 1-line, 5x7.
    writeCommand(eightbit_1line_5x7);
	//7
	//Espera 4.167 ms.
    __delay_us(4167);
	//Delay10KTCYx(5); //5x10k = 50,000
	//8
	//8-bit, 1-line, 5x7. 
    writeCommand(eightbit_1line_5x7);
	//9
	//Espera 0.8333 ms.
	__delay_us(833); //1x10k = 10,000
	//10
	//8-bit, 1-line, 5x7.
	writeCommand(eightbit_1line_5x7);
	//11
	//Esperar hasta que el LCD se desocupe.
	//Usamos la primera condición
	while(BusyLCD());
	//12
	//8-bit, 2-line, 5x7.
	writeCommand(eightbit_2lines_5x7);
	//13
	//Esperar hasta que el LCD se desocupe.
	while(BusyLCD());
	//14
	//display, cursor, blink OFF.
    writeCommand(display_cursor_blink_OFF);
	//15
	//Esperar hasta que el LCD se desocupe.
	while(BusyLCD());
	//16
	//display, cursor, blink ON.
    writeCommand(display_cursor_blink_ON);
	//17
	//Espera que LCD se desocupe.
	while(BusyLCD());
	//18
	//Limpiar pantalla.
    writeCommand(CLEAR_DISPLAY);
	//19
	//Espera que LCD se desocupe.
	while(BusyLCD());
	//20
	//Enviar al cursor al inicio.
    writeCommand(GO_HOME);
	//21
	//Espera que LCD se desocupe.
	while(BusyLCD());
}
unsigned char BusyLCD(){ 
	//Paso 1 Señal de control RW = LEER
	RW = LEER;
	//Paso 2 Señal de control RS = COMANDO
	RS = COMMAND;
	//Paso 3 Esperar 1.5 us
	__delay_us(2);
	//Paso 4 Señal de control EN = HABILITADO
	E = HABILITADO;
	//Paso 5 Esperar 1.5 us
	__delay_us(2);
	//Paso 6 Checar la bandera de ocupado":
    
    E=DESHABILITADO;
    RW=ESCRIBIR;
    
    if(LATD & 0x80){
        //LED = 1;
        return 1;
    }
    else
        return 0;
}
void writeCommand(unsigned char cmd){
	//Paso 1 Poner en el puerto de datos el valor del comando
	LATD = cmd;
	//Paso 2 Configurar el puerto de datos como salida
	TRISD = OUTPUT;
	//Paso 3 Señal de control RW = ESCRIBIR
	RW = ESCRIBIR;
	//Paso 4 Señal de control RS = COMANDO
	RS = COMMAND;
	//Paso 5 Esperar 1.5 us
	__delay_us(2);
	//Paso 6 Señal de control EN = HABILITADO
	E = HABILITADO;
	//Paso 7 Esperar 1.5 us
    __delay_us(2);
	//Paso 8 Señal de control EN = DESHABILITADO
	E = DESHABILITADO;
	//Paso 9 Esperar 1.5 us
    __delay_us(2);
	//Paso 10 Configurar el puerto de datos como entrada
    TRISD = 255;
}
void writeData(unsigned char data){
	//Paso 1 Poner en el puerto de datos el valor del comando
    LATD = data;
	//Paso 2 Configurar el puerto de datos como salida
    TRISD = OUTPUT;
	//Paso 3 Señal de control RW = ESCRIBIR
	RW = ESCRIBIR;
	//Paso 4 Señal de control RS = DATA
	RS = DATA;
	//Paso 5 Esperar 1.5 us
    __delay_us(2);
	//Paso 6 Señal de control EN = HABILITADO
	E = HABILITADO;
	//Paso 7 Esperar 1.5 us
    __delay_us(2);
	//Paso 8 Señal de control EN = DESHABILITADO
	E = DESHABILITADO;
	//Paso 9 Señal de control RS = COMANDO
	RS = COMMAND;
	//Paso 10 Configurar el puerto de datos como entrada
    TRISD = 255;
    
}
////SEND DATA TO LCD///////

void dataToLCD(unsigned char data[]){
    int k=0;
     while(data[k]!='\0'){
        __delay_ms(3);
        writeData(data[k]);
        k++;
    }
}
void LCDmarkerP1(){
    __delay_us(40); writeCommand(0x80);//1st line 
    for(int j=0;j<3;j++){ //3 o 4
    __delay_us(40); writeCommand(0x14);  
    }
    sprintf(P1buffer,"%d",P1points);
    __delay_us(40); dataToLCD(P1buffer);   
}

void LCDmarkerP1Space(){
    __delay_us(40); writeCommand(0x80);//1st line 
    for(int j=0;j<3;j++){ //3 o 4
    __delay_us(40); writeCommand(0x14);  
    }
    __delay_ms(2); writeData(' ');
      
}

void LCDmarkerP2Space(){
    __delay_us(40); writeCommand(0x80);//1st line 
    for(int j=0;j<7;j++){ //8 o 9
    __delay_us(40); writeCommand(0x14);  
    }
    __delay_ms(2); writeData(' ');
}

void LCDmarkerP2(){
    __delay_us(40); writeCommand(0x80);//1st line 
    for(int j=0;j<7;j++){ //8 o 9
    __delay_us(40); writeCommand(0x14);  
    }
    sprintf(P2buffer,"%d",P2points);
    __delay_us(40); dataToLCD(P2buffer);   
}

void LCDmarkerP3(){
    __delay_us(40); writeCommand(0x80);//1st line 
    for(int j=0;j<11;j++){ //8 o 9
    __delay_us(40); writeCommand(0x14);  
    }
    sprintf(P3buffer,"%d",P3points);
    __delay_us(40); dataToLCD(P3buffer);   
}

void LCDmarkerP4(){
    __delay_us(40); writeCommand(0x80);//1st line 
    for(int j=0;j<15;j++){ //8 o 9
    __delay_us(40); writeCommand(0x14);  
    }
    sprintf(P4buffer,"%d",P4points);
    __delay_us(40); dataToLCD(P4buffer);   
}

void LCD_Vel(){
    __delay_us(40); writeCommand(0xC0);//2nd line 
    for(int j=0;j<4;j++){
    __delay_us(40); writeCommand(0x14);  
    }
   sprintf(VelBuffer,"%d",velocity);
   __delay_us(40); dataToLCD(VelBuffer);   
}

void LCD_time(){
    __delay_us(40); writeCommand(0xC0);//2nd line 
    for(int j=0;j<7;j++){
    __delay_us(40); writeCommand(0x14);  
    }
   sprintf(TimeBuffer,"%d",timeCount);
   __delay_us(40); dataToLCD(TimeBuffer);   
}


void printBoost(){
    __delay_us(40); writeCommand(0xC0);//2nd line 
    for(int j=0;j<11;j++){
    __delay_us(40); writeCommand(0x14);  
    }
   
   __delay_us(40); dataToLCD("BOOST");   
}

void clearBoost(){
    __delay_us(40); writeCommand(0xC0);//2nd line 
    for(int j=0;j<11;j++){
    __delay_us(40); writeCommand(0x14);  
    }
   
   __delay_us(40); dataToLCD("     ");   
}

void gameInit(){
    unsigned char celebration[]="Welcome!";
    dataToLCD(celebration);
    __delay_ms(2000); writeCommand(CLEAR_DISPLAY);
    __delay_ms(2); writeCommand(0b00111000); //2 lines
    __delay_ms(2); writeCommand(0xC); //cursor off
}

void celebrationP1(){
    __delay_ms(100); writeCommand(CLEAR_DISPLAY);
     unsigned char celebration1[]="Luis Oliva won!!";
     dataToLCD(celebration1);
    __delay_us(40); writeCommand(0xC0); //2nd line,1st place
    unsigned char timeC[]="TIME: ";
     dataToLCD(timeC);
     LCD_time();
    __delay_ms(2); writeCommand(0xC); //cursor off
}

void celebrationP2(){
    __delay_ms(100); writeCommand(CLEAR_DISPLAY);
     unsigned char celebration2[]="William B won!!!";
    dataToLCD(celebration2);
    __delay_us(40); writeCommand(0xC0); //2nd line,1st place
     unsigned char timeC[]="TIME:";
     dataToLCD(timeC);
     LCD_time();
    __delay_ms(2); writeCommand(0xC); //cursor off
}

void celebrationP3(){
    __delay_ms(100); writeCommand(CLEAR_DISPLAY);
     unsigned char celebration3[]="Adriana S won!!!";
    dataToLCD(celebration3);
    __delay_us(40); writeCommand(0xC0); //2nd line,1st place
     unsigned char timeC[]="TIME:";
     dataToLCD(timeC);
     LCD_time();
    __delay_ms(2); writeCommand(0xC); //cursor off
}

void celebrationP4(){
    __delay_ms(100); writeCommand(CLEAR_DISPLAY);
     unsigned char celebration4[]="Carlos M won!!!!";
    dataToLCD(celebration4);
    __delay_us(40); writeCommand(0xC0); //2nd line,1st place
     unsigned char timeC[]="TIME:";
     dataToLCD(timeC);
     LCD_time();
    __delay_ms(2); writeCommand(0xC); //cursor off
}

void LCD_static(){
    unsigned char Vel[]="Vel: T:  ";
    unsigned char P[]="P1: P2: P3: P4: ";
    
    dataToLCD(P); //3, Marker at 4
    //__delay_us(40); writeCommand(0x14); //4
    //__delay_us(40); writeCommand(0x14); //5
    //dataToLCD(P2); //8, Marker at 9
    __delay_us(40); writeCommand(0xC0); //2nd line,1st place
    dataToLCD(Vel);//4, 
    //Vel at 5
}

void TMR1Config(){
    T1CONbits.TMR1CS=0; //Every pulse by the internal osc will increase TMR1 register
    // w/48Mhz it'll increase every 0.0833us
    T1CONbits.RD16=1;
    T1CONbits.T1OSCEN=0;
    T1CONbits.T1CKPS0=1; //Psc,11-8;10-4;01-2;00-1
    T1CONbits.T1CKPS1=1;
    T1CONbits.T1RUN=0;
    
    TMR1ON=1;
    
}

int randomTurn(){
    int n=TMR1;
    float number=n%4;
    int r=0;
    if(number==0){
        r=1;
        i=0;
    }
    
    if (number==1){
        r=2;
        i=7;
    }

    if (number==2){
        r=3;
        i=11;
    }    
    
    if (number==3){
        r=4;
        i=15;
    }
    
    return r;

}

void binaryCounter(int decNum){
    
    
    if (decNum==0){
        LED0=_OFF;
        LED1=_OFF;
        LED2=_OFF;
        LED3=_OFF;
    }
    
    if (decNum==1){
        LED0=_ON;
        LED1=_OFF;
        LED2=_OFF;
        LED3=_OFF;
    }
    
    if (decNum==2){
        LED0=_OFF;
        LED1=_ON;
        LED2=_OFF;
        LED3=_OFF;
    }
    
    if (decNum==3){
        LED0=_ON;
        LED1=_ON;
        LED2=_OFF;
        LED3=_OFF;
    }
    if (decNum==4){
        LED0=_OFF;
        LED1=_OFF;
        LED2=_ON;
        LED3=_OFF;
    }
    
    if (decNum==5){
        LED0=_ON;
        LED1=_OFF;
        LED2=_ON;
        LED3=_OFF;
    }
    
    if (decNum==6){
        LED0=_OFF;
        LED1=_ON;
        LED2=_ON;
        LED3=_OFF;
    }
    
    if (decNum==7){
        LED0=_ON;
        LED1=_ON;
        LED2=_ON;
        LED3=_OFF;
    }
    
    if (decNum==8){
        LED0=_OFF;
        LED1=_OFF;
        LED2=_OFF;
        LED3=_ON;
    }
    
    if (decNum==9){
        LED0=_ON;
        LED1=_OFF;
        LED2=_OFF;
        LED3=_ON;
    }
    
    if (decNum==10){
        LED0=_OFF;
        LED1=_ON;
        LED2=_OFF;
        LED3=_ON;
    }
    
    if (decNum==11){
        LED0=_ON;
        LED1=_ON;
        LED2=_OFF;
        LED3=_ON;
    }
    
    if (decNum==12){
        LED0=_OFF;
        LED1=_OFF;
        LED2=_ON;
        LED3=_ON;
    }
    
    if (decNum==13){
        LED0=_ON;
        LED1=_OFF;
        LED2=_ON;
        LED3=_ON;
    }
    
    if (decNum==14){
        LED0=_OFF;
        LED1=_ON;
        LED2=_ON;
        LED3=_ON;
    }
    
    if (decNum==15){
        LED0=_ON;
        LED1=_ON;
        LED2=_ON;
        LED3=_ON;
        //decNum=-1;
    }
}

void PrintDisplay(){
    LCD_static();
    LCD_Vel();
    LCDmarkerP1();
    LCDmarkerP2();
    LCDmarkerP3();
    LCDmarkerP4();
    LCD_time();
}

void wait_in_ms(unsigned int time_in_ms){
    count=0;
    double ms=(time_in_ms*0.001);
    while(count < (375*ms)){
        while (!T0IF);
        T0IF = 0;
        count++;
        TMR0L = 6;
    }

}

void __interrupt() isrPlayer1(void){
   if(INT0IF==1){
       if (disableInterrupt==0){
           //jugador 3 
            if(PORTCbits.RC0==1){
                //boton 2 del or fue presionado
                __delay_ms(100);
                breakon=1;
                
                if (i==11){
                    speed=speed*.50;
                    powerBoost=1;
                    printBoost();
 
                }else{
                    if (powerBoost==1){
                        speed=speed*2;
                        powerBoost=0;
                        clearBoost();
                    }
                    else{
                        speed=speed*0.75;
                        velocity=velocity+1;
                        LCD_Vel();
                    }
                }
                
                if(i<8 || i>11){
                    flagOnGame=0;
                    winner=ofensor;
                    addScore=1;
                    DISABLELEDS=_OFF;
                    startGame=1;
                    disableInterrupt=1;
                }


                //jugador 4
            }else{
                __delay_ms(100);
                breakon=1;
                
                if(i==15){
                    powerBoost=1;
                    speed=speed*.50;
                    printBoost();
                }else{
                    if (powerBoost==1){
                        speed=speed*2;
                        powerBoost=0;
                        clearBoost();
                    }
                    else{
                        speed=speed*0.75;
                        velocity=velocity+1;
                        LCD_Vel();
                    }
                }
                
               
                if(i<12){
                    flagOnGame=0;
                    winner=ofensor;
                    addScore=1;
                    DISABLELEDS=_OFF;
                    startGame=1;
                     disableInterrupt=1;
                }
            }
       }
       INT0IF=0;
   }
   
   if(INT1IF==1){
       if (disableInterrupt==0){
            //boton jugador 1
            __delay_ms(100);
            breakon=1;
            
            if (i==0){
                powerBoost=1;
                speed=speed*.5;
                printBoost();
            }else{
                if (powerBoost==1){
                    speed=speed*2;
                    powerBoost=0;
                    clearBoost();
                }
                else{
                    speed=speed*0.75;
                    velocity=velocity+1;
                    LCD_Vel();
                }
            }
            
           
               
            if(i>3){
                flagOnGame=0;
                winner=ofensor;
                addScore=1;
                DISABLELEDS=_OFF;
                startGame=1;
                disableInterrupt=1;
            }
       }
       INT1IF=0;
   }
   
   if(INT2IF==1){
       //jugador 2
       if (disableInterrupt==0){
            //boton independiente fue presionado
            __delay_ms(100);
            breakon=1;
            
            if (i==7){
                powerBoost=1;
                speed=speed*.5;
                printBoost();
            }else{
                if (powerBoost==1){
                    speed=speed*2;
                    powerBoost=0;
                    clearBoost();
                }
                else{
                    speed=speed*0.75;
                    velocity=velocity+1;
                    LCD_Vel();
                }
            }
               
            if(i<4 || i>7){
                flagOnGame=0;
                winner=ofensor;
                addScore=1;
                DISABLELEDS=_OFF;
                startGame=1;
                disableInterrupt=1;
            }
       }
       INT2IF=0;
   }
   
   if(TMR1IF==1){
       tmr1Overflow++;
       if(tmr1Overflow==23){
        tmr1Overflow=0;  
        if(endGame==0){
            timeCount=timeCount+1;
        }
       }
       TMR1IF=0;
    }
}

void USARTinit(void){
    //CONFIGURAR TXSTA, RCSTA, BAUDCON.
    LATCbits.LC7=1;//RX ENTRADA 
    LATCbits.LC6=0; //0 TX ES SALIDA 
    //USART EN ASINCRONO
    SPEN=1; 
    SYNC=0;
    TXEN=1; //HABILITA TRANSMISION
    TX9=0; 
    //////////////////////////////////
    SPBRG=25; //
    //////////////////////////////////
    BRG16=0; //8 BITS BAUD RATE GENERATOR
    BRGH=1; //0 BAJA, 1ALTA, VELOCIDAD BAUDIOS  
    TXIE=0; 
    /////////////////////////////
    CREN=1;
    RCIE=1; 
    RX9=0;
    TXIF=1;
    RCIF=0;   
}


void sendChar(unsigned char dato){ 
   while(TRMT==0); //Espero a que TSR ESTE VACIO
   TXREG=dato;  
}

void sendString(unsigned char string[]){
    unsigned int i=0;
    while(string[i]!='\0'){
        sendChar(string[i]);
        i++;
      __delay_ms(5);
    }
    contador=0;
}

void mode_ESP(){
    unsigned char mode[]="AT+CWMODE=1\r\n"; //cliente
    sendString(mode);
}
void nw_ESP(){
    unsigned char command[]="AT+CWJAP=";
    unsigned char name[]="iPhone de Luis";
    unsigned char pw[]="luigy123";
    unsigned char crlf[]="\r\n";
    sendString(command);
    sendChar('"'); sendString(name); sendChar('"');
    sendChar(',');
    sendChar('"'); sendString(pw); sendChar('"');
    sendString(crlf);
}
void mux_ESP(){
    unsigned char mode[]="AT+CIPMUX=0\r\n";
    sendString(mode);
}
void cip_ESP(){
    unsigned char mode[]="AT+CIPMODE=0\r\n";
    sendString(mode);
}
///AT SEND TEMP & DAYTIME/////////////////
void sendWinnerWilliam(){
    unsigned char comando[]="AT+CIPSEND=70\r\n"; //98
    unsigned char link3[]="GET /powerboost/getVariables.php?jugador=";
    unsigned char link4[]="&tiempo=";
    unsigned char link5[]="\r\n";
    unsigned char link[]="GET /powerboost/getVariables.php?jugador='william'&tiempo=";
    unsigned char link1[]="HTTP/1.1Host:localhostCache-Control:no-cache\r\n"; //101 link + 5 array
    /*unsigned char link2[]=" HTTP/1.1";
    unsigned char crlf[]="\r\n";
     unsigned char link3[]="Host: localhost";
     unsigned char link4[]="Cache-Control: no-cache";
    sendString(comando);
    __delay_ms(2000);
    __delay_ms(2000);
    sendString(link);
    sendString(temperature);
    sendString(link2);
    sendString(crlf); 
    sendString(link3);
    sendString(crlf);
    sendString(link3);
    sendString(crlf); sendString(crlf);*/
    sendString(comando);
    __delay_ms(2000);
    sendString(link);
    sendString(TimeBuffer);
    sendString(link5);
    //sendString(timeCount);
       __delay_ms(2000);
    __delay_ms(2000);   
    __delay_ms(2000);
    __delay_ms(2000);
    sendString(link1);
    
}

//inician modificaciones
void sendWinnerLuis(){
    unsigned char comando[]="AT+CIPSEND=70\r\n"; //98
    unsigned char link3[]="GET /powerboost/getVariables.php?jugador=";
    unsigned char link4[]="&tiempo=";
    unsigned char link5[]="\r\n";
    unsigned char link[]="GET /powerboost/getVariables.php?jugador='luis'&tiempo=";
    unsigned char link1[]="HTTP/1.1Host:localhostCache-Control:no-cache\r\n"; //101 link + 5 array
    /*unsigned char link2[]=" HTTP/1.1";
    unsigned char crlf[]="\r\n";
     unsigned char link3[]="Host: localhost";
     unsigned char link4[]="Cache-Control: no-cache";
    sendString(comando);
    __delay_ms(2000);
    __delay_ms(2000);
    sendString(link);
    sendString(temperature);
    sendString(link2);
    sendString(crlf); 
    sendString(link3);
    sendString(crlf);
    sendString(link3);
    sendString(crlf); sendString(crlf);*/
    sendString(comando);
    __delay_ms(2000);
    sendString(link);
    sendString(TimeBuffer);
    sendString(link5);
    //sendString(timeCount);
       __delay_ms(2000);
    __delay_ms(2000);   
    __delay_ms(2000);
    __delay_ms(2000);
    sendString(link1);
    
}

void sendWinnerAdriana(){
    unsigned char comando[]="AT+CIPSEND=70\r\n"; //98
    unsigned char link3[]="GET /powerboost/getVariables.php?jugador=";
    unsigned char link4[]="&tiempo=";
    unsigned char link5[]="\r\n";
    unsigned char link[]="GET /powerboost/getVariables.php?jugador='adriana'&tiempo=";
    unsigned char link1[]="HTTP/1.1Host:localhostCache-Control:no-cache\r\n"; //101 link + 5 array
    /*unsigned char link2[]=" HTTP/1.1";
    unsigned char crlf[]="\r\n";
     unsigned char link3[]="Host: localhost";
     unsigned char link4[]="Cache-Control: no-cache";
    sendString(comando);
    __delay_ms(2000);
    __delay_ms(2000);
    sendString(link);
    sendString(temperature);
    sendString(link2);
    sendString(crlf); 
    sendString(link3);
    sendString(crlf);
    sendString(link3);
    sendString(crlf); sendString(crlf);*/
    sendString(comando);
    __delay_ms(2000);
    sendString(link);
    sendString(TimeBuffer);
    sendString(link5);
    //sendString(timeCount);
       __delay_ms(2000);
    __delay_ms(2000);   
    __delay_ms(2000);
    __delay_ms(2000);
    sendString(link1);
    
}

void sendWinnerCarlos(){
        unsigned char comando[]="AT+CIPSEND=70\r\n"; //98
    unsigned char link3[]="GET /powerboost/getVariables.php?jugador=";
    unsigned char link4[]="&tiempo=";
    unsigned char link5[]="\r\n";
    unsigned char link[]="GET /powerboost/getVariables.php?jugador='carlos'&tiempo=";
    unsigned char link1[]="HTTP/1.1Host:localhostCache-Control:no-cache\r\n"; //101 link + 5 array
    /*unsigned char link2[]=" HTTP/1.1";
    unsigned char crlf[]="\r\n";
     unsigned char link3[]="Host: localhost";
     unsigned char link4[]="Cache-Control: no-cache";
    sendString(comando);
    __delay_ms(2000);
    __delay_ms(2000);
    sendString(link);
    sendString(temperature);
    sendString(link2);
    sendString(crlf); 
    sendString(link3);
    sendString(crlf);
    sendString(link3);
    sendString(crlf); sendString(crlf);*/
    sendString(comando);
    __delay_ms(2000);
    sendString(link);
    sendString(TimeBuffer);
    sendString(link5);
    //sendString(timeCount);
       __delay_ms(2000);
    __delay_ms(2000);   
    __delay_ms(2000);
    __delay_ms(2000);
    sendString(link1);
    
}
void publishTwitter(){ //1 Dia 0 Noche
    unsigned char comando[]="AT+CIPSEND=97\r\n"; //97
    unsigned char link[]="GET /updateFotoresistor.php?fotoresistor=";
    unsigned char link1[]=" HTTP/1.1\r\nHost: localhost\r\nCache-Control: no-cache\r\n\r\n";
    sendString(comando);
    __delay_ms(2000);
    __delay_ms(2000);
    sendString(link);
    sendString(link1);
}
void host_ESP(){
    unsigned char tcp[]="TCP";
    unsigned char server[]="172.20.10.7";
    unsigned char command[]="AT+CIPSTART=";
    unsigned char port[]="80\r\n";
    sendString(command);
    sendChar('"'); sendString(tcp); sendChar('"');
    sendChar(',');
    sendChar('"'); sendString(server); sendChar('"');
    sendChar(','); sendString(port);
}

void cleanBuffer(unsigned char var[]) {
    int i = 0;
    while(var[i] != '\0'){
        var[i] = '\0';
        i++;
    }
} //Remover datos de los buffers usados


void started_PIC_ESP(){
    unsigned char started[]="AT\r\n"; //D0A0
    sendString(started);
}
void main(void) {
    configPIC();
    TMR0L = 6;
    unsigned int count = 0;
    TMR0ON = 1;
    ///LCD///
    initLCD();
     __delay_us(4167);
    writeCommand(CLEAR_DISPLAY);
    __delay_us(4167);
    writeCommand(0b00111000); //2 lines
     __delay_us(4167);
    writeCommand(0xC);
    USARTinit();
    gameInit();
    LCD_static();
    LCDmarkerP1();
    LCDmarkerP2();
    LCDmarkerP3();
    LCDmarkerP4();
    LCD_Vel();
    TMR1Config();

    //variable en la cual se asigna el saque inicial del juego
    int firstServe=0;
    LEDCENTER=_OFF;
    DISABLELEDS=_OFF;
    GIE=1;
    RCIE=0;
    TXIE=0;
    
    
    while (1){
        if (firstPulse==0){
            LCD_time();
        }
        
        //asigna el saque al jugador 1 y aumenta su marcador
        if(winner==1 && addScore==1){
            P1points=P1points+1;
            LCDmarkerP1();
            addScore=0;
            velocity=1;
            LCD_Vel();
            clearBoost();
        }
        
        //asigna el sque al jugador 2 y aumenta su marcador
        if(winner==2 && addScore==1){
            P2points=P2points+1;
            LCDmarkerP2();
            addScore=0;
            velocity=1;
            LCD_Vel();
            clearBoost();
        }
        
        if(winner==3 && addScore==1){
            P3points=P3points+1;
            LCDmarkerP3();
            addScore=0;
            velocity=1;
            LCD_Vel();
            clearBoost();
        }
        
        if(winner==4 && addScore==1){
            P4points=P4points+1;
            LCDmarkerP4();
            addScore=0;
            velocity=1;
            LCD_Vel();
            clearBoost();
        }
        
           
        if (startGame==1){
            playerTurn=ofensor;
                if (playerTurn==1){
                    i=0;
                    binaryCounter(i);
                }
                if(playerTurn==2){
                    i=7;
                    binaryCounter(i);
                }
                if(playerTurn==3){
                    i=11;
                    binaryCounter(i);
                }
                if(playerTurn==4){
                    i=15;
                    binaryCounter(i);
                }
            DISABLELEDS=_ON;
            startGame=0;
        }
        
        if (firstPulse==1){
            if (PORTBbits.RB1==1 || PORTBbits.RB0==1){
            //asigna el saque aleatorio
                timeCount=0;
                LCD_time();
                wait_in_ms(250);
                playerTurn=randomTurn();
                if (playerTurn==1){
                    binaryCounter(i);
                }
                if(playerTurn==2){
                    binaryCounter(i);
                }
                if(playerTurn==3){
                    binaryCounter(i);
                }
                if(playerTurn==4){
                    binaryCounter(i);
                }
                //se apaga la bandera una vez que ya se asigno el saque
                firstPulse=0; 
                P1points=0;
                P2points=0;
                velocity=1;
                LCD_Vel();
                LCD_time();
                DISABLELEDS=_ON;
            }
        }
       
        
        if (PORTBbits.RB1==1 && playerTurn==1){
                    //badnera que inicia el movimiento de los LEDS
                    wait_in_ms(250);
                    flagOnGame=1;
                    velocity=1;
                    addScore=1;
                    speed=500;
                    //bandera de error de jugador se desactiva cuando se reanuda el juego
                    breakon=0;
                    winner=0;
                    serve=1;
                    powerBoost=0;
                    //if para poder hacer el set up inicial para el saque aleatorio
                    
        }
       
        if (PORTBbits.RB2==1 && playerTurn==2){
                    //badnera que inicia el movimiento de los LEDS
                    wait_in_ms(250);
                    flagOnGame=1;
                    velocity=1;
                    addScore=1;
                    speed=500;
                    //bandera de error de jugador se desactiva cuando se reanuda el juego
                    breakon=0;
                    winner=0;
                    serve=1;
                    powerBoost=0;
                    //if para poder hacer el set up inicial para el saque aleatorio
                    
        }
                
        
        if (PORTBbits.RB0==1 && PORTCbits.RC0==1 && playerTurn==3){
                    //badnera que inicia el movimiento de los LEDS
                    wait_in_ms(250);
                    flagOnGame=1;
                    velocity=1;
                    addScore=1;
                    speed=500;
                    //bandera de error de jugador se desactiva cuando se reanuda el juego
                    breakon=0;
                    winner=0;
                    serve=1;
                    powerBoost=0;
                    //if para poder hacer el set up inicial para el saque aleatorio
                    
        }
               
        
        if (PORTBbits.RB0==1 && PORTCbits.RC0==0 && playerTurn==4){
                    //badnera que inicia el movimiento de los LEDS
                    wait_in_ms(250);
                    flagOnGame=1;
                    velocity=1;
                    addScore=1;
                    speed=500;
                    //bandera de error de jugador se desactiva cuando se reanuda el juego
                    breakon=0;
                    winner=0;
                    serve=1;
                    powerBoost=0;   
                    //if para poder hacer el set up inicial para el saque aleatorio
                    
        }
        
        if (serve==1 && flagOnGame==1){
            disableInterrupt=0;
            LEDCENTER=_OFF;
            if (playerTurn==1 && flagOnGame==1){
                ofensor=1;
                for(i=i;i<4;i++){
                    binaryCounter(i);
                    wait_in_ms(speed);
                    
                    if(breakon==1){
                    //breakon=0;
                    disableInterrupt=1;
                    break; 
                    }
                }
            }
            
            if (playerTurn==2 && flagOnGame==1){
                ofensor=2;
                for(i=i;i>3;i--){
                    binaryCounter(i);
                    wait_in_ms(speed);
                    
                    if(breakon==1){
                    //breakon=0;
                    
                    disableInterrupt=1;                   
                    break; 
                    }
                }
            }
            
            if (playerTurn==3 && flagOnGame==1){
                ofensor=3;
                for (i=i; i>7;i--){
                    binaryCounter(i);
                    wait_in_ms(speed);

                    if(breakon==1){
                       //breakon=0;
                       disableInterrupt=1;
                       break; 
                    }
                }
            }
            
            if (playerTurn==4 && flagOnGame==1){
                ofensor=4;
                for (i=i; i>11;i--){
                    binaryCounter(i);
                    wait_in_ms(speed);

                    if(breakon==1){
                       //breakon=0;
                       disableInterrupt=1;
                       break; 
                    }
                }
            }
            
            serve=0;
            playerTurn=randomTurn();
            if (breakon==1){
                LEDCENTER=_OFF;
            }
            else{
                LEDCENTER=_ON;
            }
            DISABLELEDS=_OFF;
            wait_in_ms(speed);
            LEDCENTER=_OFF;
        }
        
        if (serve==0 && flagOnGame==1){
            if (playerTurn==1 && flagOnGame==1){
                i=3;
                for (i=i; i>-2;i--){
                    if(i==-1){
                        DISABLELEDS=_OFF;
                        startGame=1;
                        flagOnGame=0;
                        breakon=1;
                        winner=ofensor;
                        disableInterrupt=1;
                    }
                   
                    if(i!=-1){
                        binaryCounter(i);
                         DISABLELEDS=_ON;
                        wait_in_ms(speed);
                    }


                     if(breakon==1){
                       breakon=0;
                       break; 
                    }
                    
                }

                if (flagOnGame==1){
                    for (i=i; i<4;i++){
                        binaryCounter(i);
                        wait_in_ms(speed);


                         if(breakon==1){
                           breakon=0;
                           break; 
                        }
                    } 
                    ofensor=1;
                    LEDCENTER=_ON;
                    DISABLELEDS=_OFF;
                    wait_in_ms(speed);
                    LEDCENTER=_OFF;
                }
            }


            if (playerTurn==2 & flagOnGame==1){
                i=4;
                for (i=i;i<9;i++){
                    if(i==8){
                        DISABLELEDS=_OFF;
                        startGame=1;
                        flagOnGame=0;
                        breakon=1;
                        winner=ofensor;
                        disableInterrupt=1;
                    }
                   
                    
                    if(i!=8){
                        binaryCounter(i);
                        DISABLELEDS=_ON;
                        wait_in_ms(speed);
                    }
                    
                    if (breakon==1){
                        breakon=0;
                        break;
                    }
                }
                
                //i=7;
                if(flagOnGame==1){
                    for (i=i; i>3;i--){
                        binaryCounter(i);
                        wait_in_ms(speed);

                         if(breakon==1){
                           breakon=0;
                           break; 
                        }
                    }
                    ofensor=2;
                    LEDCENTER=_ON;
                    DISABLELEDS=_OFF;
                    wait_in_ms(speed);
                    LEDCENTER=_OFF;
                }
            }


            if (playerTurn==3 && flagOnGame==1){
                i=8;
                for (i=i; i<13;i++){
                    if(i==12){
                        DISABLELEDS=_OFF;
                        startGame=1;
                        flagOnGame=0;
                        breakon=1;
                        winner=ofensor;
                        disableInterrupt=1;
                    }
                   
                    if (i!=12){
                        binaryCounter(i);
                        DISABLELEDS=_ON;
                        wait_in_ms(speed);

                    }
                    
                    if(breakon==1){
                       breakon=0;
                       break; 
                    }
                }
                
                if (flagOnGame==1){
                    //i=11;
                    for (i=i; i>7;i--){
                        binaryCounter(i);
                        wait_in_ms(speed);


                         if(breakon==1){
                           breakon=0;
                           break; 
                        }
                    }
                    ofensor=3;
                    LEDCENTER=_ON;
                    DISABLELEDS=_OFF;
                    wait_in_ms(speed);
                    LEDCENTER=_OFF;

                }
            }

            if (playerTurn==4 && flagOnGame==1){
                i=12;
                for (i=i; i<17;i++){   
                    if(i==16){
                        DISABLELEDS=_OFF;
                        startGame=1;
                        flagOnGame=0;
                        breakon=1;
                        winner=ofensor;
                        disableInterrupt=1;
                    }
                   
                    if(i!=16){
                        binaryCounter(i);
                        DISABLELEDS=_ON;
                        wait_in_ms(speed);

                    }

                     if(breakon==1){
                       breakon=0;
                       break; 
                    } 
                }
                
                if (flagOnGame==1){
                    //i=15;
                    for (i=i; i>11;i--){
                        binaryCounter(i);
                        wait_in_ms(speed);


                         if(breakon==1){
                           breakon=0;
                           break; 
                        } 
                    }
                    ofensor=4;
                    LEDCENTER=_ON;
                    DISABLELEDS=_OFF;
                    wait_in_ms(speed);
                    DISABLELEDS=_ON;
                    LEDCENTER=_OFF;
                }
            }
            
            playerTurn=randomTurn();
        }
        
        if (P1points>4){
            endGame=1;
            GIE=0;
            DISABLELEDS=_OFF;
            for (int ii=0;ii<3;ii++){
                __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                wait_in_ms(250);
                PrintDisplay();
                wait_in_ms(250);
                __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                wait_in_ms(250);
                PrintDisplay();
                wait_in_ms(250);
            }
            
            __delay_ms(2); writeCommand(CLEAR_DISPLAY);
            celebrationP1();
            wait_in_ms(2500);
            wait_in_ms(2500);
            //codigo del esp
             started_PIC_ESP();
                wait_in_ms(1000);
                mode_ESP();
                wait_in_ms(1000);
        mux_ESP();
        wait_in_ms(1000);
        __delay_ms(500);
                    host_ESP();
                    __delay_ms(2000);
                     __delay_ms(2000);
                    sendWinnerLuis();
                    RESET();
       
            
        }else{
            if(P2points>4){
                DISABLELEDS=_OFF;
                for (int ii=0;ii<3;ii++){
                    __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                    wait_in_ms(250);
                    PrintDisplay();
                    wait_in_ms(250);
                    __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                    wait_in_ms(250);
                    PrintDisplay();
                    wait_in_ms(250);
                }
           
                __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                celebrationP2();
                wait_in_ms(2500);
                wait_in_ms(2500);
                //codigo del esp
                 started_PIC_ESP();
                wait_in_ms(1000);
                mode_ESP();
                wait_in_ms(1000);
        mux_ESP();
        wait_in_ms(1000);
        __delay_ms(500);
                    host_ESP();
                    __delay_ms(2000);
                     __delay_ms(2000);
                    sendWinnerWilliam();
                    RESET();
       
            
            }else{
                if(P3points>4){
                    for (int ii=0;ii<3;ii++){
                        __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                        wait_in_ms(250);
                        PrintDisplay();
                        wait_in_ms(250);
                        __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                        wait_in_ms(250);
                        PrintDisplay();
                        wait_in_ms(250);
                    }

                    __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                    celebrationP3();
                    __delay_ms(2500);
                    __delay_ms(2500);
                    //codigo del esp
                      started_PIC_ESP();
                wait_in_ms(1000);
                mode_ESP();
                wait_in_ms(1000);
        mux_ESP();
        wait_in_ms(1000);
        __delay_ms(500);
                    host_ESP();
                    __delay_ms(2000);
                     __delay_ms(2000);
                    sendWinnerAdriana();
                    RESET();
       

                }else{
                    if (P4points>4){
                        for (int ii=0;ii<3;ii++){
                            __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                            wait_in_ms(250);
                            PrintDisplay();
                            wait_in_ms(250);
                            __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                            wait_in_ms(250);
                            PrintDisplay();
                            wait_in_ms(250);
                        }

                    __delay_ms(2); writeCommand(CLEAR_DISPLAY);
                    celebrationP4();
                    __delay_ms(2500);
                    __delay_ms(2500);
                    //codigo del esp
                      started_PIC_ESP();
                wait_in_ms(1000);
                mode_ESP();
                wait_in_ms(1000);
        mux_ESP();
        wait_in_ms(1000);
        __delay_ms(500);
                    host_ESP();
                    __delay_ms(2000);
                     __delay_ms(2000);
                    sendWinnerCarlos();
                    RESET();
                    }           
                }
            }
        }    
    }
}
