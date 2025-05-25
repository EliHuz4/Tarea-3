// GRAPHQUEST - CODIGO JUEGO - Nestor Calderon - Matias Salas

// LIBRERIAS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "TDAS/graph.h" //Incluimos el archivo "graph.h", que contiene funciones LIST, MAP y EXTRAS
// FIN DE LAS LIBRERIAS

// STRUCTS
typedef struct { // Struct Item
    char nombre[101]; // Nombre tipo char
    int valor; // Valor del item tipo int
    int peso; // Peso del item tipo int
    struct Escenario* lugar_original; // Un puntero a Escenario que contiene la info de el lugar original de ese item
} Item;

typedef struct Escenario { // Struct Escenarios
    int id; // ID del escenario tipo int
    char nombre[100]; // Nombre del escenario tipo char
    char descripcion[300]; // Descripcion del escenario tipo char
    List* items; // puntero a Lista de items por cada escenario 
    List* items_originales; // puntero a Lista de items por escenario (copia)
    struct Escenario* arriba; // puntero a Escenario que contiene la info de el escenario superior al actual
    struct Escenario* abajo; // puntero a Escenario que contiene la info de el escenario inferior al actual
    struct Escenario* izquierda; // puntero a Escenario que contiene la info de el escenario lateral izquierdo al actual
    struct Escenario* derecha; // puntero a Escenario que contiene la info de el escenario lateral derecho al actual
    int es_final; // Variable tipo int para saber si el escenario actual es el final o no
    int id_arriba_temp; // ID temporal del escenario superior al actual
    int id_abajo_temp; // ID temporal del escenario inferior al actual
    int id_izquierda_temp; // ID temporal del escenario lateral izquierdo al actual
    int id_derecha_temp; // ID temporal del escenario lateral derecho al actual
} Escenario;

typedef struct { // Struct TipoJugador
    List* inventario; // Puntero a Lista que guarda el inventario de el jugador
    int peso_total; // Int que almacena el peso total de el jugador en el momento actual de la partida
    int puntaje; // Int que almacena el puntaje final de el jugador
    int tiempo_restante; // Int que almacena el tiempo restante del jugador por cada movimiento
    Escenario* escenario_actual; // Puntero a Escenario que almacena el escenario actual en el que esta el jugador
} TipoJugador;
// FIN DE LOS STRUCTS

// FUNCIONES
int comparar_enteros(void* a, void* b) { // Funcion que compara dos punteros a void y retorna un int
    int* key1 = (int*)a; // Se hace un cast a int para el primer dato
    int* key2 = (int*)b; // Se hace un cast a int para el segundo dato
    return (*key1 == *key2); // Si son iguales devuelve 1, si son distintos son 0
}

bool leer_escenarios(Map** escenarios) { // Funcion para leer informacion a partir de un csv con columnas pre-definidas
    FILE *archivo = fopen("data/graphquest.csv", "r"); // Se abre el csv
    if (archivo == NULL) {
        perror("Error al abrir el archivo"); // Si el archivo es NULL se muestra un error
        return false;
    }

    leer_linea_csv(archivo, ','); // Saltamos los titulos de cada columna (saltamos toda la primera fila)
    *escenarios = map_create(comparar_enteros); // Creamos el mapa de escenarios
    char **campos;
    
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) { // Leemos cada fila del archivo
        List* preItems = split_string(campos[3], ";"); // Separamos los items del escenario mediante el caracter ";"
        List* item_list = list_create(); // Lista de los items parseados
        Escenario* esc = malloc(sizeof(Escenario)); // Reservamos espacio para un nuevo escenario

        if (esc == NULL) { // Si no se puede reservar realizamos un free de todas las variables y retornamos false
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

        esc->id = atoi(campos[0]); // Convertimos ID a int
        strncpy(esc->nombre, campos[1], sizeof(esc->nombre) - 1); // Copiamos el nombre a la variable del struct
        esc->nombre[sizeof(esc->nombre) - 1] = '\0';
        strncpy(esc->descripcion, campos[2], sizeof(esc->descripcion) - 1); // Copiamos la descripcion a la variable del struct
        esc->descripcion[sizeof(esc->descripcion) - 1] = '\0';

        esc->items = item_list; // Guardamos la lista vacia de items
        
        // Guardamos los ids temporales para luego conectar escenarios
        esc->id_arriba_temp = (strcmp(campos[4], "-1") == 0) ? -1 : atoi(campos[4]);
        esc->id_abajo_temp = (strcmp(campos[5], "-1") == 0) ? -1 : atoi(campos[5]);
        esc->id_izquierda_temp = (strcmp(campos[6], "-1") == 0) ? -1 : atoi(campos[6]);
        esc->id_derecha_temp = (strcmp(campos[7], "-1") == 0) ? -1 : atoi(campos[7]);

        // Inicializamos en NULL todas las direcciones
        esc->arriba = NULL; 
        esc->abajo = NULL;
        esc->izquierda = NULL;
        esc->derecha = NULL;
        
        esc->es_final = (strcmp(campos[8], "Si") == 0); // Si el archivo en la columna "es_final" dice "Si" este escenario sera el final

        // Creamos lista para guardar los items originales
        esc->items_originales = list_create();
        if (esc->items_originales == NULL) { // Si no se puede reservar realizamos un free de todas las variables y retornamos false
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
        
        
        int *id_clave = (int*) malloc(sizeof(int)); // Creamos un puntero para usar la ID como clave del mapa
        if (id_clave == NULL) { // Si no se puede reservar realizamos un free de todas las variables y retornamos false
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

        *id_clave = esc->id; // Guardamos la ID del escenario como clave
        map_insert(*escenarios, id_clave, esc); // Insertamos el escenario al mapa con la id como clave
        
        for (char *item = list_first(preItems); item != NULL; item = list_next(preItems)) {
            List* values = split_string(item, ","); // Separamos el nombre del item, el valor y el peso del item
            if (list_size(values) < 3) { // Si no estan los 3 datos, los saltamos
                for (char *val = list_first(values); val != NULL; val = list_next(values)) {
                    free(val);
                }
                list_clean(values);
                free(values);
                continue;
            }

            Item* new_item = malloc(sizeof(Item)); // Creamos un nuevo item
            if (new_item == NULL) { // Si no se puede reservar realizamos un free de todas las variables y retornamos false
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

            // Copiamos los datos del item
            strncpy(new_item->nombre, list_first(values), sizeof(new_item->nombre) - 1);
            new_item->nombre[sizeof(new_item->nombre) - 1] = '\0';
            new_item->valor = atoi(list_next(values));
            new_item->peso = atoi(list_next(values));
            new_item->lugar_original = esc;

            list_pushBack(item_list, new_item); // Agregamos el item a la lista del escenario

            for (char *val = list_first(values); val != NULL; val = list_next(values)) { // Hacemos free a la lista de values
                free(val);
            }
            list_clean(values);
            free(values);
        }

        for (char *p_item_str = list_first(preItems); p_item_str != NULL; p_item_str = list_next(preItems)) { // Hacemos free a la lista de strings originales de los items
            free(p_item_str);
        }
        list_clean(preItems);
        free(preItems);

        for (int i = 0; campos[i] != NULL; i++) { // Hacemos free para cada campo de el csv
            free(campos[i]);
        }
        free(campos);
    }
    fclose(archivo); // Cerramos el archivo

    // Conectamos los escenarios segun sus IDs temporales
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
        current_pair = map_next(*escenarios); // Pasamos al siguiente
    }
    
    return true; // Si todo salio segun lo esperado, retornamos true
}

void avanzar_direccion(TipoJugador* jugador, Map* escenarios) { // Funcion para que el jugador avance en una direccion a seleccionar

    // Mostramos un menu de opciones a elegir por el usuario
    printf("\nHacia donde deseas avanzar?\n");
    printf("1. Arriba\n");
    printf("2. Abajo\n");
    printf("3. Izquierda\n");
    printf("4. Derecha\n");
    printf("Seleccione una opcion: ");
    
    int direccion;
    scanf("%d", &direccion); // Guardamos la opcion seleccionada
    getchar(); // Limpiamos el buffer

    if (direccion < 1 || direccion > 4) { // Si la opcion seleccionada no fue valida retornamos
        printf("Direccion invalida.\n");
        return;
    }

    Escenario* siguiente_escenario = NULL;

    // Dependiando de la opcion seleccionada el escenario actual se cambia al escenario seleccionado
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
    // Si no hay escenario en esa direccion, no avanza y muestra un mensaje
    if (siguiente_escenario == NULL) { 
        printf("No puedes avanzar en esa direccion.\n");
        return;
    }

    // Se calcula el tiempo que se gasta segun el peso del jugador
    int peso = jugador->peso_total;
    int tiempo_usado = (peso + 10) / 10;
    jugador->tiempo_restante -= tiempo_usado; // Se descuenta el tiempo

    // Actualizamos el escenario actual de el jugador segun su seleccion
    jugador->escenario_actual = siguiente_escenario;

    // Se muestra la informacion de el nuevo escenario y el tiempo restante del jugador
    printf("\nTe has movido a: %s\n", siguiente_escenario->nombre);
    printf("%s\n", siguiente_escenario->descripcion);
    printf("Tiempo restante: %d\n", jugador->tiempo_restante);

    // Si el jugador se queda sin tiempo se termina el juego
    if (jugador->tiempo_restante <= 0) {
        printf("\nSe acabo el tiempo! GAME OVER.\n");
        printf("Mejor suerte para la proxima :/");
        return;
    }

    // Si el escenario es el final, se muestra el inventario del jugador y el puntaje final
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

void mostrar_estado(TipoJugador *Jugador){ // Funcion que muestra toda la informacion sobre el jugador

    // Si no hay un escenario actual, se muestra un mensaje de error
    if (Jugador->escenario_actual == NULL) {
        printf("ERROR: Escenario actual es NULL\n");
        return;
    }

    Escenario* esc = Jugador->escenario_actual;
    printf("\n=== Estado Actual ===\n\n");

    // Mostramos el nombre y descripcion de el escenario
    printf("Escenario - %s -\n",esc->nombre);
    printf("%s\n\n", esc->descripcion);

    // Moestramos los items que existen en este escenario
    printf("Items de - %s -\n", esc->nombre);
    
    bool hayItems = false;

    for (Item* item = list_first(esc->items); item != NULL; item = list_next(esc->items)) {
        printf("   - %s (Valor: %d, Peso: %d)\n", item->nombre, item->valor, item->peso);
        hayItems = true; // Verificador de items en el escenario
    }

    if (!hayItems) printf("   - No hay items disponibles.\n"); // Si no hay items, se muestra un mensaje indicandolo

    printf("\n- Tiempo restante: %d\n", Jugador->tiempo_restante); // Se muestra el tiempo restante del jugador

    // Se muestra el inventario de el jugador
    printf("\n- Inventario del jugador:\n");
    if (list_size(Jugador->inventario) == 0) {
        printf("   - Inventario vacio.\n");
    } else {
        for (Item* i = list_first(Jugador->inventario); i != NULL; i = list_next(Jugador->inventario)) {
            printf("   - %s (Valor: %d, Peso: %d)\n", i->nombre, i->valor, i->peso);
        }
    }

    // Mostramos el peso total y el puntaje acumulado hasta el momento
    printf("   - Peso total: %d\n", Jugador->peso_total);
    printf("   - Puntaje acumulado: %d\n", Jugador->puntaje);

    // Muestra las direcciones de desplazamiento posible (indicando los nombres de cada escenario adyacente)
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

bool reiniciar_partida(TipoJugador *jugador, Map *Esc, int id_inicio) { // Funcion para reiniciar la partida de el jugador
    char temp[100];
    int opcion;

    // Mostramos un submenu de confirmacion
    printf("\n\nEstas seguro que quieres reiniciar?\nTendras que volver a cargar el laberinto...\n");
    printf("1. Si\n");
    printf("2. No\n");
    printf("Ingrese una opcion: ");
    
    scanf("%99s", temp); // Guardamos la opcion del usuario
    getchar(); // Limpiamos el buffer

    if(!isdigit(temp[0])){ // Si la opcion del usuario no es valida se muestra un mensaje y se reanuda la partida
        printf("Opcion Invalida.\nReanudando Partida...\n");
        return false;
    }

    else opcion = atoi(temp); // Se convierte la opcion a int
    
    if(opcion == 1){ // Si la opcion fue "Si"
        // Revisamos que el jugador y el mapa no sean null
        if (jugador == NULL || Esc == NULL) {
        printf("Error: puntero jugador o escenarios es NULL\n");
        return true;
        }

        // Liberamos items del inventario actual del jugador
        Item* i = list_first(jugador->inventario);
        while (i != NULL) {
            Item* temp = i;
            i = list_next(jugador->inventario);
            free(temp);
        }
        list_clean(jugador->inventario); // Liberamos la lista del inventario de el jugador

        // Reseteamos los valores del jugador
        jugador->peso_total = 0;
        jugador->puntaje = 0;
        jugador->tiempo_restante = 10;

        // Buscamos el escenario inicial
        int clave_temp = id_inicio;
        Escenario* esc_ini = (Escenario*) map_search(Esc, &clave_temp);
        if (esc_ini == NULL) {
            printf("Error al reiniciar: escenario inicial (ID %d) no encontrado.\n", id_inicio);
            return true;
        }
        // Si no tiene los items originales guardados, no se puesde restaurar
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
        list_clean(esc_ini->items); // Liberamos la lista de items

        // Copiamos los items originales al escenario actual
        for (Item* item = list_first(esc_ini->items_originales); item != NULL; item = list_next(esc_ini->items_originales)) {
            Item* copia = malloc(sizeof(Item));
            if (copia == NULL) {
                printf("Error al asignar memoria al restaurar items.\n");
                continue;
            }
            // Copiamos los datos del item
            strcpy(copia->nombre, item->nombre);
            copia->valor = item->valor;
            copia->peso = item->peso;
            list_pushBack(esc_ini->items, copia);
        }

        // Asignamos escenario inicial al jugador
        jugador->escenario_actual = esc_ini;

        printf("GAME OVER\n");
        printf("Redirigiendo al menu principal...\n");
        return true;
    }
    else if(opcion == 2){
        // Si la opcion fue "No", se reanuda la partida y se retorna false
        printf("\nReinicio Cancelado.\nReanudando Partida...\n");
        return false;
    }
    else{ // Si la opcion fue distinta de Si o No, se reanuda la partida y se retorna false
        printf("Opcion Invalida.\nReanudando Partida...\n");
        return false;
    }
}

void recoger_items(TipoJugador* jugador){ // Funcion para recoger items del escenario
    // Obtenemos la lista de items del escenario actual del jugador
    List* items = jugador->escenario_actual->items;

    // Si no hay items, se muestra un mensaje
    if (list_size(items) == 0){
        printf("No hay items en el escenario.\n");
        return;
    }

    // Mostramos todos los items disponibles por escenario con un indice por cada uno
    printf("\nItems en el escenario:\n");
    int index = 1;
    List* copia_items = list_create();
    for (Item* item = list_first(items); item != NULL; item = list_next(items)) {
        printf("%d. %s (Valor: %d - Peso: %d)\n", index++, item->nombre, item->valor, item->peso);
        list_pushBack(copia_items, item);
    }

    // Pedimos al usuario que seleccione items por numero
    printf("\nIngresa los numeros de los items que quieres en tu inventario (separados por espacios, con el caracter 0 cancelas):\n");

    char buffer[260];

    // Leemos toda la linea de entrada
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("Error al leer la entrada.\n");
        list_clean(copia_items);
        free(copia_items);
        return;
    }

    buffer[strcspn(buffer, "\n")] = 0; // Quitamos el salto de linea

    char* token_str = strtok(buffer, " "); 
    int seleccionados = 0;

    List* items_a_eliminar_del_escenario = list_create(); // Lista de items que se eliminaran del escenario

    // Procesamos las selecciones
    while (token_str != NULL) {
        int opt = atoi(token_str);

        if (opt == 0) { // Si la opcion fue 0, se cancela la operacion
            list_clean(copia_items);
            free(copia_items);
            list_clean(items_a_eliminar_del_escenario);
            free(items_a_eliminar_del_escenario);
            return;
        }
        
        // Buscamos el item seleccionado en la copia por su indice
        Item* selected_item_ptr = NULL;
        int current_index = 1;
        for (Item* item_in_copy = list_first(copia_items); item_in_copy != NULL; item_in_copy = list_next(copia_items)) {
            if (current_index == opt) {
                selected_item_ptr = item_in_copy;
                break;
            }
            current_index++;
        }

        if (selected_item_ptr != NULL) { // Verificamos que el item no este repetido
            bool ya_seleccionado = false;
            for (Item* marcado_para_eliminar = list_first(items_a_eliminar_del_escenario); marcado_para_eliminar != NULL; marcado_para_eliminar = list_next(items_a_eliminar_del_escenario)) {
                if (marcado_para_eliminar == selected_item_ptr) {
                    ya_seleccionado = true;
                    break;
                }
            }

            if (!ya_seleccionado) {
                // Copiamos el item para el inventario
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
                copia_item_inventario->lugar_original = jugador->escenario_actual; // Asignamos el escenario actual como lugar_original

                // Añadimos el item al inventario y actualizamos los datos de el jugador
                list_pushBack(jugador->inventario, copia_item_inventario);
                jugador->peso_total += copia_item_inventario->peso;
                jugador->puntaje += copia_item_inventario->valor;
                list_pushBack(items_a_eliminar_del_escenario, selected_item_ptr);
                seleccionados++;
            }
        } else { // Si la opcion no esta asociada a un item, se muestra un mensaje
            printf("Advertencia: La opcion '%s' no corresponde a un item valido.\n", token_str);
        }

        token_str = strtok(NULL, " ");
    }

    // Eliminamos los items seleccionados del escenario original
    for (Item* item_to_remove_from_scenario = list_first(items_a_eliminar_del_escenario);
    item_to_remove_from_scenario != NULL;
    item_to_remove_from_scenario = list_next(items_a_eliminar_del_escenario)) {

        void* current_list_item = list_first(items);

        while (current_list_item != NULL) {
            if (current_list_item == item_to_remove_from_scenario) {
                list_popCurrent(items); // Removemos el item de el escenario
                free(item_to_remove_from_scenario); // Liberamos la memoria de ese item
                break;
            }
            current_list_item = list_next(items);
        }
    }
    // Libearmos las listas temporales
    list_clean(copia_items);
    free(copia_items);
    list_clean(items_a_eliminar_del_escenario);
    free(items_a_eliminar_del_escenario);

    if (seleccionados > 0) { // Si se seleccionaron items se resta 1 al tiempo restante y se muestran los items recogidos
        jugador->tiempo_restante -= 1;
        printf("%d item(s) recogido(s) - Tiempo restante: %d\n", seleccionados, jugador->tiempo_restante);
    } else { // Si no se seleccionaron items se muestra un mensaje
        printf("No se recogio ningun item valido o la operacion fue cancelada.\n");
    }
}

void descartar_item(TipoJugador* jugador, char* nombre_item) {
    if (jugador == NULL || jugador->inventario == NULL) {
        printf("Error: jugador o inventario es NULL.\n");
        return;
    }

    Item* item = list_first(jugador->inventario);
    while (item != NULL) {
        if (strcmp(item->nombre, nombre_item) == 0) {
            jugador->peso_total -= item->peso;

            // Remover el item del inventario (elimina del cursor actual)
            list_remove(jugador->inventario, item);

            // Verificamos si el item tiene escenario original
            if (item->lugar_original != NULL && item->lugar_original->items != NULL) {
                list_pushBack(item->lugar_original->items, item);
                printf("Has descartado el item \"%s\" y ha sido devuelto al escenario \"%s\".\n", nombre_item, item->lugar_original->nombre);
            } else {
                printf("Has descartado el item \"%s\" pero no se encontro su lugar original. Sera eliminado.\n", nombre_item);
                free(item); // si no tiene lugar_original, lo eliminamos
            }
            return;
        }
        item = list_next(jugador->inventario);
    }
    printf("Item \"%s\" no encontrado en el inventario.\n", nombre_item);
}

void iniciar_multiplayer(Map* escenarios) {
    TipoJugador* jugador1 = malloc(sizeof(TipoJugador));
    TipoJugador* jugador2 = malloc(sizeof(TipoJugador));

    if (jugador1 == NULL || jugador2 == NULL) {
        perror("Error al asignar memoria para Jugadores");
        if (jugador1) free(jugador1);
        if (jugador2) free(jugador2);
        return;
    }

    jugador1->inventario = list_create();
    jugador2->inventario = list_create();
    jugador1->peso_total = jugador2->peso_total = 0;
    jugador1->puntaje = jugador2->puntaje = 0;
    jugador1->tiempo_restante = jugador2->tiempo_restante = 10;

    int id_inicio = 1;
    int* key_inicio = &id_inicio;
    jugador1->escenario_actual = map_search(escenarios, key_inicio);
    jugador2->escenario_actual = map_search(escenarios, key_inicio);

    if (!jugador1->escenario_actual || !jugador2->escenario_actual) {
        printf("Error: Escenario inicial (ID %d) no encontrado.\n", id_inicio);
        list_clean(jugador1->inventario); free(jugador1->inventario); free(jugador1);
        list_clean(jugador2->inventario); free(jugador2->inventario); free(jugador2);
        return;
    }

    int turno = 1;

    while (1) {
        // Verificar si ambos jugadores ya terminaron
        int j1_termino = jugador1->escenario_actual->es_final || jugador1->tiempo_restante <= 0;
        int j2_termino = jugador2->escenario_actual->es_final || jugador2->tiempo_restante <= 0;
        if (j1_termino && j2_termino) break;

        // Solo puede jugar si no ha terminado
        TipoJugador* jugador = (turno == 1) ? jugador1 : jugador2;
        if (jugador->escenario_actual->es_final || jugador->tiempo_restante <= 0) {
            printf("\nJugador %d ya ha terminado su partida. Turno del otro jugador.\n", turno);
            turno = (turno == 1) ? 2 : 1;
            continue;
        }

        printf("\n======= TURNO DEL JUGADOR %d =======\n", turno);

        for (int accion = 0; accion < 2; accion++) {
            mostrar_estado(jugador);
            printf("\n--- Accion %d de 2 ---\n", accion + 1);
            printf("1. Recoger item(s)\n");
            printf("2. Descartar item(s)\n");
            printf("3. Avanzar en una direccion\n");
            printf("4. Reiniciar partida (jugador actual)\n");
            printf("5. Salir del juego\n");
            printf("Seleccione una opcion: ");

            int opcion;
            scanf("%d", &opcion);
            while (getchar() != '\n');

            if (opcion == 5) {
                printf("Jugador %d ha salido del juego.\n", turno);
                goto liberar_recursos;
            }

            switch (opcion) {
                case 1:
                    recoger_items(jugador);
                    break;

                case 2: {
                    char nombre_item[101];
                    printf("Ingrese el nombre del item que desea descartar: ");
                    scanf(" %[^\n]", nombre_item);
                    descartar_item(jugador, nombre_item);
                    break;
                }

                case 3:
                    avanzar_direccion(jugador, escenarios);
                    break;

                case 4:
                    if (reiniciar_partida(jugador, escenarios, id_inicio)) {
                        jugador->tiempo_restante = 10;
                        jugador->puntaje = 0;
                        jugador->peso_total = 0;
                        list_clean(jugador->inventario);
                        jugador->inventario = list_create();
                        jugador->escenario_actual = map_search(escenarios, key_inicio);
                    }
                    break;

                default:
                    printf("Opcion invalida.\n");
                    accion--; // repetir la acción
            }

            // Si terminó por tiempo o final, se rompe el turno
            if (jugador->escenario_actual->es_final || jugador->tiempo_restante <= 0)
                break;
        }

        turno = (turno == 1) ? 2 : 1;
    }

    // Mostrar resultado final
    printf("\n===== FIN DE LA PARTIDA =====\n\n");
    if (jugador1->escenario_actual->es_final && jugador2->escenario_actual->es_final) {
        printf("Ambos jugadores han llegado al escenario final!\n");
    } else {
        printf("Uno o ambos jugadores se quedaron sin tiempo. GAME OVER.\n");
    }

    printf("\n--- Puntajes Finales ---\n");
    printf("Jugador 1: Puntaje = %d | Peso = %d | Tiempo restante = %d\n",
           jugador1->puntaje, jugador1->peso_total, jugador1->tiempo_restante);
    printf("Jugador 2: Puntaje = %d | Peso = %d | Tiempo restante = %d\n",
           jugador2->puntaje, jugador2->peso_total, jugador2->tiempo_restante);

liberar_recursos:
    for (Item* i = list_first(jugador1->inventario); i != NULL; i = list_next(jugador1->inventario)) free(i);
    list_clean(jugador1->inventario); free(jugador1->inventario); free(jugador1);

    for (Item* i = list_first(jugador2->inventario); i != NULL; i = list_next(jugador2->inventario)) free(i);
    list_clean(jugador2->inventario); free(jugador2->inventario); free(jugador2);
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

            case 2:{
                char nombre_item[101];
                printf("Ingrese el nombre del item que desea descartar: ");
                scanf(" %[^\n]", nombre_item);
                descartar_item(jugador, nombre_item);
                break;
            }

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
// FIN DE LAS FUNCIONES

// MAIN
int main(){
    Map* escenarios = NULL;
    int opcion;
    bool laberinto_cargado = false;

    do {
        printf("\n=====================================\n");
        printf("      ** GraphQuest ** \n");
        printf("=====================================\n");
        printf("1. Cargar Laberinto\n");
        printf("2. Iniciar Partida --> (Singleplayer)\n");
        printf("3. Iniciar Partida --> (Multiplayer)\n");
        printf("4. Salir\n\n"); 
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
                if(!laberinto_cargado){
                    printf("Primero debes cargar el laberinto.\n");
                    break;
                }
                iniciar_multiplayer(escenarios);
                break;
            case 4:
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
    } while (opcion != 4);

    return 0;
}
// FIN DEL MAIN