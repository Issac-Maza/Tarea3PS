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

    char archivoSaliente[100] = "Enc-";
    strcat(archivoSaliente, archivoDeEntrada);

    FILE *salida = fopen(archivoSaliente, "w");
    srand(time(NULL));
    int desplazamiento = rand() % 7 + 1;
    int keyXOR = rand() % 2;

    FILE *archivoResumen = fopen("resumen.txt", "a");
    if (archivoResumen) {
        fprintf(archivoResumen, "%s-%d-%d\n", archivoSaliente, desplazamiento, keyXOR);
        printf("Escribiendo en resumen.txt: %s-%d-%d\n", archivoSaliente, desplazamiento, keyXOR); // Depuración
        fclose(archivoResumen);
    } else {
        printf("Error: No se pudo abrir el archivo resumen.txt para escribir.\n");
    }

    int caracter;
    while ((caracter = fgetc(entrada)) != EOF) {
        caracter = ((caracter << desplazamiento) | (caracter >> (8 - desplazamiento))) ^ keyXOR;
        fputc(caracter, salida);
    }

    fclose(entrada);
    fclose(salida);
    printf("Archivo encriptado como: %s\n", archivoSaliente);
}

void descifrarArchivo(const char *archivoEncriptado) {
    if (strncmp(archivoEncriptado, "Enc-", 4) != 0) {
        printf("Error: El archivo proporcionado no tiene el formato esperado (Enc-).\n");
        return;
    }

    FILE *archivoResumen = fopen("resumen.txt", "r");
    if (!archivoResumen) {
        printf("Error: No se encontró el archivo resumen.txt.\n");
        printf("Asegúrese de encriptar un archivo antes de intentar desencriptar.\n");
        return;
    }

    char archivo[100];
    int desplazamiento, keyXOR, encontrado = 0;

    while (fscanf(archivoResumen, "%99[^-]-%d-%d\n", archivo, &desplazamiento, &keyXOR) != EOF) {
        archivo[strcspn(archivo, "\n")] = '\0'; // Elimina salto de línea
        printf("Leyendo: %s-%d-%d\n", archivo, desplazamiento, keyXOR); // Depuración
        printf("Comparando '%s' con '%s'\n", archivo, archivoEncriptado); // Comparación

        if (strcmp(archivo, archivoEncriptado) == 0) {
            encontrado = 1;
            break;
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

    char archivoSalida[100] = "Des-";
    strcat(archivoSalida, archivoEncriptado + 4); // Remueve "Enc-"
    FILE *salida = fopen(archivoSalida, "w");

    if (!salida) {
        printf("Error: No se pudo crear el archivo %s.\n", archivoSalida);
        fclose(entrada);
        return;
    }

    int caracter;
    while ((caracter = fgetc(entrada)) != EOF) {
        caracter = ((caracter ^ keyXOR) >> desplazamiento) | ((caracter ^ keyXOR) << (8 - desplazamiento));
        fputc(caracter, salida);
    }

    fclose(entrada);
    fclose(salida);
    printf("Archivo desencriptado como: %s\n", archivoSalida);
}

int main() {
    int opcion;
    char nombreArchivo[100];

    do {
        printf("1. Encriptar archivo\n");
        printf("2. Desencriptar archivo\n");
        printf("3. SALIR\n");
        printf("OPCION: ");
        scanf("%d", &opcion);
        getchar(); // Limpia el buffer

        if (opcion == 1) {
            printf("Nombre del archivo a encriptar: ");
            scanf("%99s", nombreArchivo);
            cifrarArchivo(nombreArchivo);
        } else if (opcion == 2) {
            printf("Nombre del archivo encriptado: ");
            scanf("%99s", nombreArchivo);
            descifrarArchivo(nombreArchivo);
        }
    } while (opcion != 3);

    printf("Saliendo...\n");
    return 0;
}

