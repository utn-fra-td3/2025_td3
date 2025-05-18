#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
 
// Declaro la variable del handler dee la tarea
TaskHandle_t encender1, apagar2;

 void task_LED_ON(void *params) {
	while(1) {  

       // Suspendo la otra tarea
       vTaskSuspend (apagar2);

       // Enciendo
        gpio_put(PICO_DEFAULT_LED_PIN, 1);

        // Demora de encendido equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));

        // Me qudo preparadopara la próxima vez
       vTaskResume(apagar2);

       // Me suspendo a mi mismo
       vTaskSuspend (NULL);
    }
 }
    void task_LED_OFF(void *params) {
        while(1) {

            // Suspendo la otra tarea
            vTaskSuspend (encender1);

            // Apago
            gpio_put(PICO_DEFAULT_LED_PIN, 0);

            // Demora de apagado equivalentes a 500 ms
            vTaskDelay(pdMS_TO_TICKS(500));

            // Me qudo preparadopara la próxima vez
            vTaskResume(encender1);

           // Me suspendo a mi mismo
           vTaskSuspend (NULL);
        }
    }

int main()
{
    stdio_init_all();

    // Inicializacion de GPIO
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);

    // Creacion de tareas
    xTaskCreate(task_LED_ON, "Encendido", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), &encender1);
    xTaskCreate(task_LED_OFF, "Apagado", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), &apagar2);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);

}
