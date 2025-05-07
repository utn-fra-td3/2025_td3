#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN 25

void led_on_task(void *pvParameters) 
{
    while (1) 
    {
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(1500));  // Encendido durante 1 segundo
    }
}

void led_off_task(void *pvParameters)
 {
    // Pequeño retardo para desfasar esta tarea
    vTaskDelay(pdMS_TO_TICKS(500));  // Comienza a la mitad del ciclo del ON

    while (1)
    {
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));  // Apagado durante 1 segundo
    }
}

int main() 
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    /*Creacion de tareas*/
    xTaskCreate(led_on_task, "LED ON", 256, NULL, 1, NULL);
    xTaskCreate(led_off_task, "LED OFF", 256, NULL, 1, NULL);
    /*Toma el control el SCHEDULER*/
    vTaskStartScheduler();

    while (1) {}
    return 0;
}

