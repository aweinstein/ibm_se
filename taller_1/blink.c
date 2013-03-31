/******************************************************************************
* blink.c
* 
* Autor: Alejandro Weinstein
* Licencia: New BSD 
* Descripcion:
* Rutina para hacer parpadear un LED
*
******************************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "blink.h"

/******************************************************************************
* Nombre de la funcion	: void blink(uint16_t t_on, uint16_t t_off)
*    returns            : void
*    t_on               : tiempo de encendido en milisegundos
*    t_off              : tiempo de apagado en milisegundos
* Creada por		: Alejandro Weinstein
* Fecha Creacion	: 2013-03-31
*
* Descripcion : Enciende el LED por t_on milisegundos. Luego apaga el LED por
*     t_off milisegundos.
******************************************************************************/
void blink(uint16_t t_on, uint16_t t_off)
{
  uint16_t i;

  PORTC = 0xFF;
  for(i=0; i<t_on; i++) {
    _delay_ms(1);
  }
  PORTC = 0x00;
  for(i=0; i<t_off; i++) {
    _delay_ms(1);
  }
}
