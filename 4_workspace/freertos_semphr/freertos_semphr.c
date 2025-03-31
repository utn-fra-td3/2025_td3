#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "semphr.h"

// GPIO de boton
#define SEMPHR_BTN  20

// Semaforo para desbloquear tarea
SemaphoreHandle_t semphr;

/**
 * @brief Tarea de inicializacion
 */
void task_init(void *params) {
    // Inicializacion de GPIO para pulsador
    gpio_init(SEMPHR_BTN);
    gpio_set_dir(SEMPHR_BTN, false);
    gpio_pull_up(SEMPHR_BTN);
    // Inicializacion de GPIO para LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);
    // Inicializacion de semaforo
    vSemaphoreCreateBinary(semphr);
    // Elimino tarea para liberar recursos
    vTaskDelete(NULL);
}

/**
 * @brief Tarea que maneja el semaforo
 */
void task_semphr(void *params) {
    // Estado anterior del semaforo
    bool taken = false;

    while(1) {
        // Intenta tomar el semaforo para bloquear la otra tarea
        if(!taken) { 
            xSemaphoreTake(semphr, portMAX_DELAY);
            taken = true;
        }
        // Entrega el semaforo al apretar el boton
        if(!gpio_get(SEMPHR_BTN)) {
            xSemaphoreGive(semphr);
            taken = false;
        }
        // Bloqueo para dar lugar a otra tarea
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/**
 * @brief Tarea que maneja el LED
 */
void task_led(void *params) {

    while(1) {
        // Intenta tomar el semaforo, se bloquea si no esta disponible
        xSemaphoreTake(semphr, portMAX_DELAY);
        // Conmuta el LED
        gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
        // Devuelve el semaforo
        xSemaphoreGive(semphr);
        // Bloqueo chico para dar lugar a otra tarea
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(task_semphr, "Semphr", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_led, "LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Inicia el sistema operativo
    vTaskStartScheduler();
    while (true);
}
