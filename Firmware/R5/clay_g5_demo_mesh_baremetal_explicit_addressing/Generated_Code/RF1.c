/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : RF1.c
**     Project     : clay_g5_demo_mesh_baremetal
**     Processor   : MK20DX256VLH7
**     Component   : nRF24L01
**     Version     : Component 01.086, Driver 01.00, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-10-02, 19:26, # CodeGen: 14
**     Abstract    :
**         This component implements a driver for the Nordic Semiconductor nRF24L01 2.4 GHz transceiver.
**     Settings    :
**     Contents    :
**         ConfigureSPI               - void RF1_ConfigureSPI(void);
**         WriteRegister              - void RF1_WriteRegister(uint8_t reg, uint8_t val);
**         ReadRegister               - uint8_t RF1_ReadRegister(uint8_t reg);
**         ReadRegisterData           - void RF1_ReadRegisterData(uint8_t reg, uint8_t *buf, uint8_t bufSize);
**         WriteRegisterData          - void RF1_WriteRegisterData(byte reg, uint8_t *buf, uint8_t bufSize);
**         WriteRead                  - uint8_t RF1_WriteRead(uint8_t val);
**         Write                      - void RF1_Write(uint8_t val);
**         GetStatus                  - uint8_t RF1_GetStatus(void);
**         GetStatusClrIRQ            - uint8_t RF1_GetStatusClrIRQ(void);
**         ResetStatusIRQ             - void RF1_ResetStatusIRQ(uint8_t flags);
**         TxPayload                  - void RF1_TxPayload(uint8_t *payload, uint8_t payloadSize);
**         RxPayload                  - void RF1_RxPayload(uint8_t *payload, uint8_t payloadSize);
**         StopRxTx                   - void RF1_StopRxTx(void);
**         StartRxTx                  - void RF1_StartRxTx(void);
**         EnableAutoAck              - uint8_t RF1_EnableAutoAck(uint8_t pipes);
**         SetStaticPipePayload       - uint8_t RF1_SetStaticPipePayload(uint8_t pipe, uint8_t payloadBytes);
**         EnableDynamicPayloadLength - uint8_t RF1_EnableDynamicPayloadLength(uint8_t pipeMask);
**         WriteFeature               - uint8_t RF1_WriteFeature(uint8_t featureMask);
**         ReadFeature                - uint8_t RF1_ReadFeature(uint8_t *featureMask);
**         ReadNofRxPayload           - uint8_t RF1_ReadNofRxPayload(uint8_t *nof);
**         ReadObserveTxRegister      - uint8_t RF1_ReadObserveTxRegister(uint8_t *nofLoss, uint8_t *nofRetransmitted);
**         ReadReceivedPowerDetector  - uint8_t RF1_ReadReceivedPowerDetector(uint8_t *rpd);
**         SetChannel                 - uint8_t RF1_SetChannel(uint8_t channel);
**         GetChannel                 - uint8_t RF1_GetChannel(uint8_t *channel);
**         ConstantCarrierWave        - uint8_t RF1_ConstantCarrierWave(bool turnOn);
**         SetOutputPower             - uint8_t RF1_SetOutputPower(int8_t power);
**         GetOutputPower             - uint8_t RF1_GetOutputPower(int8_t *power);
**         SetDataRate                - uint8_t RF1_SetDataRate(uint16_t rate);
**         GetDataRate                - uint8_t RF1_GetDataRate(uint16_t *rate);
**         GetFifoStatus              - uint8_t RF1_GetFifoStatus(uint8_t *status);
**         PollInterrupt              - void RF1_PollInterrupt(void);
**         Deinit                     - void RF1_Deinit(void);
**         Init                       - void RF1_Init(void);
**
**     License   : Open Source (LGPL)
**     Copyright : Erich Styger, 2013-2014, all rights reserved.
**     Web       : www.mcuoneclipse.com
**     This an open source software implementing a driver using Processor Expert.
**     This is a free software and is opened for education, research and commercial developments under license policy of following terms:
**     * This is a free software and there is NO WARRANTY.
**     * No restriction on use. You can use, modify and redistribute it for personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
**     * Redistributions of source code must retain the above copyright notice.
** ###################################################################*/
/*!
** @file RF1.c
** @version 01.00
** @brief
**         This component implements a driver for the Nordic Semiconductor nRF24L01 2.4 GHz transceiver.
*/         
/*!
**  @addtogroup RF1_module RF1 module documentation
**  @{
*/         

/* MODULE RF1. */

#include "Events.h"
#include "RF1.h"

/* Macros to hide low level functionality */
#define RF1_WAIT_US(x)  WAIT1_Waitus(x)          /* wait for the given number of micro-seconds */
#define RF1_WAIT_MS(x)  WAIT1_Waitms(x)          /* wait for the given number of milli-seconds */
#define RF1_CE_LOW()    CE1_ClrVal()             /* put CE LOW */
#define RF1_CE_HIGH()   CE1_SetVal()             /* put CE HIGH */
#define RF1_CSN_LOW()   CSN1_ClrVal()            /* put CSN LOW, activate bus */
#define RF1_CSN_HIGH()  CSN1_SetVal()            /* put CSN HIGH, deactivate bus */

#define RF1_USE_SPI_BLOCK_MODE   0               /* using SPI block read/write */

/* HW SPI */
#define RF1_SPI_Enable()                   SM1_Enable()
#define RF1_SPI_Disable()                  SM1_Disable()
#define RF1_SPI_SetFastMode()              SM1_SetFastMode()
#define RF1_SPI_SetSlowMode()              SM1_SetSlowMode()
#define RF1_SPI_SetShiftClockPolarity(val) (void)SM1_SetShiftClockPolarity(val)
#define RF1_SPI_SetIdleClockPolarity(val)  (void)SM1_SetIdleClockPolarity(val)
#define RF1_SPI_GetCharsInTxBuf()          SM1_GetCharsInTxBuf()
#define RF1_SPI_GetCharsInRxBuf()          SM1_GetCharsInRxBuf()
#define RF1_SPI_SendChar(ch)               SM1_SendChar(ch)
#define RF1_SPI_RecvChar(p)                SM1_RecvChar(p)
#define RF1_SPI_SetBaudRateMode(m)         SM1_SetBaudRateMode(m)

/* Internal method prototypes */
static uint8_t SPIWriteRead(uint8_t val);
static void SPIWriteReadBuffer(uint8_t *bufOut, uint8_t *bufIn, uint8_t bufSize);
static void SPIWriteBuffer(uint8_t *bufOut, uint8_t bufSize);

/*
** ===================================================================
**     Method      :  RF1_ConfigureSPI (component nRF24L01)
**     Description :
**         Configures the SPI bus
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void RF1_ConfigureSPI(void)
{
#if RF1_SWITCH_BUS
  RF1_SPI_Disable();
  (void)RF1_SPI_SetShiftClockPolarity(0); /* falling edge */
  (void)RF1_SPI_SetIdleClockPolarity(0); /* low idle clock polarity */
  (void)RF1_SPI_SetBaudRateMode(RF1_BAUD_RATE_MODE); /* set bus speed */
  RF1_SPI_Enable();
#endif
}

/*
** ===================================================================
**     Method      :  SPIWriteRead (component nRF24L01)
**
**     Description :
**         Write a byte to the SPI and reads the shifted value
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static uint8_t SPIWriteRead(uint8_t val)
{
  uint8_t ch;

  while(RF1_SPI_GetCharsInTxBuf()!=0) {} /* wait until tx is empty */
  while(RF1_SPI_SendChar(val)!=ERR_OK) {} /* send character */
  while(RF1_SPI_GetCharsInTxBuf()!=0) {} /* wait until data has been sent */
  while(RF1_SPI_GetCharsInRxBuf()==0) {} /* wait until we receive data */
  while(RF1_SPI_RecvChar(&ch)!=ERR_OK) {} /* get data */
  return ch;
}

/*
** ===================================================================
**     Method      :  SPIWriteReadBuffer (component nRF24L01)
**
**     Description :
**         Write a buffer to the SPI and the same time reads the shifted 
**         values
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void SPIWriteReadBuffer(uint8_t *bufOut, uint8_t *bufIn, uint8_t bufSize)
{
#if RF1_USE_SPI_BLOCK_MODE
  uint16_t snt, rcvd;

  while(RF1_SPI_GetCharsInTxBuf()!=0) {} /* wait until tx is empty */
  while(SPI_SendBlock(bufOut, bufSize, &snt)!=ERR_OK) {
    /* send buffer */
  }
  while(RF1_SPI_GetCharsInTxBuf()!=0) {} /* wait until data has been sent */
  while(RF1_SPI_GetCharsInRxBuf()==0) {} /* wait until we receive data */
  while(SPI_RecvBlock(bufIn, bufSize, &rcvd)!=ERR_OK) {
    /* get data */
  }
#else
  uint8_t i;

  for(i=0;i<bufSize;i++) {
    bufIn[i] = SPIWriteRead(bufOut[i]);
  }
#endif
}

/*
** ===================================================================
**     Method      :  SPIWriteBuffer (component nRF24L01)
**
**     Description :
**         Write a buffer to the SPI bus without returning the shifted in 
**         values
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void SPIWriteBuffer(uint8_t *bufOut, uint8_t bufSize)
{
#if RF1_USE_SPI_BLOCK_MODE
  uint16_t snt, rcvd;
  static uint8_t dummyBuf[SPI_INP_BUF_SIZE];

  if (bufSize>SPI_INP_BUF_SIZE) { /* not enough dummy buffer */
    uint8_t i;

    for(i=0;i<bufSize;i++) {
      (void)SPIWriteRead(bufOut[i]);
    }
  } else {
    while(RF1_SPI_GetCharsInTxBuf()!=0) {} /* wait until tx is empty */
    while(SPI_SendBlock(bufOut, bufSize, &snt)!=ERR_OK) {
      /* send buffer */
    }
    while(RF1_SPI_GetCharsInTxBuf()!=0) {} /* wait until data has been sent */
    while(RF1_SPI_GetCharsInRxBuf()==0) {} /* wait until we receive data */
    while(SPI_SPI.RecvBlock(dummyBuf, bufSize, &rcvd)!=ERR_OK) {
      /* get data */
    }
  }
#else
  uint8_t i;

  for(i=0;i<bufSize;i++) {
    (void)SPIWriteRead(bufOut[i]);
  }
#endif
}

/*
** ===================================================================
**     Method      :  RF1_Deinit (component nRF24L01)
**     Description :
**         Driver deinitialization
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void RF1_Deinit(void)
{
  /* nothing needed */
}

/*
** ===================================================================
**     Method      :  RF1_Init (component nRF24L01)
**     Description :
**         Initialization method
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void RF1_Init(void)
{
  RF1_CE_LOW();   /* CE high: do not send or receive data */
  RF1_CSN_HIGH(); /* CSN low: not sending commands to the device */
  RF1_ConfigureSPI(); /* set up SPI bus */
}

/*
** ===================================================================
**     Method      :  RF1_OnInterrupt (component nRF24L01)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void IRQ1_OnInterrupt(void)
{
  RF1_CE_LOW(); /* pull CE Low to disable transceiver */
  RF1_OnInterrupt(); /* call user event (if enabled)... */
}

/*
** ===================================================================
**     Method      :  RF1_WriteRegister (component nRF24L01)
**     Description :
**         Write a register value to the transceiver
**     Parameters  :
**         NAME            - DESCRIPTION
**         reg             - Register address to write
**         val             - Value to write
**     Returns     : Nothing
** ===================================================================
*/
void RF1_WriteRegister(uint8_t reg, uint8_t val)
{
#if RF1_SWITCH_BUS
  RF1_ConfigureSPI(); /* change bus speed */
#endif
  RF1_CSN_LOW(); /* initiate command sequence */
  (void)SPIWriteRead(RF1_W_REGISTER|reg); /* write register command */
  (void)SPIWriteRead(val); /* write value */
  RF1_CSN_HIGH(); /* end command sequence */
  RF1_WAIT_US(10); /* insert a delay until next command */
}

/*
** ===================================================================
**     Method      :  RF1_ReadRegister (component nRF24L01)
**     Description :
**         Reads a register from the transceiver
**     Parameters  :
**         NAME            - DESCRIPTION
**         reg             - Register address
**     Returns     :
**         ---             - value read
** ===================================================================
*/
uint8_t RF1_ReadRegister(uint8_t reg)
{
  uint8_t val;

#if RF1_SWITCH_BUS
  RF1_ConfigureSPI(); /* change bus speed */
#endif
  RF1_CSN_LOW();
  (void)SPIWriteRead(reg);
  val = SPIWriteRead(0); /* write dummy */
  RF1_CSN_HIGH();
  RF1_WAIT_US(10);
  return val;
}

/*
** ===================================================================
**     Method      :  RF1_ReadRegisterData (component nRF24L01)
**     Description :
**         Read multiple bytes from the bus.
**     Parameters  :
**         NAME            - DESCRIPTION
**         reg             - Register address
**       * buf             - Pointer to buffer where to store the values
**         bufSize         - Size of the destination buffer
**     Returns     : Nothing
** ===================================================================
*/
void RF1_ReadRegisterData(uint8_t reg, uint8_t *buf, uint8_t bufSize)
{
#if RF1_SWITCH_BUS
  RF1_ConfigureSPI(); /* change bus speed */
#endif
  RF1_CSN_LOW();
  (void)SPIWriteRead(RF1_R_REGISTER|reg);
  SPIWriteReadBuffer(buf, buf, bufSize);
  RF1_CSN_HIGH();
  RF1_WAIT_US(10);
}

/*
** ===================================================================
**     Method      :  RF1_WriteRegisterData (component nRF24L01)
**     Description :
**         Write multiple bytes to the bus.
**     Parameters  :
**         NAME            - DESCRIPTION
**         reg             - Register address
**       * buf             - Pointer to data buffer to write
**         bufSize         - Size of buffer in bytes
**     Returns     : Nothing
** ===================================================================
*/
void RF1_WriteRegisterData(byte reg, uint8_t *buf, uint8_t bufSize)
{
#if RF1_SWITCH_BUS
  RF1_ConfigureSPI(); /* change bus speed */
#endif
  RF1_CSN_LOW();
  (void)SPIWriteRead(RF1_W_REGISTER|reg); /* not masking registers as it would conflict with RF1_W_TX_PAYLOAD */
  SPIWriteBuffer(buf, bufSize);
  RF1_CSN_HIGH();
  RF1_WAIT_US(10);
}

/*
** ===================================================================
**     Method      :  RF1_WriteRead (component nRF24L01)
**     Description :
**         Writes a byte and reads back one byte
**     Parameters  :
**         NAME            - DESCRIPTION
**         val             - Value to write to the bus
**     Returns     :
**         ---             - Value read from the bus
** ===================================================================
*/
uint8_t RF1_WriteRead(uint8_t val)
{
#if RF1_SWITCH_BUS
  RF1_ConfigureSPI(); /* change bus speed */
#endif
  RF1_CSN_LOW();
  val = SPIWriteRead(val);
  RF1_CSN_HIGH();
  RF1_WAIT_US(10);
  return val;
}

/*
** ===================================================================
**     Method      :  RF1_Write (component nRF24L01)
**     Description :
**         Writes a byte to the bus, without returning the byte read.
**     Parameters  :
**         NAME            - DESCRIPTION
**         val             - Value to write
**     Returns     : Nothing
** ===================================================================
*/
void RF1_Write(uint8_t val)
{
#if RF1_SWITCH_BUS
  RF1_ConfigureSPI(); /* change bus speed */
#endif
  RF1_CSN_LOW();
  (void)SPIWriteRead(val);
  RF1_CSN_HIGH();
  RF1_WAIT_US(10);
}

/*
** ===================================================================
**     Method      :  RF1_GetStatus (component nRF24L01)
**     Description :
**         Returns the device status byte
**     Parameters  : None
**     Returns     :
**         ---             - status byte
** ===================================================================
*/
uint8_t RF1_GetStatus(void)
{
  return RF1_WriteRead(RF1_NOP);
}

/*
** ===================================================================
**     Method      :  RF1_ResetStatusIRQ (component nRF24L01)
**     Description :
**         Reset the given flags mask of status bits
**     Parameters  :
**         NAME            - DESCRIPTION
**         flags           - Flags, one or more of
**                           RF24_STATUS_RX_DR, RF24_STATUS_TX_DS,
**                           RF24_STATUS_MAX_RT
**     Returns     : Nothing
** ===================================================================
*/
void RF1_ResetStatusIRQ(uint8_t flags)
{
  RF1_WriteRegister(RF1_STATUS, flags); /* reset all IRQ in status register */
}

/*
** ===================================================================
**     Method      :  RF1_GetStatusClrIRQ (component nRF24L01)
**     Description :
**         Returns the device status byte and clears any IRQ bits in
**         the status register.
**     Parameters  : None
**     Returns     :
**         ---             - status byte
** ===================================================================
*/
uint8_t RF1_GetStatusClrIRQ(void)
{
  uint8_t status;

#if RF1_SWITCH_BUS
  RF1_ConfigureSPI(); /* change bus speed */
#endif
  RF1_CSN_LOW(); /* initiate command sequence */
  status = SPIWriteRead(RF1_W_REGISTER|RF1_STATUS); /* get status */
  (void)SPIWriteRead(status&(RF1_STATUS_RX_DR|RF1_STATUS_TX_DS|RF1_STATUS_MAX_RT)); /* reset IRQ Bits */
  RF1_CSN_HIGH(); /* end command sequence */
  RF1_WAIT_US(10); /* insert a delay until next command */
  return status;
}

/*
** ===================================================================
**     Method      :  RF1_TxPayload (component nRF24L01)
**     Description :
**         Send the payload to the Tx FIFO and send it
**     Parameters  :
**         NAME            - DESCRIPTION
**       * payload         - Pointer to buffer with payload to
**                           send
**         payloadSize     - Size of payload buffer
**     Returns     : Nothing
** ===================================================================
*/
void RF1_TxPayload(uint8_t *payload, uint8_t payloadSize)
{
  RF1_Write(RF1_FLUSH_TX); /* flush old data */
  RF1_WriteRegisterData(RF1_W_TX_PAYLOAD, payload, payloadSize); /* write payload */
  RF1_CE_HIGH(); /* start transmission */
  RF1_WAIT_US(15); /* keep signal high for 15 micro-seconds */
  RF1_CE_LOW();  /* back to normal */
}

/*
** ===================================================================
**     Method      :  RF1_RxPayload (component nRF24L01)
**     Description :
**         Receive the Rx payload from the FIFO and stores it in a
**         buffer.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * payload         - Pointer to the payload buffer
**         payloadSize     - Size of the payload buffer
**     Returns     : Nothing
** ===================================================================
*/
void RF1_RxPayload(uint8_t *payload, uint8_t payloadSize)
{
  RF1_CE_LOW(); /* need to disable rx mode during reading RX data */
  RF1_ReadRegisterData(RF1_R_RX_PAYLOAD, payload, payloadSize); /* rx payload */
  RF1_CE_HIGH(); /* re-enable rx mode */
}

/*
** ===================================================================
**     Method      :  RF1_StopRxTx (component nRF24L01)
**     Description :
**         Stops sending or receiving (sets CE pin to LOW).
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void RF1_StopRxTx(void)
{
  RF1_CE_LOW(); /* disable RX or TX with pulling CE LOW */
}

/*
** ===================================================================
**     Method      :  RF1_StartRxTx (component nRF24L01)
**     Description :
**         Starts sending or receiving (sets CE pin to HIGH).
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void RF1_StartRxTx(void)
{
  RF1_CE_HIGH(); /* enabling RX or TX with pulling CE HIGH */
}

/*
** ===================================================================
**     Method      :  RF1_SetChannel (component nRF24L01)
**     Description :
**         Sets the radio channel (RF_CH register).
**     Parameters  :
**         NAME            - DESCRIPTION
**         channel         - Channel number, valid channel
**                           numbers are 0x0 to 0x7F
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_SetChannel(uint8_t channel)
{
  RF1_WriteRegister(RF1_RF_CH, channel&0x7F); /* set channel */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_GetChannel (component nRF24L01)
**     Description :
**         Returns the radio channel (RF_CH register).
**     Parameters  :
**         NAME            - DESCRIPTION
**       * channel         - Pointer to where to store the
**                           channel number.
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_GetChannel(uint8_t *channel)
{
  *channel = RF1_ReadRegister(RF1_RF_CH); /* read channel */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_EnableAutoAck (component nRF24L01)
**     Description :
**         Enables Auto-Acknowledgment for the given pipe(s) (RF_EN_AA
**         register).
**     Parameters  :
**         NAME            - DESCRIPTION
**         pipes           - mask of pipes to be enabled (ENAA_P0
**                           to ENAA_P5)
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_EnableAutoAck(uint8_t pipes)
{
  RF1_WriteRegister(RF1_EN_AA, pipes&RF1_EN_AA_ENAA_ALL); /* enable auto acknowledge for the given pipes */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_ReadNofRxPayload (component nRF24L01)
**     Description :
**         Returns the number of received payload bytes for the top
**         R_RX_PAYLOAD in the RX FIFO
**     Parameters  :
**         NAME            - DESCRIPTION
**       * nof             - Pointer to where to store the number of
**                           bytes. A return value of 0 means that the
**                           pipe is not used, and if the value is >32
**                           then the data is not valid.
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_ReadNofRxPayload(uint8_t *nof)
{
  *nof = RF1_ReadRegister(RF1_R_RX_PL_WID); /* read number of RX payload for pipe */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_SetStaticPipePayload (component nRF24L01)
**     Description :
**         Specifies the static payload for a pipe.
**     Parameters  :
**         NAME            - DESCRIPTION
**         pipe            - Pipe number, 0 to 5
**         payloadBytes    - Number of payload pipes
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_SetStaticPipePayload(uint8_t pipe, uint8_t payloadBytes)
{
  if (pipe > 5) {
    return ERR_FAULT; /* only pipe 0 to 5 allowed */
  }
  if (payloadBytes>32) {
    return ERR_FAULT;
  }
  RF1_WriteRegister(RF1_RX_PW_P0+pipe, payloadBytes); /* write number of RX payload for pipe */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_EnableDynamicPayloadLength (component nRF24L01)
**     Description :
**         Enables dynamic payload length for the give pipes
**     Parameters  :
**         NAME            - DESCRIPTION
**         pipeMask        - Mask of pipes, with 1 for pipe 0,
**                           2 for pipe 1, 4 for pipe 2 and so on.
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_EnableDynamicPayloadLength(uint8_t pipeMask)
{
  /* note: dynamic payload requires EN_DPL and ENAA_Px set for the pipe */
  if (pipeMask>0x3F) {
    return ERR_FAULT; /* only pipe 0 to 5 allowed */
  }
  RF1_WriteRegister(RF1_DYNPD, pipeMask); /* write number of RX payload for pipe */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_WriteFeature (component nRF24L01)
**     Description :
**         Writes the FEATURE register
**     Parameters  :
**         NAME            - DESCRIPTION
**         featureMask     - Mask of FEATURE, e.g.
**                           FEATURE_EN_DPL
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_WriteFeature(uint8_t featureMask)
{
  if (featureMask>(RF1_FEATURE_EN_DPL|RF1_FEATURE_EN_ACK_PAY|RF1_FEATURE_EN_DYN_PAY)) {
    return ERR_FAULT; /* mismatch of feature mask */
  }
  RF1_WriteRegister(RF1_FEATURE, featureMask); /* write number of RX payload for pipe */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_ReadFeature (component nRF24L01)
**     Description :
**         Reads the FEATURE register
**     Parameters  :
**         NAME            - DESCRIPTION
**       * featureMask     - Pointer to value of
**                           FEATURE register
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_ReadFeature(uint8_t *featureMask)
{
  *featureMask = RF1_ReadRegister(RF1_FEATURE); /* read FEATURE register */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_ReadObserveTxRegister (component nRF24L01)
**     Description :
**         Reads the OBSERVE_TX register to return the count of loss
**         packets and count of retransmitted packets.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * nofLoss         - Pointer to number of lost packets
**       * nofRetransmitted - Pointer to
**                           number of retransmitted packets
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_ReadObserveTxRegister(uint8_t *nofLoss, uint8_t *nofRetransmitted)
{
  uint8_t val;

  val = RF1_ReadRegister(RF1_OBSERVE_TX); /* read OBSERVE_TX register */
  *nofLoss = (uint8_t)((val&0xF0)>>4); /* high nibble */
  *nofRetransmitted = (uint8_t)(val&0x0F); /* high nibble */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_ReadReceivedPowerDetector (component nRF24L01)
**     Description :
**         Reads the RPD (Received Power Detector) register. 
**     Parameters  :
**         NAME            - DESCRIPTION
**       * rpd             - Pointer to RPD bit. Bit is one for
**                           received power levels above -64 dBm, zero
**                           for less than -64 dBm.
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_ReadReceivedPowerDetector(uint8_t *rpd)
{
  *rpd = RF1_ReadRegister(RF1_RPD); /* read RPD register */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_ConstantCarrierWave (component nRF24L01)
**     Description :
**         Put the transceiver into contant carrier wave output for
**         testing. The output power of a radio is a critical factor
**         for achieving wanted range. Output power is also the first
**         test
**     Parameters  :
**         NAME            - DESCRIPTION
**         turnOn          - Set to true to start constant
**                           carrier wave, false to stop it.
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_ConstantCarrierWave(bool turnOn)
{
  /* note: RF channel, output power should be set independently */
  uint8_t val;

  if (turnOn) {
    /* Set PWR_UP = 1 and PRIM_RX = 0 in the CONFIG register. */
    RF1_WriteRegister(RF1_CONFIG, RF1_PWR_UP|RF1_PRIM_TX);
    RF1_WAIT_US(1500); /* Wait 1.5ms PWR_UP->standby */
    /* In the RF register set:
       - CONT_WAVE = 1.
       - PLL_LOCK = 1.
       - RF_PWR (is set independently)
    */
    val = RF1_ReadRegister(RF1_RF_SETUP); /* read RF_SETUP register */
    val |= RF1_RF_SETUP_CONT_WAVE|RF1_RF_SETUP_PLL_LOCK; /* set bits */
    RF1_WriteRegister(RF1_RF_SETUP, val);
    RF1_CE_HIGH(); /* CE is high for carrier transmission */
    /* CE is kept high as long the carrier is needed */
  } else {
    val = RF1_ReadRegister(RF1_RF_SETUP); /* read RF_SETUP register */
    val &= ~(RF1_RF_SETUP_CONT_WAVE|RF1_RF_SETUP_PLL_LOCK); /* clear bits */
    RF1_WriteRegister(RF1_RF_SETUP, val); /* write back value */
    RF1_CE_LOW(); /* pull CE Low to disable transceiver */
  }
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_SetOutputPower (component nRF24L01)
**     Description :
**         Sets the output power
**     Parameters  :
**         NAME            - DESCRIPTION
**         power           - Ouput power in dBm, either 0, -10, -12
**                           or -18
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_SetOutputPower(int8_t power)
{
  uint8_t val;

  /* get current register value */
  val = RF1_ReadRegister(RF1_RF_SETUP); /* read RF_SETUP register */
  val &= ~RF1_RF_SETUP_RF_PWR_MASK; /* clear bits */
  if (power==-18) {
    val |= RF1_RF_SETUP_RF_PWR_18;
  } else if (power==-12) {
    val |= RF1_RF_SETUP_RF_PWR_12;
  } else if (power==-10) {
    val |= RF1_RF_SETUP_RF_PWR_10;
  } else if (power==0) {
    val |= RF1_RF_SETUP_RF_PWR_0;
  } else {
    return ERR_RANGE;
  }
  RF1_WriteRegister(RF1_RF_SETUP, val);
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_GetOutputPower (component nRF24L01)
**     Description :
**         Returns the current output power
**     Parameters  :
**         NAME            - DESCRIPTION
**       * power           - Pointer to where to store the value.
**                           Returns the output power in dBm
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_GetOutputPower(int8_t *power)
{
  uint8_t val;

  val = RF1_ReadRegister(RF1_RF_SETUP); /* read RF_SETUP register */
  if ((val&RF1_RF_SETUP_RF_PWR_MASK)==RF1_RF_SETUP_RF_PWR_18) {
    *power = -18;
  } else if ((val&RF1_RF_SETUP_RF_PWR_MASK)==RF1_RF_SETUP_RF_PWR_12) {
    *power = -12;
  } else if ((val&RF1_RF_SETUP_RF_PWR_MASK)==RF1_RF_SETUP_RF_PWR_10) {
    *power = -10;
  } else if ((val&RF1_RF_SETUP_RF_PWR_MASK)==RF1_RF_SETUP_RF_PWR_0) {
    *power = 0;
  } else {
    return ERR_RANGE;
  }
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_SetDataRate (component nRF24L01)
**     Description :
**         Sets the data rate
**     Parameters  :
**         NAME            - DESCRIPTION
**         rate            - Data rate, either 250, 1000 or 2000
**                           (kbps)
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_SetDataRate(uint16_t rate)
{
  uint8_t val;

  /* get current register value */
  val = RF1_ReadRegister(RF1_RF_SETUP); /* read RF_SETUP register */
  val &= ~RF1_RF_SETUP_RF_DR_MASK; /* clear bits */
  if (rate==250) {
    val |= RF1_RF_SETUP_RF_DR_250;
  } else if (rate==1000) {
    val |= RF1_RF_SETUP_RF_DR_1000;
  } else if (rate==2000) {
    val |= RF1_RF_SETUP_RF_DR_2000;
  } else { /* illegal parameter */
    return ERR_RANGE;
  }
  RF1_WriteRegister(RF1_RF_SETUP, val);
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_GetDataRate (component nRF24L01)
**     Description :
**         Returns the current data rate
**     Parameters  :
**         NAME            - DESCRIPTION
**       * rate            - Pointer to where to store the value.
**                           Returns the data rate, either 250, 1000 or
**                           2000 (kbps)
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_GetDataRate(uint16_t *rate)
{
  uint8_t val;

  val = RF1_ReadRegister(RF1_RF_SETUP); /* read RF_SETUP register */
  if ((val&RF1_RF_SETUP_RF_DR_MASK)==RF1_RF_SETUP_RF_DR_250) {
    *rate = 250;
  } else if ((val&RF1_RF_SETUP_RF_DR_MASK)==RF1_RF_SETUP_RF_DR_1000) {
    *rate = 1000;
  } else if ((val&RF1_RF_SETUP_RF_DR_MASK)==RF1_RF_SETUP_RF_DR_2000) {
    *rate = 2000;
  } else {
    return ERR_RANGE;
  }
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  RF1_PollInterrupt (component nRF24L01)
**     Description :
**         If there is no interrupt line available, this method polls
**         the device to check if there is an interrupt. If 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void RF1_PollInterrupt(void)
{
  uint8_t status;

  status = RF1_GetStatus();
  if (status&(RF1_STATUS_RX_DR|RF1_STATUS_TX_DS|RF1_STATUS_MAX_RT)) {
    RF1_CE_LOW(); /* pull CE Low to disable transceiver */
    RF1_OnInterrupt(); /* call user event (if enabled)... */
  }
}

/*
** ===================================================================
**     Method      :  RF1_GetFifoStatus (component nRF24L01)
**     Description :
**         Returns the FIFO_STATUS register value
**     Parameters  :
**         NAME            - DESCRIPTION
**       * status          - Pointer to where to store the FIFO
**                           status value.
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RF1_GetFifoStatus(uint8_t *status)
{
  *status = RF1_ReadRegister(RF1_FIFO_STATUS); /* read FIFO_STATUS register */
  return ERR_OK;
}

/* END RF1. */

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
