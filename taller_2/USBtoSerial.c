/*
             LUFA Library
     Copyright (C) Dean Camera, 2013.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the USBtoSerial project. This file contains the main tasks of
 *  the project and is responsible for the initial application hardware configuration.
 */

#include "USBtoSerial.h"
#include "serial.h"

/** Circular buffer to hold data from the host before it is sent to the device via the serial port. */
static RingBuffer_t USBtoUSART_Buffer;

/** Underlying data buffer for \ref USBtoUSART_Buffer, where the stored bytes are located. */
static uint8_t      USBtoUSART_Buffer_Data[128];

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
  {
    .Config =
    {
      .ControlInterfaceNumber         = 0,
      .DataINEndpoint                 =
      {
	.Address                = CDC_TX_EPADDR,
	.Size                   = CDC_TXRX_EPSIZE,
	.Banks                  = 1,
      },
      .DataOUTEndpoint                =
      {
	.Address                = CDC_RX_EPADDR,
	.Size                   = CDC_TXRX_EPSIZE,
	.Banks                  = 1,
      },
      .NotificationEndpoint           =
      {
	.Address                = CDC_NOTIFICATION_EPADDR,
	.Size                   = CDC_NOTIFICATION_EPSIZE,
	.Banks                  = 1,
      },
    },
  };


/******************************************************************************
* Nombre de la funcion	: void serial_task(void)
*    returns            : void
* Creada por		: Alejandro Weinstein
* Fecha Creacion	: 2013-04-08
*
* Descripcion : Llama a las tareas USB de LUFA
******************************************************************************/
void serial_task(void)
{
  CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
  USB_USBTask();
  ;
}

/******************************************************************************
* Nombre de la funcion	: void serial_init(void)
*    returns            : void
* Creada por		: Alejandro Weinstein
* Fecha Creacion	: 2013-04-08
*
* Descripcion : Inicializa el purto serial virtual USB
******************************************************************************/
void serial_init(void)
{
  SetupHardware();

  RingBuffer_InitBuffer(&USBtoUSART_Buffer, USBtoUSART_Buffer_Data, 
			sizeof(USBtoUSART_Buffer_Data));
  
  LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
  GlobalInterruptEnable();
}

/******************************************************************************
* Nombre de la funcion	: int16_t serial_read(void)
*    int16_t            : Si se leyo un byte exitosamente, retorna el byte, si
*                         no, retorna -1
* Creada por		: Alejandro Weinstein
* Fecha Creacion	: 2013-04-08
*
* Descripcion : Lee un byte desde el puerto serial virtual
******************************************************************************/
int16_t serial_read(void)
{
  int16_t ReceivedByte;
  ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
	  
  /* Read bytes from the USB OUT endpoint */
  if (!(ReceivedByte < 0)) {
    return ReceivedByte;
  } else {
    return -1;
  }

}

/******************************************************************************
* Nombre de la funcion	: int8_t serial_write(uint8_t data)
*    returns int8_t     : Si se envio un byte exitosamente, retorna 0. Si hubo
*                         un error, retorna -1
*    data               : dato a enviar
* Creada por		: Alejandro Weinstein
* Fecha Creacion	: 2013-04-08
*
* Descripcion : Envia un byte por el puerto serial
******************************************************************************/
int8_t serial_write(uint8_t data)
{
  uint8_t err_code;
  err_code = CDC_Device_SendByte(&VirtualSerial_CDC_Interface, data);
  if ( err_code == ENDPOINT_READYWAIT_NoError) {
    return 0;
  } else {
    return 1;
  }
}


/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  /* Disable clock division */
  clock_prescale_set(clock_div_1);

  /* Hardware Initialization */
  LEDs_Init();
  USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
  LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
  bool ConfigSuccess = true;
  
  ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
  
  LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
  CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}


/** Event handler for the CDC Class driver Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
  uint8_t ConfigMask = 0;

  switch (CDCInterfaceInfo->State.LineEncoding.ParityType)
    {
    case CDC_PARITY_Odd:
      ConfigMask = ((1 << UPM11) | (1 << UPM10));
      break;
    case CDC_PARITY_Even:
      ConfigMask = (1 << UPM11);
      break;
    }
  
  if (CDCInterfaceInfo->State.LineEncoding.CharFormat == CDC_LINEENCODING_TwoStopBits)
    ConfigMask |= (1 << USBS1);
  
  switch (CDCInterfaceInfo->State.LineEncoding.DataBits)
    {
    case 6:
      ConfigMask |= (1 << UCSZ10);
      break;
    case 7:
      ConfigMask |= (1 << UCSZ11);
      break;
    case 8:
      ConfigMask |= ((1 << UCSZ11) | (1 << UCSZ10));
      break;
    }
  
  /* Must turn off USART before reconfiguring it, otherwise incorrect operation may occur */
  /* UCSR1B = 0; */
  /* UCSR1A = 0; */
  /* UCSR1C = 0; */
  
  /* Set the new baud rate before configuring the USART */
  /* UBRR1  = SERIAL_2X_UBBRVAL(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS); */
  
  /* Reconfigure the USART in double speed mode for a wider baud rate range at the expense of accuracy */
  /* UCSR1C = ConfigMask; */
  /* UCSR1A = (1 << U2X1); */
  /* UCSR1B = ((1 << RXCIE1) | (1 << TXEN1) | (1 << RXEN1)); */
}

