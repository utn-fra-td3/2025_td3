#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN 25		//	25 ES EL PIN DEL LED

// Creo handles de tareas
TaskHandle_t hLedOn;		//	Creo el puntero de la tarea => (TaskHabdle_t) Tipo de la tarea
TaskHandle_t hLedOff;

//	SIEMPRE HAY QUE DEFINIR LAS TAREAS ANTES DEL MAIN

//	ESTRUCTURA DE UNA TAREA	//

//	STATIC VOID -TAREA- (No usamos pasar parametros a tarea)
//	ANTES DEL WHILE SE EJECUTA UNA SOLA VEZ. (Declarar variables, y todo lo que sea configuracion de la tarea) (Simil setup de arduino)
//	WHILE (1)
//	DENTRO DEL WHILE ES EL CODIGO DE LA TAREA (Simil loop arduino)

//	NO SE PUEDE SABER EN QUE PARTE DEL PROGRAMA ESTOY, DEPENDE DEL KERNEL, EL CUAL DEPENDE DE LA PRIORIDAD
								
// TAREA LED ON
static void task_LedOn(void *pvParameters){
	
	while(1){
		// Suspendo la tarea LedOff
		vTaskSuspend(hLedOff);

		// Enciendo el led
		gpio_put(LED_PIN, true);

		// Delay 1000 ms
        vTaskDelay(pdMS_TO_TICKS(1000));			//	BLOCKEA LA TAREA POR LA CANTIDAD DE TICKS QUE LE DIGO. CADA TICK ES 1ms. TERMINA EL TIEMPO Y LA TAREA SE DESBLOQUEA

												//	BLOCKEO POR SOFT (vtaskDELAY), o POR UN EVENTO DE HARDWARE (por ejemplo un boton)

												// PDMS_TO_TICK MACRO QUE CONVIERTE MS A TICKS
		

		// Habilito la tarea LedOff
		vTaskResume(hLedOff);

        // Suspendo la tarea LedOn para que entre LedOff
        vTaskSuspend(NULL);
	}
}

// TAREA LED OFF
static void task_LedOff(void *pvParameters){
	
	while(1){
		// Apago el led
        gpio_put(LED_PIN, false);

		// Delay 1500 ms
        vTaskDelay(pdMS_TO_TICKS(1500));

		// Habilito la tarea LedOn
		vTaskResume(hLedOn);
	}
}

int main(void) {
    stdio_init_all();								
    
    gpio_init(PICO_DEFAULT_LED_PIN);				//	HABILITAR EL GPIO
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);		//	PONERLO COMO SALIDA (False = Entrada) (True = Salida)
    gpio_put(PICO_DEFAULT_LED_PIN, 0);				//	SETEO QUE ARRANQUE EN OFF

    // Creacion de tareas
	xTaskCreate(task_LedOn, 			//	Creo tarea y le pongo nombre
	"LED-ON",  							//	Etiqueta de la tarea
	configMINIMAL_STACK_SIZE, 			//	Espacio que va a ocupar en el stack 
	NULL, 								//	Parametros (No lo usamos)
	2,									//	Prioridad de la tarea (En este caso es la mas prioritaria) (CUanto mas grande, mayor prioridad)
	&hLedOn); 							//	Es el hanndler, es el puntero de la tarea. Se usa para manejar la tarea. Ej: Suspender la tarea, reactivar, etc
	
	xTaskCreate(task_LedOff, 
	"LED-OFF", 
	configMINIMAL_STACK_SIZE, 
	NULL, 
	1, 									//	Prioridad en la tarea (En este caso es la menos prioritaria)
	&hLedOff);

    // START SCHEDULER
    vTaskStartScheduler();				//	Arranque del kernel del sistema
    while(1);							//	Si todo funciona correctamente, nunca llegamos a esta linea
}