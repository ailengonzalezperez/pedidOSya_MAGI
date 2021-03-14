/*
 * restaurante.h
 *
 *  Created on: 4 oct. 2020
 *      Author: utnso
 */

#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_
#include<semaphore.h>
#include<pthread.h>
#include "planificacion_platos.h"
#include "auxiliares_restaurante.h"


typedef struct {
	int cantidad_cocineros;
	int pos_x;
	int pos_y;
	t_list* afinidades;
	t_list* platos;
	t_list* precios;
	int cantidad_hornos;
} t_metadata;

char* nombre_resto;

int aux_id_pedido;

t_metadata* metadata; //var global

t_list* recetas;
t_list* pedidos;

char* ip_restaurante;
char* puerto_escucha;
t_log* logger;
char* ip_sindicato;
char* puerto_sindicato;
char* ip_cliente;
char* puerto_cliente;
int socket_sindicato;
int socket_cliente;
char* puerto_app;
char* ip_app;
int fd_app;

t_config* config;
t_list* pedidos;


sem_t mutexLOGGER;
sem_t mutexRECETAS;
sem_t mutexPEDIDOS;

pthread_t threadSERVER;
pthread_t threadPLANIFICADOR;
pthread_t threadFinalizadorPedidos;

void crearColasReady();
/*struct config_log{
	t_config* config;
	t_log* logger;
	int socket;
};


int escucha_restaurante(struct config_log* datos_comunicacion);
int habla_restaurante(struct config_log* datos_comunicacion);
int crear_socket_saliente(char* ip, char* puerto, t_log* logger);
*Posible estructura y funciones para hacer entrada y salida de socket*/



#endif /* RESTAURANTE_H_ */
