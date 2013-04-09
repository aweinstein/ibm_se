/******************************************************************************
* Taller 2 Sistemas Embebidos
* Departamento Ingenieria Biomedica, Universidad de Valparaiso
*
* Autor: Alejandro Weinstein
* Licencia: New BSD
* Descripcion:
* Ejemplo de comunicacion serial
*
******************************************************************************/
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "serial.h"

int main(void)
{
  int16_t data_in;

  serial_init();
  while(1) {
    data_in = serial_read();
    if(data_in >= 0) {
      serial_write('O');
      serial_write('K');
      serial_write('\r');
      serial_write('\n');
      serial_write((uint8_t)data_in);
      serial_write('\r');
      serial_write('\n');
      }
  }

  return 0;
}
