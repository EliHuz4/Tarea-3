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

int comparar_enteros(void* a, void* b) {
    return *(int*)a == *(int*)b;
}

bool leer_escenarios(Map** escenarios) {
    // Intenta abrir el archivo CSV que contiene datos de películas
    FILE *archivo = fopen("data/graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo"); // Informa si el archivo no puede abrirse
        return false;
    }

    leer_linea_csv(archivo, ','); // Lee la cabecera de el CSV para no contabilizarla las tarde
    *escenarios = map_create(comparar_enteros);
    char **campos;
    // Leer y parsear una línea del archivo CSV. La función devuelve un array de
    // strings, donde cada elemento representa un campo de la línea CSV procesada.
    
    // Lee cada línea del archivo CSV hasta el final

    int contador_lineas = 0;

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        List* preItems = split_string(campos[3], ";");
        List* item_list = list_create();
        Escenario* esc = malloc(sizeof(Escenario));

        esc->id = atoi(campos[0]);

        strncpy(esc->nombre, campos[1], sizeof(esc->nombre) - 1);
        esc->nombre[sizeof(esc->nombre) - 1] = '\0';
        strncpy(esc->descripcion, campos[2], sizeof(esc->descripcion) - 1);
        esc->descripcion[sizeof(esc->descripcion) - 1] = '\0';





        printf("DEBUG: NOMBRE ESCENARIO %s\n", esc->nombre);
        printf("DEBUG: DESCRIPCION ESCENARIO %s\n", esc->descripcion);





        esc->items = item_list;
        esc->conexiones[0] = atoi(campos[4]);
        esc->conexiones[1] = atoi(campos[5]);
        esc->conexiones[2] = atoi(campos[6]);
        esc->conexiones[3] = atoi(campos[7]);
        esc->es_final = (strcmp(campos[8], "Si") == 0);
        
        int *id_clave = (int*) malloc(sizeof(int));
        if (id_clave == NULL) {
            perror("Error al asignar memoria para la clave ID");
            free(esc);
            fclose(archivo);
            return false;
        }

        *id_clave = esc->id;
        map_insert(*escenarios, id_clave, esc);
        
        for (char *item = list_first(preItems); item != NULL; item = list_next(preItems)) {
            List* values = split_string(item, ",");
            if (list_size(values) < 3) {
                for (char *val = list_first(values); val != NULL; val = list_next(values)) {
                    free(val);
                }
                list_clean(values);
                free(values);
                continue;
            }

            Item* new_item = malloc(sizeof(Item));
            if (new_item == NULL) {
                printf("Error: No se pudo asignar memoria para Item.\n");
                fclose(archivo);
                return false;
            }

            strncpy(new_item->nombre, list_first(values), sizeof(new_item->nombre) - 1);
            new_item->nombre[sizeof(new_item->nombre) - 1] = '\0';
            new_item->valor = atoi(list_next(values));
            new_item->peso = atoi(list_next(values));

            list_pushBack(item_list, new_item);

            for (char *val = list_first(values); val != NULL; val = list_next(values)) {
                free(val);
            }

            list_clean(values);
            free(values);
        }

        for (char *p_item_str = list_first(preItems); p_item_str != NULL; p_item_str = list_next(preItems)) {
            free(p_item_str);
        }
        list_clean(preItems);
        free(preItems);

        for (int i = 0; campos[i] != NULL; i++) {
            free(campos[i]);
        }
        free(campos);
    }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas
  return true; //retorna true para verificar luego si es que ya se leyó el archivo
}

void avanzar_direccion(TipoJugador* jugador, Map* escenarios) {
    printf("\nHacia donde deseas avanzar?\n");
    printf("1. Arriba\n");
    printf("2. Abajo\n");
    printf("3. Izquierda\n");
    printf("4. Derecha\n");
    printf("Seleccione una opcion: ");
    
    int direccion;
    scanf("%d", &direccion);
    getchar();

    if (direccion < 1 || direccion > 4) {
        printf("Direccion invalida.\n");
        return;
    }

    int siguiente_id = jugador->escenario_actual->conexiones[direccion - 1];
    if (siguiente_id == -1) {
        printf("No puedes avanzar en esa direccion.\n");
        return;
    }

    Escenario* siguiente_escenario = (Escenario*) map_search(escenarios, &siguiente_id);
    if (!siguiente_escenario) {
        printf("Error: escenario destino no encontrado.\n");
        return;
    }

    int peso = jugador->peso_total;
    int tiempo_usado = (peso + 1 + 9) / 10;
    jugador->tiempo_restante -= tiempo_usado;

    jugador->escenario_actual = siguiente_escenario;

    printf("\nTe has movido a: %s\n", siguiente_escenario->nombre);
    printf("%s\n", siguiente_escenario->descripcion);
    printf("Tiempo restante: %d\n", jugador->tiempo_restante);

    if (jugador->tiempo_restante <= 0) {
        printf("\nSe acabó el tiempo! GAME OVER.\n");
        return;
    }

    if (siguiente_escenario->es_final) {
        printf("\n¡Has llegado al escenario final!\n");
        printf("Inventario final:\n");
        for (Item* i = list_first(jugador->inventario); i != NULL; i = list_next(jugador->inventario)) {
            printf(" - %s\n", i->nombre);
        }
        printf("Puntaje final: %d\n", jugador->puntaje);
        return;
    }
}

void mostrar_estado(TipoJugador *Jugador){
    Escenario* esc = Jugador->escenario_actual;
    printf("=== Estado Actual ===\n");
    printf("Escenario - %s -\n",esc->nombre);
    printf("%s\n", esc->descripcion);
    printf("Items de - %s -", esc->nombre);

    bool hayItems = false;

    for (Item* item = list_first(esc->items); item != NULL; item = list_next(esc->items)) {
        printf("    - %s (Valor: %d, Peso: %d)\n", item->nombre, item->valor, item->peso);
        hayItems = true;
    }

    if (!hayItems) printf("    - No hay items disponibles.\n");

     // Tiempo restante
    printf("\n- Tiempo restante: %d\n", Jugador->tiempo_restante);

    // Inventario del jugador
    printf("\n- Inventario del jugador:\n");
    if (list_size(Jugador->inventario) == 0) {
        printf("    - Inventario vacio.\n");
    } else {
        for (Item* i = list_first(Jugador->inventario); i != NULL; i = list_next(Jugador->inventario)) {
            printf("    - %s (Valor: %d, Peso: %d)\n", i->nombre, i->valor, i->peso);
        }
    }

    printf("  - Peso total: %d\n", Jugador->peso_total);
    printf("  - Puntaje acumulado: %d\n", Jugador->puntaje);

    // Direcciones disponibles
    printf("\n- Acciones posibles desde este escenario:\n");
    const char* direcciones[] = {"Arriba", "Abajo", "Izquierda", "Derecha"};
    for (int i = 0; i < 4; i++) {
        if (esc->conexiones[i] != -1) {
            printf("  - %s (hacia Escenario %d)\n", direcciones[i], esc->conexiones[i]);
        }
    }
}


// Funcion que da comienzo a una nueva partida
void iniciar_partida(Map* escenarios) {
    TipoJugador* jugador = malloc(sizeof(TipoJugador));
    jugador->inventario = list_create();
    jugador->peso_total = 0;
    jugador->puntaje = 0;
    jugador->tiempo_restante = 10;

    int id_inicio = 1;
    jugador->escenario_actual = (Escenario*) map_search(escenarios, &id_inicio);
    if(jugador->escenario_actual)
    {
        printf("DEBUG: ESCENARIO ACTUAL %s\n", jugador->escenario_actual->nombre);
        printf("DEBUG: DESCRIPCION ACTUAL %s\n", jugador->escenario_actual->descripcion);
    }
    else{
        printf("NO ESTA\n");
    }


    int opcion;
    while (1) {
        mostrar_estado(jugador);
        printf("\n=== Opciones ===\n");
        printf("1. Recoger item(s)\n");
        printf("2. Descartar item(s)\n");
        printf("3. Avanzar en una direccion\n");
        printf("4. Reiniciar partida\n");
        printf("5. Salir del juego\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1:
                // A implementar: recoger_item(jugador);
                //jugador->tiempo_restante--;
                break;

            case 2:
                // A implementar: descartar_item(jugador);
                //jugador->tiempo_restante--;
                break;

            case 3:
                avanzar_direccion(jugador,escenarios);
                break;
            case 4:
                printf("Reiniciando partida...\n");
                list_clean(jugador->inventario);
                jugador->peso_total = 0;
                jugador->puntaje = 0;
                jugador->tiempo_restante = 10;
                jugador->escenario_actual = (Escenario*) map_search(escenarios, &id_inicio);
                break;

            case 5:
                printf("Saliendo del juego...\n");
                return;

            default:
                printf("Opcion invalida :(\n");
        }

        if (jugador->tiempo_restante <= 0) {
            printf("\nSe acabo el tiempo! ESTAS MUERTO.\n");
            return;
        }

        if (jugador->escenario_actual->es_final) {
            printf("\nHas llegado al escenario final!\n");
            printf("Inventario final:\n");
            for (Item* i = list_first(jugador->inventario); i != NULL; i = list_next(jugador->inventario)) {
                printf(" - %s\n", i->nombre);
            }
            printf("Puntaje final: %d\n", jugador->puntaje);
            return;
        }
    }
}


int main(){
    Map* escenarios;
    int opcion;
    bool flag = false;

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
                flag = leer_escenarios(&escenarios);
                if(flag) printf("Laberinto Cargado Correctamente...\n");
                else printf("Error al cargar el laberinto.\n");
                break;

            case 2:
                if(!flag){
                    printf("Primero debes cargar el laberinto.\n");
                    break;
                }
                iniciar_partida(escenarios);
                break;
                
            default:
                printf("\nOpcion invalida. Por favor, intente de nuevo.\n");
        }
    } while (opcion != 2 || !flag);

    return 0;
}