////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//Universidad del valle de Guatemala
//Alan Gomez - 22115
//Samuel Tortola - 22094
//Digital - 3 IE9059
//Laboratorio 6 
//Hardware: Raspberry Pi 5 8GB RAM
//Temporizadores, Tareas Periódicas y Sincronización Simple
//FECHA 2/03/2025
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
//Bibliotecas
////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////////
//Definiciones
////////////////////////////////////////////////////////////////////////////////////|
#define MAX_LETRAS 100
#define MAX_CADENAS 60

#define PRIMERO "primero.txt"  // archivos no están o no se quieren guardar
#define SEGUNDO "segundo.txt"  // en el directorio donde está el programa.

////////////////////////////////////////////////////////////////////////////////////
//Variables globales
////////////////////////////////////////////////////////////////////////////////////
pthread_t hilo[3]; // 3 hilos
FILE *fp_original, *fp_primero, *fp_segundo;// Archivos

////////////////////////////////////////////////////////////////////////////////////
//Funciones
////////////////////////////////////////////////////////////////////////////////////
void FIRST(void *ptr) {
    // Implementación de la función PRIMERO

    
}

void SECOND(void *ptr) {
    // Implementación de la función SEGUNDO
  
    
}

void THIRD(void *ptr) {
    // Implementación de la función TERCERO

    
}

////////////////////////////////////////////////////////////////////////////////////
//MAIN
////////////////////////////////////////////////////////////////////////////////////

int main(void) {

    pthread_create(&hilos[0], NULL, (void*)&FIRST, NULL);
    pthread_create(&hilos[2], NULL, (void*)&SECOND, NULL);
    pthread_create(&hilos[3], NULL, (void*)&THIRD, NULL);

    while(1){

    }
}


