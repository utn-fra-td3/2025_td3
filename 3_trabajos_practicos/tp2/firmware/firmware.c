#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Definición de la cola
QueueHandle_t tempQueue;

// Prototipos
void vTaskTempSensor(void *params);
void vTaskTempConsumer(void *params);

int main() {
    stdio_init_all();  // Inicializa la consola USB

    // Inicializa ADC
    adc_init();
    adc_set_temp_sensor_enabled(true);  //activo el sensor interno de temperatura que esta onchip
    adc_select_input(4);  // Canal interno - me paro en el canal que quiero medir

    // Crea cola de 1 posicion tipo float ya que hace "send" y "receive" constantemente, no me interesa guardar
    tempQueue = xQueueCreate(1, sizeof(float));   //tamaño y tipo de dato
    if (tempQueue == NULL) {
        printf("Error: no se pudo crear la cola\n");
        while (1);
    }

    // Crea tareas
    xTaskCreate(vTaskTempSensor, "TempSensor", configMINIMAL_STACK_SIZE, NULL, 1, NULL);   //prioridad 1, 128 palabras
    xTaskCreate(vTaskTempConsumer, "TempConsumer", configMINIMAL_STACK_SIZE, NULL, 1, NULL);   //prioridad 1, 128 palabras

    // Inicia scheduler
    vTaskStartScheduler();

    // Nunca debe llegar aquí
    while (1);
    return 0;
}

// Tarea que lee temperatura y la manda a la cola
void vTaskTempSensor(void *params) {
    while (1) {
        adc_select_input(4);   //hago selección del canal antes de tomar la medicion
        uint16_t raw = adc_read();  //variable que captura la medición del adc de 12 bits: 4096
        float voltage = raw * 3.3f / 4095;
        float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;   //transforma en función de la ecuación del datasheet

        // Envía temperatura a la cola
        if (xQueueSend(tempQueue, &temperature, 0) != pdPASS) {  //&temperature -> direccion de memoria donde ESTA la medicion
            printf("Cola llena: no se pudo enviar temperatura\n");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));  // Espera 5 segundos
    }
}

// Tarea que consume temperatura desde la cola
void vTaskTempConsumer(void *params) {
    float receivedTemp;  //variable que guarda el valor obtenido de la cola
    while (1) {
        // Espera indefinidamente por un mensaje
        if (xQueueReceive(tempQueue, &receivedTemp, portMAX_DELAY) == pdTRUE) {  //&receivedTemp -> direccion de memoria donde GUARDO la medicion
            printf("Temperatura recibida: %.2f °C\n", receivedTemp);
        }
    }
}