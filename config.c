#include "config.h"

void configPIC(void)
{
    TRISA = OUTPUT;
    TRISB = OUTPUT;
    TRISC = OUTPUT;
    TRISD = OUTPUT;
    TRISE = OUTPUT;
    
    LED_TRIS = OUTPUT;
    
    //TMR0 config
    T0PS0 = 0;
    T0PS1 = 1;
    T0PS2 = 1;
    PSA = 0;
    T0CS = 0;
    T08BIT = 1;
    
    ADCON1=0xFF;
    
    GIE = _ON;
    
    TMR0IE=0;
    
    IPEN=1; //Enable Priority Interrupts
    PEIE=1; //Enable low priority peripheral  interrupts
    INTEDG0 = 1; //Interrupt on rising edge of int pin
    //Player 2 
    INT0IE = 1; //Enable external interrupt
    INT1IE = 1; //Enable external interrupt
    INTEDG1 = 1; //Interrupt on rising edge of int pin
    INT1IP=1; //high priority
    //Velocity
    INT2IE = 1; //Enable external interrupt
    INTEDG2 = 1; //Interrupt on rising edge of int pin
    //INT2IP=0; //low priority
    //TMR1
    TMR1IE=1;
    TMR1IP=1;
    
    
    //T0CON = ObO1000110;
    
    //configurar pines
    
    TRISBbits.RB0=1;
    TRISBbits.RB1=1;
    TRISBbits.RB2=1;
    TRISCbits.RC0=1;
    TRISAbits.RA0=0;
    TRISAbits.RA1=0;
    TRISAbits.RA2=0;
    TRISEbits.RE0=0;
    TRISEbits.RE1=0;
    TRISEbits.RE2=0;
    TRISCbits.RC1=0;
    LED1=_OFF;
    LED2=_OFF;
    LED3=_OFF;
    LEDCENTER=_OFF;
    DISABLELEDS=_OFF;
    
    
}
