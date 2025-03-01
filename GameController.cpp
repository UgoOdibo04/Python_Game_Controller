/**
* Name:  Ugochukwu Odibo
* Date:  8/02/2024
* Assignment: Lab 5 - Game Controller
* Youtube: https://youtu.be/OuUr5WEUH7E?feature=shared
*/

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

#define BUTTON_PDIR P1DIR
#define BUTTON_P1IN P1IN
#define BUTTON_POUT P1OUT
#define BUTTON_PREN P1REN
#define BUTTON_BIT BIT3

void GpioInit();
void ADCinit();
void UARTinit();
void writeUART(uint8_t data);
uint16_t readADC();
bool isbuttonPressed();
void updateController();

int main(void) {
    WDTCTL = WDTPW + WDTHOLD;
    UARTinit();
    ADCinit();
    GpioInit();
    while (1) {
        updateController();
        __delay_cycles(10000);
    }
}

void ADCinit(){
    //Initialize ADC
    ADC10CTL1 = INCH_4 ;                          
    ADC10CTL0 = SREF_0 | ADC10SHT_3 | REFON | ADC10ON;   
}

void GpioInit(){
    //Initialize Button
    BUTTON_PDIR &= ~BUTTON_BIT;                  
    BUTTON_PREN |= BUTTON_BIT;                  
    BUTTON_POUT |= BUTTON_BIT;                  
}

void UARTinit(){
    //Initialize UART
    P1SEL = BIT1 | BIT2;          
    P1SEL2 = BIT1 | BIT2;         

    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;
    UCA0CTL1 &= ~UCSWRST;
}

bool isbuttonPressed(){
    return (BUTTON_P1IN & BUTTON_BIT) == 0x00;
}

void writeUART(uint8_t data){
    while(!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = data;
}

uint16_t readADC(){
    ADC10CTL0 |= ENC | ADC10SC;             // Sampling and conversion start
    while(ADC10CTL1 & ADC10BUSY);           // Prevent Overwriting data
        return ADC10MEM;
}

void updateController(){
    uint16_t adcVal = readADC();
    uint8_t convertedVal = adcVal >> 2;

    if(convertedVal != 255){
        writeUART(convertedVal);
    }

    if(isbuttonPressed()){
        writeUART(255);
    }
}
