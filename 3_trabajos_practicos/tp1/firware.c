#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#define DEFAULT_PIN_LED 22

TaskHandle_t handle_led_on = NULL;
TaskHandle_t handle_led_off = NULL;

void task_init(void *params) {
    gpio_init(DEFAULT_PIN_LED);
    gpio_set_dir(DEFAULT_PIN_LED, GPIO_OUT);
    gpio_put(DEFAULT_PIN_LED, 0);
    vTaskDelete(NULL);  // Termina esta tarea de inicialización
}

void task_led_on(void *params) {
    while (1) {
        printf("Tarea LED ON ejecutándose\n");
        gpio_put(DEFAULT_PIN_LED, true);

       
        vTaskResume(handle_led_off);  // Reactivo la tarea de apagar LED
        vTaskSuspend(NULL);           // Suspendo esta tarea
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_led_off(void *params) {
    while (1) {
        printf("Tarea LED OFF ejecutándose\n");
        gpio_put(DEFAULT_PIN_LED, false);


        
        vTaskResume(handle_led_on);   // Reactivo la tarea de prender LED
        vTaskSuspend(NULL);           // Suspendo esta tarea
        vTaskDelay(pdMS_TO_TICKS(15000));

    }
}

int main(void) {
    stdio_init_all();

    // Crear tarea de inicialización
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_led_on, "LED_ON", configMINIMAL_STACK_SIZE, NULL, 1, &handle_led_on);
    xTaskCreate(task_led_off, "LED_OFF", configMINIMAL_STACK_SIZE, NULL, 1, &handle_led_off);

    // Arrancar scheduler
    vTaskStartScheduler();

    // Si llegamos acá, hubo un error en scheduler
    while (1) { }
}
