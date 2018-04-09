
#include <avr/io.h>
#define F_CPU 8000000UL    // tan so thach anh 
#include <avr/interrupt.h>
#include "UART/USART.h"
// khai báo chân k?t n?i L298N
#define IN1   1			//PC1
#define IN2   2			//PC2
#define IN3	  3			//PC3
#define IN4   0			//PC0
// khai bao chân k?t n?i n?t nh?n
#define DATA_INFR		0			// PD5	



#define LUI		        PORTC = (PORTC & (0b11110000))|(0b00001010)		 //lui					
#define TIEN		    PORTC = (PORTC & (0b11110000))|(0b00000101)		// tien					

#define START_PWM		TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);\
						TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS10);\
						OCR1A = duty;\
						OCR1B = duty;\
						ICR1 = 50000;
	
#define STOP_PWM		TCCR1A = 0;\
						TCCR1B = 0;
#define BAUD1 9600
#define MYUBRR F_CPU/16/BAUD1-1
void RE_TRAI(void);
void RE_PHAI(void);
void DI_TIEN(void);
void DI_LUI(void);
void DUNG(void);

uint8_t tien=0;
unsigned char check_start(void);
unsigned long int infr_sensor(void);
void USART1_Init( unsigned int ubrr);
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
void SendString(char *StringPtr);
#include <util/delay.h>
uint8_t Start=0;
uint8_t Clockwise =0;
uint32_t duty=36000;   // m?i l?n t?ng + 5000
int32_t l=0;

char buff[30];
int main(void)
{
	DDRD= 0b00011111;	// khai báo chân (PD0->PD4 chân OUT)(PD5->PD7 chân in)
	PORTD= 0xFF;		// không dùng pull-Up
	DDRC=0xFF;			// port C chân OUT
	DDRB = 0b11111110;  // khai bao chan (PB0 Chân IN )(PB1->PB7 chan OUT)
	PORTB = 0b00000001;		//	
	TIEN;
	
	START_PWM;
	DUNG();
	USART1_Init(F_CPU/16/BAUD1-1);
	while(1)
	
	{
			l=infr_sensor();
			if(l!=0) 
			{
				uint32_t nguyen=l/100;
				uint32_t tphan=0;
				tphan=(uint32_t)l%100;
				if(l==32850)
				{
					RE_PHAI();
					SendString("PHAI\r\n");
					_delay_ms(500);
					DUNG();						
				}
				if(l==32883)
				{
					DI_TIEN();
					SendString("TIEN\r\n");
					
					_delay_ms(500);
					DUNG();
				}
				if(l==32858)
				{
					DI_LUI();
					SendString("LUI\r\n");
					
					_delay_ms(500);
					DUNG();
				}
				if(l==32887)
				{
					RE_TRAI();
					SendString("TRAI\r\n");
					
					_delay_ms(500);
					DUNG();
				}
				sprintf(buff,"gia tri : %d,%d\r\n",l-(32800),tphan);
				SendString(&buff);
				/*while(l)
				{
					//PORTD=0x0F;
					_delay_us(1);
					PORTD=0x00;
					_delay_us(1);
					l--;
				}*/
			}
			

		
	}
}

void RE_TRAI(void)
{
	OCR1A=27000;
	OCR1B=3500;
}
void RE_PHAI(void)
{
	OCR1A=3700;
	OCR1B=18000;
}
void DI_TIEN(void)
{
	TIEN;
	OCR1A=25000;
	OCR1B=15000;
}
void DI_LUI(void)
{
	LUI;
	OCR1A=25000;
	OCR1B=15000;
}
void DUNG(void)
{
	OCR1A=0;
	OCR1B=0;
}


unsigned char check_start(void)
{
	unsigned char i;
	if((PINB &(1<<DATA_INFR))==0)              // Neu co canh xuong cua tin hieu
	{
		_delay_us(500);          // Delay de chong nhieu
		if((PINB &(1<<DATA_INFR))==0)        // Neu dung la co canh xuong cua tin hieu
		{
			for(i=0;i<40;i++)  // Chia nho xung START thanh 40 doan, moi doan dai 200us
			{
				_delay_us(200);
				if((PINB &(1<<DATA_INFR))==1) return 0;    // Neu tin hieu len muc cao (1) --> khong phai xung START
			}
			while((PINB &(1<<DATA_INFR))==1);
			return 1;          // Dung la xung start
		}
		
	}
}

unsigned long int infr_sensor(void)
{
	unsigned char i;
	unsigned int Data_Infr;
	Data_Infr=0;
	if(check_start())                  // Neu dung xung START
	{
		while((PINB &(1<<DATA_INFR)) ==1);          // Cho het bit 1 tiep theo
		while((PINB &(1<<DATA_INFR)) ==0);          // Cho het bit 0 tiep theo
		for(i=0;i<24;i++)            // Lay 24 bit du lieu (bao gom 8 bit 0 va 8 bit 1 dau tien sau xung start)
		{
			//while(DATA_INFR==0);
			_delay_us(750);          // Delay 750us de kiem tra bit
			if((PINB &(1<<DATA_INFR)) ==0) Data_Infr=Data_Infr*2 + 1;
			if((PINB &(1<<DATA_INFR)) ==1) Data_Infr=Data_Infr*2;
			while((PINB &(1<<DATA_INFR)) ==1);    // cho canh len
			while((PINB &(1<<DATA_INFR)) ==0);    // cho canh len
		}
		_delay_ms(600);                // Cho het tin hieu (chong nhieu)
	}
	return Data_Infr;                  // Gia tri ma kenh tra ve
}

void USART1_Init( unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}
void USART_Transmit( unsigned char data )
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}
unsigned char USART_Receive( void )
{
	return UDR0;
}

void SendString(char *StringPtr)
{
	
	while(*StringPtr != 0x00)
	{
		USART_Transmit(*StringPtr);
		StringPtr++;
	}

}