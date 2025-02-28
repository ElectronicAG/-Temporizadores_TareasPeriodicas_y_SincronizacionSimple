/* Lab6_files_y_strings.c 
   Por: Luis Alberto Rivera
   
   Este programa muestra funciones para manejo de cadenas y archivos.
   Abre un archivo y guarda las líneas impares de ese archivo en un archivo
   nuevo, y las líneas pares del archivo original en otro archivo nuevo.
   
   Este programa se usó para generar los archivos a utilizar en el lab.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LETRAS 100
#define MAX_CADENAS 60
#define ORIGINAL "Prueba.txt"  // Se podría agregar una ruta completa, si los
#define PRIMERO "primero.txt"  // archivos no están o no se quieren guardar
#define SEGUNDO "segundo.txt"  // en el directorio donde está el programa.

int main()
{
	FILE *fp_original, *fp_primero, *fp_segundo;	// para los archivos
	char Cadena[MAX_LETRAS+1];	// string (cadena de caracteres).
	char StringArray[MAX_CADENAS][MAX_LETRAS+1]; // arreglo de cadenas
	int i, cont = 0;

	fp_original = fopen(ORIGINAL, "r");  // abrir para lectura
	if(fp_original == NULL)
    {
		perror("Error al abrir el archivo.");
		exit(0);
	}
	
	// Bucle que lee todas las líneas de un archivo y las guarda en el StringArray.
	// El bucle para al detectarse el final del archivo.
	// Tener cuidado de que el número de líneas no supere MAX_CADENAS, o el StringArray no
	// tendrá espacio suficiente.
	// Se pudo implementar usando un for loop. Se pudo usar StringArray directamente en
	// fgets, pero se hace así para ilustrar el uso de las funciones.
	while(fgets(Cadena, MAX_LETRAS, fp_original) != NULL)
    {
		strcpy(StringArray[cont], Cadena);	// strcpy = "string copy"
		printf(StringArray[cont]);	// muestra la cadena copiada en la terminal
		fflush(stdout);
		cont++;
	}
	fclose(fp_original);	// cierra el archivo original.
	
	printf("\nNúmero de líneas leídas: %d\n", cont);
	
	// Abre los archivos para escritura
	fp_primero = fopen(PRIMERO, "w");
	fp_segundo = fopen(SEGUNDO, "w");
	
	for(i = 0; i < cont; i++)
    {
		if((i%2) == 0)	// Líneas impares (corresponden a índices pares)
			fputs(StringArray[i], fp_primero);	// escribe una línea en el archivo
		else	// Líneas pares (corresponden a índices impares)
			fputs(StringArray[i], fp_segundo);	// escribe una línea en el archivo
	}

	fclose(fp_primero);	// cierra el archivo
	fclose(fp_segundo);	// cierra el archivo
		
	printf("\nListo...\n");

	return 0;
}

