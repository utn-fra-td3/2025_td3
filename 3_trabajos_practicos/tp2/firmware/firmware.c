#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/irq.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Cantidad de lecturas
#define SAMPLES 4

// Estructura para pasar los datos del sensor
typedef struct {
    uint16_t lectura;
    float voltaje;
    float temperatura;
} sensor_data_t;

// Cola para datos del sensor
QueueHandle_t queue_sensor;

// Handler para la interrupcion del ADC
void adc_irq_handler(void) {
    // Variable para verificar la necesidad de un cambio tarea
    BaseType_t to_higher_priority_task = false;
    // Deshabilito la interrupcion y detengo el ADC
    adc_irq_set_enabled(false);
    adc_run(false);
    // Variable para calcular el promedio de muestras
    uint32_t lectura = 0;
    for(uint8_t i = 0; i < SAMPLES; i++) { lectura += adc_fifo_get(); }
    // Limpio el FIFO
    adc_fifo_drain();
    // Datos para la cola
    sensor_data_t data = { .lectura = (uint16_t) (lectura / SAMPLES) };
    data.voltaje = data.lectura * 3.3 / (1 << 12);
    // El calculo de temperatura sale de la documentacion del SDK
    data.temperatura = 27 - (data.voltaje - 0.706) / 0.001721;
    // Envio por cola
    xQueueOverwriteFromISR(queue_sensor, &data, &to_higher_priority_task);
    // Reviso si es necesario el cambio a otra tarea
    portYIELD_FROM_ISR(to_higher_priority_task);
}


//Tarea que escribe por consola
void task_print(void *params) {
    // Estructura para la cola
    sensor_data_t data = {0};

    while(1) {
        // Leo el ultimo valor que haya en la cola
        xQueuePeek(queue_sensor, &data, portMAX_DELAY);
        printf("Temperatura: %.2f C\n\n", data.temperatura);
        // Bloqueo para no saturar la consola
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

//Tarea que lee el ADC
void task_adc(void *params) {

    while(1) {
        // Habilito la interrupcion del ADC nuevamente y corro las conversiones
        adc_irq_set_enabled(true);
        adc_run(true);
        // Bloqueo la tarea por un tiempo
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

//Programa principal

int main(void) {

    stdio_init_all();

    // Inicializacion del ADC y sensor de temperatura
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_TEMPERATURE_CHANNEL_NUM);

    // Inicializacion de cola para estructura
    queue_sensor = xQueueCreate(1, sizeof(sensor_data_t));

    // Inicializo la interrupcion del ADC y la cantidad de lecturas necesarias
    adc_fifo_setup(true, false, SAMPLES, false, false);
    adc_irq_set_enabled(true);
    irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_irq_handler);
    irq_set_enabled(ADC_IRQ_FIFO, true);
    adc_run(true);

    // Creacion de tareas
    xTaskCreate(task_print, "Pantalla", 2 * configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_adc, "Lectura", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    // Arranca el sistema operativo
    vTaskStartScheduler();
    while (true);
}