## GRAPHQUEST
---
## ¡Conviértete en el mejor GraphQuester!
¿Estás listo para una aventura que pondrá a prueba tu ingenio y velocidad? En GraphQuest, te embarcarás en un emocionante RPG lineal donde cada decisión cuenta. Explora escenarios vibrantes, recolecta objetos estratégicamente y compite contra el reloj para alcanzar la puntuación más alta.

¿Qué te espera en GraphQuest?
Tú eres el héroe (o los héroes): Juega solo o únete a un amigo para una experiencia cooperativa inolvidable.
Exploración y estrategia: Navega por diversos escenarios, pero ¡cuidado! El tiempo es limitado y un movimiento en falso podría significar el fin de tu partida.
Decisiones cruciales: Cada objeto recolectado y cada camino elegido te acerca o te aleja de la victoria. ¿Serás capaz de tomar las decisiones correctas bajo presión?
Competencia global: Demuestra tu habilidad y agilidad mental para convertirte en el mejor GraphQuester del mundo, un verdadero explorador nato.
GraphQuest no es solo un juego, es un desafío. ¿Estás listo para aceptarlo?

¡Juega ahora y demuestra que tienes lo necesario para dominar el arte de la exploración estratégica!

---
## Cómo compilar y ejecutar

Este sistema ha sido desarrollado en lenguaje C y puede ejecutarse fácilmente utilizando **Visual Studio Code** junto con una extensión para C/C++, como **C/C++ Extension Pack** de Microsoft. Para comenzar a trabajar con el sistema en tu equipo local, sigue estos pasos:
---
### Requisitos previos:

- Tener instalado [Visual Studio Code](https://code.visualstudio.com/).
- Instalar la extensión **C/C++** (Microsoft).
- Tener instalado un compilador de C (como **gcc**). Si estás en Windows, se recomienda instalar [MinGW](https://www.mingw-w64.org/) o utilizar el entorno [WSL](https://learn.microsoft.com/en-us/windows/wsl/).

---
### Pasos para compilar y ejecutar:

- Descarga y descomprime el archivo .zip en una carpeta de tu elección.
- Abre el proyecto en Visual Studio Code
- Inicia Visual Studio Code.
    Selecciona **Archivo > Abrir carpeta...** y elige la carpeta donde descomprimiste el proyecto.
- Compila el código
    Abre el archivo principal (**tarea3.c**).
    Abre la terminal integrada (**Terminal > Nueva terminal**).
    En la terminal, compila el programa con el siguiente comando:
        ```
        gcc tarea3.c TDAS/graph.c -o tarea3.exe
        ```
- Ejecuta el programa
    Una vez compilado, puedes ejecutar la aplicación con:
        ```
        ./tarea3.exe
        ```

---
## Funcionalidades

### Funcionando correctamente:

- Menu principal con todas sus funciones: cargar laberinto, iniciar partida (Singleplayer y Multiplayer) y Salir.
- Sub-menú partida con todas sus funciones: Recoger items, Descartar items, Avanzar en una direccion, Reiniciar Partida y Salir de la partida.
- Sub-menú Recoger Items: Listado de items y recoleccion.
- Sub-menú Descartar Items: Entrada de texto para descartar item y descarte.
- Sub-menú Avanzar en una direccion: Listado de direcciones y seleccion.
- Sub-menú Reiniciar Partida: Confirmacion de reinicio y reinicio.
- Salir de la partida.

### Problemas conocidos:

- Al reiniciar la partida hay que volver a cargar el laberinto, del caso contrario no apareceran los objetos recogidos.
- No se puede iniciar el multiplayer luego de el singleplayer sin antes haber cargado nuevamente el laberinto.
- No se puede iniciar el singleplayer luego de el multiplayer sin antes haber cargado nuevamente el laberinto.

### A mejorar:

- Que los objetos descartados se almacenen en el escenario actual de el jugador.

## Ejemplo de uso
**Paso 0: Menú**

Al ejecutar el codigo aparecera el siguiente menú

```
=====================================
      ** GraphQuest ** 
=====================================
1. Cargar Laberinto
2. Iniciar Partida --> (Singleplayer)
3. Iniciar Partida --> (Multiplayer)
4. Salir
```

**Paso 1: Cargar Laberinto**

Se comienza cargando el laberinto.

```
**Opción seleccionada: 1) Cargar Laberinto**

Cargando el laberinto...
Laberinto Cargado Correctamente...
```

El juego cargará el laberinto a partir de un archivo csv.

**Paso 2: Iniciar Partida --> (SinglePlayer)**

Una vez iniciada la partida se mostrara el siguiente menú de opciones a elegir por el jugador.

```
**Opción seleccionada: 2) Iniciar Partida --> (SinglePlayer)**

=== Estado Actual ===

Escenario - Entrada principal -
Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver.        

Items de - Entrada principal -
   - No hay items disponibles.

- Tiempo restante: 10

- Inventario del jugador:
   - Inventario vacio.
   - Peso total: 0
   - Puntaje acumulado: 0

- Acciones posibles desde este escenario:
   - Arriba: (No hay camino)
   - Abajo: Cocina
   - Izquierda: (No hay camino)
   - Derecha: (No hay camino)

=== Opciones ===
1. Recoger item(s)
2. Descartar item(s)
3. Avanzar en una direccion
4. Reiniciar partida
5. Salir del juego
```

**Paso 2.1: Avanzar en una direccion**

El jugador puede avanzar en una direccion en particular, en caso que no exista una habitacion disponible en una direccion en especifico se especifica en el sub menu anterior.

```
**Opción seleccionada: 3) Avanzar en una direccion**

Hacia donde deseas avanzar?
1. Arriba
2. Abajo
3. Izquierda
4. Derecha
Seleccione una opcion: 2

Te has movido a: Cocina
Restos de una batalla culinaria. Cuchillos por aqui, migajas por alla. Alguien cocinoÔÇª o fue cocinado.
Tiempo restante: 9
```
Al moverse en una direccion se mostrará el nombre del nuevo escenario actual, la descripcion de el escenario y el tiempo restante del jugador.


**Paso 2.2: Recoger Item**

Al seleccionar la opción de recoger item, se mostrará un sub menú con los items disponibles en el escenario y una casilla para poder ingresar el indice del item que se desea recoger.

```
**Opción seleccionada: 1) Recoger Item(s)**

Items en el escenario:
1. Cuchillo (Valor: 3 - Peso: 1)
2. Pan (Valor: 2 - Peso: 1)

Ingresa los numeros de los items que quieres en tu inventario (separados por espacios, con el caracter 0 cancelas):
1
1 item(s) recogido(s) - Tiempo restante: 8
```
Al ingresar el indice de los items que se quieren recoger se mostrará la cantidad de items recogidos y el tiempo restante del jugador, en caso que se ingrese un indice que no exista el sistema mostrará un mensaje indicando dicho caso y luego reanudando la partida:

```
[ EJEMPLO DE LO MENCIONADO ]
Ingresa los numeros de los items que quieres en tu inventario (separados por espacios, con el caracter 0 cancelas):
5
Advertencia: La opcion '5' no corresponde a un item valido.
No se recogio ningun item valido o la operacion fue cancelada.
```

**Paso 2.3: Descartar Items**

Mediante la opción "Descartar Items" el jugador tendrá que escribir (de manera identica) el nombre de el item que desea eliminar, luego el juego eliminará el item de el inventario del jugador y lo devolvera al escenario original de el item.

```
**Opción seleccionada: 2) Descartar Item**

Ingrese el nombre del item que desea descartar: Cuchillo
Has descartado el item "Cuchillo" y ha sido devuelto al escenario "Cocina".
```
En caso de que se desee descartar un item que no exista en el inventario de el jugador, se mostrará un mensaje.
```
[ EJEMPLO DE LO MENCIONADO ]
Ingrese el nombre del item que desea descartar: Pan
Item "Pan" no encontrado en el inventario.
```
**Paso 2.4: Reiniciar Partida**

Al seleccionar esta opción, se abrira un sub menu de confirmacion.

```
**Opción seleccionada: 4) Reiniciar Partida**

Estas seguro que quieres reiniciar?
Tendras que volver a cargar el laberinto...
1. Si
2. No
Ingrese una opcion: 
```
En caso de que el jugador seleccione la opcion "Si", la partida se reiniciará y habrá que cargar el laberinto nuevamente. Al reinciar el jugador tendrá todos sus parametros reestablecidos (Inventario, tiempo restante, escenario actual).

**Paso 2.5: Salir de la partida**
El jugador en cualquier momento puede decidir si salir de la partida o no, en caso de seleccionar esta alternativa, la partida terminará y se enviará al jugador al menú principal.
```
Opción seleccionada: 5) Salir de la partida

Saliendo del juego...
```
**Paso 3: Iniciar Partida --> (MultiPlayer)**
```
Opción seleccionada: 3) Iniciar Partida --> (Multiplayer)

======= TURNO DEL JUGADOR 1 =======

=== Estado Actual ===

Escenario - Entrada principal -
Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver.        

Items de - Entrada principal -
   - No hay items disponibles.

- Tiempo restante: 10

- Inventario del jugador:
   - Inventario vacio.
   - Peso total: 0
   - Puntaje acumulado: 0

- Acciones posibles desde este escenario:
   - Arriba: (No hay camino)
   - Abajo: Cocina
   - Izquierda: (No hay camino)
   - Derecha: (No hay camino)

--- Accion 1 de 2 ---
1. Recoger item(s)
2. Descartar item(s)
3. Avanzar en una direccion
4. Reiniciar partida (jugador actual)
5. Salir de la partida
Seleccione una opcion:

```
**Diferencias entre SinglePlayer y MultiPlayer:**
- Existen 2 jugadores, se juega de forma local cooperativo.
- Si a uno de los dos jugadores se le acaba el tiempo, ambos pierden la partida.
- Para ganar, ambos deben completar el laberinto (llegar al escenario final).
- Para la opcion "Reiniciar partida", se reinicia el personaje del jugador. NO SE REINICIA TODA LA PARTIDA.
- Al reiniciar la partida no hay que volver a cargar el laberinto
- Si un jugador sale de la partida, ambos abandonaran la partida cursada.
- El inventario no es por equipo, cada jugador tiene su propio inventario.
  
**Paso 4: Salir del Juego**

Al seleccionar esta opción el juego finaliza por completo.

```
Opción seleccionada: 4) Salir

Ingrese una opcion: 4
Saliendo del programa.

```

**CODIGO CREADO POR NESTOR CALDERON Y MATIAS SALAS**
