#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Tarea para encender el LED
 */
void task_led_on(void *params) {
    while (1) {
        // Encender el LED
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        // Mantener encendido por 1000 ms
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Tarea para apagar el LED
 */
void task_led_off(void *params) {
    while (1) {
        // Apagar el LED
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        // Mantener apagado por 1500 ms
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}

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
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_led_on, "LED On", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task_led_off, "LED Off", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Arranca el scheduler
    vTaskStartScheduler();
    while (1);
}
