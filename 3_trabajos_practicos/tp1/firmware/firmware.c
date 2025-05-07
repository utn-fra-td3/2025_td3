#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#define on 1000
#define off 1500

/**
 * @brief Tarea de blinky de LED
 */
void task_blinky_on(void *params) {
    
    while(1) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        // Demora de ticks equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(on+off));
    }
}

void task_blinky_off(void *params) {

    vTaskDelay(pdMS_TO_TICKS(on));
    while(1) {
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    // Demora de ticks equivalentes a 500 ms
    vTaskDelay(pdMS_TO_TICKS(off+on));
    }
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();
    // Inicializacion de GPIO
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    // Creacion de tareas
    xTaskCreate(task_blinky_on, "On", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task_blinky_off, "Off", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}