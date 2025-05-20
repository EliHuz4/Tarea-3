#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "TDAS/list.c"
#include "TDAS/list.h"
#include "TDAS/queue.h"
#include "TDAS/stack.h"
#include "TDAS/map.h"
#include "TDAS/map.c"
#include "TDAS/extra.h"
#include "TDAS/extra.c"

typedef struct {
    char nombre[101];
    int valor;
    int peso;
} Item;

typedef struct {
    int id;
    char nombre[101];
    char descripcion[301];
    List* items;      // Lista de Item*
    int conexiones[4]; // [arriba, abajo, izquierda, derecha]
    bool es_final;
} Escenario;

typedef struct {
    List* inventario; // Lista de Item*
    int peso_total;
    int puntaje;
    int tiempo_restante;
    Escenario* escenario_actual;
} TipoJugador;


// EliJuano esta wea la paso el profe 
void leer_escenarios() {
  // Intenta abrir el archivo CSV que contiene datos de películas
  FILE *archivo = fopen("data/graphquest.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  char **campos;
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV


  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    printf("ID: %d\n", atoi(campos[0]));
    printf("Nombre: %s\n", campos[1]);
    printf("Descripción: %s\n", campos[2]);

    List* items = split_string(campos[3], ";");

    printf("Items: \n");
    for(char *item = list_first(items); item != NULL; 
          item = list_next(items)){

        List* values = split_string(item, ",");
        char* item_name = list_first(values);
        int item_value = atoi(list_next(values));
        int item_weight = atoi(list_next(values));
        printf("  - %s (%d pts, %d kg)\n", item_name, item_value, item_weight);
        list_clean(values);
        free(values);
    }

    int arriba = atoi(campos[4]);
    int abajo = atoi(campos[5]);
    int izquierda = atoi(campos[6]);
    int derecha = atoi(campos[7]);

    if (arriba != -1) printf("Arriba: %d\n", arriba);
    if (abajo != -1) printf("Abajo: %d\n", abajo);
    if (izquierda != -1) printf("Izquierda: %d\n", izquierda);
    if (derecha != -1) printf("Derecha: %d\n", derecha);

    
    int is_final = atoi(campos[8]);
    if (is_final) printf("Es final\n");

    list_clean(items);
    free(items);
    
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas

}



// Funcion que da comienzo a una nueva partida
void iniciar_partida() {
    /*if ( alguna wea del grafo == NULL) {
        printf("Primero debes cargar el laberinto.\n");
        return;
    }*/

    TipoJugador jugador;
    jugador.inventario = list_create();
    jugador.peso_total = 0;
    jugador.puntaje = 0;
    jugador.tiempo_restante = 10;

    //jugador.escenario_actual

    int opcion;
    while (1) {
        //mostrar_estado(&jugador);
        printf("\n=== Opciones ===\n");
        printf("1. Recoger item(s)\n");
        printf("2. Descartar item(s)\n");
        printf("3. Avanzar en una direccion\n");
        printf("4. Reiniciar partida\n");
        printf("5. Salir del juego\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1:
                // A implementar: recoger_item(&jugador);
                jugador.tiempo_restante--;
                break;

            case 2:
                // A implementar: descartar_item(&jugador);
                jugador.tiempo_restante--;
                break;

            case 3:
                // A implementar: avanzar_direccion(&jugador);
                break;

            case 4:
                printf("Reiniciando partida...\n");
                list_clean(jugador.inventario);
                jugador.peso_total = 0;
                jugador.puntaje = 0;
                jugador.tiempo_restante = 10;
                //jugador.escenario_actual  
                break;

            case 5:
                printf("Saliendo del juego...\n");
                return;

            default:
                printf("Opcion invalida :(\n");
        }

        if (jugador.tiempo_restante <= 0) {
            printf("\n¡Se acabo el tiempo! ESTAS MUERTO.\n");
            return;
        }

        if (jugador.escenario_actual->es_final) {
            printf("\nHas llegado al escenario final!\n");
            printf("Inventario final:\n");
            for (Item* i = list_first(jugador.inventario); i != NULL; i = list_next(jugador.inventario)) {
                printf(" - %s\n", i->nombre);
            }
            printf("Puntaje final: %d\n", jugador.puntaje);
            return;
        }
    }
}


int main(){

     int opcion;

    do {
        printf("\n=====================================\n");
        printf("          ** GraphQuest **        \n");
        printf("=====================================\n");
        printf("1. Cargar Laberinto\n");
        printf("2. Iniciar Partida\n\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1:
                printf("\nCargando el laberinto...\n");
                leer_escenarios();
                break;

            case 2:
                iniciar_partida();
                break;

            default:
                printf("\nOpcion invalida. Por favor, intente de nuevo.\n");
        }

    } while (opcion != 2);

    return 0;
}