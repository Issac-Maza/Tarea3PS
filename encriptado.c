#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//TUVE QUE HACER UN ABECEDARIO YA QUE NO DESCIFRA DE MANERA NORMAL 
// Definir un abecedario personalizado
const char *nuevo_abecedario = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+[]{}|;:,.<>?/`~ ";

// Función para encontrar la posición de un carácter en el abecedario
int encontrar_posicion(char c, const char *abecedario) {
    char *pos = strchr(abecedario, c);
    return (pos) ? (pos - abecedario) : -1;
}

// Validar si un archivo existe
int validar_archivo(const char *nombre) {
    FILE *file = fopen(nombre, "r");
    if (file) {
        fclose(file);
        return 1;  // El archivo existe
    }
    return 0;  // El archivo no existe
}

// Cifrar un carácter
char cifrar_caracter(char c, int desplazamiento, int direccion, int claveXOR, const char *abecedario) {
    int pos = encontrar_posicion(c, abecedario);
    if (pos == -1) return c;  // Si el carácter no está en el abecedario, no se modifica
    int longitud = strlen(abecedario);

    // Aplicar desplazamiento
    if (direccion == 0) {  // Desplazamiento a la izquierda
        pos = (pos - desplazamiento + longitud) % longitud;
    } else {  // Desplazamiento a la derecha
        pos = (pos + desplazamiento) % longitud;
    }

    // Aplicar XOR
    return abecedario[pos] ^ claveXOR;
}

// Descifrar un carácter
char descifrar_caracter(char c, int desplazamiento, int direccion, int claveXOR, const char *abecedario) {
    // Invertir XOR
    c ^= claveXOR;

    int pos = encontrar_posicion(c, abecedario);
    if (pos == -1) return c;  // Si el carácter no está en el abecedario, no se modifica
    int longitud = strlen(abecedario);

    // Aplicar desplazamiento inverso
    if (direccion == 0) {  // Desplazamiento a la izquierda
        pos = (pos + desplazamiento) % longitud;
    } else {  // Desplazamiento a la derecha
        pos = (pos - desplazamiento + longitud) % longitud;
    }

    return abecedario[pos];
}

// Cifrar un archivo
void cifrar_archivo(const char *entrada, const char *salida, const char *resumen, const char *abecedario) {
    if (!validar_archivo(entrada)) {
        printf("Error: El archivo %s no existe.\n", entrada);
        return;
    }

    FILE *in = fopen(entrada, "r");
    FILE *out = fopen(salida, "w");
    FILE *res = fopen(resumen, "a");
    if (!in || !out || !res) {
        printf("Error al abrir los archivos.\n");
        if (in) fclose(in);
        if (out) fclose(out);
        if (res) fclose(res);
        return;
    }

    srand(time(NULL));
    int direccion = rand() % 2;  // 0 = izquierda, 1 = derecha
    int desplazamiento = rand() % 7 + 1;  // Entre 1 y 7
    int claveXOR = rand() % 256;  // Entre 0 y 255

    // Escribir en el archivo resumen.txt con el formato correcto
    fprintf(res, "%s-%d-%d-%d\n", salida, direccion, desplazamiento, claveXOR);
    fclose(res);  // Cerrar archivo resumen

    // Procesar y cifrar el archivo
    char c;
    while ((c = fgetc(in)) != EOF) {
        char cifrado = cifrar_caracter(c, desplazamiento, direccion, claveXOR, abecedario);
        fputc(cifrado, out);
    }

    fclose(in);
    fclose(out);

    printf("Archivo cifrado como: %s\n", salida);
    printf("Parámetros guardados en resumen.txt: %s-%d-%d-%d\n", salida, direccion, desplazamiento, claveXOR);
}

// Descifrar un archivo
void descifrar_archivo(const char *entrada, const char *salida, const char *resumen, const char *abecedario) {
    FILE *in = fopen(entrada, "r");
    FILE *out = fopen(salida, "w");
    FILE *res = fopen(resumen, "r");
    if (!in || !out || !res) {
        printf("Error al abrir los archivos.\n");
        if (in) fclose(in);
        if (out) fclose(out);
        if (res) fclose(res);
        return;
    }

    char linea[300];
    char prefijo[10], archivo[200];
    int direccion, desplazamiento, claveXOR, encontrado = 0;

    // Leer línea por línea de resumen.txt
    while (fgets(linea, sizeof(linea), res)) {
        linea[strcspn(linea, "\r\n")] = '\0';  // Eliminar saltos de línea y retornos de carro

        // Separar la línea en cinco partes
        if (sscanf(linea, "%9[^-]-%199[^-]-%d-%d-%d", prefijo, archivo, &direccion, &desplazamiento, &claveXOR) == 5) {
        
            printf("Prefijo: '%s', Archivo: '%s', Dirección: %d, Desplazamiento: %d, ClaveXOR: %d\n",
                   prefijo, archivo, direccion, desplazamiento, claveXOR);

            
            char archivo_completo[300];
            snprintf(archivo_completo, sizeof(archivo_completo), "%s-%s", prefijo, archivo);

            // Comparar con el archivo encriptado
            if (strcmp(archivo_completo, entrada) == 0) {
                encontrado = 1;
                break;
            }
        } else {
            printf("Error al procesar la línea: '%s'\n", linea);
        }
    }

    if (!encontrado) {
        printf("Error: No se encontró información para %s en resumen.txt.\n", entrada);
        fclose(in);
        fclose(out);
        fclose(res);
        return;
    }

    // Descifrar el archivo
    char c;
    while ((c = fgetc(in)) != EOF) {
        char descifrado = descifrar_caracter(c, desplazamiento, direccion, claveXOR, abecedario);
        fputc(descifrado, out);
    }

    fclose(in);
    fclose(out);
    fclose(res);

    printf("Archivo descifrado como: %s\n", salida);
}

int main() {
    char entrada[95], salida[100];
    const char *resumen = "resumen.txt";
    int opcion;

    do {
        printf("1. Cifrar archivo\n");
        printf("2. Descifrar archivo\n");
        printf("3. SALIR\n");
        printf("OPCION: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Nombre del archivo a cifrar: ");
                scanf("%94s", entrada);
                snprintf(salida, sizeof(salida), "Enc-%s", entrada);
                cifrar_archivo(entrada, salida, resumen, nuevo_abecedario);
                break;
            case 2:
                printf("Nombre del archivo a descifrar: ");
                scanf("%94s", entrada);
                snprintf(salida, sizeof(salida), "Des-%s", entrada + 4);
                descifrar_archivo(entrada, salida, resumen, nuevo_abecedario);
                break;
            case 3:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    } while (opcion != 3);

    return 0;
}
