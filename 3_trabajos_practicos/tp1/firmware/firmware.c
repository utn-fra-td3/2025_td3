#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

// Declaración de los handles de tareas
TaskHandle_t handle_Tarea1 = NULL;
TaskHandle_t handle_Tarea2 = NULL;

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
 * @brief Tarea1 de blinky de LED
 */
void task_Tarea1(void *params) {
    
    while(1) {
        // Toggle
        vTaskSuspend(handle_Tarea2);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        // Demora de ticks equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));
        vTaskResume(handle_Tarea2);
        vTaskSuspend(handle_Tarea1);
        //TENGO QUE BLOQUEAR LA TAREA
    }
}

/**
 * @brief Tarea2 de blinky de LED
 */
void task_Tarea2(void *params) {
    
    while(1) {
        // Toggle
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        // Demora de ticks equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));
        vTaskResume(handle_Tarea1);
    }
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(task_Tarea1, "Tarea1", configMINIMAL_STACK_SIZE, NULL, 2, &handle_Tarea1);
    xTaskCreate(task_Tarea2, "Tarea2", configMINIMAL_STACK_SIZE, NULL, 1, &handle_Tarea2);
    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}