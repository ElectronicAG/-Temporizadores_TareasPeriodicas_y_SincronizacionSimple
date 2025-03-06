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
#define MAX_LETRAS 100  //Máximo de letras en una línea
#define MAX_CADENAS 60  //Máximo de líneas a leer

#define PRIMERO "Lab6_primero.txt"    //Archivo 1
#define SEGUNDO "Lab6_segundo.txt"  //Archivo 2
#define RECONSTRUIDO "Lab6_reconstruido.txt"  //Archivo final



                                      //1 milisegundo (1 ms) equivale a 1,000,000 nanosegundos (1,000,000 ns).
#define PERIOD_FIRST  8000000  // n    Periodo Hilo 1
#define PERIOD_SECOND 8000000  // n    Periodo Hilo 2
#define PERIOD_THIRD  4000000  // n/2    Periodo Hilo 3

#define DES_FIRST  1000000         // n   Desfase  Hilo 1
#define DES_SECOND DES_FIRST + 4000000  // n/2  Desfase Hilo 2
#define DES_THIRD  DES_FIRST + 2000000  //n/4    Desfase Hilo 3


#define MI_PRIORIDAD 1  //Definir prioridad de los hilos


////////////////////////////////////////////////////////////////////////////////////
//Variables globales
////////////////////////////////////////////////////////////////////////////////////

FILE *primero, *segundo, *reconstruido;// Archivos
char buffer[MAX_LETRAS + 1];  // Buffer global
char StringArray[MAX_CADENAS][MAX_LETRAS+1]; // arreglo de cadenas
int cont = 0;  //Contador de líneas leídas


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
int configurar_timer(long periodo, long desfase) {
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timer_fd == -1) {
        perror("Error al crear el timer");
        exit(EXIT_FAILURE);
    }

    struct itimerspec itval;
    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = periodo; // Periodo diferente por hilo
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = desfase; // Desfase inicial para alternancia

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

	int timer_fd = configurar_timer(PERIOD_FIRST, DES_FIRST); 
    uint64_t expirations;


	while (fgets(buffer, MAX_LETRAS, primero) != NULL) {
        read(timer_fd, &expirations, sizeof(expirations)); // Esperar timer
      
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

	int timer_fd = configurar_timer(PERIOD_SECOND, DES_SECOND); 
    uint64_t expirations;

	while (fgets(buffer, MAX_LETRAS, segundo) != NULL) {

        read(timer_fd, &expirations, sizeof(expirations)); // Esperar timer
  
    }

    fclose(segundo);
	
	pthread_exit(0);	// Para salir correctamente del hilo.
  
    
}

// Función del hilo 3 que guarda en StringArray
void THIRD(void *ptr) {  

	configurar_prioridad();
    int timer_fd = configurar_timer(PERIOD_THIRD, DES_THIRD);   
    uint64_t expirations;

    while (cont < MAX_CADENAS) {
        strcpy(StringArray[cont], buffer);
        cont++;
        read(timer_fd, &expirations, sizeof(expirations)); // Esperar timer
    }

    pthread_exit(0);	// Para salir correctamente del hilo.
}


////////////////////////////////////////////////////////////////////////////////////
// Función para escribir al archivo RECONSTRUIDO
////////////////////////////////////////////////////////////////////////////////////
void escribirEnArchivo() {
    // Abrir en modo escritura (si ya existe, se sobrescribe)
    reconstruido = fopen(RECONSTRUIDO, "w");
    if (reconstruido == NULL) {
        perror("Error al abrir archivo RECONSTRUIDO");
        return;  
    }

    // Escribir cada línea de StringArray
    for (int i = 0; i < cont; i++) {

        fputs(StringArray[i], reconstruido);

        // Mostrar en la terminal:
        printf("%s", StringArray[i]);
        

    }

    fclose(reconstruido);
}

////////////////////////////////////////////////////////////////////////////////////
//MAIN
////////////////////////////////////////////////////////////////////////////////////

int main(void) {
    pthread_t hilos[3]; // 3 hilos
    pthread_create(&hilos[0], NULL, (void*)&FIRST, NULL);    //Crear hilo 1
    pthread_create(&hilos[1], NULL, (void*)&SECOND, NULL);   //Crear hilo 2
    pthread_create(&hilos[2], NULL, (void*)&THIRD, NULL);    //Crear hilo 3


   // Esperar a que elos hilos terminen
   pthread_join(hilos[0], NULL);
   pthread_join(hilos[1], NULL);
   pthread_join(hilos[2], NULL);

    // Llamar a la función de escritura en archivo
   escribirEnArchivo();

   return(0);

}
