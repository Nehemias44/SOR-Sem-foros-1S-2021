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
                p(sem_mezclar)   p(sem_agregar)  p(sem_empanar)  p(sem_sarten)    p(sem_horno)   p(sem_armar)
   cortar()        mezclar()        agregar()       empanar()       cocinar()       hornear()       armar()
v(sem_mezclar)  v(sem_agregar)   v(sem_empanar)  v(sem_cocinar)  v(sem_armar)     v(sem_armar)
```

