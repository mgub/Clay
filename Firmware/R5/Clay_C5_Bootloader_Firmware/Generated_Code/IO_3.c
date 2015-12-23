/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : IO_3.c
**     Project     : Clay_C5_Bootloader_Firmware
**     Processor   : MK20DX256VLH7
**     Component   : BitIO_LDD
**     Version     : Component 01.033, Driver 01.03, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-12-22, 22:37, # CodeGen: 10
**     Abstract    :
**         The HAL BitIO component provides a low level API for unified
**         access to general purpose digital input/output pins across
**         various device designs.
**
**         RTOS drivers using HAL BitIO API are simpler and more
**         portable to various microprocessors.
**     Settings    :
**          Component name                                 : IO_3
**          Pin for I/O                                    : ADC1_SE5a/PTE1/LLWU_P0/UART1_RX/I2C1_SCL
**          Pin signal                                     : 
**          Direction                                      : Input/Output
**          Initialization                                 : 
**            Init. direction                              : Output
**            Init. value                                  : 0
**            Auto initialization                          : no
**          Safe mode                                      : no
**     Contents    :
**         Init     - LDD_TDeviceData* IO_3_Init(LDD_TUserData *UserDataPtr);
**         SetDir   - void IO_3_SetDir(LDD_TDeviceData *DeviceDataPtr, bool Dir);
**         SetInput - void IO_3_SetInput(LDD_TDeviceData *DeviceDataPtr);
**         GetVal   - bool IO_3_GetVal(LDD_TDeviceData *DeviceDataPtr);
**         PutVal   - void IO_3_PutVal(LDD_TDeviceData *DeviceDataPtr, bool Val);
**         ClrVal   - void IO_3_ClrVal(LDD_TDeviceData *DeviceDataPtr);
**         SetVal   - void IO_3_SetVal(LDD_TDeviceData *DeviceDataPtr);
**
**     Copyright : 1997 - 2014 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file IO_3.c
** @version 01.03
** @brief
**         The HAL BitIO component provides a low level API for unified
**         access to general purpose digital input/output pins across
**         various device designs.
**
**         RTOS drivers using HAL BitIO API are simpler and more
**         portable to various microprocessors.
*/         
/*!
**  @addtogroup IO_3_module IO_3 module documentation
**  @{
*/         

/* MODULE IO_3. */

/* {Default RTOS Adapter} No RTOS includes */
#include "IO_3.h"

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct {
  LDD_TUserData *UserDataPtr;          /* Pointer to user data */
} IO_3_TDeviceData;                    /* Device data structure type */

typedef IO_3_TDeviceData *IO_3_TDeviceDataPtr ; /* Pointer to the device data structure. */

/* {Default RTOS Adapter} Static object used for simulation of dynamic driver memory allocation */
static IO_3_TDeviceData DeviceDataPrv__DEFAULT_RTOS_ALLOC;
/*
** ===================================================================
**     Method      :  IO_3_Init (component BitIO_LDD)
*/
/*!
**     @brief
**         Initializes the device. Allocates memory for the device data
**         structure, allocates interrupt vectors and sets interrupt
**         priority, sets pin routing, sets timing, etc. If the "Enable
**         in init. code" is set to "yes" value then the device is also
**         enabled(see the description of the Enable() method). In this
**         case the Enable() method is not necessary and needn't to be
**         generated. 
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer will be
**                           passed as an event or callback parameter.
**     @return
**                         - Pointer to the dynamically allocated private
**                           structure or NULL if there was an error.
*/
/* ===================================================================*/
LDD_TDeviceData* IO_3_Init(LDD_TUserData *UserDataPtr)
{
  /* Allocate device structure */
  IO_3_TDeviceDataPtr DeviceDataPrv;

  /* {Default RTOS Adapter} Driver memory allocation: Dynamic allocation is simulated by a pointer to the static object */
  DeviceDataPrv = &DeviceDataPrv__DEFAULT_RTOS_ALLOC;
  DeviceDataPrv->UserDataPtr = UserDataPtr; /* Store the RTOS device structure */
  /* Configure pin as output */
  /* GPIOE_PDDR: PDD|=2 */
  GPIOE_PDDR |= GPIO_PDDR_PDD(0x02);
  /* Set initialization value */
  /* GPIOE_PDOR: PDO&=~2 */
  GPIOE_PDOR &= (uint32_t)~(uint32_t)(GPIO_PDOR_PDO(0x02));
  /* Initialization of Port Control register */
  /* PORTE_PCR1: ISF=0,MUX=1 */
  PORTE_PCR1 = (uint32_t)((PORTE_PCR1 & (uint32_t)~(uint32_t)(
                PORT_PCR_ISF_MASK |
                PORT_PCR_MUX(0x06)
               )) | (uint32_t)(
                PORT_PCR_MUX(0x01)
               ));
  /* Registration of the device structure */
  PE_LDD_RegisterDeviceStructure(PE_LDD_COMPONENT_IO_3_ID,DeviceDataPrv);
  return ((LDD_TDeviceData *)DeviceDataPrv);
}
/*
** ===================================================================
**     Method      :  IO_3_SetDir (component BitIO_LDD)
*/
/*!
**     @brief
**         Sets a pin direction (available only if the direction =
**         _[input/output]_).
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by <Init> method.
**     @param
**         Dir             - Direction to set. Possible values:
**                           <false> - Input
**                           <true> - Output
*/
/* ===================================================================*/
void IO_3_SetDir(LDD_TDeviceData *DeviceDataPtr, bool Dir)
{
  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  if (Dir) {
    /* Output */
    GPIO_PDD_SetPortOutputDirectionMask(IO_3_MODULE_BASE_ADDRESS, IO_3_PORT_MASK);
  } else {
    /* Input */
    GPIO_PDD_SetPortInputDirectionMask(IO_3_MODULE_BASE_ADDRESS, IO_3_PORT_MASK);
  }
}

/*
** ===================================================================
**     Method      :  IO_3_SetInput (component BitIO_LDD)
*/
/*!
**     @brief
**         Sets a pin direction to input (available only if the
**         direction = _[input/output]_).
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by <Init> method.
*/
/* ===================================================================*/
void IO_3_SetInput(LDD_TDeviceData *DeviceDataPtr)
{
  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  GPIO_PDD_SetPortInputDirectionMask(IO_3_MODULE_BASE_ADDRESS, IO_3_PORT_MASK);
}

/*
** ===================================================================
**     Method      :  IO_3_GetVal (component BitIO_LDD)
*/
/*!
**     @brief
**         Returns the input/output value. If the direction is [input]
**         then the input value of the pin is read and returned. If the
**         direction is [output] then the last written value is read
**         and returned (see <Safe mode> property for limitations).
**         This method cannot be disabled if direction is [input].
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by <Init> method.
**     @return
**                         - Input or output value. Possible values:
**                           <false> - logical "0" (Low level)
**                           <true> - logical "1" (High level)
*/
/* ===================================================================*/
bool IO_3_GetVal(LDD_TDeviceData *DeviceDataPtr)
{
  uint32_t PortData;                   /* Port data masked according to the bit used */

  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  if ((GPIO_PDD_GetPortDirection(IO_3_MODULE_BASE_ADDRESS) & IO_3_PORT_MASK) == 0U) {
    /* Port is configured as input */
    PortData = GPIO_PDD_GetPortDataInput(IO_3_MODULE_BASE_ADDRESS) & IO_3_PORT_MASK;
  } else {
    /* Port is configured as output */
    PortData = GPIO_PDD_GetPortDataOutput(IO_3_MODULE_BASE_ADDRESS) & IO_3_PORT_MASK;
  }
  return (PortData != 0U) ? (bool)TRUE : (bool)FALSE;
}

/*
** ===================================================================
**     Method      :  IO_3_PutVal (component BitIO_LDD)
*/
/*!
**     @brief
**         The specified output value is set. If the direction is <b>
**         input</b>, the component saves the value to a memory or a
**         register and this value will be written to the pin after
**         switching to the output mode (using <tt>SetDir(TRUE)</tt>;
**         see <a href="BitIOProperties.html#SafeMode">Safe mode</a>
**         property for limitations). If the direction is <b>output</b>,
**         it writes the value to the pin. (Method is available only if
**         the direction = <u><tt>output</tt></u> or <u><tt>
**         input/output</tt></u>).
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by <Init> method.
**     @param
**         Val             - Output value. Possible values:
**                           <false> - logical "0" (Low level)
**                           <true> - logical "1" (High level)
*/
/* ===================================================================*/
void IO_3_PutVal(LDD_TDeviceData *DeviceDataPtr, bool Val)
{
  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  if (Val) {
    GPIO_PDD_SetPortDataOutputMask(IO_3_MODULE_BASE_ADDRESS, IO_3_PORT_MASK);
  } else { /* !Val */
    GPIO_PDD_ClearPortDataOutputMask(IO_3_MODULE_BASE_ADDRESS, IO_3_PORT_MASK);
  } /* !Val */
}

/*
** ===================================================================
**     Method      :  IO_3_ClrVal (component BitIO_LDD)
*/
/*!
**     @brief
**         Clears (set to zero) the output value. It is equivalent to
**         the [PutVal(FALSE)]. This method is available only if the
**         direction = _[output]_ or _[input/output]_.
**     @param
**         DeviceDataPtr   - Pointer to device data
**                           structure returned by <Init> method.
*/
/* ===================================================================*/
void IO_3_ClrVal(LDD_TDeviceData *DeviceDataPtr)
{
  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  GPIO_PDD_ClearPortDataOutputMask(IO_3_MODULE_BASE_ADDRESS, IO_3_PORT_MASK);
}

/*
** ===================================================================
**     Method      :  IO_3_SetVal (component BitIO_LDD)
*/
/*!
**     @brief
**         Sets (to one) the output value. It is equivalent to the
**         [PutVal(TRUE)]. This method is available only if the
**         direction = _[output]_ or _[input/output]_.
**     @param
**         DeviceDataPtr   - Pointer to device data
**                           structure returned by <Init> method.
*/
/* ===================================================================*/
void IO_3_SetVal(LDD_TDeviceData *DeviceDataPtr)
{
  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  GPIO_PDD_SetPortDataOutputMask(IO_3_MODULE_BASE_ADDRESS, IO_3_PORT_MASK);
}

/* END IO_3. */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
