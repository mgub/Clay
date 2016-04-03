/*
 * UDP_Receive.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: thebh
 *
 *      This class receives messages over UDP and puts them into a buffer.
 */

/*
 * States:
 *      Disable -- No receiving occurs.
 *          -Go to configure when enable received
 *      Configure -- set up serial port, interrupts, etc
 *          -Go to Idle upon completion
 *      Idle
 *          -Watch for received data.
 *          -Go to Disable if disabled.
 *      Rx_Blocked
 *          -Something else has exclusive access to the rx buffer. Wait for it to finish.
 *          -Exit to Idle when exclusive access has been relinquished
 *          -Exit to Disable if disabled
 *      Data_Received
 *          -Put data into rx buffer and return to Idle.
 * */

////Includes //////////////////////////////////////////////////////
#include "esp_common.h"

#include "uart.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "UDP_Receiver.h"

#include "../include/AddressSerialization.h"
#include "Clay_Config.h"

#include "Message_Queue.h"

////Typedefs  /////////////////////////////////////////////////////
typedef enum
{
	Disable,
	Configure,
	Idle,
	Enqueue_Message,
	Rx_Blocked,
	Data_Received,
	UDP_STATE_MAX
} UDP_Receiver_States;
////Globals   /////////////////////////////////////////////////////

////Local vars/////////////////////////////////////////////////////
static UDP_Receiver_States State;

static int ret;

static uint8_t *UDP_Rx_Buffer;
static uint16_t UDP_Rx_Count;

static size_t fromlen;

static struct sockaddr_in from;

static struct sockaddr_in lastSourceAddress;
static struct sockaddr_in server_addr;

static int nNetTimeout;

static int32 sock_fd;
static bool Connected;
static int32 testCounter;

static Message tempMessage;
static char * source_addr;
static char * dest_addr;
static xTaskHandle udp_rx_task;

////Local Prototypes///////////////////////////////////////////////
static bool Connect();
static bool Receive();

////Global implementations ////////////////////////////////////////
bool ICACHE_RODATA_ATTR UDP_Receiver_Init()
{
	bool rval = false;
	nNetTimeout = UDP_RX_TIMEOUT_MSEC;

	State = Disable;

	taskENTER_CRITICAL();
	UDP_Rx_Buffer = zalloc(UDP_RX_BUFFER_SIZE_BYTES);
	source_addr = zalloc(CLAY_ADDR_STRING_BUF_LENGTH);
	dest_addr = zalloc(CLAY_ADDR_STRING_BUF_LENGTH);
	Initialize_Message_Queue(&incoming_message_queue);
	taskEXIT_CRITICAL();

	xTaskCreate(UDP_Receiver_State_Step, "udprx1", 512, NULL, 2, udp_rx_task);

	testCounter = 0;

	return rval;
}

void ICACHE_RODATA_ATTR UDP_Receiver_State_Step()
{
	for (;;)
	{
		switch (State)
		{
		case Disable:
		{
			if (wifi_station_get_connect_status() == STATION_GOT_IP)
			{
				State = Configure;
			}
			break;
		}

		case Configure:
		{
			if (Connect())
			{
				State = Idle;
			}
			break;
		}

		case Idle:
		{
			if (Receive())
			{
				State = Enqueue_Message;
			}
			break;
		}

		case Enqueue_Message:
		{
			taskENTER_CRITICAL();
			Serialize_Address(lastSourceAddress.sin_addr.s_addr,
					ntohs(lastSourceAddress.sin_port), source_addr,
					MAXIMUM_DESTINATION_LENGTH);
			taskEXIT_CRITICAL();

			taskYIELD();

			taskENTER_CRITICAL();
			Initialize_Message(&tempMessage,
					message_type_strings[MESSAGE_TYPE_UDP], source_addr,
					dest_addr, UDP_Rx_Buffer);
			taskEXIT_CRITICAL();

			taskYIELD();

			taskENTER_CRITICAL();
			Queue_Message(&incoming_message_queue, &tempMessage);
			taskEXIT_CRITICAL();

			State = Idle;
			break;
		}

		case Rx_Blocked:
		{
			State = Idle;
			break;
		}

		case UDP_STATE_MAX:
		default:
		{
			break;
		}
		}
		taskYIELD();
	}
}

////Local implementations /////////////////////////////////////////
static bool ICACHE_RODATA_ATTR Connect()
{
	Connected = false;

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(UDP_RX_PORT);
	server_addr.sin_len = sizeof(server_addr);

	taskENTER_CRITICAL();
	Serialize_Address(Get_IP_Address(), UDP_RX_PORT, dest_addr,
	CLAY_ADDR_STRING_BUF_LENGTH);
	taskEXIT_CRITICAL();

	///create the socket
	do
	{
		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock_fd == -1)
		{
			Connected = false;
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	} while (sock_fd == -1);

	//bind the socket
	do
	{
		ret = bind(sock_fd, (struct sockaddr * ) &server_addr,
				sizeof(server_addr));
		if (ret != 0)
		{
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	} while (ret != 0);
	Connected = true;

	setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (char * ) &nNetTimeout,
			sizeof(int));

	return Connected;
}

static bool ICACHE_RODATA_ATTR Receive()
{
	bool rval = false;

	taskENTER_CRITICAL();
	memset(UDP_Rx_Buffer, 0, UDP_RX_BUFFER_SIZE_BYTES);
	memset(&from, 0, sizeof(from));
	taskEXIT_CRITICAL();

	taskYIELD();

	fromlen = sizeof(struct sockaddr_in);

	// attempt to receive
	ret = recvfrom(sock_fd, (uint8 * ) UDP_Rx_Buffer, UDP_RX_BUFFER_SIZE_BYTES,
			0, (struct sockaddr * ) &from, (socklen_t * ) &fromlen);

	if (ret > 0)
	{
		rval = true;
		UDP_Rx_Count = ret;

		//store the source address
		lastSourceAddress.sin_addr = from.sin_addr;
		lastSourceAddress.sin_family = from.sin_family;
		lastSourceAddress.sin_len = from.sin_len;
		lastSourceAddress.sin_port = from.sin_port;
		int i = 0;
		for (; i < SIN_ZERO_LEN; ++i)
		{
			lastSourceAddress.sin_zero[i] = from.sin_zero[i];
		}
	}

	return rval;
}
