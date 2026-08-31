# ADA — Práctica 1: Fuerza Bruta y Backtracking
 
Contraseñas bajo ataque y bajo diseño: enumeración exhaustiva (Módulo FB) y construcción con poda (Módulo BT).
 
## Integrantes
 
* Miguel Ángel Alzate
* Samuel Quintero
* Joseph Bueñanos
---
 
## 1. Requisitos del sistema
 
Para compilar y ejecutar el proyecto de forma reproducible en cualquier entorno (Linux, macOS o Windows vía MinGW/WSL), se requiere:
 
* **Compilador C++:** `g++` con soporte para el estándar C++17 o superior.
* **Librerías estándar:** archivos de cabecera estándar de C++ (`<chrono>`, `<vector>`, `<string>`, `<iostream>`).
---
 
## 2. Estructura del proyecto
 
```text
.
├── src/
│   ├── main.cpp          # Punto de entrada, coordina los módulos FB y BT
│   ├── bt_solver.cpp/.hpp# Módulo BT (Backtracking con y sin poda)
│   ├── fb_solver.cpp/.hpp# Módulo FB (Fuerza bruta vs Ataque por diccionario)
│   ├── metrics_csv.cpp   # Generador y exportador de métricas a formato CSV
│ - ├── metrics.csv.hpp   
├── third_party/      # Dependencias externas (picosha2.h)
├── tests/                # Casos de prueba y verificación de la semilla del grupo
├── resources/            # Recursos de entrada (diccionario.txt, verificar_semilla.cpp)
├── results/              # Archivos de salida: bt_resultados.csv y fb_resultados.csv
└── report/
    └── Informe.pdf       # Documento del informe técnico final
```
 
---
 
## 3. Compilación
 
Para compilar todo el proyecto y generar el binario ejecutable (`ada_p1`), abre la terminal en la raíz del repositorio y ejecuta:
 
```bash
g++ -std=c++17 -O2 -I src src/main.cpp src/bt_solver.cpp src/fb_solver.cpp src/metrics_csv.cpp -o ada_p1
```
 
---
 
## 4. Ejecución
 
Para correr los experimentos y generar los reportes de métricas:
 
```bash
./ada_p1
```
 
*(En Windows PowerShell o CMD utilizar `./ada_p1.exe`)*
 
El programa ejecutará secuencialmente ambos módulos (FB y BT) e imprimirá un resumen en consola. Los resultados detallados de cada experimento se escribirán automáticamente en los archivos:
 
* `results/fb_resultados.csv`
* `results/bt_resultados.csv`
---
 
## 5. Nota sobre tratabilidad y variantes costosas del Módulo BT
 
### 5.1 El problema
 
En `src/main.cpp` se encuentra parametrizado el flag global `EJECUTAR_VARIANTES_COSTOSAS`.
 
Las variantes de mayor tamaño (n = 6, n = 8, n = 10) están marcadas con `costosa = true` debido a que, sobre el alfabeto de diseño `ALFABETO_BT` (|Σ| = 69), generan árboles de búsqueda teóricos superiores a 10¹⁰ nodos en la versión sin poda. Intentar resolverlas de forma exhaustiva provocaría tiempos de ejecución excesivos (más de 30 minutos), desbordando los recursos de hardware local.
 
### 5.2 Comportamiento por defecto
 
Por defecto `EJECUTAR_VARIANTES_COSTOSAS = false`. Esto permite ejecutar la instancia de calibración `bt_demo_n4` (n = 4), la cual finaliza en milisegundos y proporciona las métricas empíricas comparativas (con poda vs. sin poda) guardadas en `results/bt_resultados.csv`.
 
### 5.3 Tratamiento en el informe
 
Para el informe técnico, las variantes mayores (n ≥ 6) se analizan mediante su tamaño teórico acumulado Σ_{k=0}^{n} |Σ|^k como evidencia del muro exponencial.
 
---
 
## 6. Reproducción de instancias y verificación de semilla
 
Las instancias evaluadas por este equipo se obtienen a partir de una semilla numérica derivada de los apellidos de sus integrantes (según la Sección 9 del enunciado del proyecto). Para verificar la validez de la semilla ejecutada por el código:
 
```bash
g++ -std=c++17 -O2 -o verificar_semilla resources/verificar_semilla.cpp
./verificar_semilla
```
 
---
 
## 7. Resultados y métricas
 
Los archivos `results/fb_resultados.csv` y `results/bt_resultados.csv` contienen el registro cuantitativo de las pruebas (tiempos de ejecución en milisegundos, candidatos/nodos evaluados y porcentaje de reducción por poda).
 
Las gráficas comparativas generadas a partir de estos datos se comentan en detalle dentro del informe técnico disponible en `report/Informe.pdf`.
