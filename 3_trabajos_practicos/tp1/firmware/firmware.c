#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "pico/cyw43_arch.h"

#define DELAY 500

// Creo el handle de ambas tareas (sus referencias)
TaskHandle_t Handle_blinkON = NULL;
TaskHandle_t Handle_blinkOFF = NULL;

/**
 * @brief Tarea de inicializacion
 */
 void task_init(void *params) {
    // Inicializacion de GPIO
    cyw43_arch_init();
    //Para que arranque el led encendido, suspendo inicialmente la tarea que apaga el led
    vTaskSuspend(Handle_blinkOFF);
    // Elimino la tarea para liberar recursos
    vTaskDelete(NULL);
}

/**
 * @brief Tarea que enciende el led
 */
void task_blinkON(void *params) {
    while(1) {
        // Enciendo el led
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        // Demora de ticks equivalentes a DELAY ms
        vTaskDelay(pdMS_TO_TICKS(DELAY));
        // Activo la otra
        vTaskResume(Handle_blinkOFF);
        //Suspendo la tarea de apagado, y activo la otra
        vTaskSuspend(NULL);
    }
}

/**
 * @brief Tarea que apaga el led
 */
 void task_blinkOFF(void *params) {
    
    while(1) {
        // Apago el led
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        // Demora de ticks equivalentes a DELAY ms
        vTaskDelay(pdMS_TO_TICKS(DELAY));
        // Activo la otra
        vTaskResume(Handle_blinkON);
        //Suspendo la tarea de encendido, y activo la otra
        vTaskSuspend(NULL);
    }
}


/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_blinkON, "BlinkON", configMINIMAL_STACK_SIZE, NULL, 1, &Handle_blinkON);
    xTaskCreate(task_blinkOFF, "BlinkOFF", configMINIMAL_STACK_SIZE, NULL, 1, &Handle_blinkOFF);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}