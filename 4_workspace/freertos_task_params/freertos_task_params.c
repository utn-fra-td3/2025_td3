#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Parametros de ADC para tareas
 */
typedef struct {
    uint8_t gpio;
    uint8_t channel;
} adc_t;

/**
 * @brief Tarea que elije el canal de ADC y lo convierte
 */
void task_adc(void *params) {
    // Parseo al tipo correcto
    adc_t *adc = (adc_t*) params;
    // Uso el campo de GPIO para configurar
    adc_gpio_init(adc->gpio);

    while(1) {
        // Elijo el canal
        adc_select_input(adc->channel);
        // Convierto y envio por consola
        printf("Canal %d tiene un valor de 0x%03x\n", adc->channel, adc_read());
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();
    // Inicializa el ADC
    adc_init();

    // Estructuras para pasar a tareas
    adc_t ch_0 = { .gpio = 26, .channel = 0 };
    adc_t ch_1 = { .gpio = 27, .channel = 1 };
    adc_t ch_2 = { .gpio = 28, .channel = 2 };

    // Creo tareas
    xTaskCreate(task_adc, "Ch0", configMINIMAL_STACK_SIZE, (void*) &ch_0, 1, NULL);
    xTaskCreate(task_adc, "Ch1", configMINIMAL_STACK_SIZE, (void*) &ch_1, 1, NULL);
    xTaskCreate(task_adc, "Ch2", configMINIMAL_STACK_SIZE, (void*) &ch_2, 1, NULL);

    // Inicia el sistema operativo
    vTaskStartScheduler();
    while(1);
}
