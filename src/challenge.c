#include <FreeRTOS.h>
#include <task.h>
#include <console.h>

uint8_t *raw_buffer;
uint8_t temp_message_type;
uint32_t temp_message_length;
uint8_t temp_message_value;
uint32_t index = 0;

/**
 * Call this to "send" data over the (simulated) serial interface.
 * @param message message buffer
 * @param length length of the message
 */
void send(uint8_t* message, uint32_t length);

/**
 * This will get called for each byte of data received.
 * @param data received byte
 */
void receive_ISR(uint8_t data) {

	*raw_buffer++ = data;
	temp_message_type = (uint8_t)((raw_buffer[0] & 0xF0)>> 4);
	temp_message_length = (uint8_t)(raw_buffer[0] & 0x0F);
	index++;

}

/**
 * Initialize challenge. This is called once on startup, before any interrupts can occur.
 */
void challenge_init() {
	// disable all global interrupt

}

/**
 * Main function for the coding challenge. Must not return.
 *
 * Please use the following API functions if necessary:
 *
 * print string to stdout
 * console_print("format %d", 123);
 *
 * millisecond delay
 * vTaskDelay(123);
 *
 * get elapsed milliseconds
 * TickType_t ms = xTaskGetTickCount();
 */
void challenge_run() {
	uint32_t start = xTaskGetTickCount();
	uint16_t num1, num2 ;
	uint32_t result;
	while(1)
	{ 
		if ((temp_message_type == 1) && (temp_message_length == 0))
		{
			console_print(" Received: Empty message\n");
			vTaskDelay(123);
			send(raw_buffer,0);
			console_print(" Sent: Empty message\n");
			index = 0;
		}

		if ((temp_message_type == 2) && (temp_message_length ==4) && (index == 4))
		{
			console_print(" Received: Add message\n");
			vTaskDelay(123);
			num1 = (uint16_t)(raw_buffer[1]) + (uint16_t)(raw_buffer[2]);
			num2 = (uint16_t)(raw_buffer[3]) + (uint16_t)(raw_buffer[4]);
			result = num1 + num2;
			raw_buffer[1] = (result >> 31) & 0xFF;
			raw_buffer[2] = (result >> 16) & 0xFF;
			raw_buffer[3] = (result >> 8) & 0xFF;
			raw_buffer[4] = (result >> 0) & 0xFF;
			send(raw_buffer, 4);
			console_print(" Sent:  Add message\n");

		}


		if ((temp_message_type == 3) && (temp_message_length == 2) )
		{
		
			console_print(" Received: Result message\n");
		}


		if ((temp_message_type == 4) && (temp_message_length ==3) && (index == 3))
		{
			uint16_t delay;
			console_print(" Received: Delay message\n");
			vTaskDelay(123);
			delay = (uint16_t)(raw_buffer[1]) + (uint16_t)(raw_buffer[2]);
			uint16_t elapsed = xTaskGetTickCount() - start;
			if (delay > 0 && delay == elapsed)
			{
				raw_buffer[1] = raw_buffer[3];
				raw_buffer[2] = 0x00;
				send(raw_buffer, 3);
			}
			else
			{
				console_print("ERROR: delay %u ms (expected %u ms)\n", elapsed, delay);
			}
		}
		if ((temp_message_type == 5) && (temp_message_length == 1))
		{
		
			console_print(" Received: Timeout message\n");
		}

		if ((temp_message_type == 6) && (temp_message_length > 4))
		{
			console_print(" Received: Log message\n");
		}

	}

}
