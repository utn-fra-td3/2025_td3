#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "hardware/adc.h"
#include "task.h"
#include "queue.h"

QueueHandle_t q_adc;


// DEFINO LA ESTRUCTURA DE LECTURA

typedef struct {
    uint16_t raw;               //  FORMATO DE 2 BYTES
    float voltage;              //  FORMATO PUNTO FLOTANTE
    float temperature;          //  FORMATO PUNTO FLOTANTE
} sensor_data;


// DEFINO LAS FUNCIONES

void task_print_temp(void *params)
{
    sensor_data data = {0};                                   //  DEFINO ESTRUCTURA PARA LA COLA E INICIALIZO EN 0
    while (1)
    {
        xQueuePeek(q_adc, &data, portMAX_DELAY);                //  LEO EL ULTIMO VALOR EN LA COLA
        printf("Temperature: %.2f C\n\n", data.temperature);    //  ESCRIBO EL VALOR EN LA PANTALLA
        vTaskDelay(pdMS_TO_TICKS(1000));                        //  LE DOY DELAY A LA CONSOLA
    }
    
}

void task_init(void *params) {

    adc_init();                                                  //  INICIALIZO EL ADC
    adc_set_temp_sensor_enabled(true);                           //  HABILITO EL SENSOR DE TEMPERATURA
    adc_select_input(ADC_TEMPERATURE_CHANNEL_NUM);               //  SELECCIONO EL CANAL

    q_adc = xQueueCreate(1, sizeof(sensor_data));                // INICIALIZO LA COLA

    vTaskDelete(NULL);                                           // ELIMINO LA TAREA PARA QUE NO VUELVA A ARRANCAR, YA QUE TIENE PRIORIDAD MAXIMA
}

void task_adc(void *params) {

    sensor_data data = {0};                                      // DEFINO ESTRUCTURA PARA LA COLA E INICIALIZO EN 0

    while(1) {
  
        data.raw = adc_read();                                   // LEO EL DATO DEL SENSOR

        // CALCULO LA TEMPERATURA CON LA CONVERSION

        data.voltage = data.raw * 3.3 / (1 << 12);
        data.temperature = 27 - (data.voltage - 0.706) / 0.001721;
  
        xQueueOverwrite(q_adc, &data);                    // ESCRIBO EL VALOR AL FINAL DE LA COLA
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