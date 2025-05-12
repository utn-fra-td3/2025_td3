#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#define configTurnOnTime 1000
#define configTurnOffTime 1500

#define LED_PIN 0

bool ledState = 0;

int pico_led_init(void) {
    return cyw43_arch_init();
}

void pico_set_led(bool led_on) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
}

void TaskTurnOn(void* arg){
    for(;;)
    {
        if(!ledState){
            printf("Enciendo...\n");
            pico_set_led(true);
            vTaskDelay(pdMS_TO_TICKS(configTurnOnTime));
            ledState = 1;
        }
        else{
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    vTaskDelete(NULL);
}

void TaskTurnOff(void* arg){
    for(;;)
    {
        if(ledState){
            printf("Apago...\n");
            pico_set_led(false);
            vTaskDelay(pdMS_TO_TICKS(configTurnOffTime));
            ledState = 0;
        }
        else{
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    vTaskDelete(NULL);
}

int main()
{
    stdio_init_all();
    pico_led_init();

    xTaskCreate(
        TaskTurnOn,
        "TaskTurnOn",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
    );

    xTaskCreate(
        TaskTurnOff,
        "TaskTurnOff",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
    );

    vTaskStartScheduler();

    while (true) {
    }
}
