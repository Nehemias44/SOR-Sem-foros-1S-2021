#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LIMITE 50

//creo estructura de semaforos 
struct semaforos {
	sem_t sem_mezclar;
	sem_t sem_salar;
	sem_t sem_agregar;
	sem_t sem_empanar;
	sem_t sem_cocinar;
	sem_t sem_hornear;
	sem_t sem_armar;
};

//creo los pasos con los ingredientes
struct paso {
	char accion [LIMITE], ingredientes[10][LIMITE];   
};

//creo los parametros de los hilos 
struct parametro {
	int equipo_param;
	struct semaforos semaforos_param;
	struct paso pasos_param[10];
};

sem_t sem_salero, sem_sarten, sem_horno, sem_escribir;
FILE *salida_log;

int es_letra(char c)
{
	return  (c >= 'a' && c <= 'z') || c == (char)164 ||
		(c >= 'A' && c <= 'Z') || c == (char)165;
}

//funcion para imprimir las acciones, los ingredientes de la accion y escribir la salido (log) en un archivo
void* imprimirAccion(void *data, char *accionIn) {

	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	sem_wait(&sem_escribir);
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
			printf("\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
			fprintf(salida_log, "\t%s %d %c %s %s  \n ",
				"Equipo", mydata->equipo_param, '-', "accion", mydata->pasos_param[i].accion);
			//calculo la longitud del array de ingredientes
			int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) /sizeof(mydata->pasos_param[i].ingredientes[0]) );
			//indice para recorrer array de ingredientes
			int h;
			printf("\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param);
			fprintf(salida_log, "\t%s %d %s\n",
				"Equipo", mydata->equipo_param, "----------ingredientes :-----------");
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
					printf("\tEquipo %d ingrediente  %d : %s \n",
					       mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
					fprintf(salida_log, "\t%s %d %s %d : %s \n", 
						"Equipo", mydata->equipo_param, "ingredientes",h,mydata->pasos_param[i].ingredientes[h]);
				}
			}
		}
	}
	sem_post(&sem_escribir);
	return NULL;
}

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

void* ejecutarReceta(void *i) {
	
	//variables semaforos
	sem_t sem_mezclar;
	//crear variables semaforos aqui
	sem_t sem_salar;
	sem_t sem_agregar;
	sem_t sem_empanar;
	sem_t sem_cocinar;
	sem_t sem_hornear;
	sem_t sem_armar; 
	
	//variables hilos
	pthread_t p1, p2, p3, p4, p5, p6, p7, p8; 
	//crear variables hilos aqui
	
	//numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);
	
	printf("Ejecutando equipo %d \n", p);

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//seteo los valores al struct
	
	//seteo numero de grupo
	pthread_data->equipo_param = p;

	//seteo semaforos
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	//setear demas semaforos al struct aqui
	pthread_data->semaforos_param.sem_salar   = sem_salar;
	pthread_data->semaforos_param.sem_agregar = sem_agregar;		
	pthread_data->semaforos_param.sem_empanar = sem_empanar;
	pthread_data->semaforos_param.sem_cocinar = sem_cocinar;
	pthread_data->semaforos_param.sem_hornear = sem_hornear; 
	pthread_data->semaforos_param.sem_armar   = sem_armar;	

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
		
	//inicializo los semaforos
	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	//inicializar demas semaforos aqui
	sem_init(&(pthread_data->semaforos_param.sem_salar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_agregar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_empanar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_cocinar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_hornear),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_armar),0,0);
	
	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
	int rc;
	rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
			    cortar,                        //funcion a ejecutar
			    pthread_data);                 //parametros de la funcion a ejecutar, pasado por referencia
	//crear demas hilos aqui

	rc = pthread_create(&p2,
			    NULL,
			    mezclar,
			    pthread_data);

	rc = pthread_create(&p3,
			    NULL,
			    salar,
			    pthread_data);

	rc = pthread_create(&p4,
			    NULL,
			    agregar,
			    pthread_data);

	rc = pthread_create(&p5,
			    NULL,
			    empanar,
			    pthread_data);

	rc = pthread_create(&p6,
			    NULL,
			    cocinar,
			    pthread_data);

	rc = pthread_create(&p7,
			    NULL,
			    hornear,
			    pthread_data);

	rc = pthread_create(&p8,
			    NULL,
			    armar,
			    pthread_data);
	
	//join de todos los hilos
	pthread_join (p1,NULL);
	//crear join de demas hilos
	pthread_join(p2 ,NULL);
	pthread_join(p3 ,NULL);
	pthread_join(p4 ,NULL);
	pthread_join(p5 ,NULL);
	pthread_join(p6 ,NULL);
	pthread_join(p7 ,NULL);
	pthread_join(p8 ,NULL);
	//valido que el hilo se alla creado bien 
	if (rc){
		printf("Error:unable to create thread, %d \n", rc);
		exit(-1);
	}

	 
	//destruccion de los semaforos 
	sem_destroy(&sem_mezclar);
	//destruir demas semaforos
	sem_destroy(&sem_salar);
	sem_destroy(&sem_agregar);
	sem_destroy(&sem_empanar);
	sem_destroy(&sem_cocinar);
	sem_destroy(&sem_hornear);
	sem_destroy(&sem_armar);	
	//salida del hilo
	pthread_exit(NULL);
}

int main ()
{
	sem_init(&sem_salero, 0, 1);
	sem_init(&sem_sarten, 0, 1);
	sem_init(&sem_horno, 0, 1);
	sem_init(&sem_escribir, 0, 1);

	//creo los nombres de los equipos 
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
	int *equipoNombre4 =malloc(sizeof(*equipoNombre4));	
	//faltan equipos
  
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;
	*equipoNombre4 = 4;	

	//creo las variables los hilos de los equipos
	pthread_t equipo1; 
	pthread_t equipo2;
	pthread_t equipo3; 
	pthread_t equipo4;
 
	salida_log = fopen("subwayArgento.log", "w"); 
	if (!salida_log) exit(EXIT_FAILURE);
 
	//inicializo los hilos de los equipos
	rc = pthread_create(&equipo1,                      //identificador unico
                            NULL,                          //atributos del thread
			    ejecutarReceta,                //funcion a ejecutar
			    equipoNombre1); 

	rc = pthread_create(&equipo2,                      //identificador unico
                            NULL,                          //atributos del thread
			    ejecutarReceta,                //funcion a ejecutar
			    equipoNombre2);
	rc = pthread_create(&equipo3,                      //identificador unico
                            NULL,                          //atributos del thread
			    ejecutarReceta,                //funcion a ejecutar
			    equipoNombre3);
	rc = pthread_create(&equipo4,                      //identificador unico
                            NULL,                          //atributos del thread
			    ejecutarReceta,                //funcion a ejecutar
			    equipoNombre4);

	if (rc){
		printf("Error:unable to create thread, %d \n", rc);
		exit(-1);
	} 

	//join de todos los hilos
	pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo3,NULL);
	pthread_join (equipo4,NULL);
	
	pthread_exit(NULL);
	fclose(salida_log);
}

//Para compilar:   gcc subwayArgento.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable
