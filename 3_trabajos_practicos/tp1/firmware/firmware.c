#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Tarea de inicializacion
 */
void task_init(void *params) {
    // Inicializacion de GPIO
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    // Elimino la tarea para liberar recursos
    vTaskDelete(NULL);
}

/**
 * @brief Tarea de blinky de LED
 */
void task_encendido(void *params) {

    vTaskDelay(pdMS_TO_TICKS(1000)); 

    while(1) {
        // Toggle
        gpio_put(PICO_DEFAULT_LED_PIN, 0);

        // Demora de ticks equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(2500));
    }
}
void task_apagado(void *params) {

   

    while(1) {
        // Toggle
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        // Demora de ticks equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(2500));
    }
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_encendido, "Blinky", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task_apagado, "Blinky", configMINIMAL_STACK_SIZE, NULL, 1, NULL);


    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}