#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Tarea de inicializacion
 */
void task_init(void *params) {
    // Inicializacion de GPIO
    cyw43_arch_init();

    // Elimino la tarea para liberar recursos
    vTaskDelete(NULL);
}

/**
 * @brief Tarea de blinky de LED
 */
void task_blinky(void *params) {
    
    while(1) {
        // Toggle
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN));
        // Demora de ticks equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_blinky, "Blinky", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}