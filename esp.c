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


void initLCD();
unsigned char BusyLCD();
void writeCommand(unsigned char); //unsigned char byte value (0-255)
void writeData(unsigned char);
void initADC();
unsigned int readFromADC(unsigned char);


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
    __delay_ms(17);
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

void initADC(){
    TRISA = 0x03; //AN0 y AN1 se pone en entrada
    //TRISAbits.RA0=1;
    //TRISAbits.RA1=1;
    TRISD = 0; //EL PUERTO D SE PONE EN SALIDA
    
    ADCON0 = 0x81;
    ADCON1 = 0;
    
    ADCON2bits.ACQT = 7; //YA QUE EL TAD ES MULTIPLICADO POR 2
    ADCON2bits.ADCS = 6; //ES EL NUMERO DE BINARO A DECIMAL DEL NUMERO QUE MULTIPLICA AL Tosc
    ADCON2bits.ADFM = 1; //JUSTIFICACION A LA DER.
    
    /ADCON0 = 0; //PARA TENER TODO POR AHORA DESHABILITADO*************6666
    ADCON1bits.PCFG = 0b1101; //TODOS LOS PUERTOS AN0 Y AN1 PONIENDO TODO EL PUERTO EN 0
    ADCON1bits.VCFG = 0; //ES PARA EL VOLTAJE DE REFENCIA
    ADCON2bits.ACQT = 7; //YA QUE EL TAD ES MULTIPLICADO POR 2
    ADCON2bits.ADCS = 6; //ES EL NUMERO DE BINARO A DECIMAL DEL NUMERO QUE MULTIPLICA AL Tosc
    ADCON2bits.ADFM = 0; //JUSTIFICACION A LA IZQ 
     
}

unsigned int readFromADC(unsigned char AN){
    ADCON0bits.CHS = AN;
    //unsigned char ADC_Result = 0;
    ADCON0bits.ADON = 1; //SE ACTIVA EL CONVERTIDOR
    //while(1){
    __delay_ms(5);
        ADCON0bits.GO_DONE = 1; //el minimo que se uso fue 2*TAD = 001 y da 1, por eso GO_DONE = 1;
        __delay_ms(5);
        while(ADCON0bits.GO_DONE == 1); //mientras que se este convirtiendo
        //YA QUE SE HAYA CONVERTIDO 
        /*ADIF = 0;
        //ADCON0bits.ADON = 0; //SE DESACTIVA EL CONVERTIDOR 
        ADC_Result = ADRESH; //VALOR 
        return ADC_Result;*/
        return ((ADRESH<<8)+ADRESL);
        //return (256*ADRESH) + ADRESL;
    //} 
}

    //////////////
    unsigned char rESP[20];
    int contador=0;

    ///////////////  USART  ///////////////
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


void __interrupt() isrReceptor(void){
    if(RCIF==1){
        RCIF=0;
        rESP[contador]=RCREG;
        contador++;       
    }
    return;
}

//////////AT COMMANDS//////////////////
void started_PIC_ESP(){
    unsigned char started[]="AT\r\n"; //D0A0
    sendString(started);
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
void send_TEMP_ESP(unsigned char temperature[]){
    unsigned char comando[]="AT+CIPSEND=99\r\n"; //98
    unsigned char link[]="GET /updateTemperatura.php?temperatura=";
    unsigned char link1[]=" HTTP/1.1\r\nHost: localhost\r\nCache-Control: no-cache\r\n\r\n"; //101 link + 5 array
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
    __delay_ms(2000);
    sendString(link);
    sendString(temperature);
    sendString(link1);
    
}
void send_FOTORRESISTOR_ESP(unsigned char daytime){ //1 Dia 0 Noche
    unsigned char comando[]="AT+CIPSEND=97\r\n"; //97
    unsigned char link[]="GET /updateFotoresistor.php?fotoresistor=";
    unsigned char link1[]=" HTTP/1.1\r\nHost: localhost\r\nCache-Control: no-cache\r\n\r\n";
    sendString(comando);
    __delay_ms(2000);
    __delay_ms(2000);
    sendString(link);
    sendChar(daytime);
    sendString(link1);
}
void host_ESP(){
    unsigned char tcp[]="TCP";
    unsigned char server[]="172.20.10.6";
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


unsigned char value;
float voltage;
unsigned char dataSensor[6];
unsigned int value2;
float voltage2;
unsigned char dataSensor2[6];

void main(void) {
    
    configPIC();
    initLCD(); 
    writeCommand(CLEAR_DISPLAY);
    __delay_us(4167);
     writeCommand(0b00111000); //0x38 hace lo mismo, 2 lineas
     __delay_us(4167);
    __delay_ms(2000);
    __delay_ms(2000);
    __delay_ms(2000);
    __delay_ms(2000);
    __delay_ms(2000);
    
    initADC();
    USARTinit();
    __delay_ms(2000);
    __delay_ms(2000);
    mode_ESP();
    __delay_ms(2000);
    __delay_ms(2000);
    nw_ESP();
    __delay_ms(2000);
    __delay_ms(2000);
    mux_ESP();
    __delay_ms(2000);
    __delay_ms(2000);
    cip_ESP();
    __delay_ms(2000);
    __delay_ms(2000);

    
    while(1){
        host_ESP();
    __delay_ms(2000);
        //TEMPERATURA
        //CAMBIOS DE ESTADO DEL LED, MOTOR Y BUZZER
        PORTCbits.RC0 = ~PORTCbits.RC0;
        PORTCbits.RC1 = ~PORTCbits.RC1; //MOTOR
        PORTCbits.RC2 = ~PORTCbits.RC2;
        
        unsigned int i=0;
        __delay_ms(5);
        writeData('T');
        __delay_ms(5);
        writeData('E');
        __delay_ms(5);
        writeData('M');
        __delay_ms(5);
        writeData('P');
        __delay_ms(5);
        writeData(':');
        value = readFromADC(0x00); //se leeran los datos de pin AN0
        voltage = ((value*4.5*100)/1023); //regla de 3 para poder 
        writeCommand(0xC0); 
        sprintf(dataSensor, "%.2f", voltage);
        i=0;
        send_TEMP_ESP(dataSensor);
        __delay_ms(1000);
        sendString("AT+CIPCLOSE\r\n");
        while(dataSensor[i]!='\0'){
        __delay_ms(30);
        writeData(dataSensor[i]);
        i++;
        }
        host_ESP();
        __delay_ms(1000);
        __delay_ms(1000);
        writeCommand(CLEAR_DISPLAY);
        
        //FOTORESISTOR
        //CAMBIOS DE ESTADO DEL LED, Y BUZZER
        PORTCbits.RC0 = ~PORTCbits.RC0;
        PORTCbits.RC2 = ~PORTCbits.RC2;
        __delay_ms(5);
        writeData('D');
        __delay_ms(5);
        writeData('/');
        __delay_ms(5);
        writeData('N');
        __delay_ms(5);
        writeData(':');
        host_ESP();
        __delay_ms(1000);
        value2 = readFromADC(0x01); //se leeran los datos del pin AN1 VALUE2 sirve de igual manera se establece como umbral
        voltage2 = ((value2)); 
        writeCommand(0xC0); 
        //sprintf(dataSensor2, "%.2f", voltage2);
        unsigned int i2=0; 
        while(dataSensor2[i2]!='\0'){
        __delay_ms(30);
        writeData(dataSensor2[i2]);
        i2++;
        }
        //host_ESP();
       // __delay_ms(1000);
        if(value2 < 500){
            __delay_ms(5);
            writeData('N');
            __delay_ms(5);
            writeData('O');
            __delay_ms(5);
            writeData('C');
            __delay_ms(5);
            writeData('H');
            __delay_ms(5);
            writeData('E');
            send_FOTORRESISTOR_ESP(0);
            sendString("AT+CIPCLOSE\r\n");
            __delay_ms(1000);
            host_ESP();
        }else{
            __delay_ms(5);
            writeData('D');
            __delay_ms(5);
            writeData('I');
            __delay_ms(5);
            writeData('A');
            send_FOTORRESISTOR_ESP(1);
            sendString("AT+CIPCLOSE\r\n");
            __delay_ms(1000);
            host_ESP();
            //__delay_ms(1000);
        }
        __delay_ms(1000);
        writeCommand(CLEAR_DISPLAY);
        
    } 
}
