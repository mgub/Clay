/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : BuzzerOut.h
**     Project     : C6_Bringup
**     Processor   : MK64FN1M0VLL12
**     Component   : BitIO_LDD
**     Version     : Component 01.033, Driver 01.03, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-01-27, 21:56, # CodeGen: 11
**     Abstract    :
**         The HAL BitIO component provides a low level API for unified
**         access to general purpose digital input/output pins across
**         various device designs.
**
**         RTOS drivers using HAL BitIO API are simpler and more
**         portable to various microprocessors.
**     Settings    :
**          Component name                                 : BuzzerOut
**          Pin for I/O                                    : PTD4/LLWU_P14/SPI0_PCS1/UART0_RTS_b/FTM0_CH4/FB_AD2/EWM_IN/SPI1_PCS0
**          Direction                                      : Output
**          Initialization                                 : 
**            Init. direction                              : Output
**            Init. value                                  : 1
**            Auto initialization                          : yes
**          Safe mode                                      : no
**     Contents    :
**         Init   - LDD_TDeviceData* BuzzerOut_Init(LDD_TUserData *UserDataPtr);
**         GetVal - bool BuzzerOut_GetVal(LDD_TDeviceData *DeviceDataPtr);
**         PutVal - void BuzzerOut_PutVal(LDD_TDeviceData *DeviceDataPtr, bool Val);
**         ClrVal - void BuzzerOut_ClrVal(LDD_TDeviceData *DeviceDataPtr);
**         SetVal - void BuzzerOut_SetVal(LDD_TDeviceData *DeviceDataPtr);
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
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
** @file BuzzerOut.h
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
**  @addtogroup BuzzerOut_module BuzzerOut module documentation
**  @{
*/         

#ifndef __BuzzerOut_H
#define __BuzzerOut_H

/* MODULE BuzzerOut. */

/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited beans */
#include "IO_Map.h"
#include "GPIO_PDD.h"

#include "Cpu.h"

#ifdef __cplusplus
extern "C" {
#endif 



/*! Peripheral base address of a device allocated by the component. This constant can be used directly in PDD macros. */
#define BuzzerOut_PRPH_BASE_ADDRESS  0x400FF0C0U
  
/*! Device data structure pointer used when auto initialization property is enabled. This constant can be passed as a first parameter to all component's methods. */
#define BuzzerOut_DeviceData  ((LDD_TDeviceData *)PE_LDD_GetDeviceStructure(PE_LDD_COMPONENT_BuzzerOut_ID))

/* Methods configuration constants - generated for all enabled component's methods */
#define BuzzerOut_Init_METHOD_ENABLED  /*!< Init method of the component BuzzerOut is enabled (generated) */
#define BuzzerOut_GetVal_METHOD_ENABLED /*!< GetVal method of the component BuzzerOut is enabled (generated) */
#define BuzzerOut_PutVal_METHOD_ENABLED /*!< PutVal method of the component BuzzerOut is enabled (generated) */
#define BuzzerOut_ClrVal_METHOD_ENABLED /*!< ClrVal method of the component BuzzerOut is enabled (generated) */
#define BuzzerOut_SetVal_METHOD_ENABLED /*!< SetVal method of the component BuzzerOut is enabled (generated) */

/* Definition of implementation constants */
#define BuzzerOut_MODULE_BASE_ADDRESS PTD_BASE_PTR /*!< Name of macro used as the base address */
#define BuzzerOut_PORTCONTROL_BASE_ADDRESS PORTD_BASE_PTR /*!< Name of macro used as the base address */
#define BuzzerOut_PORT_MASK 0x10U      /*!< Mask of the allocated pin from the port */



/*
** ===================================================================
**     Method      :  BuzzerOut_Init (component BitIO_LDD)
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
LDD_TDeviceData* BuzzerOut_Init(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Method      :  BuzzerOut_GetVal (component BitIO_LDD)
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
bool BuzzerOut_GetVal(LDD_TDeviceData *DeviceDataPtr);

/*
** ===================================================================
**     Method      :  BuzzerOut_PutVal (component BitIO_LDD)
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
void BuzzerOut_PutVal(LDD_TDeviceData *DeviceDataPtr, bool Val);

/*
** ===================================================================
**     Method      :  BuzzerOut_ClrVal (component BitIO_LDD)
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
void BuzzerOut_ClrVal(LDD_TDeviceData *DeviceDataPtr);

/*
** ===================================================================
**     Method      :  BuzzerOut_SetVal (component BitIO_LDD)
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
void BuzzerOut_SetVal(LDD_TDeviceData *DeviceDataPtr);

/* END BuzzerOut. */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif
/* ifndef __BuzzerOut_H */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/