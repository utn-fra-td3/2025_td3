#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"

#define LED_ON 1000 // en ms
#define LED_OFF 1500 // en ms

/**
 * @brief Tareas de blinky de LED
 */
void task_blinky_on(void *params) {
	while(1){
    // Switch On
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
	// Mensaje
	//printf("\nPrendido");
    // Demora de ticks equivalentes a 500 ms
    vTaskDelay(pdMS_TO_TICKS(LED_ON + LED_OFF));
	}
}

void task_blinky_off(void *params) {
    vTaskDelay(pdMS_TO_TICKS(LED_ON));
    while(1){
	// Switch Off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
	// Mensaje
	//printf("\nApagado");
    // Demora de ticks equivalentes a 500 ms
    vTaskDelay(pdMS_TO_TICKS(LED_ON + LED_OFF));
	}
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Initialise the Wi-Fi chip
    cyw43_arch_init();

    // Creacion de tareas
    xTaskCreate(task_blinky_on, "Prendido", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_blinky_off, "Apagado", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}