/*
 * serializacion_restaurante.h
 *
 *  Created on: 5 nov. 2020
 *      Author: utnso
 */

#ifndef SERIALIZACION_RESTAURANTE_H_
#define SERIALIZACION_RESTAURANTE_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include<semaphore.h>
#include"restaurante.h"
#include"auxiliares_restaurante.h"
#include "planificacion_platos.h"


void atender_request_restaurante(uint32_t request_fd);


void obtener_restaurante(uint32_t);
void consultar_platos(uint32_t);
void crear_pedido(uint32_t);
void agregar_plato(uint32_t);
void confirmar_pedido(uint32_t);
void plato_listo(char*,char*);
void consultar_pedido(uint32_t);
void terminar_pedido(char*);
void obtener_receta(char*);
bool obtener_pedido(char*);

//auxiliar
bool existePedidoEnLista(t_pedido*);

#endif /* SERIALIZACION_RESTAURANTE_H_ */
