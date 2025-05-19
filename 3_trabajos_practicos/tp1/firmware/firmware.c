#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"

void task_init(void *params) {
    // Inicializacion de GPIO
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT );
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    // Elimino la tarea para liberar recursos
    vTaskDelete(NULL);
}

void task_led_on(void *params)
{
    while(1) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);  // Encendido el led

        vTaskDelay(pdMS_TO_TICKS(1000));  // Demora para volver a encender
    }
}

void task_led_off(void *params)
{
    vTaskDelay(pdMS_TO_TICKS(500)); // Desfase inicial de 500ms para diferenciarse con la tarea anterior

    while(1) {
        gpio_put(PICO_DEFAULT_LED_PIN, 0);  // Apagado del led
        vTaskDelay(pdMS_TO_TICKS(1000));  // Demora para volver a apagar
    }
}

int main()
{
    stdio_init_all();

// se procede a crear las tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_led_on, "LED_ON", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task_led_off, "LED_OFF", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1);
}
