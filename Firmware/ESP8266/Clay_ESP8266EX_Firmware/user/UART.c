/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "UART.h"
#include "Multibyte_Ring_Buffer.h"
#include "Queues.h"

enum
{
	UART_EVENT_RX_CHAR, UART_EVENT_MAX
};

typedef struct _os_event_
{
	uint32 event;
	uint32 param;
} os_event_t;

uint8 uart_no = UART0;     //UartDev.buff_uart_no;
uint8 BytesAvailable = 0;
volatile uint8 BytesRead;

xTaskHandle xUartTaskHandle;
xQueueHandle xQueueUart;

LOCAL STATUS ICACHE_RODATA_ATTR uart_tx_one_char(uint8 uart, uint8 TxChar)
{
	while (true)
	{
		uint32 fifo_cnt = READ_PERI_REG(UART_STATUS(uart))
				& (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S);

		if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126)
		{
			break;
		}
	}

	WRITE_PERI_REG(UART_FIFO(uart), TxChar);
	return OK;
}

LOCAL void ICACHE_RODATA_ATTR uart1_write_char(char c)
{
	uart_tx_one_char(UART1, c);
}

LOCAL void ICACHE_RODATA_ATTR uart0_write_char(char c)
{
	uart_tx_one_char(UART0, c);
}

#if 0
LOCAL void uart_rx_intr_handler_ssc(void)
{
	//TODO: how do i this?
	/* uart0 and uart1 intr combine togther, when interrupt occur, see reg 0x3ff20020, bit2, bit0 represents
	 * uart1 and uart0 respectively
	 */
	os_event_t e;
	portBASE_TYPE xHigherPriorityTaskWoken;

	uint8 RcvChar;
	uint8 uart_no = 0;

	if (UART_RXFIFO_FULL_INT_ST
			!= (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_FULL_INT_ST))
	{
		return;
	}

	RcvChar = READ_PERI_REG(UART_FIFO(uart_no)) & 0xFF;

	WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_RXFIFO_FULL_INT_CLR);

	e.event = UART_EVENT_RX_CHAR;
	e.param = RcvChar;

	xQueueSendFromISR(xQueueUart, (void * )&e, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

LOCAL void uart_config(uint8 uart_no, UART_ConfigTypeDef *uart)
{
	if (uart_no == UART1)
	{
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_U1TXD_BK);
	}
	else
	{
		/* rcv_buff size if 0x100 */
		_xt_isr_attach(ETS_UART_INUM, (_xt_isr) uart_rx_intr_handler_ssc, NULL);
		PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
	}

	uart_div_modify(uart_no, UART_CLK_FREQ / (uart->baud_rate));

	WRITE_PERI_REG(UART_CONF0(uart_no),
//                  uart->exist_parity |
			uart->parity | (uart->stop_bits << UART_STOP_BIT_NUM_S) | (uart->data_bits << UART_BIT_NUM_S));

	//clear rx and tx fifo,not ready
	SET_PERI_REG_MASK(UART_CONF0(uart_no), UART_RXFIFO_RST | UART_TXFIFO_RST);
	CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_RXFIFO_RST | UART_TXFIFO_RST);

	if (uart_no == UART0)
	{
		//set rx fifo trigger
		WRITE_PERI_REG(UART_CONF1(uart_no), ((0x01 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S));
	}
	else
	{
		WRITE_PERI_REG(UART_CONF1(uart_no), ((0x01 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S));
	}

	//clear all interrupt
	WRITE_PERI_REG(UART_INT_CLR(uart_no), 0xffff);
	//enable rx_interrupt
	SET_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_RXFIFO_FULL_INT_ENA);
}
#endif

#if 0
LOCAL void ICACHE_RODATA_ATTR uart_task(void *pvParameters)
{
	os_event_t e;

	for (;;)
	{
		if (xQueueReceive(xQueueUart, (void * )&e, (portTickType)portMAX_DELAY))
		{
			switch (e.event)
			{
				case UART_EVENT_RX_CHAR:
				printf("%c", e.param);
				break;

				default:
				break;
			}
		}
	}

	vTaskDelete(NULL);
}
#endif

#if 0
void uart_init(void)
{
	while (READ_PERI_REG(UART_STATUS(0)) & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S))
	;

	while (READ_PERI_REG(UART_STATUS(1)) & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S))
	;

	UART_ConfigTypeDef uart;

	uart.baud_rate = BIT_RATE_74880;
	uart.data_bits = UART_WordLength_8b;
	uart.flow_ctrl = USART_HardwareFlowControl_None;
	// uart.exist_parity = PARITY_DIS;
	uart.parity = USART_Parity_None;
	uart.stop_bits = USART_StopBits_1;

	uart_config(UART0, &uart);
	uart_config(UART1, &uart);

	os_install_putc1(uart1_write_char);

	_xt_isr_unmask(1 << ETS_UART_INUM);

	xQueueUart = xQueueCreate(32, sizeof(os_event_t));

	xTaskCreate(uart_task, (uint8 const * )"uTask", 512, NULL, tskIDLE_PRIORITY + 2, &xUartTaskHandle);
}
#endif

//=================================================================

void ICACHE_RODATA_ATTR UART_SetWordLength(UART_Port uart_no,
		UART_WordLength len)
{
	SET_PERI_REG_BITS(UART_CONF0(uart_no), UART_BIT_NUM, len, UART_BIT_NUM_S);
}

void ICACHE_RODATA_ATTR UART_SetStopBits(UART_Port uart_no,
		UART_StopBits bit_num)
{
	SET_PERI_REG_BITS(UART_CONF0(uart_no), UART_STOP_BIT_NUM, bit_num,
			UART_STOP_BIT_NUM_S);
}

void ICACHE_RODATA_ATTR UART_SetLineInverse(UART_Port uart_no,
		UART_LineLevelInverse inverse_mask)
{
	CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_LINE_INV_MASK);
	SET_PERI_REG_MASK(UART_CONF0(uart_no), inverse_mask);
}

void ICACHE_RODATA_ATTR UART_SetParity(UART_Port uart_no,
		UART_ParityMode Parity_mode)
{
	CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_PARITY | UART_PARITY_EN);

	if (Parity_mode == USART_Parity_None)
	{
	}
	else
	{
		SET_PERI_REG_MASK(UART_CONF0(uart_no), Parity_mode | UART_PARITY_EN);
	}
}

void ICACHE_RODATA_ATTR UART_SetBaudrate(UART_Port uart_no, uint32 baud_rate)
{
	uart_div_modify(uart_no, UART_CLK_FREQ / baud_rate);
}

//only when USART_HardwareFlowControl_RTS is set , will the rx_thresh value be set.
void ICACHE_RODATA_ATTR UART_SetFlowCtrl(UART_Port uart_no,
		UART_HwFlowCtrl flow_ctrl, uint8 rx_thresh)
{
	if (flow_ctrl & USART_HardwareFlowControl_RTS)
	{
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_U0RTS);
		SET_PERI_REG_BITS(UART_CONF1(uart_no), UART_RX_FLOW_THRHD, rx_thresh,
				UART_RX_FLOW_THRHD_S);
		SET_PERI_REG_MASK(UART_CONF1(uart_no), UART_RX_FLOW_EN);
	}
	else
	{
		CLEAR_PERI_REG_MASK(UART_CONF1(uart_no), UART_RX_FLOW_EN);
	}

	if (flow_ctrl & USART_HardwareFlowControl_CTS)
	{
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_UART0_CTS);
		SET_PERI_REG_MASK(UART_CONF0(uart_no), UART_TX_FLOW_EN);
	}
	else
	{
		CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_TX_FLOW_EN);
	}
}

bool ICACHE_RODATA_ATTR UART_CheckTxFifoEmpty(UART_Port uart_no)
{
	return !(READ_PERI_REG(UART_STATUS(uart_no))
			& (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S));
}

void ICACHE_RODATA_ATTR UART_WaitTxFifoEmpty(UART_Port uart_no) //do not use if tx flow control enabled
{
	while (!UART_CheckTxFifoEmpty(uart_no))
		;
}

///same as resetFifo, but it accepts the flags to set so you can just set rx or tx.
void ICACHE_RODATA_ATTR UART_ResetFifo_WithFlagOption(UART_Port uart_no,
		uint32 flags)
{
	SET_PERI_REG_MASK(UART_CONF0(uart_no),
			flags & (UART_RXFIFO_RST | UART_TXFIFO_RST));
	CLEAR_PERI_REG_MASK(UART_CONF0(uart_no),
			flags & (UART_RXFIFO_RST | UART_TXFIFO_RST));
}

void ICACHE_RODATA_ATTR UART_ResetRxFifo(UART_Port uart_no)
{
	UART_ResetFifo_WithFlagOption(uart_no, UART_RXFIFO_RST);
}
void ICACHE_RODATA_ATTR UART_ResetTxFifo(UART_Port uart_no)
{
	UART_ResetFifo_WithFlagOption(uart_no, UART_TXFIFO_RST);
}

void ICACHE_RODATA_ATTR UART_ResetFifo(UART_Port uart_no)
{
	UART_ResetFifo_WithFlagOption(uart_no, UART_RXFIFO_RST | UART_TXFIFO_RST);
}

void ICACHE_RODATA_ATTR UART_ClearIntrStatus(UART_Port uart_no, uint32 clr_mask)
{
	WRITE_PERI_REG(UART_INT_CLR(uart_no), clr_mask);
}

void ICACHE_RODATA_ATTR UART_SetIntrEna(UART_Port uart_no, uint32 ena_mask)
{
	SET_PERI_REG_MASK(UART_INT_ENA(uart_no), ena_mask);
}

void ICACHE_RODATA_ATTR UART_intr_handler_register(void *fn, void *arg)
{
	_xt_isr_attach(ETS_UART_INUM, fn, arg);
}

void ICACHE_RODATA_ATTR UART_SetPrintPort(UART_Port uart_no)
{
	if (uart_no == 1)
	{
		os_install_putc1(uart1_write_char);
	}
	else
	{
		os_install_putc1(uart0_write_char);
	}
}

void ICACHE_RODATA_ATTR UART_ParamConfig(UART_Port uart_no,
		UART_ConfigTypeDef *pUARTConfig)
{
	if (uart_no == UART1)
	{
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_U1TXD_BK);
	}
	else
	{
		PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD);
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
	}

	UART_SetFlowCtrl(uart_no, pUARTConfig->flow_ctrl,
			pUARTConfig->UART_RxFlowThresh);
	UART_SetBaudrate(uart_no, pUARTConfig->baud_rate);

	WRITE_PERI_REG(UART_CONF0(uart_no),
			((pUARTConfig->parity == USART_Parity_None) ? 0x0 : (UART_PARITY_EN | pUARTConfig->parity)) | (pUARTConfig->stop_bits << UART_STOP_BIT_NUM_S) | (pUARTConfig->data_bits << UART_BIT_NUM_S) | ((pUARTConfig->flow_ctrl & USART_HardwareFlowControl_CTS) ? UART_TX_FLOW_EN : 0x0) | pUARTConfig->UART_InverseMask);

	UART_ResetFifo(uart_no);
}

void ICACHE_RODATA_ATTR UART_IntrConfig(UART_Port uart_no,
		UART_IntrConfTypeDef *pUARTIntrConf)
{

	uint32 reg_val = 0;
	UART_ClearIntrStatus(uart_no, UART_INTR_MASK);
	reg_val = READ_PERI_REG(UART_CONF1(uart_no))
			& ~((UART_RX_FLOW_THRHD << UART_RX_FLOW_THRHD_S) | UART_RX_FLOW_EN);

	reg_val |= (
			(pUARTIntrConf->UART_IntrEnMask & UART_RXFIFO_TOUT_INT_ENA) ?
					((((pUARTIntrConf->UART_RX_TimeOutIntrThresh)
							& UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S)
							| UART_RX_TOUT_EN) :
					0);

	reg_val |= (
			(pUARTIntrConf->UART_IntrEnMask & UART_RXFIFO_FULL_INT_ENA) ?
					(((pUARTIntrConf->UART_RX_FifoFullIntrThresh)
							& UART_RXFIFO_FULL_THRHD)
							<< UART_RXFIFO_FULL_THRHD_S) :
					0);

	reg_val |= (
			(pUARTIntrConf->UART_IntrEnMask & UART_TXFIFO_EMPTY_INT_ENA) ?
					(((pUARTIntrConf->UART_TX_FifoEmptyIntrThresh)
							& UART_TXFIFO_EMPTY_THRHD)
							<< UART_TXFIFO_EMPTY_THRHD_S) :
					0);

	WRITE_PERI_REG(UART_CONF1(uart_no), reg_val);
	CLEAR_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_INTR_MASK);
	SET_PERI_REG_MASK(UART_INT_ENA(uart_no), pUARTIntrConf->UART_IntrEnMask);
}

LOCAL void uart0_rx_intr_handler(void *para)
{
	/* uart0 and uart1 intr combine togther, when interrupt occur, see reg 0x3ff20020, bit2, bit0 represents
	 * uart1 and uart0 respectively
	 */

	uint32 uart_intr_status = READ_PERI_REG(UART_INT_ST(uart_no));

	while (uart_intr_status != 0x0)
	{
		if (UART_FRM_ERR_INT_ST == (uart_intr_status & UART_FRM_ERR_INT_ST))
		{
			//printf("FRM_ERR\r\n");
			WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_FRM_ERR_INT_CLR);
		}
		else if (UART_RXFIFO_FULL_INT_ST
				== (uart_intr_status & UART_RXFIFO_FULL_INT_ST))
		{
//			printf("full\n");
			BytesAvailable = (READ_PERI_REG(UART_STATUS(UART0))
					>> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
			BytesRead = 0;

			uint8_t temp;
			while (BytesRead < BytesAvailable)
			{
				temp = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
				Multibyte_Ring_Buffer_Enqueue(&serial_rx_multibyte, &temp, 1);
				++BytesRead;
			}

			WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
		}
		else if (UART_RXFIFO_TOUT_INT_ST
				== (uart_intr_status & UART_RXFIFO_TOUT_INT_ST))
		{
//			printf("to\n");
			BytesAvailable = (READ_PERI_REG(UART_STATUS(UART0))
					>> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
			BytesRead = 0;

			uint8_t temp;
			while (BytesRead < BytesAvailable)
			{
				temp = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
				Multibyte_Ring_Buffer_Enqueue(&serial_rx_multibyte, &temp, 1);
				++BytesRead;
			}

			WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
		}
		else if (UART_TXFIFO_EMPTY_INT_ST
				== (uart_intr_status & UART_TXFIFO_EMPTY_INT_ST))
		{
//         printf("empty\n\r");
			WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_TXFIFO_EMPTY_INT_CLR);
			CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_TXFIFO_EMPTY_INT_ENA);
		}
		else
		{
			//skip
		}
		uart_intr_status = READ_PERI_REG(UART_INT_ST(uart_no));
	}
}

void ICACHE_RODATA_ATTR uart_init_new(void)
{
	UART_WaitTxFifoEmpty(UART0);
	UART_WaitTxFifoEmpty(UART1);

	UART_ConfigTypeDef uart_config;
	uart_config.baud_rate = BIT_RATE_230400;
	uart_config.data_bits = UART_WordLength_8b;
	uart_config.parity = USART_Parity_None;
	uart_config.stop_bits = USART_StopBits_1;
	uart_config.flow_ctrl = USART_HardwareFlowControl_None;
	uart_config.UART_RxFlowThresh = 120;
	uart_config.UART_InverseMask = UART_None_Inverse;
	UART_ParamConfig(UART0, &uart_config);

	UART_IntrConfTypeDef uart_intr;
	uart_intr.UART_IntrEnMask = UART_RXFIFO_TOUT_INT_ENA | UART_FRM_ERR_INT_ENA
			| UART_RXFIFO_FULL_INT_ENA | UART_TXFIFO_EMPTY_INT_ENA;
	uart_intr.UART_RX_FifoFullIntrThresh = 10;   //interrupt when we rx 10 bytes

	//this matches the uart tx block time between threads
	uart_intr.UART_RX_TimeOutIntrThresh = 2; //time out and call the interrupt when we haven't rx'd for 2 somethings.
	uart_intr.UART_TX_FifoEmptyIntrThresh = 20;
	UART_IntrConfig(UART0, &uart_intr);

	UART_SetPrintPort(UART0);
	UART_intr_handler_register(uart0_rx_intr_handler, NULL);
	ETS_UART_INTR_ENABLE();

	/*
	 UART_SetWordLength(UART0,UART_WordLength_8b);
	 UART_SetStopBits(UART0,USART_StopBits_1);
	 UART_SetParity(UART0,USART_Parity_None);
	 UART_SetBaudrate(UART0,74880);
	 UART_SetFlowCtrl(UART0,USART_HardwareFlowControl_None,0);
	 */

}

STATUS ICACHE_RODATA_ATTR uart_tx_array(uint8 uart, uint8 * array,
		uint32_t count)
{
	int i = 0;
	for (; i < count; ++i)
	{
		uart_tx_one_char(uart, array[i]);
	}

	return OK;
}
