// C++ includes
#include <iterator>
#include <portmacro.h>
// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
// Xilinx includes
#include "xil_printf.h"
#include "xparameters.h"
// Project includes
#include "detector.hpp"
#include "pynq_ssd_msg.hpp"

#define TIMER_ID	1
#define DELAY_10_SECONDS	10000UL
#define DELAY_1_SECOND		1000UL
#define TIMER_CHECK_THRESHOLD	9
/*-----------------------------------------------------------*/

/* The Tx and Rx tasks as described at the top of this file. */
/*-----------------------------------------------------------*/

/* The queue used by the Tx and Rx tasks, as described at the top of this
file. */
static TaskHandle_t xTxTask;
static TaskHandle_t xRxTask;
static QueueHandle_t xQueue = NULL;
static TimerHandle_t xPollTimer = NULL;
char HWstring[15] = "Hello World";
long RxtaskCntr = 0;

#if (configSUPPORT_STATIC_ALLOCATION == 1)
#define QUEUE_BUFFER_SIZE		100

uint8_t ucQueueStorageArea[ QUEUE_BUFFER_SIZE ];
StackType_t xStack1[ configMINIMAL_STACK_SIZE ];
StackType_t xStack2[ configMINIMAL_STACK_SIZE ];
StaticTask_t xTxBuffer,xRxBuffer;
StaticTimer_t xTimerBuffer;
static StaticQueue_t xStaticQueue;
#endif

void ZynqDetector::udp_rx_task( void *pvParameters )
{
    uint16_t op;
    uint16_t obj;
    udp_msg_t udp_msg;

    while(1)
    {
        // Read UDP packet
        op = udp_msg.op >> 14;
        uint16_t obj = udp_msg.op & 0x3F;
        switch( obj )
        {
            case MSG_VER:
                // send a fast_req to fast_access_task
                break;
            default:
                ;
        }
    }
}

void ZynqDetector::udp_tx_task( void *pvParameters )
{
    while(1)
    {
        active_resp_queue = ( resp_queue_set, portMAX_DELAY );

        if ( active_resp_queue == )
    }

}

void ZynqDetector::fast_access_task( void *pvParameters )
{
    
}

void ZynqDetector::slow_access_task( void *pvParameters )
{
    active_slow_req_queue  = ( slow_req_queue_set, portMAX_DELAY );

}

void ZynqDetector::asic_cfg_task( void *pvParameters )
{

}

ZynqDetector::ZynqDetector( void )
{
	const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );


	// Create queues
	fast_access_req_queue  = xQueueCreate( 100, sizeof( fast_access_req_t ) );
	slow_access_req_queue  = xQueueCreate( 100, sizeof( slow_access_req_t ) );
    asic_cfg_req_queue     = xQueueCreate( 4,   sizeof( asic_cfg_req_t ) );
	fast_access_resp_queue = xQueueCreate( 100, sizeof( fast_access_resp_t ) );
	slow_access_resp_queue = xQueueCreate( 100, sizeof( slow_access_resp_t ) );
    asic_cfg_resp_queue    = xQueueCreate( 4,   sizeof( asic_cfg_resp_t ) );

    // Create queue sets
    slow_req_queue_set  = xQueueCreateSet(
        SLOW_ACCESS_REQ_QUEUE_SIZE +
        ASIC_CFG_REQ_QUEUE_SIZE );

    xQueueAddToSet( slow_access_req_queue, slow_req_queue_set );
    xQueueAddToSet( asic_cfg_req_queue, slow_req_queue_set );

    resp_queue_set = xQueueCreateSet( 
        FAST_ACCESS_RESP_QUEUE_SIZE +
        SLOW_ACCESS_RESP_QUEUE_SIZE +
        ASIC_CFG_RESP_QUEUE_SIZE );

    xQueueAddToSet( fast_access_resp_queue, resp_queue_set );
    xQueueAddToSet( slow_access_resp_queue, resp_queue_set );
    xQueueAddToSet( asic_cfg_resp_queue, resp_queue_set );

    // Create tasks
	xTaskCreate( udp_rx_task, 				 // The function that implements the task.
                 ( const char * ) "UDP_RX",  // Text name for the task, provided to assist debugging only.
				 configMINIMAL_STACK_SIZE,   // The stack allocated to the task.
				 NULL, 					     // The task parameter is not used, so set to NULL.
				 tskIDLE_PRIORITY,			 // The task runs at the idle priority.
				 &udp_rx_task_handle );

	xTaskCreate( udp_tx_task,
				 ( const char * ) "UDP_TX",
				 configMINIMAL_STACK_SIZE,
				 NULL,
				 tskIDLE_PRIORITY + 1,
				 &udp_tx_task_handle );

	xTaskCreate( fast_access_task,
				 ( const char * ) "FAST_ACCESS",
				 configMINIMAL_STACK_SIZE,
				 NULL,
				 tskIDLE_PRIORITY + 1,
				 &fast_access_task_handle );

	xTaskCreate( slow_access_task,
				 ( const char * ) "SLOW_ACCESS",
				 configMINIMAL_STACK_SIZE,
				 NULL,
				 tskIDLE_PRIORITY + 1,
				 &slow_access_task_handle );

	/* Create a timer with a timer expiry of 10 seconds. The timer would expire
	 after 10 seconds and the timer call back would get called. In the timer call back
	 checks are done to ensure that the tasks have been running properly till then.
	 The tasks are deleted in the timer call back and a message is printed to convey that
	 the example has run successfully.
	 The timer expiry is set to 10 seconds and the timer set to not auto reload. */
	xPollTimer = xTimerCreate( (const char *) "Timer",
							   x1second,
							   pdFALSE,
							   (void *) TIMER_ID,
							   vTimerCallback);
	// Check timer creation
	configASSERT( xPollTimer );

	/* start the timer with a block time of 0 ticks. This means as soon
	   as the schedule starts the timer will start running and will expire after
	   10 seconds */
	xTimerStart( xPollTimer, 0 );

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}


/*-----------------------------------------------------------*/
static void prvTxTask( void *pvParameters )
{
const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );

	for( ;; )
	{
		/* Delay for 1 second. */
		vTaskDelay( x1second );

		/* Send the next value on the queue.  The queue should always be
		empty at this point so a block time of 0 is used. */
		xQueueSend( xQueue,			/* The queue being written to. */
					HWstring, /* The address of the data being sent. */
					0UL );			/* The block time. */
	}
}

/*-----------------------------------------------------------*/
static void prvRxTask( void *pvParameters )
{
char Recdstring[15] = "";

	for( ;; )
	{
		/* Block to wait for data arriving on the queue. */
		xQueueReceive( 	xQueue,				/* The queue being read. */
						Recdstring,	/* Data is read into this address. */
						portMAX_DELAY );	/* Wait without a timeout for data. */

		/* Print the received data. */
		xil_printf( "Rx task received string from Tx task: %s\r\n", Recdstring );
		RxtaskCntr++;
	}
}

/*-----------------------------------------------------------*/
static void poll_timer_callback( TimerHandle_t pxTimer )
{
	long lTimerId;
	configASSERT( pxTimer );

	lTimerId = ( long ) pvTimerGetTimerID( pxTimer );

	if (lTimerId != TIMER_ID) {
		xil_printf("FreeRTOS Hello World Example FAILED");
	}

    if( std::size(poll_list) != 0 )
    {}
	
}

