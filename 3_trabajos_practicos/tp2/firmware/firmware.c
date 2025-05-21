#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#define TEMP_SENSOR_CH  4

// Cola para mandar numeros aleatorios
QueueHandle_t q_adc;

/**
 * @brief Estructura para pasar los datos del sensor
 */
typedef struct {
    uint16_t raw;
    float voltage;
    float temperature;
} sensor_data_t;

/**
 * @brief Handler para la interrupcion del ADC
 */
void adc_irq_handler(void) {
    // Variable para verificar la necesidad de un cambio tarea
    BaseType_t to_higher_priority_task = false;
    // Deshabilito la interrupcion y detengo el ADC
    adc_irq_set_enabled(false);
    adc_run(false);
    // Variable para calcular el promedio de muestras
    uint32_t raw = 0;
    for(uint8_t i = 0; i < 4; i++) { raw += adc_fifo_get(); }
    // Limpio el FIFO
    adc_fifo_drain();

    // Extraigo datos del promedio para enviar temperatura por la Cola
    sensor_data_t data = { .raw = (uint16_t) (raw / 4) };
    data.voltage = data.raw * 3.3 / (1 << 12);
    data.temperature = 27 - (data.voltage - 0.706) / 0.001721;
    
    // Envio por cola
    xQueueOverwriteFromISR(q_adc, &data.temperature, &to_higher_priority_task);
    
    // CAMBIO DE CONTEXTO
    // Reviso si es necesario el cambio a otra tarea y salgo de interrupción
    portYIELD_FROM_ISR(to_higher_priority_task);
}

/**
 * @brief Tarea que escribe por consola lo que llega
 * de la cola
 */
void task_print(void *params) {
    float temp_C;
    while(1) {
        // Uso peek para lectura
        xQueuePeek(q_adc, &temp_C, portMAX_DELAY);
        // Escribo por consola
        printf("Temperatura: %.2fC\n", temp_C);
        vTaskDelay(500);
    }
}

void task_adc(void *params) {
    while (1)
    {
        // Habilito interrupcion del ADC y corro las conversiones
        adc_irq_set_enabled(true);
        adc_run(true);
        vTaskDelay(500);
    }
}



/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(TEMP_SENSOR_CH);

    q_adc = xQueueCreate (1, sizeof(float));

    // Inicializo la interrupcion del ADC y la cantidad de lecturas necesarias
    adc_fifo_setup(true, false, 4, false, false);
    adc_irq_set_enabled(true);

    // Seteo el Handler (función void para tratar la interrupción)
    irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_irq_handler);

    // Habilito interrupcion del ADC y corro las conversiones
    irq_set_enabled(ADC_IRQ_FIFO, true);
    adc_run(true);

    /* Creacion de tareas */ 
    xTaskCreate(task_print, "Print", 2 * configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_adc, "ADC", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}
