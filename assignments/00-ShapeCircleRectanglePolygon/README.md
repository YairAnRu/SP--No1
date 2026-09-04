# Ejercicio - Herencia - Shape/Circle/Rectangle/Polygon

**Diagrama de clases UML:**

```mermaid
classDiagram
      Shape <|-- Circle
      Shape <|-- Rectangle
      Shape <|-- Polygon
      Shape:-int x
      Shape:-int y
      Shape:Shape()
      Shape:Shape(int, int)
      Shape:+string draw()
      Shape:int getValueX()
      Shape:int getValueY()
      class Circle {
        -int r
        +Circle()
        +Circle(int, int, int)
      }
      class Rectangle {        
      }
      class Polygon {
      }
        

```

**Diagrama de secuencia UML:**
```mermaid
   sequenceDiagram
      participant m as main
      participant s as Shape
      participant c as Circle
      
      m->>s : create()
      m->>s : draw()
      m->>s : getValueX()
      m->>s : getValueY()
      m->>c : create()
      m->>c : draw()
      m->>c : getValueX()
      m->>c : getValueY()
      m->>c : getRadio()
      
```

Este proyecto tiene la estructura básica para cualquier proyecto C++. 

La carpeta contiene el código fuente y el archivo de construcción ```make```.

La carpeta `build` contiene el codigo binario generado por el archivo de construcción ```make```.

Consulta el archivo assignments/README.md para instrucciones sobre la compilación y ejecución del proyecto.
