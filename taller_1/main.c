/******************************************************************************
* Taller 1 Sistemas Embebidos
* Departamento Ingenieria Biomedica, Universidad de Valparaiso
*
* Autor: Alejandro Weinstein
* Licencia: New BSD
* Descripcion:
* Parpadeo de un LED en Arduino Leonardo
*
******************************************************************************/
#include <avr/io.h>
#include <inttypes.h>
#include "blink.h"
#include "main.h"

/* Prototipo de funciones */
void init(void);

int main (void)
{
  init();
  while(1) {
    blink(100, 900);
  }
  return 0;
}

/******************************************************************************
* Nombre de la funcion	: void init(void)
*    returns            : void
* Creada por		: Alejandro Weinstein
* Fecha Creacion	: 2013-03-31
*
* Descripcion : Inicializa los perifericos
******************************************************************************/
void init(void)
{
  LED_DDR |= (1 << LED_PIN);
}

