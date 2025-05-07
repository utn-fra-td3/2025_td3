#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN 25
#define LED_ON 	1
#define LED_OFF	0

// Creo handles de tareas
TaskHandle_t hLedOn;
TaskHandle_t hLedOff;

// TAREA LED ON
static void task_LedOn(void *pvParameters){
	
	while(1){
		// Suspendo la tarea LedOff
		vTaskSuspend(hLedOff);

		// Enciendo el led
		gpio_put(LED_PIN, true);

		// Delay 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));

		// Habilito la tarea LedOff
		vTaskResume(hLedOff);

        // Suspendo la tarea LedOn para que entre LedOff
        vTaskSuspend(NULL);
	}
}

// TAREA LED OFF
static void task_LedOff(void *pvParameters){
	
	while(1){
		// Apago el led
        gpio_put(LED_PIN, false);

		// Delay 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));

		// Habilito la tarea LedOn
		vTaskResume(hLedOn);
	}
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();
    
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);

    // Creacion de tareas
	xTaskCreate(task_LedOn, "LED-ON", configMINIMAL_STACK_SIZE, NULL, 2, &hLedOn);
	xTaskCreate(task_LedOff, "LED-OFF", configMINIMAL_STACK_SIZE, NULL, 1, &hLedOff);

    // START SCHEDULER
    vTaskStartScheduler();
    while(1);
}