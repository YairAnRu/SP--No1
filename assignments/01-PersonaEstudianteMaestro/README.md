# Ejercicio - Herencia - Persona/Estudiante/Maestro

**Diagrama de clases UML:**

```mermaid
classDiagram
      Persona <|-- Estudiante
      Persona <|-- Maestro
      Persona:#string nombre
      Persona:#int edad
      Persona:+Persona()
      Persona:+Persona(string, int)
      Persona:+getNombre()
      Persona:+setNombre()
      Persona:+getEdad()
      Persona:+setEdad()
      Persona:+muestraDatos()
      class Estudiante {
        -string carrera
        +Estudiante()
        +Estudiante(string, int, string)
        +getCarrera()
        +setCarrera()
        +muestraDatos()
      }
      class Maestro {
        -string depto
        +setDepto()
        +getDepto()
        +muestraDatos()
      }

```

**Diagrama de secuencia UML:**
```mermaid
   sequenceDiagram
      participant m as main
      participant p as Persona
      participant e as Estudiante
      
      m->>p : create()
      m->>e : create()
      m->>e : muestraDatos()
      m->>e : getNombre()
      m->>e : getEdad()
      m->>e : getCarrera()
      m->>p : muestraDatos()
      m->>p : getNombre()
      m->>p : getEdad()
```

Este proyecto tiene la estructura básica para cualquier proyecto C++. 

La carpeta contiene el código fuente y el archivo de construcción ```make```.

La carpeta `build` contiene el codigo binario generado por el archivo de construcción ```make```.

Consulta el archivo assignments/README.md para instrucciones sobre la compilación y ejecución del proyecto.
