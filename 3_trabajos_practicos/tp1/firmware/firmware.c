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
 * @brief Tarea de prender de LED
 */
void task_led_on(void *params) {
    
    while(1) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        // Demora de ticks equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
/**
 * @brief Tarea de apagar de LED
 */
void task_led_off(void *params) {
    
    vTaskDelay(pdMS_TO_TICKS(500));
    while(1) {
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        // Demora de ticks equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init,      "Init",     configMINIMAL_STACK_SIZE, 
        NULL,
        tskIDLE_PRIORITY + 2,
        NULL
    );

    // Inicializacion de tareas para el led
    xTaskCreate(task_led_on,    "LedOn",    configMINIMAL_STACK_SIZE, 
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
    );
    xTaskCreate(task_led_off,   "LedOff",   configMINIMAL_STACK_SIZE, 
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
    );

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}