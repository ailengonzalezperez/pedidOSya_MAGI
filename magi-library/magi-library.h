#ifndef SOCKET_H_
#define SOCKET_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include "magi-library.h"

#define TRUE 1
#define FALSE 0
/**************************************************************************************
 * Aca arranca toda la estructura de los sockets que se van a usar para
 * los clientes, la app y los restaurantes.
 *
 **************************************************************************************/

typedef enum
{
	MENSAJE,
	PAQUETE,
	AGREGAR_PLATO,
	CONSULTAR_RESTAURANTE,
	CONSULTAR_PLATOS,
	CONSULTAR_PEDIDO,
	CONFIRMAR_PEDIDO,
	CREAR_PEDIDO,
	DEVOLVER_PLATOS,
	FINALIZAR_PEDIDO,
	GUARDAR_PEDIDO,
	GUARDAR_PLATO,
	OBTENER_PEDIDO,
	OBTENER_RESTAURANTE,
	OBTENER_RECETA,
	PLATO_LISTO,
	SELECCIONAR_RESTAURANTE,
	SUSCRIPCION,
	TERMINAR_PEDIDO,
	OPCODEERROR = -1, //codigo para hacer pruebas, se puede borrar en cualquier momento
	OK = 20,
	FAIL = 21,
}op_code;

typedef struct
{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef enum{
	CONFIRMADO,
	EN_PROCESO,
	CANCELADO,
	FINALIZADO
}estado;

typedef struct
{
	int id_pedido;
	estado estado;
	t_list* platos;
	int pos_x;
	int pos_y;
}t_pedido;

typedef struct{
	int id_pedido;
	char* afinidad; //nombre del plato
	int cantidad_total;
	int cantidad_lista;
	estado estado;
	t_list* pasos;
	t_list* ciclos_pendientes;
	uint32_t precio;
}t_plato;

typedef struct{
	char* nombre_plato;
	t_list* pasos;
	t_list* ciclos;
}t_receta;
/*
typedef struct{
	char* nombre_plato;
	t_list* operaciones;
}t_receta;
*/
typedef struct{
	char* accion; //nombre de la operacion
	int ciclos_cpu;
}t_operacion;



t_log* logger;



void iniciar_servidor(void (*atender_request)(uint32_t), t_config* config);
int esperar_cliente(int);
t_log* iniciar_logger(char* logFile, char* proceso);
int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);
t_config* leer_config(char* nombre);
void leer_consola(t_log* logger);
t_paquete* armar_paquete();
int min(int num1, int num2);
int max(int num1, int num2);
//-----------------------MENSAJES----------------------

//------------------------ ENVIAR
void enviar_paquete(t_paquete* paquete, int socket_cliente);
//void enviar_paquete(t_paquete *paquete, uint32_t socket_cliente);
//void* serializar_paquete(t_paquete* paquete, int bytes);
//------------------------ DES/SERIALIZAR
t_buffer* serializar_lista_nombres(t_list*);
t_list* deserializar_lista_nombres(t_buffer*);
t_buffer* serializar_numero(uint32_t );
uint32_t deserializar_numero(t_buffer*);
t_buffer* serializar_nombre(char*);
char* deserializar_nombre(t_buffer*);
//t_buffer* serializar_Pedido_completo(t_pedido*);
//t_pedido* deserializar_Pedido_completo(t_buffer*);


//------------------------ RECIBIR
int recibir_operacion(int);
t_list* recibir_paquete(int);
void* recibir_buffer(int);

//------------------------ EXTRAS
size_t peso_de_una_lista(t_list*);
t_paquete* crear_paquete_personalizado(op_code codigo_operacion);
void crear_buffer(t_paquete*);
void loggear_lista_nombres(t_list*, t_log*);
void copiar_array_a_lista(t_list*, char**);
// ------------------------------ ELIMINAR
void eliminar_paquete(t_paquete* paquete);

//-----------------------MENSAJES----------------------


#endif /* SOCKET_H_ */


