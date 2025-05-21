#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "hardware/adc.h"
#include "hardware/irq.h"
#include "task.h"
#include "queue.h"

#define SAMPLES 4               // DEFINO CANTIDAD DE LECTURAS


QueueHandle_t q_adc;

// DEFINO LA ESTRUCTURA DE LECTURA

typedef struct {
    uint32_t raw;               //  FORMATO DE 2 BYTES
    float voltage;              //  FORMATO PUNTO FLOTANTE
    float temperature;          //  FORMATO PUNTO FLOTANTE
}sensor_data;


// DEFINO LAS FUNCIONES

void adc_irq_handler(void) {
   
    BaseType_t to_higher_priority_task = false;          // VERIFICO LA NECESDAD DE CAMBIAR TAREA
    
    adc_irq_set_enabled(false);                          // PARO LA INTERRUPCON
    adc_run(false);                                      // DESHABILITO EL ADC
    
    sensor_data adc_value;                              // VARIABLE PARA PROMEDIO
    adc_value.raw = 0;                                  // INICIALIZO EN 0

    for(uint8_t i = 0; i < SAMPLES; i++)                 
        { 
            adc_value.raw += adc_fifo_get();                  // LEO EL FIFO PARA LA CANTIDAD DE MUESTRAS QUE ELEGI
        }

    adc_fifo_drain();                                     // LIMPIO EL FIFO

    xQueueOverwriteFromISR(q_adc, &adc_value, &to_higher_priority_task);      // ENVIO EL DATO A LA COLA
}

void task_print_temp(void *params)
{
    sensor_data data;                                   //  DEFINO ESTRUCTURA PARA LA COLA E INICIALIZO EN 0
    data.raw = 0;

    while (1)
    {
        xQueueReceive(q_adc, &data, portMAX_DELAY);                        //  LEO EL ULTIMO VALOR EN LA COLA

        data.raw = (uint32_t) (data.raw / SAMPLES);                     // CALCULO DE TEMPERATURA
        data.voltage = data.raw * 3.3 / (1 << 12);                      // CALCULO DE TEMPERATURA
        data.temperature = 27 - (data.voltage - 0.706) / 0.001721;      // CALCULO DE TEMPERATURA

        data.temperature = 

        printf("Temperatura: %.2f C\n\n", data.temperature);    //  ESCRIBO EL VALOR EN LA PANTALLA
    }
    
}

void task_init(void *params) {

    adc_init();                                                  //  INICIALIZO EL ADC
    adc_set_temp_sensor_enabled(true);                           //  HABILITO EL SENSOR DE TEMPERATURA
    adc_select_input(ADC_TEMPERATURE_CHANNEL_NUM);               //  SELECCIONO EL CANAL

    adc_fifo_setup(true, false, SAMPLES, false, false);          // SETEO EL FIFO
    adc_irq_set_enabled(true);                                   // ACTIVO LA INTERRUPCIN DEL ADC
    irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_irq_handler);    // LA INTERRUPCION LLAMA A LA FUNCION
    irq_set_enabled(ADC_IRQ_FIFO, true);                         // HABILITA LA INTERRUPCION
    adc_run(true);                                               // PONGO A CORRER AL ADC

    q_adc = xQueueCreate(1, sizeof(sensor_data));                // INICIALIZO LA COLA

    vTaskDelete(NULL);                                           // ELIMINO LA TAREA PARA QUE NO VUELVA A ARRANCAR, YA QUE TIENE PRIORIDAD MAXIMA
}

void task_adc(void *params) {

      while(1) {
        
        adc_irq_set_enabled(true);                          // INICIO LA INTERRUPCON
        adc_run(true);                                      // HABILITO EL ADC
        vTaskDelay(pdMS_TO_TICKS(500));                     // HAGO SALTAR LA CONVERSION DEL ADC

      }
}

int main(void) {

    stdio_init_all();                   //  INICIALIZO EL STDIO

    // CREO LAS TAREAS

    xTaskCreate(task_print_temp,
    "PANTALLA",
    2 * configMINIMAL_STACK_SIZE,       //  UTILIZO EL DOBLE DEL STACK PARA QUE NO DESBORDE
    NULL,
    2,
    NULL);

    xTaskCreate(task_init,
    "INICIO",
    configMINIMAL_STACK_SIZE,
    NULL,
    3,                                  //  PRIORIDAD MAYOR YA QUE ES EL QUE INICIA EL ADC
    NULL);

    xTaskCreate(task_adc,
    "ADC",
    configMINIMAL_STACK_SIZE,
    NULL,
    1,
    NULL);

    //  INICIO EL SISTEMA

    vTaskStartScheduler();
    while (true);
}