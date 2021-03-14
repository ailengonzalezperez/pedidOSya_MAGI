/*
 * app.h
 *
 *  Created on: 25 oct. 2020
 *      Author: utnso
 */

#ifndef APP_H_
#define APP_H_
#include<semaphore.h>

t_list* cola_new;
t_list* cola_ready;
t_list* cola_blocked;
t_list* cola_exec;
t_list* cola_exit;

t_list* lista_repartidores;

t_list* lista_restaurantes;

t_list* lista_pedidos;

sem_t m_cola_NEW;
sem_t m_cola_READY;
sem_t m_cola_EXEC;
sem_t m_cola_BLOCKED;
sem_t m_cola_EXIT;

sem_t m_planif_exec;
sem_t m_planif_ready;
sem_t m_planif_blocked;

sem_t hay_alguien;

sem_t mutexIdPedido;

sem_t pedidoEntregado;

int aux_id_pedido;

typedef enum{FIFO, SJF, HRRN, ERROR} PLANIFICADOR_APP; //Incluyo ERROR por si es necesario para manejo de excepción

typedef struct{
	char* nombre;
	int pos_x;
	int pos_y;
	char* nombre_restaurante;
}t_cliente;

t_list* lista_clientes;

typedef struct{
	int posicion_x;
	int posicion_y;
	char* nombre;
	char* ip;
	char* puerto;
}t_restaurantes;

typedef struct{
	int id_pedido;
	int destino_x;
	int destino_y;
	t_restaurantes* restaurante;
	estado estado_pedido;
} pedido;

typedef struct{
	int id_repartidor;
	int pos_x;
	int pos_y;
	int destino_x;
	int destino_y;
	int tiempo_descanso;//por archivo config
	int frecuencia_descanso;//por archivo config
	int tiempo_descansado;
	int tiempo_en_movimiento;
	pedido* pedido_asignado;//pedido asignado
	int estimacion_anterior;
}repartidores;




typedef struct{
	char* ip_comanda;//IP_COMANDA=127.0.0.1
	char* puerto_comanda;//	PUERTO_COMANDA=5001
	char* puerto_escucha;//PUERTO_ESCUCHA=5004
	char* ip_app;//	IP_APP=127.0.0.1
	char* multiprocesamiento;//	GRADO_DE_MULTIPROCESAMIENTO=1
	char* algoritmo;//ALGORITMO_DE_PLANIFICACION=FIFO
	char* alpha;//ALPHA=0,5
	char* estimacion;//	ESTIMACION_INICIAL=2
	char* repartidores;//REPARTIDORES=[1|1,5|2,1|4]
    char* Fdescanso;//FRECUENCIA_DE_DESCANSO=[8,5,10]
	char* Tdescanso;//TIEMPO_DE_DESCANSO=[2,1,4]
	char* log;//ARCHIVO_LOG=app.log
	char* platosDefault;//PLATOS_DEFAULT=[Plato1, Plato2, Plato3]
	char* pos_x_rest; //POSICION_REST_DEFAULT_X=0
	char* pos_y_rest; //POSICION_REST_DEFAULT_Y=0
	int retardo;
}t_config_app;

//struc config
t_config_app* config_a;

//LOGGER Y CONFIG
	t_log* logger;
	t_config* config;

// listas



#endif /* APP_H_ */
