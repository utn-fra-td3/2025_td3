#include <stdio.h>
#include "pico/stdlib.h"
/*ahora con la libreria delotro ejercicio*/
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
void task_blinky(void *params) {
    
    while(1) {
        // Toggle
        gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
         vTaskDelay(pdMS_TO_TICKS(1000));

        gpio_put(PICO_DEFAULT_LED_PIN_INVERTED, !gpio_get(PICO_DEFAULT_LED_PIN));
        vTaskDelay(pdMS_TO_TICKS(3000));
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
