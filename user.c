/*
	user.c 記憶ゲーム
	Created by 1447347 3EP2-54 依田真明
	Copyright(C) 2016 Masaaki Yoda @ Kanazawa.Inst.Tech
				All rights reserved.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>

#define N 4
#define SW ((~PINC>>4)&3)
#define EEPADDR 0x000
#define C_MAX 2000UL
#define SW_COUNT 30

unsigned char data;

typedef struct Pos {
	char m; //行
	char n; //列
} Pos;
Pos pos[5]; 

static char sPortC[5];
static char sPortD[5];
static char sPortB[5];


void init_rand(){
	srand(eeprom_read_word((uint16_t *) EEPADDR));
	eeprom_write_word((uint16_t *) EEPADDR, rand());
}



int main(void)
{
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	
	TCCR1A = 0b00000000;
	TCCR1B = 0b00000100;
	unsigned int i, j;
	int check = 0;

	unsigned int cnt;
	unsigned char pre_data;
	unsigned char tmp;
	unsigned char n;
	int y = 1;
	
	init_rand();
	//ランダムに５つの場所を生成
	for(i=0; i<5; i++){
		pos[i].m = rand() % 8 + 1;
		pos[i].n = rand() % 8 + 1;
		for(j=0; j<i; j++){
			while(pos[i].m == pos[j].m && pos[i].n == pos[j].n){ //同一の場所があった場合、再生成
				pos[i].m = rand() % 8 + 1;
				pos[i].n = rand() % 8 + 1;		
			}
		}
	}

	//LED光らせる
	for(i=0; i<=5; i++){
		TCNT1 = 0; //タイマ割り込み
		while(TCNT1 < 60000){
			wdt_reset();
		}
		switch(pos[i].m){
		case 1:	
			sPortC[i] = PORTC = 0x3f;
			sPortD[i] = PORTD = 0x7a;
			break;	
		case 2:
			sPortC[i] = PORTC = 0x3f;
			sPortD[i] = PORTD = 0xba;
			break;	
		case 3:
			sPortC[i] = PORTC = 0x3f;
			sPortD[i] = PORTD = 0xda;
			break;	
		case 4:
			sPortC[i] = PORTC = 0x3f;
			sPortD[i] = PORTD = 0xea;
			break;	
		case 5:
			sPortC[i] = PORTC = 0x37;
			sPortD[i] = PORTD = 0xfa;
			break;	
		case 6:
			sPortC[i] = PORTC = 0x3b;
			sPortD[i] = PORTD = 0xfa;
			break;	
		case 7:
			sPortC[i] = PORTC = 0x3d;
			sPortD[i] = PORTD = 0xfa;
			break;	
		case 8:		
			sPortC[i] = PORTC = 0x3e;
			sPortD[i] = PORTD = 0xfa;
			break;		
					
		}
		switch(pos[i].n){
		case 1:
			sPortB[i] = PORTB = 0x80;
			break;
		case 2:
			sPortB[i] = PORTB = 0x40;
			break;
		case 3:
			sPortB[i] = PORTB = 0x20;
			break;
		case 4:
			sPortB[i] = PORTB = 0x10;
			break;
		case 5:
			sPortB[i] = PORTB = 0x08;
			break;
		case 6:
			sPortB[i] = PORTB = 0x04;
			break;
		case 7:
			sPortB[i] = PORTB = 0x02;
			break;
		case 8:			
			sPortB[i] = PORTB = 0x01;
			break;					
		}
	}
	TCCR1B = 0b00000000;
	PORTC = 0x3f; 
	PORTD = 0xfa;

	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	PORTB = 0x01;
	PORTC = 0xfe;	/* SW1,2 をプルアップ  */
	PORTD = 0xff;

	pre_data = data = SW;

	sei(); //割り込み受付開始
	for (;;) {
		wdt_reset();

		if (C_MAX <= ++cnt) {
			cnt = 0;
		}

		/** 定期的にスイッチを読み取る **/
		if (cnt == 0) {
			tmp = SW;
			n = (data != tmp) ? n + 1 : 0;
			if (SW_COUNT < n) {
				data = tmp;	// スイッチが落ち着いたと判断
			}


			/** スイッチの状態が変化した時の処理 **/
			if (pre_data != data) {
				pre_data = data;	// スイッチ情報を更新
              
				switch (data) {
				case 0:
					break;
				case 1: //SW1
					if(PORTB==0x80){					
						PORTB = 0x01;
					}
					else
						PORTB=PORTB<<1;
					break;
				case 2: //SW2
					y++;
					if(y==9){
						PORTD = 0xfa;
						PORTC = 0x3e;
						y = 1;
					}

					else if(y < 5){
						PORTC = PORTC<<1;
						PORTC ^= 0x41;
					}

					else if(y == 5){
						PORTC = 0x3f;
						PORTD = 0xea;
					}

					else if(6 <= y <= 7){
						PORTD = PORTD<<1;
						PORTD ^= 0x0e;
					}
					
					else if(y == 8){
						PORTD = 0x7a;
					}
					break;
				case 3: //SW1 + SW2
					if( (PORTC == sPortC[check] && PORTD == sPortD[check]) && PORTB == sPortB[check] ){
						check++;
						PORTB = 0x01;
						PORTC = 0xfe;	/* SW1,2 をプルアップ  */
						PORTD = 0xff;
						y=1;
						if(check==5){ //全問正解のとき
							PORTC = 0x30;
							PORTD = 0x0a;
							PORTB = 0xff;
						}
					}else {

					}
					break;
				}
			}

		}

	}
	return 0;
}
