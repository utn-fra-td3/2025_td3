# workspace

Este directorio es para todos los proyectos que se vayan a desarrollar como parte del de desarrollo normal de la cátedra. También es el espacio para algunos ejemplos elementales.

## Sobre CMakeLists.txt

El CMakeLists.txt es un archivo dentro de cada proyecto de Raspberry Pi Pico que establece las reglas para compilar el proyecto. La biblioteca de [FreeRTOS](freertos/) que ya viene en este directorio tiene su propio CMakeLists.txt para establecer como compilar la biblioteca.

### Incluir FreeRTOS

Para poder hacer uso de esta biblioteca en cada proyecto sin tener que copiarla innecesariamente, es posible referenciarla en el CMakeLists.txt de cada proyecto que hagamos agregando la siguiente linea luego de `pico_sdk_init()`:

```cmake
# Add external FreeRTOS library
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../freertos ${CMAKE_BINARY_DIR}/freertos)
```

Esto incluye la biblioteca a nuestro proyecto. Para poder decirle a nuestro proyecto que linkee los archivos de código fuente de la biblioteca, vamos a tener que modificar la instrucción de `target_link_libraries()` para que quede de la siguiente forma:

```cmake
# Add the standard library to the build
target_link_libraries(PROJECT_NAME
  pico_stdlib
  freertos
)
```

Esto permite que nuestro proyecto `PROJECT_NAME` (con el nombre que corresponda), dependa de la biblioteca de FreeRTOS que tenemos de forma externa.