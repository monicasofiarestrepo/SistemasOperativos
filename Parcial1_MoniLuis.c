/*  -------------------- PRIMERA EVALUACION PARCIAL ---------------------
----------------- SISTEMAS OPERATIVOS ---------------------
Profesor:  Juan Felipe Muñoz Fermandez

      Presentado por:
    Monica Sofía Restrepo León  1000181289
    Luis Alejandro Varela Ojeda  1017280316 

Se genera el ejecutable con la siguiente linea de comando:
[root@localhost ~]# gcc -std=c99 -pthread -o <nombreEjecutable>  <nombreArchivo.c> -lrt -lpthread

Como lo hemos guardado (al no encontrar lineamientos de nombre), sería:
[root@localhost ~]# gcc -std=c99 -pthread -o Parcial1ML Parcial1_MoniLuis.c -lrt -lpthread

En algunas ocasiones genera error por el estándar de c, por esto es el "-std=c99"


El "-pthread" antes de los nombres se usa para compilar con semáforos y memoria
compartida POSIX.

"-lrt -lpthread" después de los nombres asegura que las librerías de lrt y
lpthread se incluyan al ejecutar el código, en algunos sistemas (por ejemplo
nuestras máquinas virtuales) es necesario para que se ejecute el código.
*/

#include <ctype.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>


char *texto; // variable global que se comparte entre padre e hijo

// pointers a los semaforos
sem_t *semHijo;
sem_t *semPadre;

int main(int argc, char *args[]) {

  // creamos los semaforos y los inicializamos ambos en 0

  semHijo = sem_open("SEMHIJO", O_CREAT, 0666, 0); // semaforo que controla la ejecucion del proceso hijo,no se
                   // ejecuta hasta que el padre no haya terminado su accion.
  semPadre = sem_open("SEMPADRE", O_CREAT, 0666,0); // semaforo que controlara la ejecucion del proceso padre,no
                   // se ejecuta hasta que el hijo no haya terminado su accion.

  const int SIZE = 1028; // size de area memoria compartida
  const char *name = "CADENA_DE_TEXTO"; // nombramos al area de memoria compartida
  int filedesc;          // file descriptor
  shm_unlink(name);    // se desvincula el area de memoria compartida

  filedesc = shm_open(name, O_CREAT | O_RDWR, 0666); //uno de los procesos crea el area de memoria compartida y otro proceso se engancha a ella
  if (filedesc == -1) {
    perror("Error en shm_open");
    return (-1);
  }
  ftruncate(filedesc, SIZE); //establecer tamaño de memoria compartida (en bytes)

  texto = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, filedesc, 0); //mapear el area de memoria compartida en la variable global

  if (texto == MAP_FAILED) {
    perror("Error MAP_FAILED");
    return (-1);
  }
  
  int pid = fork();  // se crea proceso hijo con la system call "fork()"

  
  //si el planificador del SO le de tiempo de CPU al proceso hijo primero, se hace este proceso:
  if (pid == 0) { //cuando pid es igual a 0, está entrando por el hijo

    while (1) {

      sem_wait(semHijo); // esperamos a que el proceso padre ponga algo en la seccion critica
      printf(
          "Proceso hijo recibe: %s",
          (char *)texto); // se imprime en consola el texto en minusculas que recibe el proceso hijo

      
      size_t len = strlen(texto); //se lee el tamañao del texto recibido
      if (texto[len - 1] == '\n') {
        texto[len - 1] = '\0';  //cambia el ultimo caracter a \0 para que no lo lea como espacio en blanco
      }
      for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] = toupper(texto[i]); // se van sobreescribiendo los datos de memoria compartida a medida que se hace el cambio de minuscula a mayuscula
      }

      sem_post(semPadre);
    }
  }
  //si el planificador del SO le da tiempo de CPU al proceso hijo primero, se hace este proceso:
  if (pid > 0) { // si el pid es mayor a 0, está entrando por el proceso padre
    while (1) {

      //se le pide al usuario la cadena de texto a convertir
      printf("Ingrese cadena de texto: ");

      fgets(texto, 1028, stdin); // se guarda el texto ingreasado por el usuario en la memoria compartida
      sem_post(semHijo); // se le da acceso al proceso hijo a la seccion critica (donde podría haber conflictos)
      sem_wait(semPadre); //se le restringe al proceso padre el acceso a la seccion critica hasta que el hijo termine de poner la cadena en mayúsculas en la sección

      printf("Proceso padre recibe de regreso: %s\n", (char *)texto);
    }
  }

  //cerramos los semáforos 
  if (sem_close(semHijo) == -1) {
    perror("sem_close");
    exit(EXIT_FAILURE);
  }
  if (sem_close(semPadre) == -1) {
    perror("sem_close");
    exit(EXIT_FAILURE);
  }
  //eliminamos los semáforos 
  if (sem_unlink("SEMHIJO") == -1) {
    perror("sem_unlink");
    exit(EXIT_FAILURE);
  }
  if (sem_unlink("SEMPADRE") == -1) {
    perror("sem_unlink");
    exit(EXIT_FAILURE);
  }
  
  //NOTA: Por las restricciones de los semáforos, el proceso padre siempre se ejecutará primero 
  //NOTA 2: Por la implementación, el programa no termina a no ser que se mate al proceso padre, osea, para salir del programa debe interrumpir en consola usando cntrl+c

  return 0; 
}
