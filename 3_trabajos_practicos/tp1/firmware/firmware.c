#include <stdio.h>
#include "pico/stdlib.h"
// El led en la pico 2W está asociado al módulo wifi
#include "pico/cyw43_arch.h"
#include "freeRTOS.h"
#include "task.h"

#define T_ON 1000 // Tiempo en ms
#define T_OFF 1500 // Tiempo en ms

void task_on(void *params) {
    while (1) {
        // Enciendo el led
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        // Bloqueo la tarea durante un periodo
        vTaskDelay(pdMS_TO_TICKS(T_ON + T_OFF));
    }
}

void task_off(void *params) {
    while (1) {
        // Bloqueo la tarea durante el encendido
        vTaskDelay(pdMS_TO_TICKS(T_ON));
        // Apago el led
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        // Bloqueo la tarea durante el apagado
        vTaskDelay(pdMS_TO_TICKS(T_OFF));
    }
}


int main()
{
    stdio_init_all();
    // Inicializa el módulo wifi donde se encuentra el led
    cyw43_arch_init();

    /**
     * Creo la tarea de encendido con prioridad 2 para encender y
     * luego ir a la tarea de apagar 
     */
    xTaskCreate(
        task_on,
        "task_on",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 2,
        NULL
    );

    // Creo la tarea de apagado
    xTaskCreate(
        task_off,
        "task_off",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
    );

    // Inicio el scheduler
    vTaskStartScheduler();

    while(1);
}
