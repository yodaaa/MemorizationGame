#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>

#define N 4
#define SW ((~PINC>>4)&3)
#define EEPADDR 0x000

typedef struct Pos {
	char m; //行
	char n; //列
} Pos;

Pos pos[5];

void init_rand(){
	srand(eeprom_read_word((uint16_t *) EEPADDR));
	eeprom_write_word((uint16_t *) EEPADDR, rand());
}

int main()
{
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	//PORTC = 0x30;
	
	TCCR1A = 0b00000000;
	TCCR1B = 0b00000100;
	unsigned int i, j;
	
	init_rand();
	//ランダムに５つの場所を生成
	for(i=0; i<5; i++){
		pos[i].m = rand() % 8 + 1;
		pos[i].n = rand() % 8 + 1;
		for(j=0; j<i; j++){
			while(pos[i].m == pos[j].m && pos[i].n == pos[j].n){
				pos[i].m = rand() % 8 + 1;
				pos[i].n = rand() % 8 + 1;		
			}
		}
	}
	//pos[0].m =8;
	//pos[0].n =5;
	//LED光らせる
	for(i=0; i<=5; i++){
		TCNT1 = 0;
		while(TCNT1 < 60000){
			wdt_reset();
		}
		switch(pos[i].m){
		case 1:	
			PORTC = 0x3f;
			PORTD = 0x7A;
			break;	
		case 2:
			PORTC = 0x3f;
			PORTD = 0xBA;
			break;	
		case 3:
			PORTC = 0x3f;
			PORTD = 0xDA;
			break;	
		case 4:
			PORTC = 0x3f;
			PORTD = 0xEA;
			break;	
		case 5:
			PORTC = 0x37;
			PORTD = 0xfA;
			break;	
		case 6:
			PORTC = 0x3B;
			PORTD = 0xfA;
			break;	
		case 7:
			PORTC = 0x3D;
			PORTD = 0xfA;
			break;	
		case 8:		
			PORTC = 0x3E;
			PORTD = 0xfA;
			break;		
					
		}
		switch(pos[i].n){
		case 1:
			PORTB = 0x80;
			break;
		case 2:
			PORTB = 0x40;
			break;
		case 3:
			PORTB = 0x20;
			break;
		case 4:
			PORTB = 0x10;
			break;
		case 5:
			PORTB = 0x08;
			break;
		case 6:
			PORTB = 0x04;
			break;
		case 7:
			PORTB = 0x02;
			break;
		case 8:			
			PORTB = 0x01;
			break;					
		}
	}
	TCCR1B = 0b00000000;
	PORTC = 0x3f; 
	PORTD = 0xfa;

	sei();
	for (;;) {
		wdt_reset();
	}
	return 0;
}
