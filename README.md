# Proyecto 1
El proyecto está escrito en c++, usa meson y ninja como sistemas de construcción y Doxygen para generar documentación. Usa las bibliotecas *jsoncpp* para manejar los JSON, *pthread* para hilos y *catch2* para pruebas unitarias.

El sistema consiste en dos programas, el servidor y el cliente. Del lado del servidor se tiene una clase para las salas, del lado del cliente se tiene una interfaz de línea de comandos y un controlador; ambas partes utilizan una clase para manejar los mensajes y ésta a su vez utiliza una para los usuarios.
___
Para correr el cliente se debe ejecutar el archivo *Cliente* que genera meson.

Para correr el servidor se debe ejecutar el archivo *Servidor*, pasándole como parámetro el número de puerto.

Para generar documentación utilice el comando *meson compile docs*.

Para correr las pruebas unitarias utilice *meson test* o ejecute los archivos *PruebaJSON* y *PruebaSala*.