#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/irq.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Handle de la cola
QueueHandle_t q_ADC;

// ISR ADC
void adc_ISR(void)
{
    static BaseType_t xHPTW = pdFALSE;
    static uint16_t adc_value;

    // Deshabilito interrupcion y conversion
    adc_irq_set_enabled(false);
    adc_run(false);

    // Leo valor de FIFO
    adc_value = adc_fifo_get();
    // Limpio el FIFO
    adc_fifo_drain();
    // Envio desde la ISR
    xQueueSendFromISR(q_ADC, &adc_value, &xHPTW);
}

// TAREA ADC
static void task_ADC(void *pvParam)
{
    // Loop de ejecucion
    while(1){
        // Habilito la irq y conversion
        adc_irq_set_enabled(true);
        adc_run(true);
        // Delay de 1 seg
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}g

// TAREA PRINT
static void task_Print(void *pvParam)
{
    uint16_t adc_raw;
    const float adc_FACTOR = 3.3f / (1<<12);
    float adc_voltage; 
    float temperatura;

    while(1){
        // Intento recibir de la cola
        if(xQueueReceive(q_ADC, &adc_raw, portMAX_DELAY) == pdPASS){
            // Calculo temperatura
            adc_voltage = adc_raw * adc_FACTOR;
            temperatura = 27 - (adc_voltage - 0.706)/0.001721;
            // Imprimo
            printf("Temperatura = %.2fC\n", temperatura);
        }
    }
}

int main()
{
    stdio_init_all();

    // ADC init
    adc_init();
    // Init sensor de temperatura
    adc_set_temp_sensor_enabled(true);
    // ADC select input del sensor
    adc_select_input(ADC_TEMPERATURE_CHANNEL_NUM);

    // Config IRQ ADC
    // Seteo el FIFO del ADC (size 1)
    adc_fifo_setup(true, false, 1, false, false);
    // Habilito irq del adc
    adc_irq_set_enabled(true);
    // Asocio handler de IRQ del FIFO a la funcion ISR (asociada al core en ejecucion)
    irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_ISR);
    // Habilito IRQ del FIFO (en el core en ejecucion)
    irq_set_enabled(ADC_IRQ_FIFO, true);

    // ADC en modo free running
    adc_run(true);

    // Creo la cola para la temperatura
    q_ADC = xQueueCreate(1, sizeof(uint16_t));

    // Creo las tareas
    xTaskCreate(task_ADC, "ADC", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task_Print, "Print", 2*configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    // START SCHEDULER
    vTaskStartScheduler();

    while (true);
}
