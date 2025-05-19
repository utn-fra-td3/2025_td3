#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*Consigna 2: Hacer un program en FreeRTOS que lea datos de un sensosr de temperatura por medio de 
* una entrada ADC y los muestre por consola. Se puede usar el sensor interno de la Raspberry Pico
* ene este punto se realizara usando interrupciones*/

/*NOTA: Se debe agregar manualmente un offset de temperatura, para la cosigna
*se tomo un offset de 24°C, por lo tanto la formula para convertir la tension
*a temperatura sera:
* temperatura= 24 - (0.706 - voltaje)/0.001721 */

QueueHandle_t lectura_adc;

//----------------------Funcion para ISR en FreeRTOS-----------------------------------------------------------
void adc_isr()
{   BaseType_t xHigherPriorityTaskWoken=pdFALSE;
    uint16_t value_adc; //variable local, solo sirve dentro de la tarea

    if(adc_fifo_get_level() > 0)
    {
        value_adc = adc_fifo_get(); //el dato que se lee de la FIFO se elimina
        xQueueSendFromISR(lectura_adc,&value_adc,&xHigherPriorityTaskWoken);
    }
    else
    {
        printf("NO HAY LECTURA DEL ADC \n");
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
//--------------------------------------------------------------------------------------------------
//-----------------------------Configuracion del ADC y de su interrupcion---------------------------
void set_adc()
{
    /*Configurio e inicializo el ADC*/
    adc_init(); //inicio el ADC
    adc_set_temp_sensor_enabled(true); //activo el sensor interno de la Pico 2
    adc_select_input(4); //es el sensor de temperatura interno, no es un pin

    /*Configuro y activo la interrupcion del ADC*/
    adc_fifo_setup(true, true, 1, false, false); //indico que la FIFO sera de un elemento
    adc_irq_set_enabled(true); //habilito la interrupcion del ADC 
    irq_set_exclusive_handler(ADC_IRQ_FIFO,adc_isr); //indico que tipo de interrupcion habilito y que funcion sera la ISR
    irq_set_enabled(ADC_IRQ_FIFO,true); //habilitacion general de la interrupcion del ADC
    /*Arranca el ADC*/
    adc_run(true);
}
//-------------------------------------------------------------------------------------------------
//------------------------------Definicion de tareas------------------------------------------------
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
        vTaskDelay(1000); 
    }
}
//-----------------------------------------------------------------------------------------------------

int main(void) 
{
    stdio_init_all();
    set_adc();
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
    xTaskCreate(impresion_temp,"Receptora",256,NULL,1,NULL);
    //inicio
    vTaskStartScheduler();

    while(1);
}