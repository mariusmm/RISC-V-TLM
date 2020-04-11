#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include<stdio.h>

#define TRACE (*(unsigned char *)0x40000000)

extern void register_timer_isr();

QueueHandle_t my_queue = NULL;

static void task_1(void *pParameter) {
    
        int data = 5;
	printf("Task 1 starts\n");

	while(1) {
		//printf("T1: Tick %ld\n",  xTaskGetTickCount() );
                xQueueSend(my_queue, &data, portMAX_DELAY);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

static void task_2(void *pParameter) {
    
        int data = 7;
        
	printf("Task 2 starts\n");

	while(1) {
		//printf("T2: Tick %ld\n",  xTaskGetTickCount() );
                xQueueSend(my_queue, &data, portMAX_DELAY);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}

static void task_3(void *pParameter) {
        int data;
        
 	printf("Task 3 starts\n");

	while(1) {
                xQueueReceive(my_queue, &data, portMAX_DELAY);
		printf("T3: Tick %ld. Recv: %ld\n",  xTaskGetTickCount(), data);
		//vTaskDelay(1000 / portTICK_PERIOD_MS);
	}   
    
}

int main( void )
{

	printf("Starting FreeRTOS test\n");

        my_queue = xQueueCreate(10, sizeof(int));
        
        /* Create tasks */
        xTaskCreate(task_1, "Task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
	xTaskCreate(task_2, "Task2", 10000, NULL, tskIDLE_PRIORITY+1, NULL);
        xTaskCreate(task_3, "Task3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
        
	/* Start the kernel.  From here on, only tasks and interrupts will run. */
	vTaskStartScheduler();

	/* Exit FreeRTOS */
	return 0;
}

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();

	TRACE='M';
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	TRACE = 'S';
	for( ;; );
}
/*-----------------------------------------------------------*/
