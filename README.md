# ALGORITMOS_LOS_PAPUS

## Para ejecutar el juego (en CodeBlocks setup 20.03 en adelante):
1. Deben compilarse tanto el proyecto Servidor como el proyecto TP, orden indistinto.
2. El paso anterior creará en cada respectiva carpeta un .exe (Servidor.exe y TP.exe).
3. Para jugar con conexión al servidor, debe ejecutarse primero Servidor.exe y luego TP.exe, caso contrario no se establecerá conexión con el servidor y se jugará sin la funcionalidad de ranking (el programa no está preparado para guardar los datos una vez se reestablezca conexión en plena ejecución, si se juega sin conexión se pierden los datos).
4. Luego de ejecutar Servidor.exe UNA vez, podrán ejecutarse hasta 5 Clientes simultáneos.
5. Una ejecución de TP.exe corresponde a una partida, las opciones son Jugar, Ver Ranking y Salir. Una vez se presiona Jugar y se juega la partida, se volverá al menú.
6. Una vez se desee dejar de jugar, deberán cerrarse todos los clientes que aún queden activos, y por último cerrar manualmente el servidor (por favor cerrar con ctrl c); evite matar procesos desde la consola para evitar comportamientos inesperados.

## A la hora de settear la configuración se recomienda:  
- El tamaño mínimo del laberinto es 15 filas y 15 columnas, menos que eso haría un camino muy simple y nada "laberíntico"; y se vería muy chico.
- El tamaño máximo del laberinto es 30 filas y 50 columnas, más que eso no entraría bien en pantalla.
- Fantasmas: 2%-4% del total de celdas (filas * columnas).
- Premios: 10%-12% del total de celdas (filas * columnas). 
- Vidas extra: 0.07% del total de celdas (filas * columnas).
- El progama está preparado para el caso en el que se indiquen más fantasmas/premios/vidas de los que entran en el laberinto; sin embargo, nada le impide indicar una cantidad de fantasmas válida que haga imposible llegar a la salida. No se recomienda hacer esto.

## En el juego:
- wasd para moverse.
- Capturar un corazón sumará una vida.
- Capturar una moneda sumará una cantidad aleatoria de puntos.
- Ser capturado por un fantasma restará una vida.
- El objetivo es llegar hasta la salida sin perder todas las vidas; si se pierden todas las vidas, se pierde la partida y no se suman puntos.4
- Jugar con sonido activado !!!
