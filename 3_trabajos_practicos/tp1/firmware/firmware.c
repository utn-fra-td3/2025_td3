#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN 25
#define on 1000   // Tiempo que el LED está encendido (ms)
#define off 1500  // Tiempo que el LED está apagado (ms)

void encender_led_task(void *pvParameters) { // Se crea la función encender_led_task.
    while (1) {
        gpio_put(LED_PIN, 1);  // Enciende el LED - gpio_put(25, 1); → pone en ALTO (enciende el LED).
        vTaskDelay(pdMS_TO_TICKS(on));  
        // Pone en espera la tarea actual durante una cantidad de ticks del sistema,
        // liberando el procesador para la ejecución de otras tareas.
        // Según la frecuencia del reloj de FreeRTOS (configTICK_RATE_HZ, normalmente 1000 Hz = 1 tick = 1 ms),
        // entonces 500 ticks son los 500ms pedidos. Termina el tiempo y la tarea se desbloquea.
    }
}

void apagar_led_task(void *pvParameters) { // Se crea la función apagar_led_task.
    while (1) {
        gpio_put(LED_PIN, 0);  // Apaga el LED - gpio_put(25, 0); → pone en BAJO (apaga el LED).
        vTaskDelay(pdMS_TO_TICKS(off));  
        // Pone en espera la tarea actual durante una cantidad de ticks del sistema,
        // liberando el procesador para la ejecución de otras tareas.
        // Según la frecuencia del reloj de FreeRTOS (configTICK_RATE_HZ, normalmente 1000 Hz = 1 tick = 1 ms),
        // entonces 500 ticks son los 500ms pedidos. Termina el tiempo y la tarea se desbloquea.
    }
}

int main() {
    stdio_init_all();                             // Inicializa la entrada/salida estándar (UART)
    gpio_init(LED_PIN);                           // Inicializa el pin 25 para usarlo como GPIO.
    gpio_set_dir(LED_PIN, GPIO_OUT);              // Pone el pin 25 en modo salida.

    // Crea la tarea "EncenderLED" que ejecutará la función encender_led_task.
    // Esta tarea tendrá 256 bytes de pila, lo cual es suficiente para tareas simples.
    // No se pasan parámetros a la tarea (primer NULL).
    // La prioridad de la tarea es 1, es decir, una prioridad baja (0 sería la mayor prioridad).
    // No se guarda un manejador para la tarea, por eso el segundo NULL.
    xTaskCreate(encender_led_task, "EncenderLED", 256, NULL, 1, NULL);

    // Crea la tarea "ApagarLED" que ejecutará la función apagar_led_task.
    // Similar a la tarea de encender el LED, con 256 bytes de pila, sin parámetros, 
    // con prioridad 1 y sin un manejador de tarea.
    xTaskCreate(apagar_led_task, "ApagarLED", 256, NULL, 1, NULL);

    vTaskStartScheduler();  // Inicia el planificador de tareas FreeRTOS que permite
                            // la ejecución concurrente de tareas según sus prioridades y otros factores.
                            // El flujo del programa pasa de la función main() al planificador de FreeRTOS
                            // después de llamar a esta función. Sin esta función, las tareas creadas
                            // con xTaskCreate() no se ejecutan.

    while (1);  // Nunca debería llegar acá
}
