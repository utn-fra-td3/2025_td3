#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "pico/cyw43_arch.h"

/**
 * @brief Tarea de inicializacion
 */
void task_init(void *params) {
    // Inicializacion de GPIO
    cyw43_arch_init();
    // Elimino la tarea para liberar recursos
    vTaskDelete(NULL);
}

TaskHandle_t xHandle_ON;
TaskHandle_t xHandle_OFF;

/**
 * @brief Tarea de blinky de LED
 */
void task_blinky_on(void *params) {
    
    while(1) {
        // Toggle
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        
        
        //vTaskPrioritySet(xHandle_ON,1);
        //vTaskPrioritySet(xHandle_OFF,2);
        
        vTaskDelay(pdMS_TO_TICKS(500));
        vTaskResume(xHandle_OFF);
        vTaskSuspend(xHandle_ON);
        // Demora de ticks equivalentes a 500 ms
        
    }
}

void task_blinky_off(void *params) {
    
    while(1) {
        // Toggle
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        
        
        //vTaskPrioritySet(xHandle_ON,2);
        //vTaskPrioritySet(xHandle_OFF,1);
        
        vTaskDelay(pdMS_TO_TICKS(500));
        vTaskResume(xHandle_ON);
        vTaskSuspend(xHandle_OFF);
        // Demora de ticks equivalentes a 500 ms
        
    }
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(task_blinky_on, "Blinky_ON", configMINIMAL_STACK_SIZE, NULL, 2, &xHandle_ON);
    xTaskCreate(task_blinky_off, "Blinky_OFF", configMINIMAL_STACK_SIZE, NULL, 2, &xHandle_OFF);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}
