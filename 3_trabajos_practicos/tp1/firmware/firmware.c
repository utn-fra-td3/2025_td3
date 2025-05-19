#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#define ON 1
#define OFF 0

TaskHandle_t xHandleLed_on, xHandleLed_off;

/**
 * Tarea de inicializacion
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
 * Tarea de encendido de LED
 */
void task_led_on(void *params) {
    
    while(1) {
        gpio_put(PICO_DEFAULT_LED_PIN, ON);
        // Demora de ticks equivalentes a 1000 ms
        vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskResume(xHandleLed_off);
        vTaskSuspend(NULL);
    }
}

/**
 * Tarea de apagado de LED
 */
void task_led_off(void *params) {
    
    while(1) {
        gpio_put(PICO_DEFAULT_LED_PIN, OFF);
        // Demora de ticks equivalentes a 1500 ms
        vTaskDelay(pdMS_TO_TICKS(1500));
        vTaskResume(xHandleLed_on);
        vTaskSuspend(NULL);
    }
}

int main()
{
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL); 
    xTaskCreate(task_led_on, "Encendido", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleLed_on);
    xTaskCreate(task_led_off, "Apagado", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleLed_off);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}

