#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t on,off;

void task_init(void *params)
{
    // Inicializacion de GPIO
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    // Elimino la tarea para liberar recursos
    vTaskDelete(NULL);
}

void task_encendido(void *params)
{
    while(1) 
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        vTaskResume(off);
        vTaskSuspend(NULL);
    }
}

void task_apagado(void *params) 
{
    while(1) 
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
        vTaskResume(on);
        vTaskSuspend(NULL);
    }
}
int main()
{
    stdio_init_all();

    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_encendido, "Encendido", configMINIMAL_STACK_SIZE, NULL, 1, &on);
    xTaskCreate(task_apagado, "Apagado", configMINIMAL_STACK_SIZE, NULL, 1, &off);

    vTaskStartScheduler();
    while(1);
}
