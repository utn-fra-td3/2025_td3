#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Definimos un manejador de mutex
SemaphoreHandle_t xMutex;

void task_init(void *params) {
    // Inicialización de GPIO
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);

    // Creo el mutex para sincronización
    xMutex = xSemaphoreCreateMutex();

    // Elimino la tarea para liberar recursos
    vTaskDelete(NULL);
}

void task_led_on(void *params) {
    while (true) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            gpio_put(PICO_DEFAULT_LED_PIN, 1);  // Enciendo LED
            vTaskDelay(pdMS_TO_TICKS(1000));
            xSemaphoreGive(xMutex);  // Libero mutex
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Pequeña espera antes de intentar de nuevo
    }
}

void task_led_off(void *params) {
    while (true) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            gpio_put(PICO_DEFAULT_LED_PIN, 0);  // Apago LED
            vTaskDelay(pdMS_TO_TICKS(1500));
            xSemaphoreGive(xMutex);  // Libero mutex
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Pequeña espera antes de intentar de nuevo
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(task_init, "Init", 128, NULL, 2, NULL);
    xTaskCreate(task_led_on, "LED ON", 128, NULL, 1, NULL);
    xTaskCreate(task_led_off, "LED OFF", 128, NULL, 1, NULL);

    vTaskStartScheduler();
    while (1);
}