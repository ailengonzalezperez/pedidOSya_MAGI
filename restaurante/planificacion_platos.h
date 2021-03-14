/*
 * planificacion_platos.h
 *
 *  Created on: 19 nov. 2020
 *      Author: utnso
 */

#ifndef PLANIFICACION_PLATOS_H_
#define PLANIFICACION_PLATOS_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include<semaphore.h>
#include"restaurante.h"
#include"serializacion_restaurante.h"
#include "auxiliares_restaurante.h"

typedef enum{
	FIFO,
	RR,
	ERROR}
PLANIFICADOR_RESTAURANTE; //Incluyo ERROR por si es necesario para manejo de excepción

//COLAS
t_list* cola_new;
t_list* cola_exec;
t_list* cola_block;
t_list* cola_exit;

int quantum;

PLANIFICADOR_RESTAURANTE algoritmo;

typedef struct{
	char* afinidad;
	t_list* platos;
	sem_t cpuLIBRE; //no deberia haber mas de 1 plato perteneciente a una determinada
	             //cola de ready ejecutandose a la vez
	sem_t platoNUEVO;
	int quantum_actual;
}cola_ready; //habra uno por cada cocinero

t_list* colas_ready; //cada elemento sera un struct cola_ready

//SEMAFOROS
sem_t mutexNEW;
sem_t mutexEXEC;
sem_t mutexBLOCK;
sem_t mutexEXIT;
sem_t mutexREADY;

sem_t mutexIdPedido;

sem_t contHORNOS;

sem_t nuevoPLATO;

sem_t platoLISTO;

//Funciones
PLANIFICADOR_RESTAURANTE planificador_actual_restaurante();
void* iniciar_planificador_platos();
void* iniciar_cocina(cola_ready* cocinero);
void* pasar_a_ready(t_plato* plato);
void* finalizadorPedidos();
void seleccionarPlatoParaEjecucion(cola_ready* cola);
//en procesarPlato, "cola_ready" representa al cocinero que esta preparando el plato,
//cuando termina de procesarlo, manda un signal avisando que se libero
void procesarPlato(t_plato* plato, cola_ready* cocinero);
//el numero_accion es el indice de la accion en la lista de pasos del plato
void ejecutar(t_plato* plato, int numero_accion, cola_ready* cocinero);
//el index_ciclos sirve para buscar la cantidad de ciclos pendientes que tiene el plato
//en la lista de ciclos para cada accion
void hornear(t_plato* plato, int index_ciclos, cola_ready* cocinero);
void hornearConFIFO(t_plato* plato, int index_ciclos);
void hornearConRR(t_plato* plato, int index_ciclos, cola_ready* cocinero);
void preparar(t_plato* plato, int index_ciclos, cola_ready* cocinero);
void prepararConFIFO(t_plato* plato, int index_ciclos);
void prepararConRR(t_plato* plato, int index_ciclos, cola_ready* cocinero);
void procesarPlatos();
bool cola_con_afinidad(cola_ready* cola);
bool cola_sin_afinidad(cola_ready* cola);
bool existen_colas_con_afinidad(char* afinidad);
void agregarPlatoACola(t_list* cola_origen, t_plato* plato);
uint32_t obtenerIndice(t_list* cola, t_plato* plato);
void delay(int milliseconds);
bool pedidoFinalizado(int id);
bool esElPedido(t_pedido* pedido);
bool platoFinalizado(t_plato* plato);

void agregar_plato_a_cola(cola_ready* cola);

void agregarAfinidadACola(char* afinidad, cola_ready* cola);

void destruir_cola_ready(cola_ready*);

#endif /* PLANIFICACION_PLATOS_H_ */
