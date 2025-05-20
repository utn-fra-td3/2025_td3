#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" //Libreria para el Led, en realidad activa 
                            //el modulo wifi que es necesario para activar el led

#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t TareaPrendeHandler,TareaApagarHandler;


void Inicializar(void *params) {
    
    // Inicializacion de GPIO
    gpio_init(CYW43_WL_GPIO_LED_PIN);
    gpio_set_dir(CYW43_WL_GPIO_LED_PIN, true);
    gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // Elimino la tarea para liberar recursos
    vTaskDelete(NULL);
}

void Tarea_Prende(void *params) {
    while(1){
        // Toggle
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN,1);
        //Despauso la Tarea Apaga
        vTaskResume(TareaApagarHandler);
        //Suspendo la Tarea Prende
        vTaskSuspend(NULL);
        }
}

void Tarea_Apaga(void *params) {
    while(1){
        // Demora de ticks equivalentes a 1000 ms   
        vTaskDelay(pdMS_TO_TICKS(1000));     
        // Toggle
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN,0);
        // Demora de ticks equivalentes a 3000 ms
        vTaskDelay(pdMS_TO_TICKS(1500));
        //Despauso la Tarea Prende
        vTaskResume(TareaPrendeHandler);
        //Suspendo la Tarea Apaga
        vTaskSuspend(NULL);
    }
}

int main(void) {
    stdio_init_all();

 
    // Inicializo el Wi-Fi chip
    cyw43_arch_init();

    // Creacion de tareas
    xTaskCreate(Inicializar, "Inicializar", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    xTaskCreate(Tarea_Prende, "Tarea_Prende", configMINIMAL_STACK_SIZE, NULL, 1, &TareaPrendeHandler);

    xTaskCreate(Tarea_Apaga, "Tarea_Apaga", configMINIMAL_STACK_SIZE, NULL, 1, &TareaApagarHandler);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);

}