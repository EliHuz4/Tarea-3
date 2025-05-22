#include "graph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// LIST
// Crea una nueva lista vacía.
List *list_create() {
    List *list = (List *)malloc(sizeof(List));
    if (list == NULL) {
        perror("Error de memoria al crear la lista");
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->size = 0;
    return list;
}

// Agrega un elemento al final de la lista.
void list_pushBack(List *list, void *data) {
    if (list == NULL) return;

    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Error de memoria al crear nodo");
        return;
    }
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = list->tail; // Apunta al nodo anterior (el actual tail)

    if (list->tail != NULL) {
        list->tail->next = newNode; // El antiguo tail ahora apunta al nuevo nodo
    }
    list->tail = newNode; // El nuevo nodo es el nuevo tail

    if (list->head == NULL) {
        list->head = newNode; // Si la lista estaba vacía, el nuevo nodo también es head
    }
    list->size++;
}

// Agrega un elemento al inicio de la lista.
void list_pushFront(List *list, void *data) {
    if (list == NULL) return;

    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Error de memoria al crear nodo");
        return;
    }
    newNode->data = data;
    newNode->next = list->head; // El nuevo nodo apunta al antiguo head
    newNode->prev = NULL;

    if (list->head != NULL) {
        list->head->prev = newNode; // El antiguo head ahora apunta al nuevo nodo
    }
    list->head = newNode; // El nuevo nodo es el nuevo head

    if (list->tail == NULL) {
        list->tail = newNode; // Si la lista estaba vacía, el nuevo nodo también es tail
    }
    list->size++;
}

// Obtiene el primer elemento de la lista y posiciona el iterador 'current'.
void *list_first(List *list) {
    if (list == NULL || list->head == NULL) {
        list->current = NULL;
        return NULL;
    }
    list->current = list->head;
    return list->current->data;
}

// Obtiene el siguiente elemento de la lista y avanza el iterador 'current'.
void *list_next(List *list) {
    if (list == NULL || list->current == NULL || list->current->next == NULL) {
        list->current = NULL; // No hay siguiente elemento o ya estamos al final
        return NULL;
    }
    list->current = list->current->next;
    return list->current->data;
}

// Obtiene el elemento anterior de la lista y retrocede el iterador 'current'.
void *list_prev(List *list) {
    if (list == NULL || list->current == NULL || list->current->prev == NULL) {
        list->current = NULL; // No hay elemento anterior o ya estamos al inicio
        return NULL;
    }
    list->current = list->current->prev;
    return list->current->data;
}

// Obtiene el último elemento de la lista y posiciona el iterador 'current'.
void *list_last(List *list) {
    if (list == NULL || list->tail == NULL) {
        list->current = NULL;
        return NULL;
    }
    list->current = list->tail;
    return list->current->data;
}

// Elimina el nodo apuntado por 'current' y retorna su dato.
void *list_popCurrent(List *list) {
    if (list == NULL || list->current == NULL) return NULL;

    Node *nodeToDelete = list->current;
    void *data = nodeToDelete->data;

    // Actualizar punteros de nodos vecinos
    if (nodeToDelete->prev != NULL) {
        nodeToDelete->prev->next = nodeToDelete->next;
    } else {
        list->head = nodeToDelete->next; // Era la cabeza
    }

    if (nodeToDelete->next != NULL) {
        nodeToDelete->next->prev = nodeToDelete->prev;
    } else {
        list->tail = nodeToDelete->prev; // Era la cola
    }

    // Mover el iterador 'current' al siguiente nodo para continuar la iteración
    list->current = nodeToDelete->next; 
    
    free(nodeToDelete);
    list->size--;
    return data;
}

// Elimina el primer nodo de la lista y retorna su dato.
void *list_popFront(List *list) {
    if (list == NULL || list->head == NULL) return NULL;

    Node *nodeToDelete = list->head;
    void *data = nodeToDelete->data;

    list->head = nodeToDelete->next;
    if (list->head != NULL) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL; // La lista queda vacía
    }

    if (list->current == nodeToDelete) {
        list->current = list->head; // Si current era el nodo eliminado, moverlo
    }

    free(nodeToDelete);
    list->size--;
    return data;
}

// Elimina el último nodo de la lista y retorna su dato.
void *list_popBack(List *list) {
    if (list == NULL || list->tail == NULL) return NULL;

    Node *nodeToDelete = list->tail;
    void *data = nodeToDelete->data;

    list->tail = nodeToDelete->prev;
    if (list->tail != NULL) {
        list->tail->next = NULL;
    } else {
        list->head = NULL; // La lista queda vacía
    }

    if (list->current == nodeToDelete) {
        list->current = list->tail; // Si current era el nodo eliminado, moverlo
    }

    free(nodeToDelete);
    list->size--;
    return data;
}

// Retorna el número de elementos en la lista.
size_t list_size(List *list) {
    if (list == NULL) return 0;
    return list->size;
}

// Limpia todos los elementos de la lista, liberando la memoria de los nodos.
// NO libera la memoria de los datos que los nodos apuntan.
void list_clean(List *list) {
    if (list == NULL) return;

    Node *current = list->head;
    Node *next;
    while (current != NULL) {
        next = current->next;
        free(current); // Liberar el nodo
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->size = 0;
}

// Remueve un elemento específico de la lista.
int list_remove(List *list, void *data) {
    if (list == NULL || list->head == NULL) return 0;

    Node *current_node = list->head;
    while (current_node != NULL) {
        if (current_node->data == data) { // Compara punteros de datos directamente
            // Guarda el nodo siguiente para mantener el iterador 'current' si apunta a este nodo
            Node *next_node = current_node->next;

            if (current_node->prev != NULL) {
                current_node->prev->next = current_node->next;
            } else {
                list->head = current_node->next; // El nodo a eliminar es la cabeza
            }

            if (current_node->next != NULL) {
                current_node->next->prev = current_node->prev;
            } else {
                list->tail = current_node->prev; // El nodo a eliminar es la cola
            }

            if (list->current == current_node) {
                list->current = next_node; // Mueve current al siguiente si era el nodo eliminado
            }
            
            free(current_node); // Libera el nodo, no el dato
            list->size--;
            return 1; // Elemento encontrado y removido
        }
        current_node = current_node->next;
    }
    return 0; // Elemento no encontrado
}


// MAP
// --- Funciones Hash predefinidas ---

// Función hash para cadenas (strings)
long string_hash(void *key, long capacity) {
    char *str_key = (char *)key;
    unsigned long hash = 5381;
    int c;
    while ((c = *str_key++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % capacity;
}

// Función hash para enteros (int*)
long int_hash(void *key, long capacity) {
    // Para enteros, simplemente usamos el valor del entero como hash.
    // Aseguramos que sea un valor positivo y dentro de la capacidad.
    return (*((int *)key) % capacity + capacity) % capacity;
}

// --- Funciones Auxiliares Internas del Mapa ---

// Redimensiona el mapa si se llena demasiado, aumentando su capacidad.
static void map_rehash(Map *map) {
    long old_capacity = map->capacity;
    map->capacity *= 2; // Duplicar la capacidad
    map->size = 0; // Reiniciar el tamaño, se recalculará al reinsertar
    
    // Almacenar los buckets viejos temporalmente
    List **old_buckets = map->buckets;
    
    // Asignar memoria para los nuevos buckets (listas de listas)
    map->buckets = (List **)calloc(map->capacity, sizeof(List *));
    if (map->buckets == NULL) {
        perror("Error de memoria al rehash (new buckets)");
        // No podemos continuar, el mapa queda en un estado inconsistente.
        // Podríamos intentar revertir o simplemente fallar.
        exit(EXIT_FAILURE); 
    }

    // Recorrer todos los elementos del mapa viejo y reinsertarlos en el nuevo mapa.
    for (long i = 0; i < old_capacity; i++) {
        if (old_buckets[i] != NULL) {
            // Recorrer la lista de pares en este bucket
            MapPair *pair = (MapPair *)list_first(old_buckets[i]);
            while (pair != NULL) {
                // Reinsertar el par en el nuevo mapa
                map_insert(map, pair->key, pair->value);
                // NOTA: map_insert creará un nuevo MapPair y lo agregará.
                // Aquí deberías decidir si quieres liberar los antiguos MapPair.
                // Para esta implementación, los MapPair antiguos de `old_buckets` 
                // serán liberados cuando se liberen las listas `old_buckets[i]`.
                pair = (MapPair *)list_next(old_buckets[i]);
            }
            list_clean(old_buckets[i]); // Limpiar la lista del bucket viejo
            free(old_buckets[i]);       // Liberar la memoria de la lista
        }
    }
    free(old_buckets); // Liberar el array de punteros a listas viejos
}

// Busca un par clave-valor en un bucket específico.
// Retorna el MapPair si se encuentra, NULL si no.
static MapPair *find_pair_in_bucket(List *bucket_list, void *key, int (*is_equal)(void *, void *)) {
    if (bucket_list == NULL) return NULL;

    MapPair *pair = (MapPair *)list_first(bucket_list);
    while (pair != NULL) {
        if (is_equal(pair->key, key)) {
            return pair;
        }
        pair = (MapPair *)list_next(bucket_list);
    }
    return NULL;
}

// --- Implementación de Funciones Públicas del TDA Map ---

Map *map_create(int (*is_equal)(void *key1, void *key2)) {
    Map *map = (Map *)malloc(sizeof(Map));
    if (map == NULL) {
        perror("Error de memoria al crear el mapa");
        return NULL;
    }

    map->capacity = 100; // Capacidad inicial
    map->size = 0;
    map->is_equal = is_equal;
    
    // Por defecto, usa hash de enteros. Puedes cambiarlo después si necesitas string_hash.
    // Idealmente, map_create debería recibir la función hash también.
    map->hash = int_hash; 

    map->buckets = (List **)calloc(map->capacity, sizeof(List *));
    if (map->buckets == NULL) {
        perror("Error de memoria al crear buckets del mapa");
        free(map);
        return NULL;
    }

    map->current_bucket = -1; // Para la iteración
    map->current_pair_list = NULL;
    map->current_pair_node = NULL;

    return map;
}

void map_insert(Map *map, void *key, void *value) {
    if (map == NULL) return;

    // Si el mapa está muy lleno, redimensionar
    if (map->size / (double)map->capacity > 0.75) { // Factor de carga del 75%
        map_rehash(map);
    }

    long bucket_index = map->hash(key, map->capacity);

    // Buscar si la clave ya existe en este bucket
    if (map->buckets[bucket_index] == NULL) {
        map->buckets[bucket_index] = list_create();
        if (map->buckets[bucket_index] == NULL) {
            perror("Error al crear lista para bucket");
            return;
        }
    }

    MapPair *existing_pair = find_pair_in_bucket(map->buckets[bucket_index], key, map->is_equal);

    if (existing_pair != NULL) {
        // La clave ya existe, actualizar el valor
        existing_pair->value = value;
    } else {
        // La clave no existe, crear un nuevo par y agregarlo
        MapPair *new_pair = (MapPair *)malloc(sizeof(MapPair));
        if (new_pair == NULL) {
            perror("Error de memoria al crear MapPair");
            return;
        }
        new_pair->key = key;
        new_pair->value = value;
        list_pushBack(map->buckets[bucket_index], new_pair);
        map->size++;
    }
}

void *map_search(Map *map, void *key) {
    if (map == NULL || map->size == 0) return NULL;

    long bucket_index = map->hash(key, map->capacity);

    // Si el bucket está vacío, la clave no puede estar ahí
    if (map->buckets[bucket_index] == NULL) {
        return NULL;
    }

    MapPair *pair = find_pair_in_bucket(map->buckets[bucket_index], key, map->is_equal);
    if (pair != NULL) {
        return pair->value;
    }
    return NULL;
}

void *map_remove(Map *map, void *key) {
    if (map == NULL || map->size == 0) return NULL;

    long bucket_index = map->hash(key, map->capacity);

    if (map->buckets[bucket_index] == NULL) {
        return NULL;
    }

    // Recorrer la lista para encontrar y eliminar el par
    MapPair *pair_to_remove = NULL;
    void *removed_value = NULL;
    
    list_first(map->buckets[bucket_index]); // Posiciona el iterador al inicio
    MapPair *current_pair = (MapPair *)list_first(map->buckets[bucket_index]);
    while (current_pair != NULL) {
        if (map->is_equal(current_pair->key, key)) {
            pair_to_remove = current_pair;
            removed_value = pair_to_remove->value; // Guardar el valor antes de liberar el par
            list_popCurrent(map->buckets[bucket_index]); // Elimina el nodo actual
            free(pair_to_remove); // Liberar la memoria del MapPair
            map->size--;
            break;
        }
        current_pair = (MapPair *)list_next(map->buckets[bucket_index]);
    }
    
    // Si el bucket queda vacío después de eliminar, podemos liberar la lista
    if (map->buckets[bucket_index] != NULL && list_size(map->buckets[bucket_index]) == 0) {
        list_clean(map->buckets[bucket_index]); // Limpiar la lista (aunque ya está vacía)
        free(map->buckets[bucket_index]);
        map->buckets[bucket_index] = NULL;
    }

    return removed_value;
}

MapPair *map_first(Map *map) {
    if (map == NULL || map->size == 0) return NULL;

    map->current_bucket = -1; // Resetear la iteración
    map->current_pair_list = NULL;
    map->current_pair_node = NULL;

    return map_next(map); // Usar map_next para encontrar el primer elemento
}

MapPair *map_next(Map *map) {
    if (map == NULL) return NULL;

    // Si ya estamos iterando en una lista de bucket
    if (map->current_pair_list != NULL) {
        MapPair *pair = (MapPair *)list_next(map->current_pair_list);
        if (pair != NULL) {
            return pair;
        }
        // Si la lista actual terminó, pasamos al siguiente bucket
        map->current_pair_list = NULL;
    }

    // Buscar el siguiente bucket no vacío
    for (map->current_bucket = map->current_bucket + 1; 
         map->current_bucket < map->capacity; 
         map->current_bucket++) {
        
        if (map->buckets[map->current_bucket] != NULL && 
            list_size(map->buckets[map->current_bucket]) > 0) {
            
            map->current_pair_list = map->buckets[map->current_bucket];
            MapPair *pair = (MapPair *)list_first(map->current_pair_list);
            return pair; // Encontramos el primer elemento en el nuevo bucket
        }
    }

    return NULL; // No hay más elementos en el mapa
}

void map_clean(Map *map) {
    if (map == NULL) return;

    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL) {
            // Recorrer la lista y liberar cada MapPair
            MapPair *pair = (MapPair *)list_first(map->buckets[i]);
            while (pair != NULL) {
                // NOTA: map_clean NO libera la memoria de key ni value.
                // Eso es responsabilidad del usuario si fueron asignados dinámicamente.
                free(pair); // Liberar el MapPair
                pair = (MapPair *)list_next(map->buckets[i]);
            }
            list_clean(map->buckets[i]); // Limpiar la lista (libera los nodos internos de la lista)
            free(map->buckets[i]);       // Liberar la estructura List
            map->buckets[i] = NULL;
        }
    }
    free(map->buckets); // Liberar el array de punteros a listas
    free(map);          // Liberar la estructura Map
}




//EXTRA
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