#include <avr/io.h>
#include "serial.h"

void serial_init(void)
{
  UBRR0H = 0;
  UBRR0L = 103;  //9600 baudios @ 16 MHz */
  UCSR0B = (1<<TXEN0) | (1<<RXEN0); 
}

int16_t serial_read(void)
{
  int16_t data;
  uint8_t tmp;

  if(UCSR0A & (1<<RXC0)) {
    if(!(UCSR0A&((1<<FE0)|(1<<DOR0)))) {
      data = UDR0;
    } else
      tmp = UDR0;
  } else
    data = -1;
  return data;
}

int8_t serial_write(uint8_t data)
{
  while(!(UCSR0A&0x20)); //Espero a que UDR0 este vacio */
  UDR0 = data;
  return 0;
}
