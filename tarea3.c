#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "TDAS/graph.h"
#include "TDAS/graph.c"

typedef struct {
    char nombre[101];
    int valor;
    int peso;
} Item;

typedef struct Escenario {
    int id;
    char nombre[100];
    char descripcion[300];
    List* items;
    List* items_originales;
    struct Escenario* arriba;
    struct Escenario* abajo;
    struct Escenario* izquierda;
    struct Escenario* derecha;
    int es_final;
    int id_arriba_temp; 
    int id_abajo_temp;
    int id_izquierda_temp;
    int id_derecha_temp;
} Escenario;

typedef struct {
    List* inventario;
    int peso_total;
    int puntaje;
    int tiempo_restante;
    Escenario* escenario_actual;
} TipoJugador;

int comparar_enteros(void* a, void* b) {
    int* key1 = (int*)a;
    int* key2 = (int*)b;
    return (*key1 == *key2);
}

bool leer_escenarios(Map** escenarios) {
    FILE *archivo = fopen("data/graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return false;
    }

    leer_linea_csv(archivo, ',');
    *escenarios = map_create(comparar_enteros);
    char **campos;
    
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        List* preItems = split_string(campos[3], ";");
        List* item_list = list_create();
        Escenario* esc = malloc(sizeof(Escenario));

        if (esc == NULL) {
            perror("Error al asignar memoria para Escenario");
            for (char *p_item_str = list_first(preItems); p_item_str != NULL; p_item_str = list_next(preItems)) {
                free(p_item_str);
            }
            list_clean(preItems);
            free(preItems);
            for (int i = 0; campos[i] != NULL; i++) {
                free(campos[i]);
            }
            free(campos);
            fclose(archivo);
            return false;
        }

        esc->id = atoi(campos[0]);
        strncpy(esc->nombre, campos[1], sizeof(esc->nombre) - 1);
        esc->nombre[sizeof(esc->nombre) - 1] = '\0';
        strncpy(esc->descripcion, campos[2], sizeof(esc->descripcion) - 1);
        esc->descripcion[sizeof(esc->descripcion) - 1] = '\0';

        esc->items = item_list;
        
        esc->id_arriba_temp = (strcmp(campos[4], "-1") == 0) ? -1 : atoi(campos[4]);
        esc->id_abajo_temp = (strcmp(campos[5], "-1") == 0) ? -1 : atoi(campos[5]);
        esc->id_izquierda_temp = (strcmp(campos[6], "-1") == 0) ? -1 : atoi(campos[6]);
        esc->id_derecha_temp = (strcmp(campos[7], "-1") == 0) ? -1 : atoi(campos[7]);

        esc->arriba = NULL; 
        esc->abajo = NULL;
        esc->izquierda = NULL;
        esc->derecha = NULL;
        
        esc->es_final = (strcmp(campos[8], "Si") == 0);

        // Crear lista para items_originales
        esc->items_originales = list_create();
        if (esc->items_originales == NULL) {
            perror("Error al crear lista items_originales");
            free(esc);
            for (char *p_item_str = list_first(preItems); p_item_str != NULL; p_item_str = list_next(preItems)) {
                free(p_item_str);
            }
            list_clean(preItems);
            free(preItems);
            for (int i = 0; campos[i] != NULL; i++) {
                free(campos[i]);
            }
            free(campos);
            fclose(archivo);
            return false;
        }
        
        
        int *id_clave = (int*) malloc(sizeof(int));
        if (id_clave == NULL) {
            perror("Error al asignar memoria para la clave ID");
            free(esc);

            for (char *p_item_str = list_first(preItems); p_item_str != NULL; p_item_str = list_next(preItems)) {
                free(p_item_str);
            }
            list_clean(preItems);
            free(preItems);
            for (int i = 0; campos[i] != NULL; i++) {
                free(campos[i]);
            }
            free(campos);
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

                for (char *p_item_str = list_first(preItems); p_item_str != NULL; p_item_str = list_next(preItems)) {
                    free(p_item_str);
                }
                list_clean(preItems);
                free(preItems);
                for (int i = 0; campos[i] != NULL; i++) {
                    free(campos[i]);
                }
                free(campos);
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
    fclose(archivo);

    MapPair* current_pair = map_first(*escenarios);
    while (current_pair != NULL) {
        Escenario* esc = (Escenario*) current_pair->value;

        if (esc->id_arriba_temp != -1) {
            int *key = &esc->id_arriba_temp;
            esc->arriba = (Escenario*) map_search(*escenarios, key);
        }
        if (esc->id_abajo_temp != -1) {
            int *key = &esc->id_abajo_temp;
            esc->abajo = (Escenario*) map_search(*escenarios, key);
        }
        if (esc->id_izquierda_temp != -1) {
            int *key = &esc->id_izquierda_temp;
            esc->izquierda = (Escenario*) map_search(*escenarios, key);
        }
        if (esc->id_derecha_temp != -1) {
            int *key = &esc->id_derecha_temp;
            esc->derecha = (Escenario*) map_search(*escenarios, key);
        }
        current_pair = map_next(*escenarios);
    }
    
    return true;
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

    Escenario* siguiente_escenario = NULL;

    if(direccion == 1){
        siguiente_escenario = jugador->escenario_actual->arriba;
    }
    else if(direccion == 2)
    {
        siguiente_escenario = jugador->escenario_actual->abajo;
    }
    else if(direccion == 3)
    {
        siguiente_escenario = jugador->escenario_actual->izquierda;
    }
    else if(direccion == 4){
        siguiente_escenario = jugador->escenario_actual->derecha;
    }

    if (siguiente_escenario == NULL) { 
        printf("No puedes avanzar en esa direccion.\n");
        return;
    }

    int peso = jugador->peso_total;
    int tiempo_usado = (peso + 10) / 10;
    jugador->tiempo_restante -= tiempo_usado;

    jugador->escenario_actual = siguiente_escenario;

    printf("\nTe has movido a: %s\n", siguiente_escenario->nombre);
    printf("%s\n", siguiente_escenario->descripcion);
    printf("Tiempo restante: %d\n", jugador->tiempo_restante);

    if (jugador->tiempo_restante <= 0) {
        printf("\nSe acabo el tiempo! GAME OVER.\n");
        return;
    }

    if (siguiente_escenario->es_final) {
        printf("\nHas llegado al escenario final!\n");
        printf("Inventario final:\n");
        for (Item* i = list_first(jugador->inventario); i != NULL; i = list_next(jugador->inventario)) {
            printf(" - %s\n", i->nombre);
        }
        printf("Puntaje final: %d\n", jugador->puntaje);
        return;
    }
}

void mostrar_estado(TipoJugador *Jugador){

    if (Jugador->escenario_actual == NULL) {
        printf("ERROR: Escenario actual es NULL\n");
        return;
    }

    Escenario* esc = Jugador->escenario_actual;
    printf("\n=== Estado Actual ===\n\n");
    printf("Escenario - %s -\n",esc->nombre);
    printf("%s\n\n", esc->descripcion);
    printf("Items de - %s -\n", esc->nombre);
    
    bool hayItems = false;

    for (Item* item = list_first(esc->items); item != NULL; item = list_next(esc->items)) {
        printf("   - %s (Valor: %d, Peso: %d)\n", item->nombre, item->valor, item->peso);
        hayItems = true;
    }

    if (!hayItems) printf("   - No hay items disponibles.\n");

    printf("\n- Tiempo restante: %d\n", Jugador->tiempo_restante);

    printf("\n- Inventario del jugador:\n");
    if (list_size(Jugador->inventario) == 0) {
        printf("   - Inventario vacio.\n");
    } else {
        for (Item* i = list_first(Jugador->inventario); i != NULL; i = list_next(Jugador->inventario)) {
            printf("   - %s (Valor: %d, Peso: %d)\n", i->nombre, i->valor, i->peso);
        }
    }

    printf("   - Peso total: %d\n", Jugador->peso_total);
    printf("   - Puntaje acumulado: %d\n", Jugador->puntaje);

    printf("\n- Acciones posibles desde este escenario:\n");
    if (esc->arriba)
        printf("   - Arriba: %s\n", esc->arriba->nombre);
    else 
        printf("   - Arriba: (No hay camino)\n");
    if (esc->abajo)
        printf("   - Abajo: %s\n", esc->abajo->nombre);
    else
        printf("   - Abajo: (No hay camino)\n");
    if (esc->izquierda)
        printf("   - Izquierda: %s\n", esc->izquierda->nombre);
    else
        printf("   - Izquierda: (No hay camino)\n");
    if (esc->derecha)
        printf("   - Derecha: %s\n", esc->derecha->nombre);
    else
        printf("   - Derecha: (No hay camino)\n");
}

bool reiniciar_partida(TipoJugador *jugador, Map *Esc, int id_inicio) {
    int opcion;
    printf("\n\nEstas seguro que quieres reiniciar?\nTendras que volver a cargar el laberinto...\n");
    printf("1. Si\n");
    printf("2. No\n");
    scanf("%i", &opcion);
    getchar();

    if(opcion == 1){
        if (jugador == NULL || Esc == NULL) {
        printf("Error: puntero jugador o escenarios es NULL\n");
        return true;
        }

        // Liberar items del inventario actual del jugador
        Item* i = list_first(jugador->inventario);
        while (i != NULL) {
            Item* temp = i;
            i = list_next(jugador->inventario);
            free(temp);
        }
        list_clean(jugador->inventario);

        // Resetear valores del jugador
        jugador->peso_total = 0;
        jugador->puntaje = 0;
        jugador->tiempo_restante = 10;

        // Buscar escenario inicial
        int clave_temp = id_inicio;
        Escenario* esc_ini = (Escenario*) map_search(Esc, &clave_temp);
        if (esc_ini == NULL) {
            printf("Error al reiniciar: escenario inicial (ID %d) no encontrado.\n", id_inicio);
            return true;
        }

        if (esc_ini->items_originales == NULL) {
            printf("Error: items_originales del escenario inicial es NULL.\n");
            return true;
        }

        // Liberar items actuales del escenario
        Item* item_actual = list_first(esc_ini->items);
        while (item_actual != NULL) {
            Item* temp = item_actual;
            item_actual = list_next(esc_ini->items);
            free(temp);
        }
        list_clean(esc_ini->items);

        // Copiar items originales al escenario actual
        for (Item* item = list_first(esc_ini->items_originales); item != NULL; item = list_next(esc_ini->items_originales)) {
            Item* copia = malloc(sizeof(Item));
            if (copia == NULL) {
                printf("Error al asignar memoria al restaurar items.\n");
                continue;
            }
            strcpy(copia->nombre, item->nombre);
            copia->valor = item->valor;
            copia->peso = item->peso;
            list_pushBack(esc_ini->items, copia);
        }

        // Asignar escenario inicial al jugador
        jugador->escenario_actual = esc_ini;

        printf("GAME OVER\n");
        printf("Redirigiendo al menu principal...\n");
        return true;
    }
    else if(opcion == 2){
        printf("\nReinicio Cancelado.\nReanudando Partida...\n");
        return false;
    }
    else{
        printf("Opcion Invalida.\nReanudando Partida...\n");
        return false;
    }
}

void recoger_items(TipoJugador* jugador){
    List* items = jugador->escenario_actual->items;

    if (list_size(items) == 0){
        printf("No hay items en el escenario.\n");
        return;
    }

    printf("\nItems en el escenario:\n");
    int index = 1;
    List* copia_items = list_create();
    for (Item* item = list_first(items); item != NULL; item = list_next(items)) {
        printf("%d. %s (Valor: %d - Peso: %d)\n", index++, item->nombre, item->valor, item->peso);
        list_pushBack(copia_items, item);
    }

    printf("\nIngresa los numeros de los items que quieres en tu inventario (separados por espacios, con el caracter 0 cancelas):\n");

    char buffer[260];

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("Error al leer la entrada.\n");
        list_clean(copia_items);
        free(copia_items);
        return;
    }

    buffer[strcspn(buffer, "\n")] = 0;

    char* token_str = strtok(buffer, " "); 
    int seleccionados = 0;

    List* items_a_eliminar_del_escenario = list_create();

    while (token_str != NULL) {
        int opt = atoi(token_str);
        if (opt == 0) {

            list_clean(copia_items);
            free(copia_items);
            list_clean(items_a_eliminar_del_escenario);
            free(items_a_eliminar_del_escenario);
            return;
        }
        
        Item* selected_item_ptr = NULL;
        int current_index = 1;
        for (Item* item_in_copy = list_first(copia_items); item_in_copy != NULL; item_in_copy = list_next(copia_items)) {
            if (current_index == opt) {
                selected_item_ptr = item_in_copy;
                break;
            }
            current_index++;
        }

        if (selected_item_ptr != NULL) {
            bool ya_seleccionado = false;
            for (Item* marcado_para_eliminar = list_first(items_a_eliminar_del_escenario); marcado_para_eliminar != NULL; marcado_para_eliminar = list_next(items_a_eliminar_del_escenario)) {
                if (marcado_para_eliminar == selected_item_ptr) {
                    ya_seleccionado = true;
                    break;
                }
            }

            if (!ya_seleccionado) {
                Item* copia_item_inventario = malloc(sizeof(Item));
                if (copia_item_inventario == NULL) {
                    printf("Error: No se pudo asignar memoria para la copia del item.\n");
                    list_clean(copia_items);
                    free(copia_items);
                    list_clean(items_a_eliminar_del_escenario);
                    free(items_a_eliminar_del_escenario);
                    return;
                }
                strcpy(copia_item_inventario->nombre, selected_item_ptr->nombre);
                copia_item_inventario->valor = selected_item_ptr->valor;
                copia_item_inventario->peso = selected_item_ptr->peso;

                list_pushBack(jugador->inventario, copia_item_inventario);
                jugador->peso_total += copia_item_inventario->peso;
                jugador->puntaje += copia_item_inventario->valor;
                list_pushBack(items_a_eliminar_del_escenario, selected_item_ptr);
                seleccionados++;
            }
        } else {
            printf("Advertencia: La opción '%s' no corresponde a un item válido.\n", token_str);
        }

        token_str = strtok(NULL, " ");
    }

    for (Item* item_to_remove_from_scenario = list_first(items_a_eliminar_del_escenario);
    item_to_remove_from_scenario != NULL;
    item_to_remove_from_scenario = list_next(items_a_eliminar_del_escenario)) {

        void* current_list_item = list_first(items);

        while (current_list_item != NULL) {
            if (current_list_item == item_to_remove_from_scenario) {
                list_popCurrent(items);
                free(item_to_remove_from_scenario);
                break;
            }
            current_list_item = list_next(items);
        }
    }

    list_clean(copia_items);
    free(copia_items);
    list_clean(items_a_eliminar_del_escenario);
    free(items_a_eliminar_del_escenario);

    if (seleccionados > 0) {
        jugador->tiempo_restante -= 1;
        printf("%d item(s) recogido(s) - Tiempo restante: %d\n", seleccionados, jugador->tiempo_restante);
    } else {
        printf("No se recogio ningun item valido o la operación fue cancelada.\n");
    }
}

void iniciar_partida(Map* escenarios) {
    TipoJugador* jugador = malloc(sizeof(TipoJugador));
    if (jugador == NULL) {
        perror("Error al asignar memoria para Jugador");
        return;
    }

    jugador->inventario = list_create();
    jugador->peso_total = 0;
    jugador->puntaje = 0;
    jugador->tiempo_restante = 10;

    int id_inicio = 1;
    int *key_inicio = &id_inicio; 
    jugador->escenario_actual = (Escenario*) map_search(escenarios, key_inicio);


    if (jugador->escenario_actual == NULL) {
        printf("Error: Escenario inicial (ID %d) no encontrado. Asegurate de que el CSV lo contenga.\n", id_inicio);
        list_clean(jugador->inventario);
        free(jugador->inventario);
        free(jugador);
        return;
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

        while (getchar() != '\n'); 

        switch (opcion) {
            case 1:
                recoger_items(jugador); 
                break;

            case 2:
                printf("Función 'Descartar item(s)' no implementada aún.\n");
                
                break;

            case 3:
                avanzar_direccion(jugador,escenarios);
                
                if (jugador->tiempo_restante <= 0 || jugador->escenario_actual->es_final) {
                    
                    for (Item* i = list_first(jugador->inventario); i != NULL; i = list_next(jugador->inventario)) {
                        free(i);
                    }
                    list_clean(jugador->inventario);
                    free(jugador->inventario);
                    free(jugador);
                    return; 
                }
                break;
            case 4:
                if (reiniciar_partida(jugador, escenarios, id_inicio)) {
                    return; 
                }
                break;
            case 5:
                printf("Saliendo del juego...\n");
                
                for (Item* i = list_first(jugador->inventario); i != NULL; i = list_next(jugador->inventario)) {
                    free(i);
                }
                list_clean(jugador->inventario);
                free(jugador->inventario);
                free(jugador);
                return;

            default:
                printf("Opcion invalida :(\n");
        }
    }
}

int main(){
    Map* escenarios = NULL;
    int opcion;
    bool laberinto_cargado = false;

    do {
        printf("\n=====================================\n");
        printf("      ** GraphQuest ** \n");
        printf("=====================================\n");
        printf("1. Cargar Laberinto\n");
        printf("2. Iniciar Partida\n");
        printf("3. Salir\n\n"); 
        printf("Ingrese una opcion: ");
        scanf("%d", &opcion);
        while (getchar() != '\n');

        switch (opcion) {
            case 1:
                printf("\nCargando el laberinto...\n");
                if (escenarios != NULL) {
                    printf("Liberando laberinto anterior...\n");
                    MapPair* pair = map_first(escenarios);
                    while (pair != NULL) {
                        Escenario* esc = (Escenario*)pair->value;
                        for (Item* item = list_first(esc->items); item != NULL; item = list_next(esc->items)) {
                            free(item);
                        }
                        list_clean(esc->items);
                        free(esc->items);
                        free(pair->key);
                        free(esc);
                        pair = map_next(escenarios);
                    }
                    map_clean(escenarios);
                    free(escenarios);
                    escenarios = NULL;
                }
                laberinto_cargado = leer_escenarios(&escenarios);
                if(laberinto_cargado) printf("Laberinto Cargado Correctamente...\n");
                else printf("Error al cargar el laberinto.\n");
                break;

            case 2:
                if(!laberinto_cargado){
                    printf("Primero debes cargar el laberinto.\n");
                    break;
                }
                iniciar_partida(escenarios);
                break;
                
            case 3:
                printf("Saliendo del programa.\n");
                if (laberinto_cargado && escenarios != NULL) { 
                    MapPair* pair = map_first(escenarios);
                    while (pair != NULL) {
                        Escenario* esc = (Escenario*)pair->value;
                        
                        for (Item* item = list_first(esc->items); item != NULL; item = list_next(esc->items)) {
                            free(item);
                        }
                        list_clean(esc->items);
                        free(esc->items);
                        free(pair->key);
                        free(esc);
                        pair = map_next(escenarios);
                    }
                    map_clean(escenarios);
                    free(escenarios);
                    escenarios = NULL;
                }
                return 0;
                
            default:
                printf("\nOpcion invalida. Por favor, intente de nuevo.\n");
        }
    } while (opcion != 3);

    return 0;
}