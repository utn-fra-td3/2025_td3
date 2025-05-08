#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "pico/cyw43_arch.h"

TaskHandle_t Blinky_On;
TaskHandle_t Blinky_Off;

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
void task_blinky_off(void *params) {
    
    while(1) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        // Demora de ticks equivalentes a 500 ms
        
        vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskResume(Blinky_On);
        vTaskSuspend(Blinky_Off);
    }
}
void task_blinky_on(void *params) {
    
    while(1) {
        // Toggle
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        // Demora de ticks equivalentes a 500 ms
        
        vTaskDelay(pdMS_TO_TICKS(1500));
        vTaskResume(Blinky_Off);
        vTaskSuspend(Blinky_On);
    }
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_blinky_on, "Blinky_ON", configMINIMAL_STACK_SIZE, NULL, 1,  &Blinky_On);
    xTaskCreate(task_blinky_off, "Blinky_OFF", configMINIMAL_STACK_SIZE, NULL, 1,  &Blinky_Off);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}