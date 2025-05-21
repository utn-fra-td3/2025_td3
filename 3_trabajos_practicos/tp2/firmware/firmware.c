#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/irq.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Cantidad de lecturas a promediar
#define SAMPLES 4

/**
 * @brief Estructura para pasar los datos del sensor
 */
typedef struct {
    uint16_t raw;
    float voltage;
    float temperature;
} sensor_data_t;

// Cola para datos del sensor
QueueHandle_t queue_sensor;

/**
 * @brief Handler para la interrupción del ADC
 */
void adc_irq_handler(void) {
    BaseType_t to_higher_priority_task = pdFALSE;

    // Deshabilito interrupciones y detengo ADC
    adc_irq_set_enabled(false);
    adc_run(false);

    // Promedio de las muestras
    uint32_t raw = 0;
    for (uint8_t i = 0; i < SAMPLES; i++) {
        raw += adc_fifo_get();
    }

    // Limpio FIFO
    adc_fifo_drain();

    // Lleno estructura con datos
    sensor_data_t data;
    data.raw = (uint16_t)(raw / SAMPLES);
    data.voltage = data.raw * 3.3f / (1 << 12);  // 12 bits -> 4096
    data.temperature = 27.0f - (data.voltage - 0.706f) / 0.001721f;

    // Envío estructura a la cola
    xQueueOverwriteFromISR(queue_sensor, &data, &to_higher_priority_task);

    // Cambio de contexto si es necesario
    portYIELD_FROM_ISR(to_higher_priority_task);
}

/**
 * @brief Tarea que imprime por consola
 */
void task_print(void *params) {
    sensor_data_t data;

    while (true) {
        xQueuePeek(queue_sensor, &data, portMAX_DELAY);
        printf("Temperature: %.2f C\n\n", data.temperature);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Tarea que inicia las conversiones ADC periódicamente
 */
void task_adc(void *params) {
    while (true) {
        adc_irq_set_enabled(true);
        adc_run(true);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Función principal
 */
int main(void) {
    stdio_init_all();

    // Inicializo la cola
    queue_sensor = xQueueCreate(1, sizeof(sensor_data_t));

    // Configuro ADC para el sensor de temperatura interno
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_TEMPERATURE_CHANNEL_NUM);
    adc_fifo_setup(true, false, SAMPLES, false, false);

    // Configuro interrupción ADC
    irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_irq_handler);
    irq_set_enabled(ADC_IRQ_FIFO, true);

    // Creo las tareas
    xTaskCreate(task_print, "Print", 2 * configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_adc, "ADC", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Inicio FreeRTOS
    vTaskStartScheduler();

    // No debería llegar acá
    while (true);
}
