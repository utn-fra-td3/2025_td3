#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*Consigna 1: Hacer un program en FreeRTOS que lea datos de un sensosr de temperatura por medio de 
* una entrada ADC y los muestre por consola. Se puede usar el sensor interno de la Raspberry Pico*/

/*NOTA: Se debe agregar manualmente un offset de temperatura, para la cosigna
*se tomo un offset de 24°C, por lo tanto la formula para convertir la tension
*a temperatura sera:
* temperatura: 24 - (0.706 - voltaje)/0.001721 */

QueueHandle_t lectura_adc;

void captura_datos(void *pvParameter)
{  uint16_t value_adc; //variable local, solo sirve dentro de la tarea

    adc_init(); //inicio el ADC
    adc_set_temp_sensor_enabled(true); //activo el sensor interno de la Pico 2
    adc_select_input(4); //es el sensor de temperatura interno, no es un pin

    while(1)
    {
        value_adc=adc_read(); //lectura del ADC que esta conectado al sensor
        if(xQueueSend(lectura_adc,&value_adc,pdMS_TO_TICKS(100)) != pdTRUE)
        {
            printf("Cola llena \n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

void impresion_temp(void *pvParameter)
{   uint16_t value_adc; //variable local, solo sirve dentro de la tarea
    float temp,voltaje;

    while (1)
    {  
        if (xQueueReceive(lectura_adc,&value_adc,pdMS_TO_TICKS(100)) == pdTRUE)
        {
            voltaje = value_adc *( 3.3f / 4095.0);
            temp = 24 - ( 0.706f - voltaje) / 0.001721f;
            printf("ADC:%d, Voltaje:%.2f V, Temperatura:%.2f °C\n",value_adc,voltaje,temp);
        }  
    }
}

int main(void) 
{
    stdio_init_all();
    //Creacion de cola
    lectura_adc = xQueueCreate(5,sizeof(uint16_t));
    if(lectura_adc == NULL)
    {
        while (1)
        {
            printf("No se puedo crear la cola \n");
        }
        
    }
    //Creacion de tareas
    xTaskCreate(captura_datos,"Emisora",256,NULL,1,NULL);
    xTaskCreate(impresion_temp,"Receptora",256,NULL,1,NULL);
    //inicio
    vTaskStartScheduler();

    while(1);
}