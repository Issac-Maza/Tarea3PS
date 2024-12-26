#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void cifrarArchivo(const char *archivoDeEntrada) {
	FILE *entrada = fopen(archivoDeEntrada, "r");
	if (!entrada) {
		printf("Error: No se pudo abrir el archivo %s.\n", archivoDeEntrada);
		return;
    	}

	char archivoSaliente[150] = "Enc-";
	strcat(archivoSaliente, archivoDeEntrada);

	FILE *salida = fopen(archivoSaliente, "w");
	if (!salida) {
		printf("Error: No se pudo crear el archivo %s.\n", archivoSaliente);
		fclose(entrada);
		return;
	}

	srand(time(NULL));
	int desplazamiento = rand() % 7 + 1;//Valor entre 1 y 7
	int direccion = rand() % 2;//0:izquierda, 1:derecha
	int keyXOR = rand() % 256;//valor entre 0 y255

	FILE *archivoResumen = fopen("resumen.txt", "a");
	if (archivoResumen) {
		fprintf(archivoResumen, "%s-%d-%d-%d\n", archivoSaliente, direccion, desplazamiento, keyXOR);
		fclose(archivoResumen);
	} else {
		printf("Error: No se pudo abrir el archivo resumen.txt para escribir.\n");
	}

	int caracter;
	while ((caracter = fgetc(entrada)) != EOF) {
		if (direccion == 0) { // Desplazamiento a la izquierda
        		caracter = ((caracter << desplazamiento) | (caracter >> (8 - desplazamiento))) & 0xFF;
		} else { // Desplazamiento a la derecha
        		caracter = ((caracter >> desplazamiento) | (caracter << (8 - desplazamiento))) & 0xFF;
        	}
		caracter ^= keyXOR;
		fputc(caracter, salida);
    	}

	fclose(entrada);
	fclose(salida);
	printf("Archivo encriptado como: %s\n", archivoSaliente);
}

void descifrarArchivo(const char *archivoEncriptado) {
	FILE *archivoResumen = fopen("resumen.txt", "r");
	if (!archivoResumen) {
		printf("Error: No se encontró el archivo resumen.txt.\n");
		return;
    	}

	char linea[200];
	char archivo[150];
	int direccion, desplazamiento, keyXOR;
	int encontrado = 0;


	while (fgets(linea, sizeof(linea), archivoResumen)) {
		// Eliminar el salto de línea al final de la línea, si existe
		linea[strcspn(linea, "\n")] = '\0';

		// Utilizar strtok para dividir la línea en partes
		char *token = strtok(linea, "-");
		if (token != NULL) {
			strncpy(archivo, token, sizeof(archivo));
			archivo[sizeof(archivo) - 1] = '\0'; // Asegurar la terminación nula

			token = strtok(NULL, "-");
			if (token != NULL) {
				direccion = atoi(token);

				token = strtok(NULL, "-");
				if (token != NULL) {
					desplazamiento = atoi(token);

					token = strtok(NULL, "-");
					if (token != NULL) {
						keyXOR = atoi(token);

						// Comparar el nombre del archivo
						if (strcmp(archivo, archivoEncriptado) == 0) {
							encontrado = 1;
							break;
						}
					}
				}
			}
		}
	}
	fclose(archivoResumen);

	if (!encontrado) {
		printf("Error: No se encontró información para %s.\n", archivoEncriptado);
		return;
	}

	FILE *entrada = fopen(archivoEncriptado, "r");
	if (!entrada) {
		printf("Error: No se pudo abrir el archivo %s.\n", archivoEncriptado);
		return;
	}

	char archivoSalida[150] = "Des-";
	strcat(archivoSalida, archivoEncriptado + 4);

	FILE *salida = fopen(archivoSalida, "w");
	if (!salida) {
		printf("Error: No se pudo crear el archivo %s.\n", archivoSalida);
		fclose(entrada);
		return;
	}

	int caracter;
	while ((caracter = fgetc(entrada)) != EOF) {
		caracter ^= keyXOR;
		if (direccion == 0) { // Desplazamiento a la izquierda
			caracter = ((caracter >> desplazamiento) | (caracter << (8 - desplazamiento))) & 0xFF;
		} else { // Desplazamiento a la derecha
			caracter = ((caracter << desplazamiento) | (caracter >> (8 - desplazamiento))) & 0xFF;
		}
		fputc(caracter, salida);
	}

	fclose(entrada);
	fclose(salida);
	printf("Archivo desencriptado como: %s\n", archivoSalida);
}

int main() {
	int opcion;
	char nombreArchivo[150];

	do {
		printf("1. Encriptar archivo\n");
		printf("2. Desencriptar archivo\n");
		printf("3. SALIR\n");
		printf("OPCION: ");
		if (scanf("%d", &opcion) != 1) {
			printf("Entrada no válida. Por favor, ingrese un número.\n");
			while (getchar() != '\n'); // Limpiar el buffer de entrada
			continue;
	}
	getchar(); // Limpiar el salto de línea del buffer

	switch (opcion) {
		case 1:
			printf("Nombre del archivo a encriptar: ");
			if (scanf("%149s", nombreArchivo) != 1) {
				printf("Entrada no válida.\n");
				while (getchar() != '\n'); // Limpiar el buffer de entrada
				continue;
			}
			cifrarArchivo(nombreArchivo);
			break;
		case 2:
			printf("Nombre del archivo encriptado: ");
			if (scanf("%149s", nombreArchivo) != 1) {
				printf("Entrada no válida.\n");
				while (getchar() != '\n'); // Limpiar el buffer de entrada
				continue;
			}
			descifrarArchivo(nombreArchivo);
			break;
		case 3:
			printf("Saliendo...\n");
			break;
		default:
			printf("Opción no válida. Por favor, seleccione 1, 2 o 3.\n");
		}
	} while (opcion != 3);

	return 0;
}

