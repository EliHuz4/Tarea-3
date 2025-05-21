#include "extra.h"
#include <stdio.h>
#include <string.h>


#define MAX_LINE_LENGTH 4096
#define MAX_FIELDS      128

char **leer_linea_csv(FILE *archivo, char separador) {
    // Las variables se asignan dinamicamente y se liberan al final
    char *linea = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    if (linea == NULL) return NULL; // Error de asignación

    char **campos = (char **)malloc(MAX_FIELDS * sizeof(char *));
    if (campos == NULL) {
        free(linea);
        return NULL; // Error de asignación
    }
    int idx = 0;

    if (fgets(linea, MAX_LINE_LENGTH, archivo) == NULL) {
        free(linea); // Liberar memoria antes de salir
        free(campos);
        return NULL;  // fin de fichero
    }

    // quitar salto de línea
    linea[strcspn(linea, "\r\n")] = '\0';

    char *ptr = linea;
    while (*ptr && idx < MAX_FIELDS - 1) {
        char *start;

        if (*ptr == '\"') {
            // campo entrecomillado
            ptr++;           // saltar la comilla inicial
            start = ptr;

            // compactar contenido: convertir "" → " y copiar el resto
            char *dest = ptr;
            while (*ptr) {
                if (*ptr == '\"' && *(ptr + 1) == '\"') {
                    *dest++ = '\"';  // una comilla literal
                    ptr += 2;        // saltar ambas
                }
                else if (*ptr == '\"') {
                    ptr++;           // fin del campo
                    break;
                }
                else {
                    *dest++ = *ptr++;
                }
            }
            *dest = '\0';        // terminar cadena

            // ahora ptr apunta justo después de la comilla de cierre
            if (*ptr == separador) ptr++;
        }
        else {
            // campo sin comillas
            start = ptr;
            while (*ptr && *ptr != separador)
                ptr++;
            if (*ptr == separador) {
                *ptr = '\0';
                ptr++;
            }
        }
        // Duplicar el string para que cada campo tenga su propia memoria
        campos[idx++] = strdup(start);
    }

    campos[idx] = NULL;
    
    //Se libera la linea original porque los campos se han duplicado.
    free(linea); 
    
    return campos;
}


List *split_string(const char *str, const char *delim) {
    List *result = list_create();
    
    char *str_copy = strdup(str); 
    if (str_copy == NULL) {
        return NULL; // Error de asignación
    }
    char *token = strtok(str_copy, delim);
    
    while (token != NULL) {
        // Eliminar espacios en blanco al inicio del token
        while (*token == ' ') {
        token++;
        }

        // Eliminar espacios en blanco al final del token
        char *end = token + strlen(token) - 1;
        while (*end == ' ' && end > token) {
        *end = '\0';
        end--;
        }

        // Copiar el token en un nuevo string
        char *new_token = strdup(token);

        if (new_token == NULL) {
                // Manejo de error: liberar lo que se ha asignado hasta ahora
                // y limpiar la lista si es necesario.
                free(str_copy);
                list_clean(result); // Asumiendo que list_clean libera los elementos
                free(result);
                return NULL; 
        }
        // Agregar el nuevo string a la lista
        list_pushBack(result, new_token);

        // Obtener el siguiente token
        token = strtok(NULL, delim);
    }
    free(str_copy);
    return result;
}

// Función para limpiar la pantalla
void limpiarPantalla() { system("clear"); }

void presioneTeclaParaContinuar() {
  puts("Presione una tecla para continuar...");
  getchar(); // Consume el '\n' del buffer de entrada
  getchar(); // Espera a que el usuario presione una tecla
}