#define set_bit(adresse,bit) ((adresse)|=(0x1<<(bit)))
#define clear_bit(adresse,bit) ((adresse)&=(~(0x1<<(bit))))
#define toggle_bit(adresse,bit) ((adresse)^=(0x1<<(bit)))
#define test_bit(adresse,bit) ((adresse)=((adresse)>>(bit))&1)

#define PORTUS PORTB4
#define DDBUS DDB4
#define PINBUS PINB4

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "uart.c"

typedef enum {IN, LEFT, RIGHT, OUT} position;


void uart_putstring(char* str){
        int i = 0;
        while(str[i]!='\0'){
                uart_putchar(str[i],NULL);
                i++;
        }
        uart_putchar('\n',NULL);
}

void toggle_led(){
        toggle_bit(PORTB,PORTB5);	// Toggle led state
}

//ISR(PCINT0_vect){}

int pulse(){
	clear_bit(PORTB,PORTUS);	// Set 0 to ultrasonic output
	set_bit(DDRB,DDBUS);		// Set us pin as output
	_delay_us(2);
	set_bit(PORTB,PORTUS);		// Set us to High (impulse)
	_delay_us(10);
	clear_bit(PORTB,PORTUS);	// Set us to Low (end of impulse)

	clear_bit(DDRB,DDBUS);		// Set us pin as input
	while(!test_bit(PINB,PINBUS));	// Wait for impulse echo
	TCNT1H = 0;
	TCNT1L = 0;
	while( test_bit(PINB,PINBUS));	// Measure duration of echo
	int duration = ((TCNT1H<<8) | TCNT1L)*2;
	_delay_us(200);
	return duration;// Multiply by 4 to get useconds and divide by two for one trip
}

int main(void){
	DDRB   = (1<<DDB5);		// Set LED port as output
	sei();
	TCCR1B |= 0b11;

	uart_putstring("Boot ok");
	while(1){
		printf("Pulse length : %d us\n", pulse());
		_delay_ms(500);
	};
	return 0;
}
