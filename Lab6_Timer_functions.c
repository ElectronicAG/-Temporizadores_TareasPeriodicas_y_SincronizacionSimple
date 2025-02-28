/* Lab6_Timer_functions.c
   Por: Luis Alberto Rivera
   Nota: Lo siguiente no es un programa funcional. Se muestran headers
         definiciones, funciones y cómo se usan, lo que será útil para
		 completar el laboratorio 6.
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>

#define MI_PRIORIDAD 1  // Rango entre 1 y 99, aunque es posible que no se permita asignar
						// prioridades > 90 o 95. A mayor valor, más alta la prioridad.

// --- Asgingar prioridad y política de escalonamiento a un hilo ---
	struct sched_param param;	// estructura que contiene la prioridad
    // Asignar prioridad y la política de escalonamiento
	param.sched_priority = MI_PRIORIDAD; // Rango entre 1 y 99, si se usa SCHED_FIFO o SCHED_RR
									// Si se usan las otras políticas (incluyendo la por defecto),
									// este valor no se usa (se debe especificar como 0).
	sched_setscheduler(0, SCHED_FIFO, &param);	// esta función retorna algo
		// Siempre es bueno chequear errores. Revisar la función sched_setscheduler()
		// Si el primer argumento es cero, se configura el hilo que llama a la función.
		// Para esta lab usaremos la política SCHED_FIFO.


// --- Configurar Timer (se podría crear una función que regrese el file descriptor) ---
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);	// retorna un file descriptor
		// Siempre es bueno chequear errores. Revisar la función timerfd_create()
		// Para este lab usaremos la opción CLOCK_MONOTONIC.

	struct itimerspec itval;	// estructura que contiene el período y el tiempo inicial
    // El Timer se "dispara" cada ## segundos + ## nanosegundos
    itval.it_interval.tv_sec = Periodo_segundos;	  // Revisar el tipo de la variable (¿int, long?)
    itval.it_interval.tv_nsec = Periodo_nanosegundos; // Revisar el tipo de la variable (¿int, long?)
	
    // El Timer empezará en ## segundos + ## nanosegundos desde el momento en que se inicie el Timer
    itval.it_value.tv_sec = TiempoIni_segundos;	     // Revisar el tipo de la variable
    itval.it_value.tv_nsec = TiempoIni_nanosegundos; // Revisar el tipo de la variable

	
// --- Arrancar el Timer (se necesita el file descriptor y la estructura itval de arriba) ---
	timerfd_settime(timer_fd, 0, &itval, NULL);	 // esta función retorna algo
		// Siempre es bueno chequear errores. Revisar la función timerfd_settime()

		
// --- Lo siguiente es para esperar que el Timer expire (se podría crear una función
//	   que tenga como argumento el file descriptor). Típicamente, lo siguiente
//     está dentro de un bucle, si se quiere tener periodicidad. O puede llamarse
//     esporádicamente, si sólo se quiere esperar una vez el tiempo configurado.
//     Cada vez que se ejecute lo siguiente, se esperará el tiempo restante del
//     período actual. La función read guardará en num_periods el número de períodos
//	   que han pasado desde la última vez que se chequeó (idealmente sólo 1 vez).
//	   La función read es "blocking function" por defecto (default). ---

	uint64_t num_periods;
// Sugerencia: llamar las siguientes instrucciones inmediatamente después de configurar
//             el timer, antes de entrar al bucle (donde se llamarán constantemente).
// 			   Esto es para que los hilos empiecen bien sincronizados desde el inicio.
	num_periods = 0;
    read(timer_fd, &num_periods, sizeof(num_periods));	 // esta función retorna algo
		              // Siempre es bueno chequear errores. Revisar la función read()
    if(num_periods > 1)
    {
        puts("MISSED WINDOW");	// similar a printf, manda la cadena al stdandard output
        exit(1);
    }
    