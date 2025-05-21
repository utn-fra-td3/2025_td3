#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "pico/cyw43_arch.h"
#include "queue.h"

#define TEMP_SENSOR_CH 4

QueueHandle_t q_adc ;

//TaskHandle_t task_print ;
//TaskHandle_t task_adc ;




void task_print(void *params){
    float temp_c ;
    while (1){
        xQueueReceive(q_adc , &temp_c , portMAX_DELAY);
        printf ("T = %.2f \n", temp_c); 
    }
}

void task_adc(void *params){
    uint16_t raw ;
    float voltage ;
    float temp_c ;
    while(1){
        raw = adc_read();
        voltage = 3.3 * raw / (1 << 12);
        temp_c = 27 - (voltage - 0.706)/0.001721 ;
        xQueueSend(q_adc , &temp_c , portMAX_DELAY );
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

//27-(ADC_Voltage - 0.706)/0.001721

int main()
{
    stdio_init_all();
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(TEMP_SENSOR_CH);

    q_adc = xQueueCreate(1 , sizeof(float) );
    xTaskCreate(task_print , "Print" , 2*configMINIMAL_STACK_SIZE ,NULL, 1 ,NULL);
    xTaskCreate(task_adc ,  "ADC" , configMINIMAL_STACK_SIZE,NULL, 1 ,NULL);

    vTaskStartScheduler();
    while (true) {
        

        
    }
}
