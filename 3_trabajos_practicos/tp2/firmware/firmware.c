#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/irq.h"

#include "FreeRTOS.h"
#include "task.h"
#include "pico/cyw43_arch.h"
#include "queue.h"

#define TEMP_SENSOR_CH 4

QueueHandle_t q_adc ;

//TaskHandle_t task_print ;
//TaskHandle_t task_adc ;


void adc_ISR(void){
    static BaseType_t xTask = pdFALSE ;
    uint32_t adc_raw = 0 ;

    if (adc_fifo_get_level()>0){
        adc_raw=adc_fifo_get();
        xQueueSendFromISR(q_adc,&adc_raw, &xTask);
    }
    portYIELD_FROM_ISR(xTask);



}

void task_print(void *params){
    
    uint32_t raw ;
    float voltage ;
    float temp_c ;
    while (1){
        xQueueReceive(q_adc , &raw , pdMS_TO_TICKS(100));
        voltage = 3.3 * raw / (1 << 12);
        temp_c = 27 - (voltage - 0.706)/0.001721 ;
        printf ("T = %.2f \n", temp_c); 
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
/*
void task_adc(void *params){
    uint16_t raw ;
    float voltage ;
    float temp_c ;
    while(1){
        raw = adc_read();
        voltage = 3.3 * raw / (1 << 12);
        temp_c = 27 - (voltage - 0.706)/0.001721 ;
        xQueueSend(q_adc , &temp_c , portMAX_DELAY );
        
    }
}
*/
//27-(ADC_Voltage - 0.706)/0.001721

int main()
{
    stdio_init_all();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(TEMP_SENSOR_CH);

    adc_fifo_setup(true,true,1,false,false);
    adc_irq_set_enabled(true);
    irq_set_exclusive_handler(ADC_IRQ_FIFO ,adc_ISR);
    irq_set_enabled(ADC_IRQ_FIFO,true);
    adc_run(true);


    q_adc = xQueueCreate(1 , sizeof(float) );
    xTaskCreate(task_print , "Print" , 2*configMINIMAL_STACK_SIZE ,NULL, 2 ,NULL);
    //xTaskCreate(task_adc ,  "ADC" , configMINIMAL_STACK_SIZE,NULL, 1 ,NULL);

    vTaskStartScheduler();
    while (true) {
        

        
    }
}
