# Juzgado Cívico: estructuras de datos

Proyecto de la materia **Estructuras de Datos** . Sistema de expedientes para un juzgado cívico, que registra a las personas remitidas por faltas administrativas, lleva su historial de sanciones y ordena los turnos de audiencia.

**Equipo:** Díaz Godínez Erick Osvaldo, Merlos Perea Ana Karen, Sánchez Ortega Tsugui

**Demo en línea:** \[enlace de GitHub Pages\]

## Qué problema resuelve

En un juzgado cívico se remite a personas por faltas como alcohol en vía pública, ruido o riñas menores. El sistema responde cuatro necesidades:

| Necesidad | TDA | Implementación | Costo |
| :---- | :---- | :---- | :---- |
| Encontrar un expediente por su folio | Árbol de búsqueda | BST (y AVL balanceado) | O(log n) |
| Guardar un historial de sanciones que crece | Lista | Lista enlazada simple | O(k) al agregar |
| Mostrar los expedientes ordenados y sin folios repetidos | Árbol de búsqueda | Recorrido in-order | O(n) |
| Atender en el orden en que llegan | Cola | Cola FIFO con frente y final | O(1) |

Aquí `n` es el número de infractores y `k` el de sanciones de una persona.

## Estructuras de datos

- **Árbol BST por folio.** Cada nodo es un infractor. Operaciones: insertar, buscar, recorrer in-order y eliminar (hoja, un hijo y dos hijos con sucesor in-order).  
- **AVL.** Es el mismo árbol, con rotaciones para mantener la altura mínima. Con folios consecutivos el BST simple se vuelve una lista (64 folios: altura 64\) y el AVL se mantiene en altura 7\.  
- **Lista enlazada.** Cada infractor guarda sus sanciones como eslabones que terminan en `NULL`.  
- **Cola.** Las personas remitidas entran por el final y se atienden por el frente.

## Contenido del repositorio

| Archivo | Descripción |
| :---- | :---- |
| `juzgado_civico.cpp` | Programa en C++ con menú en consola. |
| `index.html` | Interfaz web con botones y colores, con la misma lógica y los mismos nombres de clases. |
| `README.md` | Este archivo. |

## Cómo usarlo

Menú:

| Opción | Acción |
| :---- | :---- |
| 1 | Registrar un infractor remitido (entra a la cola) |
| 2 | Buscar por folio |
| 3 | Agregar una sanción o estatus |
| 4 | Ver todos ordenados (in-order) y la altura del árbol |
| 5 | Liberar o concluir un caso |
| 6 | Atender la siguiente audiencia (cola) |
| 7 | Cambiar entre BST simple y AVL |
| 0 | Salir |

### Interfaz web

Abre `index.html` en el navegador. No necesita instalación ni servidor. Usa fuentes de Google, así que se ve mejor con internet.

Al abrir, ya trae 7 registros de ejemplo. Con **Insertar 8 folios consecutivos** se ve cómo crece el BST, y con **AVL balanceado** se ve cómo se reacomoda. La interfaz además simula un reloj con plazo de 36 horas por persona, que no está en la versión de C++.

## Límites

- Los datos viven en memoria y se pierden al cerrar el programa o recargar la página.  
- El reloj de la interfaz es simulado.  
- No modela la audiencia como proceso legal completo. Solo registra el estatus.  
- Los registros de ejemplo son inventados y no representan casos reales.

## Contexto

El flujo se basa en el modelo de la Ciudad de México: remisión, audiencia oral y pública, y resolución con multa, arresto de hasta 36 horas o amonestación si es menor de edad. Cada estado y municipio tiene su propio reglamento, y el proyecto no afirma cómo llevan hoy sus registros.