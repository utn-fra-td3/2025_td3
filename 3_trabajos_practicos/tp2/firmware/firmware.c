#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define TEMP_SENSOR_CH  4

QueueHandle_t q_adc;

void task_print(void *params) {
    
    float temp_c;
    while(1) {
        xQueueReceive(q_adc, &temp_c, portMAX_DELAY);
        printf("T = %.2fC\n", temp_c);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

    void task_adc(void *params) {

        uint16_t raw;
        float voltaje;
        float temp_c;

        while (1) {

            raw = adc_read();
          
            voltaje = 3.3 * raw / (1 << 12 );
            temp_c = 27 - (voltaje - 0.706) / 0.001721;
            xQueueSend(q_adc, &temp_c, portMAX_DELAY);
        }
        }



int main() {

    stdio_init_all();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(TEMP_SENSOR_CH);

    q_adc = xQueueCreate(1, sizeof(float));
   

    // Crear tareas
    xTaskCreate(task_print, "Print", 2 * configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task_adc, "ADC", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Iniciar scheduler
    vTaskStartScheduler();

    while (true);

}
