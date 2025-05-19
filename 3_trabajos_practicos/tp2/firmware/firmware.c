#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
// INCLUDE FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Handles de las tareas
TaskHandle_t htask_ADC;
TaskHandle_t htask_Print;

// Handle de la cola
QueueHandle_t q_ADC;

// Struct de dato de la cola
typedef float q_ADC_t;

// TAREA ADC
static void task_ADC(void *pvParam)
{
    uint16_t adc_raw;
    const float adc_factor = 3.3f / (1<<12);
    float adc_voltage; 
    float temperatura;

    while(1){
        adc_raw = adc_read();
        adc_voltage = adc_raw * adc_factor;
        temperatura = 27 - (adc_voltage - 0.706)/0.001721;
        xQueueSend(q_ADC, &temperatura, portMAX_DELAY);
    }
}

// TAREA PRINT
static void task_Print(void *pvParam)
{
    float temperatura;
    while(1){
        xQueueReceive(q_ADC, &temperatura, portMAX_DELAY);
        printf("Temperatura = %.2fC\n", temperatura);
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

    // Creo la cola para la temperatura
    q_ADC = xQueueCreate(1, sizeof(q_ADC_t));

    // Creo las tareas
    xTaskCreate(task_ADC, "task_ADC", configMINIMAL_STACK_SIZE, NULL, 2, &htask_ADC);
    xTaskCreate(task_Print, "task_Print", 2*configMINIMAL_STACK_SIZE, NULL, 1, &htask_Print);

    // START SCHEDULER
    vTaskStartScheduler();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
