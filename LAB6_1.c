////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//Universidad del valle de Guatemala
//Alan Gomez - 22115
//Samuel Tortola - 22094
//Digital - 3 IE9059
//Laboratorio 6 
//Hardware: Raspberry Pi 5 8GB RAM
//Temporizadores, Tareas Periódicas y Sincronización Simple 
//FECHA 6/03/2025
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
//Bibliotecas
////////////////////////////////////////////////////////////////////////////////////


#define _GNU_SOURCE

#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timerfd.h>



////////////////////////////////////////////////////////////////////////////////////
//Definiciones
////////////////////////////////////////////////////////////////////////////////////|
#define MAX_LETRAS 100
#define MAX_CADENAS 60

#define PRIMERO "Lab6_primero.txt"
#define SEGUNDO "Lab6_segundo.txt"
#define RECONSTRUIDO "Lab6_reconstruido.txt"

#define PERIOD_FIRST 300000000  // 300ms
#define PERIOD_THIRD 200000000  // 200ms
#define PERIOD_SECOND 400000000 // 400ms

#define OFFSET_NS 100000000  // 100ms de desfase
#define MI_PRIORIDAD 1


////////////////////////////////////////////////////////////////////////////////////
//Variables globales
////////////////////////////////////////////////////////////////////////////////////

FILE *primero, *segundo;// Archivos
char buffer[MAX_LETRAS + 1];  // Buffer global
char StringArray[MAX_CADENAS][MAX_LETRAS+1]; // arreglo de cadenas
int cont = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;   //Mas seguridad a los hilos, evita que dos hilos accedan a la misma variable al mismo tiempo

////////////////////////////////////////////////////////////////////////////////////
//Funciones
////////////////////////////////////////////////////////////////////////////////////

// Configurar prioridad del hilo
void configurar_prioridad() {
    struct sched_param param;
    param.sched_priority = MI_PRIORIDAD;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("Error al asignar prioridad");
        exit(EXIT_FAILURE);
    }
}

// Configurar Timer con diferentes periodos
int configurar_timer(long periodo, long offset) {
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timer_fd == -1) {
        perror("Error al crear el timer");
        exit(EXIT_FAILURE);
    }

    struct itimerspec itval;
    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = periodo; // Periodo diferente por hilo
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = offset; // Desfase inicial para alternancia

    if (timerfd_settime(timer_fd, 0, &itval, NULL) == -1) {
        perror("Error al configurar el timer");
        exit(EXIT_FAILURE);
    }

    return timer_fd;

}


// Función del hilo 1 (leer Lab6_primero.txt)
void FIRST(void *ptr) {
	
	configurar_prioridad();

	primero = fopen(PRIMERO, "r");  // abrir para lectura
	if(primero == NULL)
    {
		perror("Error al abrir el primer archivo.");
		exit(0);
	}

	int timer_fd = configurar_timer(PERIOD_FIRST, OFFSET_NS); // Periodo de 300ms, sin desfase
    uint64_t expirations;


	while (fgets(buffer, MAX_LETRAS, primero) != NULL) {
        read(timer_fd, &expirations, sizeof(expirations)); // Esperar timer
        pthread_mutex_lock(&mutex);
        printf("[FIRST] Leyó: %s", buffer);
        pthread_mutex_unlock(&mutex);
    }

	fclose(primero);
	

	pthread_exit(0);	// Para salir correctamente del hilo.

    
}

// Función del hilo 2 (leer Lab6_segundo.txt)
void SECOND(void *ptr) {
	configurar_prioridad();

	segundo = fopen(SEGUNDO, "r");  // abrir para lectura
	if(segundo == NULL)
    {
		perror("Error al abrir el segundo archivo.");
		exit(0);
	}

	int timer_fd = configurar_timer(PERIOD_SECOND, 2 * OFFSET_NS); // Periodo de 400ms, inicia a los 200ms
    uint64_t expirations;

	while (fgets(buffer, MAX_LETRAS, segundo) != NULL) {
        read(timer_fd, &expirations, sizeof(expirations)); // Esperar timer
        pthread_mutex_lock(&mutex);
        printf("[SECOND] Leyó: %s", buffer);
        pthread_mutex_unlock(&mutex);
    }

    fclose(segundo);
	
	pthread_exit(0);	// Para salir correctamente del hilo.
  
    
}

// FUnción del hilo 3 que guarda en StringArray
void THIRD(void *ptr) {  

	configurar_prioridad();
    int timer_fd = configurar_timer(PERIOD_THIRD, OFFSET_NS); // Periodo de 200ms, inicia a los 100ms
    uint64_t expirations;

    while (cont < MAX_CADENAS) {
        read(timer_fd, &expirations, sizeof(expirations)); // Esperar timer
        pthread_mutex_lock(&mutex);

        if (cont < MAX_CADENAS) {
            strcpy(StringArray[cont], buffer);
            printf("[THIRD] Guardando: %s", StringArray[cont]);
            cont++;
        }

        pthread_mutex_unlock(&mutex);
    }

	pthread_exit(0);	// Para salir correctamente del hilo.
    
}


////////////////////////////////////////////////////////////////////////////////////
//MAIN
////////////////////////////////////////////////////////////////////////////////////

int main(void) {
    pthread_t hilos[3]; // 3 hilos
    pthread_create(&hilos[0], NULL, (void*)&FIRST, NULL);
    pthread_create(&hilos[1], NULL, (void*)&SECOND, NULL);
    pthread_create(&hilos[2], NULL, (void*)&THIRD, NULL);



   // Esperar a que el hilo termine
   pthread_join(hilos[0], NULL);
   pthread_join(hilos[1], NULL);
   pthread_join(hilos[2], NULL);

   printf("\nNúmero de líneas leídas: %d\n", cont);

   return(0);

}
