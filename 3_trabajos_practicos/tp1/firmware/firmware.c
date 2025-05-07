#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

// Definicion de tiempos del led
#define LED_ON_TIME 1000 // en ms
#define LED_OFF_TIME 1500 // en ms


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
        vTaskDelay(pdMS_TO_TICKS(LED_ON_TIME + LED_OFF_TIME));
    }
}
/**
 * @brief Tarea de apagar de LED
 */
void task_led_off(void *params) {
    
    vTaskDelay(pdMS_TO_TICKS(LED_ON_TIME));
    while(1) {
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        // Demora de ticks equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(LED_ON_TIME + LED_OFF_TIME));
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