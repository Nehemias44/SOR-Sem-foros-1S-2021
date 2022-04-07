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
```
// Librerias
#include <sys/stat.h>
#include <fcntl.h>

// Algoritmo
        // Abro el archivo de la receta en modo lectura
	FILE *archivo = fopen("Receta.txt", "r");

        // Valido el archivo
	if(!archivo) exit(EXIT_FAILURE);
	// Inicio la variables necesarias para leer el contenido 
	char *contenido = NULL, *s;
	size_t longitud = 0;
	int n = 0, m;
	// leo linea por linea
	while (getline(&contenido, &longitud, archivo) != -1) {
		// Posiciono el puntero en el dato de la estructura 'parametro' que vamos a setear primero
		s = pthread_data->pasos_param[n].accion;
		m = 0;
		// Recorro caracter por caracter de la linea hasta el final
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