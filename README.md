# ALGORITMOS_LOS_PAPUS

## Guía para settear el proyecto correctamente:
Si recibió el proyecto sin las carpetas Assets, SDL2, ni 3 archivos .dll dentro de la carpeta Laberintos_y_Fantasmas, deberá seguir los siguientes pasos:
1. Acceda al siguiente enlace: https://drive.google.com/file/d/133cfl5eu87jb0f3V79FMea1kgnK5Oobu/view?usp=sharing
2. Descargue el .zip que ahí se encuentra (prometo que no es una ZipBomb).
3. Descomprima el zip, habrá una carpeta "DESCARGABLES LABERINTOS Y FANTASMAS".
4. Copie todo el contenido dentro de la carpeta recién mencionada.
5. Pegue el contenido copiado dentro de ALGORITMOS_LOS_PAPUS/Laberintos_y_Fantasmas.
6. Debería quedarle la siguiente estructura dentro de la carpeta Laberintos_y_Fantasmas: <img width="519" height="924" alt="Captura de pantalla 2025-11-04 004119" src="https://github.com/user-attachments/assets/13b8ba25-4de0-4158-9260-b402b0c9ceec" />
7. Una vez tenga la estructura del proyecto setteada correctamente, abra en CodeBlocks el archivo LyF_WorkSpace.workspace que se encuentra en ALGORITMOS_LOS_PAPUS.
8. En el panel izquierdo verá dos proyectos: Servidor y TP.
9. Haga doble click sobre el proyecto Servidor, en la barra superior seleccione Project -> Build Options -> Linker settings, asegurese estar parado sobre "TP" (No debug y no Obj), allí debería ver linkeada la librería ws2_32; si no la ve, haga click en add y agreguela.
10. Repita el paso anterior para el proyecto TP, allí en Linker Settings debería ver: ws2_32, SDL2main, SDL2, SDL_ttf, SDL_mixer; si no las ve, agreguelas.
11. También en Build Options de TP, en Search Directories, debería ver SDL2\include; si no lo ve, agreguelo.
12. El proyecto ya debería haber quedado correctamente setteado para seguir adelante.
  

## Para ejecutar el juego (en CodeBlocks setup 20.03 en adelante):
1. Deben compilarse tanto el proyecto Servidor como el proyecto TP, orden indistinto.
2. El paso anterior creará en cada respectiva carpeta un .exe (Servidor.exe y TP.exe).
3. Para jugar con conexión al servidor, debe ejecutarse primero Servidor.exe y luego TP.exe, caso contrario no se establecerá conexión con el servidor y se jugará sin la funcionalidad de ranking (el programa no está preparado para guardar los datos una vez se reestablezca conexión en plena ejecución, si se juega sin conexión se pierden los datos).
4. Luego de ejecutar Servidor.exe UNA vez, podrán ejecutarse hasta 5 Clientes simultáneos.
5. Una ejecución de TP.exe corresponde a una partida, las opciones son Jugar, Ver Ranking y Salir. Una vez se presiona Jugar y se juega la partida, se volverá al menú.
6. Una vez se desee dejar de jugar, deberán cerrarse todos los clientes que aún queden activos, y por último cerrar manualmente el servidor (por favor cerrar con ctrl c); evite matar procesos desde la consola para evitar comportamientos inesperados.

Nota: Para cambiar la configuración, abra el archivo config.txt y cambie los valores en los parámetros.

## A la hora de settear la configuración se recomienda:  
- El tamaño mínimo del laberinto es 15 filas y 15 columnas, menos que eso haría un camino muy simple y nada "laberíntico"; y se vería muy chico.
- El tamaño máximo del laberinto es 30 filas y 50 columnas, más que eso no entraría bien en pantalla.
- Fantasmas: 2%-4% del total de celdas (filas * columnas).
- Premios: 10%-12% del total de celdas (filas * columnas). 
- Vidas extra: 0.07% del total de celdas (filas * columnas).
- El progama está preparado para el caso en el que se indiquen más fantasmas/premios/vidas de los que entran en el laberinto; sin embargo, nada le impide indicar una cantidad de fantasmas válida que haga imposible llegar a la salida. No se recomienda hacer esto.
- No borre el archivo config.txt, si lo borra, tendrá que volver a crearlo.

## En el juego:
- wasd para moverse.
- Capturar un corazón sumará una vida.
- Capturar una moneda sumará una cantidad aleatoria de puntos.
- Ser capturado por un fantasma restará una vida.
- El objetivo es llegar hasta la salida sin perder todas las vidas; si se pierden todas las vidas, se pierde la partida y no se suman puntos.4
- Jugar con sonido activado !!!
- Luego de jugar se mostrará una lista con los movimientos realizados, no se asuste, no son letras al azar.
