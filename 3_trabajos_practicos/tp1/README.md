# tp1

Esta actividad práctica debe hacerse en un proyecto independiente llamado `firmware` dentro de este directorio. Se debe entregar el código fuente completo dentro de este directorio ignorando el directorio destino de la compilación (_build_ o _debug_ normalmente) a través de un _.gitignore_.

Este trabajo debe hacerse en una nueva rama llamada `tp1/v1`. Se puede hacer con el comando:

```bash
git checkout -b tp1/v1
```

## Consigna

Implementar una aplicación de dos tareas que se encarguen de gestionar el encendido y apagado de un LED. Debe haber una tarea encargada de apagar el LED y otra de encenderlo.

### Primera versión

Lograr que el parpadeo del LED gestionado por las dos tareas se realice a intervalos regulares de _500 ms_.

Una vez resuelta la consiga, hacer un commit de los cambios:

```bash
git add firmware
git commit -m "Primer consigna de TP1"
```

### Segunda versión

Lograr que el encendido del LED dure _1000 ms_ y el apagado dure _1500 ms_.

Una vez resuelta la consiga, hacer un commit de los cambios:

```bash
git add firmware
git commit -m "Segunda consigna de TP1"
```

## Entrega

Pushear la rama `tp1` al repositorio personal.

```bash
git push origin tp1/v1
```

Luego, crear un pull request a la rama correspondiente del repositorio de la cátedra con el título del pull request de _Entrega de TP1_.