/*
 * TCP_Combined.c
 *
 *  Created on: Mar 31, 2016
 *      Author: thebh_000
 */

//TCP Combined Task.
// watches for messages in outgoing queue
//	sends messages after they are removed from the buffer.
// watches for messages from TCP.
//	puts messages into incoming buffer.
//
//Later, we can look at separating these into two tasks, as has been
//	done with TCP_Transmitter and TCP_Receiver, (but hopefully with
//	a working implementation...)
//
////Includes //////////////////////////////////////////////////////
#include "esp_common.h"

#include "stdio.h"
#include "string.h"

#include "UART.h"
#include "TCP_Combined.h"

#include "../include/System_Monitor.h"
#include "Message_Queue.h"
#include "Message.h"
#include "AddressSerialization.h"

////Macros ////////////////////////////////////////////////////////
#define DATA_CONNECT_ATTEMPT_MAX 		10
#define RECEIVE_DATA_SIZE				1024
#define TRANSMIT_DATA_SIZE				256
#define LOCAL_TCP_PORT					3000
#define ADDR_STRING_SIZE 				50
#define CONNECT_TIMEOUT_ms				100
#define LISTEN_TIMEOUT_ms				100
#define CONNECT_ATTEMPT_MAX				5

#define MESSAGE_TRIGGER_LEVEL			10
#define RECEIVE_BYTES_TRIGGER_LEVEL		512

#define CONNECT_RETRY_TIME				100000
////Typedefs  /////////////////////////////////////////////////////
typedef struct
{
	int socket;
	struct sockaddr_in * address;
	volatile bool connected;
} Connect_Task_Args;

////Globals   /////////////////////////////////////////////////////

////Local vars/////////////////////////////////////////////////////
static int32 listen_sock;
static int32 data_sock;

static int32 received_count;

static xTaskHandle idle_handle;

static bool connected;
static bool promoted;

static char *receive_data;
static char *transmit_data;
static char *message_ptr;

static char * local_address_string;
static char * remote_address_string;

struct sockaddr_in local_address;
struct sockaddr_in remote_address;

static Message temp_message;
static Message * temp_message_ptr;
static Message_Type * ignored_message_type;
static Message temp_tx_message;

static int receive_head;
static int receive_tail;
static int receive_size;

////Local Prototypes///////////////////////////////////////////////
static int32 Open_Listen_Connection(char * local_addr_string,
		struct sockaddr_in * local_addr);
static int32 Listen(int32 listen_socket, char * remote_addr_string,
		struct sockaddr_in * remote_addr);
static int32 Open_Data_Connection(struct sockaddr_in * remote_addr);

static bool Initiate_Connection_For_Outgoing_Message();
static bool Dequeue_And_Transmit(int32 data_sock);
static bool Receive_And_Enqueue(int32 data_sock);

static bool Send_Message(int32 destination_socket, Message * m);
static int32 Receive(int32 source_socket, char * destination,
		uint32 receive_length_max);

static bool Check_Needs_Promotion();
static uint8_t * memchr2(uint8_t * ptr, uint8_t ch, size_t size);
static void Connect_Task(void * PvParams);
static void TCP_Disconnect();

////Global implementations ////////////////////////////////////////
bool ICACHE_RODATA_ATTR TCP_Combined_Init()
{

	idle_handle = xTaskGetIdleTaskHandle();

	bool rval = true;
	promoted = false;

	taskENTER_CRITICAL();
	local_address_string = zalloc(ADDR_STRING_SIZE);
	remote_address_string = zalloc(ADDR_STRING_SIZE);
	receive_data = zalloc(RECEIVE_DATA_SIZE);
	transmit_data = zalloc(TRANSMIT_DATA_SIZE);
	taskEXIT_CRITICAL();

	taskYIELD();

	xTaskHandle TCP_combined_handle;

	//TODO: print high water mark and revise stack size if necessary.
	xTaskCreate(TCP_Combined_Task, "TCP combined", 256, NULL,
			Get_Task_Priority(TASK_TYPE_TCP_RX), &TCP_combined_handle);

	System_Register_Task(TASK_TYPE_TCP_RX, TCP_combined_handle,
			Check_Needs_Promotion);

	return rval;
}

void ICACHE_RODATA_ATTR TCP_Combined_Deinit()
{
	connected = false;

	lwip_close(data_sock);
	lwip_close(listen_sock);

	listen_sock = -1;
	data_sock = -1;

	receive_head = 0;
	receive_tail = 0;
	receive_size = 0;

	free(local_address_string);
	free(remote_address_string);
	free(receive_data);
	free(transmit_data);

	Stop_Task(TASK_TYPE_TCP_RX);
}

static int tcpLoops = 0;

void ICACHE_RODATA_ATTR TCP_Combined_Task()
{
	for (;;)
	{
//		DEBUG_Print("top of combined");
		connected = false;

		//we should reconsider recreating the listen socket every time.

#if ENABLE_TCP_COMBINED_RX
		//open listener
		while ((listen_sock = Open_Listen_Connection(local_address_string,
				&local_address)) < 0)
		{
//			if (++tcpLoops > 30)
//			{
//				tcpLoops = 0;
//				DEBUG_Print("opening listen");
//			}

			taskYIELD();
		}
#endif

		//listen for incoming connection, start a connection if we have an outgoing message.
		while (!connected)
		{
//			if (++tcpLoops > 50)
//			{
//				tcpLoops = 0;
//				DEBUG_Print("listening");
//			}

#if ENABLE_TCP_COMBINED_RX
			data_sock = Listen(listen_sock, remote_address_string,
					&remote_address);
			connected = data_sock > -1;
#endif

#if ENABLE_TCP_COMBINED_TX
			if (!connected)
			{
				connected = Initiate_Connection_For_Outgoing_Message();
			}
#endif

//			if (connected)
//			{
//				DEBUG_Print("connected, now yield, then go to sending.");
//			}

			taskYIELD();
		}

		receive_head = 0;
		receive_tail = 0;
		receive_size = 0;

		while (connected)
		{
//			if (++tcpLoops > 30)
//			{
//				tcpLoops = 0;
//				DEBUG_Print("connected");
//			}

#if ENABLE_TCP_COMBINED_TX
			connected = Dequeue_And_Transmit(data_sock);

//			if (!connected)
//			{
//				DEBUG_Print("dqt disconnect\r\n\r\n");
//			}
#endif

#if ENABLE_TCP_COMBINED_RX
			connected &= Receive_And_Enqueue(data_sock);
#endif

			taskYIELD();
		}

	}

}

////Local implementations ////////////////////////////////////////
//initialize listener socket. Set up timeout too.
static int32 ICACHE_RODATA_ATTR Open_Listen_Connection(char * local_addr_string,
		struct sockaddr_in * local_addr)
{
	uint16 server_port = LOCAL_TCP_PORT;

	int32 listen_socket = -1;
	int32 bind_result;
	int32 listen_result;

//	DEBUG_Print("olc");

	taskENTER_CRITICAL();
	memset(local_addr, 0, sizeof(struct sockaddr_in));
	taskEXIT_CRITICAL();

//	DEBUG_Print("did memset");

	taskYIELD();

	local_addr->sin_family = AF_INET;
	local_addr->sin_addr.s_addr = INADDR_ANY;
	local_addr->sin_len = sizeof(*local_addr);
	local_addr->sin_port = htons(server_port);

//	DEBUG_Print_Address(local_addr, "listen addr");

	taskENTER_CRITICAL();
	Serialize_Address(Get_IP_Address(), ntohs(local_addr->sin_port),
			local_addr_string, 50);
	taskEXIT_CRITICAL();

//	DEBUG_Print("serialized");

	taskYIELD();

	listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listen_socket > -1)
	{
//		DEBUG_Print("sock open ok");

		/* Bind to the local port */
		bind_result = lwip_bind(listen_socket, (struct sockaddr *) local_addr,
				sizeof(*local_addr));

		taskYIELD();

		if (bind_result != 0)
		{
			lwip_close(listen_socket);
			listen_socket = -1;

//			DEBUG_Print("bind fail");

		}
		else
		{
//			DEBUG_Print("bind ok");

			int millis = LISTEN_TIMEOUT_ms;
			lwip_setsockopt(listen_socket, SOL_SOCKET, SO_RCVTIMEO, &millis,
					sizeof(millis));
		}
	}

	taskYIELD();

	if (listen_socket > -1)
	{
		listen_result = lwip_listen(listen_socket, TCP_MAX_CONNECTIONS);

		if (listen_result != 0)
		{
			int32 error = 0;
			uint32 optionLength = sizeof(error);
			int getReturn = lwip_getsockopt(listen_socket, SOL_SOCKET,
			SO_ERROR, &error, &optionLength);

//			taskENTER_CRITICAL();
//			printf("listen fail:%d", error);
//			taskEXIT_CRITICAL();

			lwip_close(listen_socket);
			listen_socket = -1;
		}
	}

	taskYIELD();

	return listen_socket;
}

//accepts connections
static int32 ICACHE_RODATA_ATTR Listen(int32 listen_socket,
		char * remote_addr_string, struct sockaddr_in * remote_addr)
{
	int32 accepted_sock;
	int32 len = sizeof(struct sockaddr_in);

	if ((accepted_sock = lwip_accept(listen_socket,
			(struct sockaddr *) remote_addr, (socklen_t *) &len)) < 0)
	{
		int32 error = 0;
		uint32 option_length = sizeof(error);
		int getReturn = lwip_getsockopt(listen_socket, SOL_SOCKET, SO_ERROR,
				&error, &option_length);

		accepted_sock = -1;
	}
	else
	{
		//Set receive timeout.
		int millis = TCP_RECEIVE_CONNECTION_TIMEOUT_ms;
		lwip_setsockopt(accepted_sock, SOL_SOCKET, SO_RCVTIMEO, &millis,
				sizeof(millis));

		taskYIELD();

		taskENTER_CRITICAL();
		Serialize_Address(remote_addr->sin_addr.s_addr,
				ntohs(remote_addr->sin_port), remote_addr_string, 50);
		taskEXIT_CRITICAL();
	}

	taskYIELD();

	return accepted_sock;
}

//Message peek_message =
//		{ "192.168.1.21:3000", "192.168.1.3:1002",
//				"mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm%d",
//				"tcp" };
//Message * Peeker()
//{
//	vTaskDelay(1000 / portTICK_RATE_MS);
//	return &peek_message;
//}

static bool ICACHE_RODATA_ATTR Initiate_Connection_For_Outgoing_Message()
{
	bool rval = false;

	taskENTER_CRITICAL();
	//do not dequeue. leave the message in the queue to be processed by the send function..
	temp_message_ptr = Peek_Message(&outgoing_TCP_message_queue);
//	temp_message_ptr = Peeker();

	if (temp_message_ptr != NULL)
	{
		Initialize_Message(&temp_message, temp_message_ptr->type,
				temp_message_ptr->source, temp_message_ptr->destination,
				temp_message_ptr->content);
	}
	taskEXIT_CRITICAL();

	taskYIELD();

	if (temp_message_ptr != NULL)
	{
		temp_message_ptr = NULL;

		taskENTER_CRITICAL();
		Deserialize_Address(temp_message.destination, &remote_address,
				ignored_message_type);
		taskEXIT_CRITICAL();

		taskYIELD();

		taskENTER_CRITICAL();
		sprintf(remote_address_string, temp_message.destination);
//		printf("message for %s\r\n", remote_address_string);
		taskEXIT_CRITICAL();

		taskYIELD();

		data_sock = Open_Data_Connection(&remote_address);

//		taskENTER_CRITICAL();
//		printf("odc:%d\r\n", data_sock);
//		taskEXIT_CRITICAL();

		taskYIELD();

		rval = data_sock > -1;

		if (rval)
		{
//			DEBUG_Print("connected");

			socklen_t sockaddr_size = sizeof(local_address);
			getsockname(data_sock, (struct sockaddr* )&local_address,
					&sockaddr_size);

			taskYIELD();

			taskENTER_CRITICAL();
			Serialize_Address(local_address.sin_addr.s_addr,
					local_address.sin_port, local_address_string,
					ADDR_STRING_SIZE);
			taskEXIT_CRITICAL();

//			DEBUG_Print(local_address_string);
		}
		else
		{
			//couldn't connect. drop this message
			taskENTER_CRITICAL();
			Dequeue_Message(&outgoing_TCP_message_queue, NULL);
			taskEXIT_CRITICAL();

//			DEBUG_Print("drop on connect");
		}
		taskYIELD();

//		DEBUG_Print("returning\r\n\r\n");

	}

	return rval;
}

//used to initiate connection.
static ICACHE_RODATA_ATTR int32 Open_Data_Connection(
		struct sockaddr_in * remote_addr)
{
	int opened_socket;

	opened_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (opened_socket != -1)
	{
		int retries = 0;
		int millis = CONNECT_TIMEOUT_ms;
		setsockopt(opened_socket, SOL_SOCKET, SO_RCVTIMEO, &millis,
				sizeof(millis));

//		DEBUG_Print("sock opt done");

		taskYIELD();

//		DEBUG_Print("create connect args");

		Connect_Task_Args connect_args =
		{ opened_socket, remote_addr, false };
		xTaskHandle connect_task_handle = NULL;
		while (connect_task_handle == NULL)
		{
			//start new task to connect

//			DEBUG_Print("create connect task");

//high water mark recorded as 28 bytes. Leaving this stack at 128 for safety.
			xTaskCreate(Connect_Task, "tcp connect task",
					configMINIMAL_STACK_SIZE, &connect_args,
					Get_Task_Priority(TASK_TYPE_TCP_TX), &connect_task_handle);

			if (connect_task_handle == NULL)
			{
				//raise priority so we make sure connect task gets cleaned up.
				xTaskHandle idle_handle = xTaskGetIdleTaskHandle();

				vTaskPrioritySet(idle_handle,
						Get_Task_Priority(TASK_TYPE_TCP_TX));
				vTaskDelay(250 / portTICK_RATE_MS);
				vTaskPrioritySet(idle_handle, 0);
			}
		}

		if (connect_task_handle != NULL)
		{
			uint32 connect_start_time_us = system_get_time();

			while (!connect_args.connected
					&& (system_get_time() - connect_start_time_us)
							< CONNECT_RETRY_TIME)
			{
				vTaskDelay(10 / portTICK_RATE_MS);
			}
		}

		if (connect_args.connected)
		{
			//task doesn't need to be deleted in this case. if we connected
			//		by now, then the task has deleted itself.

			millis = TCP_RECEIVE_CONNECTION_TIMEOUT_ms;
			setsockopt(opened_socket, SOL_SOCKET, SO_RCVTIMEO, &millis,
					sizeof(millis));

			//not sure if this actually was helping. Leaving it so I don't forget about it.
//			vTaskDelay(5 / portTICK_RATE_MS);
		}
		else
		{
//			taskENTER_CRITICAL();
//			printf("delete handle: %d\r\n", (int32) connect_task_handle);
//			taskEXIT_CRITICAL();

			vTaskDelete(connect_task_handle);

			taskYIELD();

			vTaskPrioritySet(idle_handle, Get_Task_Priority(TASK_TYPE_TCP_TX));
			vTaskDelay(250 / portTICK_RATE_MS);
			vTaskPrioritySet(idle_handle, 0);

//			DEBUG_Print("deleted handle");

//			DEBUG_Print("close sock");

			lwip_close(opened_socket);
			opened_socket = -1;

//			DEBUG_Print("sock closed");
		}
	}

	return opened_socket;
}

static ICACHE_RODATA_ATTR void Connect_Task(void * PvParams)
{
	((Connect_Task_Args*) PvParams)->connected =
			connect(((Connect_Task_Args* )PvParams)->socket,
					(struct sockaddr * )(((Connect_Task_Args* )PvParams)->address),
					sizeof(struct sockaddr_in)) == 0;

	vTaskDelete(NULL);
}

//send a message to the
static ICACHE_RODATA_ATTR bool Send_Message(int32 destination_socket,
		Message * m)
{
	bool rval = false;
	struct sockaddr_in message_dest;

//	DEBUG_Print("sending message");
//	DEBUG_Print(m->destination);
//	DEBUG_Print(remote_address_string);

	taskENTER_CRITICAL();
	Deserialize_Address(m->destination, &message_dest, ignored_message_type);
	taskEXIT_CRITICAL();

	taskYIELD();

	//TODO: shouldn't a strcmp work here instead? save ourselves the deserialization.
	if (message_dest.sin_addr.s_addr == remote_address.sin_addr.s_addr
			&& message_dest.sin_port == remote_address.sin_port)
	{
//		DEBUG_Print("message matches");

		taskENTER_CRITICAL();
		size_t length = (size_t) (strlen(m->content) + 1);
		taskEXIT_CRITICAL();

		taskYIELD();

//		taskENTER_CRITICAL();
//		printf("sock:%d,size:%d,msg:%s\r\n", destination_socket, length,
//				m->content);
//		taskEXIT_CRITICAL();

//		DEBUG_Print("tx");

		taskYIELD();

		rval = lwip_write(destination_socket, m->content, length) == length;

		taskYIELD();

//		DEBUG_Print(rval ? "ok" : "nfg");

		if (!rval)
		{
			int32 error = 0;
			uint32 optionLength = sizeof(error);
			int getReturn = lwip_getsockopt(destination_socket, SOL_SOCKET,
			SO_ERROR, &error, &optionLength);

//			taskENTER_CRITICAL();
//			printf("error:%d\r\n", error);
//			taskEXIT_CRITICAL();

			switch (error)
			{
			case ECONNRESET:
			{
				//we disconnected. return false.
				TCP_Disconnect();
				break;
			}
			case EAGAIN:
			{
				rval = true;

				break;
			}
			default:
			{
				rval = true;
				break;
			}
			}
		}
	}
	else
	{
		rval = true;
//		DEBUG_Print("no match, drop");
	}

	return rval;
}

//returns size of data put into message, or -1 if the connection was reset.
static ICACHE_RODATA_ATTR int Receive(int32 source_socket, char * destination,
		uint32 receive_length_max)
{
//	taskENTER_CRITICAL();
//	printf("rx up to %d. write to %d\r\n", receive_length_max,
//			(uint32) destination);
//	taskEXIT_CRITICAL();

	int rval = lwip_recv(source_socket, destination, receive_length_max, 0);

//	taskENTER_CRITICAL();
//	printf("rx'd %d\r\n", rval);
//	taskEXIT_CRITICAL();

	if (rval < 0)
	{

		int32 error = 0;
		uint32 optionLength = sizeof(error);
		int getReturn = lwip_getsockopt(source_socket, SOL_SOCKET, SO_ERROR,
				&error, &optionLength);

//		taskENTER_CRITICAL();
//		printf("error:%d\r\n", error);
//		taskEXIT_CRITICAL();

		switch (error)
		{
		case ECONNRESET:
		{
			rval = -1;
			break;
		}
		case EAGAIN:
		{
			rval = 0;

			break;
		}
		default:
		{
			break;
		}
		}
	}

	return rval;
}

//static char type_str[] = "tcp";
//static char dest_addr[] = "192.168.1.3:1002";
//static char source_addr[] = "192.168.1.21:1002";
//static char message_content_template[] =
//		"mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm%d";
//static char message_content[256];
//
//static int message_counter = 0;
//
//static bool Create_Test_Message(Message * destination)
//{
//	bool rval = true;
//
//	vTaskDelay(100 / portTICK_RATE_MS);
//
//	taskENTER_CRITICAL();
//
//	sprintf(message_content, message_content_template, ++message_counter);
//
//	Initialize_Message(destination, type_str, local_address_string, dest_addr,
//			message_content);
//
//	taskEXIT_CRITICAL();
//	taskYIELD();
//
//	return rval;
//}

static bool ICACHE_RODATA_ATTR Dequeue_And_Transmit(int32 data_sock)
{
	bool rval = false;

//	rval = Create_Test_Message(&temp_tx_message);

	taskENTER_CRITICAL();
	rval = Dequeue_Message(&outgoing_TCP_message_queue, &temp_tx_message);
	taskEXIT_CRITICAL();

	if (rval)
	{
		taskYIELD();

		rval = Send_Message(data_sock, &temp_tx_message);

		taskYIELD();

//		DEBUG_Print(rval ? "send ok" : "send nfg");
	}
	else
	{
		//we have no reason to think we've been disconnected.
		rval = true;
	}

	return rval;
}

static bool ICACHE_RODATA_ATTR Receive_And_Enqueue(int32 data_sock)
{
	bool rval = true;

//	taskENTER_CRITICAL();
//	printf("rx params: t:%d,s:%d\r\n", receive_tail,
//			((receive_tail > receive_head
//					|| (receive_tail == receive_head && receive_size == 0)) ?
//					(RECEIVE_DATA_SIZE - receive_tail) :
//					RECEIVE_DATA_SIZE - receive_size));
//	taskEXIT_CRITICAL();

//offset into receive_data based on
	received_count = Receive(data_sock, receive_data + receive_tail,
			((receive_tail > receive_head
					|| (receive_tail == receive_head && receive_size == 0)) ?
					(RECEIVE_DATA_SIZE - receive_tail) :
					RECEIVE_DATA_SIZE - receive_size));

//	taskENTER_CRITICAL();
//	printf("rx c%d,h%d,t%d,s%d\r\n", received_count, receive_head, receive_tail,
//			receive_size);
//	taskEXIT_CRITICAL();

	if (received_count > 0 || (receive_size > 0 && received_count > -1))
	{
		receive_tail = (receive_tail + received_count) % RECEIVE_DATA_SIZE;
		receive_size += received_count;

//		taskENTER_CRITICAL();
//		printf("hd c%d,h%d,t%d,s%d\r\n", received_count, receive_head,
//				receive_tail, receive_size);
//		taskEXIT_CRITICAL();

		taskYIELD();

		//if head is not less than tail, only search until end of buffer.
		taskENTER_CRITICAL();
		message_ptr = memchr2(receive_data + receive_head, message_end[0],
				(receive_head < receive_tail ?
						receive_size : RECEIVE_DATA_SIZE - receive_head));
		taskEXIT_CRITICAL();

		taskYIELD();

		//if we didn't find the end and the queue has wrapped, then search before the head for the terminator.
		if (message_ptr == NULL
				&& (receive_tail < receive_head
						|| (receive_tail == receive_head
								&& receive_size == RECEIVE_DATA_SIZE)))
		{
			taskENTER_CRITICAL();
			message_ptr = memchr2(receive_data, message_end[0], receive_tail);
			taskEXIT_CRITICAL();

			taskYIELD();

//			taskENTER_CRITICAL();
//			printf("wa c%d,h%d,t%d,s%d\r\n", received_count, receive_head,
//					receive_tail, receive_size);
//			UART_WaitTxFifoEmpty(UART0);
//			taskEXIT_CRITICAL();

			if (message_ptr != NULL)
			{
//				taskENTER_CRITICAL();
//				printf("fm c%d,h%d,t%d,s%d\r\n", received_count, receive_head,
//						receive_tail, receive_size);
//				UART_WaitTxFifoEmpty(UART0);
//				taskEXIT_CRITICAL();

				*message_ptr = '\0';
				taskYIELD();

//				DEBUG_Print("swap");

				taskENTER_CRITICAL();
				char * swap = zalloc(strlen(message_ptr) + 1);
				strcpy(swap, receive_data);
				taskEXIT_CRITICAL();

				taskYIELD();

				taskENTER_CRITICAL();
				strncpy(receive_data, receive_data + receive_head,
				RECEIVE_DATA_SIZE - receive_head + 1);
				taskEXIT_CRITICAL();

				taskYIELD();

				taskENTER_CRITICAL();
				strcat(receive_data, swap);
				taskEXIT_CRITICAL();

				free(swap);

//				DEBUG_Print("swap done");

				taskYIELD();

				taskENTER_CRITICAL();
				receive_tail = (strlen(receive_data) + 1); //had a newline on it when we received it.
				taskEXIT_CRITICAL();

				receive_head = 0;
				receive_size = receive_tail;

//				taskENTER_CRITICAL();
//				printf("pd c%d,h%d,t%d,s%d\r\n", received_count, receive_head,
//						receive_tail, receive_size);
//				UART_WaitTxFifoEmpty(UART0);
//				taskEXIT_CRITICAL();

				message_ptr = receive_data;
			}
			else if (receive_size == RECEIVE_DATA_SIZE)
			{
				//buffer's full and no terminator was found.
				receive_head = 0;
				receive_tail = 0;
				receive_size = 0;
			}
		}
		else if (message_ptr != NULL)
		{
//			taskENTER_CRITICAL();
//			printf("found msg at %d\r\n",
//					(uint32) (message_ptr - receive_data));
//			UART_WaitTxFifoEmpty(UART0);
//			taskEXIT_CRITICAL();

			*message_ptr = '\0';
			message_ptr = receive_data + receive_head;
		}

		if (message_ptr != NULL)
		{
//			taskENTER_CRITICAL();
//			printf("msg:[%s]\r\n", message_ptr);
//			taskEXIT_CRITICAL();

			taskENTER_CRITICAL();
			//+ 1 because the message had a newline on it when we received it.
			receive_size -= (strlen(message_ptr) + 1);
			receive_head = (receive_head + strlen(message_ptr) + 1)
					% RECEIVE_DATA_SIZE;
			taskEXIT_CRITICAL();

			taskYIELD();

//			taskENTER_CRITICAL();
//			printf("nq c%d,h%d,t%d,s%d\r\n", received_count, receive_head,
//					receive_tail, receive_size);
//			UART_WaitTxFifoEmpty(UART0);
//			taskEXIT_CRITICAL();

			taskENTER_CRITICAL();
			Initialize_Message(&temp_message,
					message_type_strings[MESSAGE_TYPE_TCP],
					remote_address_string, local_address_string, message_ptr);
			taskEXIT_CRITICAL();

			taskYIELD();

			taskENTER_CRITICAL();
			Queue_Message(&incoming_message_queue, &temp_message);
			taskEXIT_CRITICAL();

			taskYIELD();
		}
	}
	else if (received_count < 0)
	{
		//Assume tcp queue has messages for the open socket. We clear
		//	it so we don't waste a bunch of cpu trying to reopen the
		//	connection.

//		DEBUG_Print("rx closed");

		TCP_Disconnect();

		rval = false;
	}

	return rval;

}

static void ICACHE_RODATA_ATTR TCP_Disconnect()
{
	taskENTER_CRITICAL();
	Initialize_Message_Queue(&outgoing_TCP_message_queue);
	taskEXIT_CRITICAL();

	lwip_close(data_sock);
	lwip_close(listen_sock); //TODO: this probably isn't necessary. review main loop

	listen_sock = -1;
	data_sock = -1;
}

static int loops = 0;

static bool ICACHE_RODATA_ATTR Check_Needs_Promotion()
{
	bool rval = false;

	//remain promoted until we empty the queue.
	taskENTER_CRITICAL();
	rval =
			outgoing_TCP_message_queue.count
					> (promoted ? 0 : MESSAGE_TRIGGER_LEVEL)|| receive_size > RECEIVE_BYTES_TRIGGER_LEVEL;
	taskEXIT_CRITICAL();

//	if (++loops > LOOPS_BEFORE_PRINT || rval
//			|| outgoing_TCP_message_queue.count)
//	{
//		loops = 0;
//
//#if ENABLE_TCP_COMBINED_TX
//		taskENTER_CRITICAL();
//		printf("\r\ntcp tx:%d\r\n", outgoing_TCP_message_queue.count);
//		taskEXIT_CRITICAL();
//		taskYIELD();
//#endif
//
//#if ENABLE_TCP_COMBINED_RX
//		taskENTER_CRITICAL();
//		printf("rxbytes:%d\r\n", receive_size);
//		taskEXIT_CRITICAL();
//		taskYIELD();
//#endif
//		taskENTER_CRITICAL();
//		printf("tcp %s\r\n", connected ? "connected" : "nc");
//		taskEXIT_CRITICAL();
//		taskYIELD();
//
//	}

	promoted = rval;

	return rval;
}

uint8_t ICACHE_RODATA_ATTR *memchr2(uint8_t *ptr, uint8_t ch, size_t size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if (*ptr == ch)
		{
			return ptr;
		}
		else
		{
			++ptr;
		}
	}

	return NULL;
}
