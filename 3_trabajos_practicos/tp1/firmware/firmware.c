#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t handle_Task_Encendido = NULL;
TaskHandle_t handle_Task_Apagado = NULL;

/**
 * @brief Tarea de blinky de LED
 */
void task_encendido(void *params) {
    
    while(1) {
        vTaskSuspend(handle_Task_Apagado);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        // Demora de ticks equivalentes a 1000 ms
        vTaskDelay(pdMS_TO_TICKS(500));
        vTaskResume(handle_Task_Apagado);
        vTaskSuspend(NULL);
    }
}

void task_apagado(void *params) {
    
    while(1) {
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        // Demora de ticks equivalentes a 1500 ms
        vTaskDelay(pdMS_TO_TICKS(500));
        vTaskResume(handle_Task_Encendido);
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
    xTaskCreate(task_encendido, "Task_Encendido", configMINIMAL_STACK_SIZE, NULL, 2, &handle_Task_Encendido);
    xTaskCreate(task_apagado, "Task_Apagado", configMINIMAL_STACK_SIZE, NULL, 1, &handle_Task_Apagado);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}