# Sistemas Operativos y Redes
Trabajo Práctico Semáforos primer semestre año 2021

## Seudo codigo sincronizacion
```
# Semaforos globales
sem_salero = 1
sem_sarten = 1
sem_hornear = 1

# Estructura Semaforos
sem_mezclar = 0
sem_salar = 0
sem_agregar = 0
sem_empanar = 0
sem_cocinar = 0
sem_hornear = 1
sem_armar = 0

# Sincronizacion
                                                                               p(sem_cocinar)   p(sem_hornear) p(sem_armar) 
                p(sem_mezclar) p(sem_salar)   p(sem_agregar)  p(sem_empanar)   p(sem_sarten)    p(sem_horno)   p(sem_armar)
   cortar()        mezclar()      salar()       agregar()       empanar()         cocinar()       hornear()       armar()
v(sem_mezclar)  v(sem_salar)   v(sem_agregar) v(sem_empanar)  v(sem_cocinar)   v(sem_armar)     v(sem_armar)
                                                                               v(sem_sarten)    v(sem_horno)
```
## Lectura receta, seteo de acciones e ingredientes 
```c
// Librerias
#include <sys/stat.h>
#include <fcntl.h>

// Algoritmo
	// Abro el archivo de la receta
	FILE *archivo = fopen("Receta.txt", "r");

	// Valido el archivo
	if(!archivo) exit(EXIT_FAILURE);
	// Inicio los datos necesarios para leer el contenido
	// *s es para recorrer y setear los datos tipo char[] de la estructura parametro
	char *contenido = NULL, *s;
	size_t longitud = 0;
	// n es para iterar el arreglo pasos_param de la estructura parametro
	// m es para iterar el arreglo pasos_param[n].ingredientes de la estructura parametro
	int n = 0, m;
	// leo linea por linea
	while (getline(&contenido, &longitud, archivo) != -1) {
		// Posiciono el puntero en el dato de la estructura 'parametro' que vamos a setear primero
		s = pthread_data->pasos_param[n].accion;
		m = 0;
		// Recorro la linea caracter por caracter hasta el final
		for(char *c = contenido; *c != '\0'; c++){
			// Cargo los caracteres que me interesan
			if (es_letra(*c)) {
				*s = *c; s++;
				// Cuando un caracter es SPC paso al siguiente dato
			} else if (*c == ' ') {
				s = pthread_data->pasos_param[n].ingredientes[m];
				m++;
			}
		}
		n++;
	}
	// Cierro el archivo y libero el espacio de memoria del contenido
	fclose(archivo);
	free(contenido);
```

## Escribir salida (log) en un arichvo
```c
// Definicion Global de puntero para el archivo, tambien se definio un semaforo (sem_escribir) global para evitar
// que varios hilos escriban en el archivo al mismo tiempo
FILE *salida_log;
// Abrir el archivo en modo de excritura
salida_log = fopen("subwayArgento.log", "w"); 
if (!salida_log) exit(EXIT_FAILURE);
// Agregar las siguientes llamadas a la funcion "fprintf" en la funcion "imprimirAccion"
// Para escribir en el archivo nombre de equipo y accion
fprintf(salida_log, "\t%s %d %c %s %s  \n ", "Equipo", mydata->equipo_param, '-', "accion", mydata->pasos_param[i].accion);
// Para escribir en el archivo nombre de equipo y titulo ingredientes
fprintf(salida_log, "\t%s %d %s\n",          "Equipo", mydata->equipo_param, "----------ingredientes :-----------");
// Para escribir en el archivo lista de ingredientes
fprintf(salida_log, "\t%s %d %s %d : %s \n", "Equipo", mydata->equipo_param, "ingredientes",h,mydata->pasos_param[i].ingredientes[h]);
```

## Codigo sincronizacion, reutilizacion de codigo
```c
// accion
#define A(str, data){\
		char *accion = str;\
		struct parametro *mydata = data;\
		imprimirAccion(mydata, accion);\
		usleep( 200000 );}
// pthread exit
#define PEXIT pthread_exit(NULL)
// struct semaforos
#define S(data) ((struct parametro*)data)->semaforos_param
// wait 
#define P(sem) sem_wait(&sem)
// signal 
#define V(sem) sem_post(&sem)
// sincronizacion
/*    funcion            /wait(sem)                              / zona critica   / signal(sem)                       / exit*/
void* cortar (void *d) {                                         A("cortar",  d); V(S(d).sem_mezclar);                PEXIT; }
void* mezclar(void *d) { P(S(d).sem_mezclar);                    A("mezclar", d); V(S(d).sem_salar)  ;                PEXIT; }
void* agregar(void *d) { P(S(d).sem_agregar);                    A("agregar", d); V(S(d).sem_empanar);                PEXIT; }
void* empanar(void *d) { P(S(d).sem_empanar);                    A("cocinar", d); V(S(d).sem_cocinar);                PEXIT; }
void* salar  (void *d) { P(S(d).sem_salar)  ; P(sem_salero)    ; A("salar",   d); V(S(d).sem_agregar); V(sem_salero); PEXIT; }
void* cocinar(void *d) { P(S(d).sem_cocinar); P(sem_sarten)    ; A("cocinar", d); V(S(d).sem_armar)  ; V(sem_sarten); PEXIT; }
void* hornear(void *d) { P(S(d).sem_hornear); P(sem_horno)     ; A("hornear", d); V(S(d).sem_armar)  ; V(sem_horno) ; PEXIT; }
void* armar  (void *d) { P(S(d).sem_armar)  ; P(S(d).sem_armar); A("armar",   d);                                     PEXIT; } 
```