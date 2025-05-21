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
 * @brief Tarea que escribe por consola lo que llega
 * de la cola
 */
void task_print(void *params) {

    while(1) {
        // Bloqueo tarea hasta que llegue el dato
        float temp_C;
        xQueueReceive(q_adc, &temp_C, portMAX_DELAY);
        // Escribo por consola
        printf("Temperatura: %.2fC\n", temp_C);

    }
}

void task_adc(void *params) {
    uint16_t raw;
    float voltaje;
    float temp_C;
    while (1)
    {
        raw = adc_read();
        voltaje = 3.3 * raw / (1<<12);
        temp_C = 27 - (voltaje - 0.706) / 0.001721;
        xQueueSend(q_adc, &temp_C, portMAX_DELAY);
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

    // Creacion de tareas
    xTaskCreate(task_print, "Print", 2 * configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task_adc, "ADC", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}
