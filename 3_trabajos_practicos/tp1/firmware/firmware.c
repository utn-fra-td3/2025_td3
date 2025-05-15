#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t handle_led_on = NULL;
TaskHandle_t handle_led_off = NULL;

void task_led_on(void *params) {
    while (1) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1); // Encender LED
        vTaskDelay(pdMS_TO_TICKS(1000));               // 1000 ms encendido
        vTaskResume(handle_led_off);                   // Reanuda tarea de apagado
        vTaskSuspend(NULL);                            // Se suspende a sí misma
    }
}

void task_led_off(void *params) {
    while (1) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0); // Apagar LED
        vTaskDelay(pdMS_TO_TICKS(1500));               // 1500 ms apagado
        vTaskResume(handle_led_on);                    // Reanuda tarea de encendido
        vTaskSuspend(NULL);                            // Se suspende a sí misma
    }
}

void task_init(void *params) {
    cyw43_arch_init();
    vTaskDelete(NULL);
}

int main(void) {
    stdio_init_all();

    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_led_on, "LED On", configMINIMAL_STACK_SIZE, NULL, 1, &handle_led_on);
    xTaskCreate(task_led_off, "LED Off", configMINIMAL_STACK_SIZE, NULL, 1, &handle_led_off);

    // Arranca solo la tarea de encendido, la de apagado se suspende inicialmente
    vTaskSuspend(handle_led_off);

    vTaskStartScheduler();
    while (1);
}