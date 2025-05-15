#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Estructura para pasar los datos del sensor
typedef struct {
    uint16_t lectura;
    float voltaje;
    float temperatura;
} sensor_data_t;

// Cola para datos del sensor
QueueHandle_t queue_sensor;


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
    // Estructura para la cola
    sensor_data_t data = {0};

    while(1) {
        // Leo el sensor y preparo los datos para la cola
        data.lectura = adc_read();
        data.voltaje = data.lectura * 3.3 / (1 << 12);
        // El calculo de temperatura sale de la documentacion del SDK
        data.temperatura = 27 - (data.voltaje - 0.706) / 0.001721;
        // Escribo la cola con siempre el ultimo valor
        xQueueOverwrite(queue_sensor, &data);
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

    // Creacion de tareas
    xTaskCreate(task_print, "Pantalla", 2 * configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_adc, "Lectura", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    // Arranca el sistema operativo
    vTaskStartScheduler();
    while (true);
}