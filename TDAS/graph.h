#ifndef EXTRA_H
#define EXTRA_H
#ifndef LIST_H
#define LIST_H
#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h> // Para size_t (aunque a veces se define en otros headers)
#include <string.h>

// LISTA
// Definición de un nodo de la lista
typedef struct Node {
    void *data;       // Puntero genérico para almacenar cualquier tipo de dato
    struct Node *next;
    struct Node *prev;
} Node;

// Definición de la estructura de la lista
typedef struct List {
    Node *head;       // Puntero al primer nodo
    Node *tail;       // Puntero al último nodo
    Node *current;    // Puntero al nodo actual para la iteración
    size_t size;      // Número de elementos en la lista
} List;

// Funciones públicas del TDA List

// Crea una nueva lista vacía.
List *list_create();

// Agrega un elemento al final de la lista.
void list_pushBack(List *list, void *data);

// Agrega un elemento al inicio de la lista.
void list_pushFront(List *list, void *data);

// Obtiene el primer elemento de la lista y posiciona el iterador 'current'.
void *list_first(List *list);

// Obtiene el siguiente elemento de la lista y avanza el iterador 'current'.
void *list_next(List *list);

// Obtiene el elemento anterior de la lista y retrocede el iterador 'current'.
void *list_prev(List *list);

// Obtiene el último elemento de la lista y posiciona el iterador 'current'.
void *list_last(List *list);

// Elimina el nodo apuntado por 'current' y retorna su dato.
// El iterador 'current' se mueve al siguiente nodo (si existe).
void *list_popCurrent(List *list);

// Elimina el primer nodo de la lista y retorna su dato.
void *list_popFront(List *list);

// Elimina el último nodo de la lista y retorna su dato.
void *list_popBack(List *list);

// Retorna el número de elementos en la lista.
size_t list_size(List *list);

// Limpia todos los elementos de la lista, liberando la memoria de los nodos.
// NO libera la memoria de los datos que los nodos apuntan.
void list_clean(List *list);

// Remueve un elemento específico de la lista.
// Retorna 1 si el elemento fue encontrado y removido, 0 en caso contrario.
// Asume que 'data' es el puntero al dato que se quiere remover.
int list_remove(List *list, void *data);


// MAP
// Definición de una pareja clave-valor para el mapa
typedef struct MapPair {
    void *key;
    void *value;
} MapPair;

// Definición de la estructura del mapa
typedef struct Map {
    List **buckets; // Array de punteros a List (cada List es un "bucket" o cubo)
    long capacity;  // Capacidad actual de la tabla hash (número de buckets)
    long size;      // Número actual de elementos en el mapa
    long current_bucket; // Índice del cubo actual para iteración
    List *current_pair_list; // Puntero a la lista del cubo actual para iteración
    void *current_pair_node; // Puntero al nodo actual en la lista para iteración

    // Puntero a la función de comparación de claves.
    // Debe devolver 1 si key1 y key2 son iguales, 0 en caso contrario.
    int (*is_equal)(void *key1, void *key2); 

    // Puntero a la función hash.
    // Debe devolver un long para un dado void* key.
    long (*hash)(void *key, long capacity); 
} Map;

// Funciones públicas del TDA Map

// Crea un nuevo mapa.
// Requiere una función de comparación de claves.
Map *map_create(int (*is_equal)(void *key1, void *key2));

// Inserta un nuevo par clave-valor en el mapa.
// Si la clave ya existe, actualiza su valor.
void map_insert(Map *map, void *key, void *value);

// Busca un valor asociado a una clave.
// Retorna el valor si se encuentra, NULL si no.
void *map_search(Map *map, void *key);

// Elimina un par clave-valor del mapa.
// Retorna el valor eliminado si se encuentra, NULL si no.
void *map_remove(Map *map, void *key);

// Obtiene el primer par clave-valor para comenzar la iteración.
// Retorna un MapPair*, NULL si el mapa está vacío.
MapPair *map_first(Map *map);

// Obtiene el siguiente par clave-valor en la iteración.
// Retorna un MapPair*, NULL si no hay más elementos.
MapPair *map_next(Map *map);

// Limpia todos los elementos del mapa (libera las estructuras internas).
// No libera la memoria de las claves ni los valores, eso es responsabilidad del usuario.
void map_clean(Map *map);

// Funciones hash predefinidas (puedes elegir la que necesites)
long string_hash(void *key, long capacity);
long int_hash(void *key, long capacity);



/** EXTRA
 * Función para leer y parsear una línea de un archivo CSV en campos
 * individuales.
 *
 * Esta función lee una línea de un archivo CSV, donde cada campo puede estar
 * opcionalmente entrecomillado y separado por un caracter definido como
 * 'separador'. La función maneja campos entrecomillados que pueden contener el
 * separador como parte del valor del campo.
 *
 * @param archivo Puntero a FILE que representa el archivo CSV abierto.
 * @param separador Caracter utilizado para separar los campos en la línea del
 * CSV.
 *
 * @return Retorna un puntero a un arreglo de cadenas (char*), donde cada
 * elemento representa un campo de la línea del CSV leída. Si no hay más líneas
 * para leer o se alcanza el fin del archivo, retorna NULL.
 *
 * Uso:
 * FILE* f = fopen("datos.csv", "r");
 * char** campos;
 * while ((campos = leer_linea_csv(f, ',')) != NULL) {
 *     // Procesar campos
 * }
 * fclose(f);
 *
 * Notas:
 * - La función utiliza memoria estática internamente, por lo que cada llamada
 * sobrescribe los datos de la llamada anterior.
 * - La función asume que ninguna línea del CSV excede MAX_LINE_LENGTH
 * caracteres y que no hay más de MAX_FIELDS campos por línea.
 */
char **leer_linea_csv(FILE *archivo, char separador);

List *split_string(const char *str, const char *delim);

// Función para limpiar la pantalla
void limpiarPantalla();

void presioneTeclaParaContinuar();

#endif /* EXTRA_H */
#endif /* LIST_H */
#endif /* MAP_H */