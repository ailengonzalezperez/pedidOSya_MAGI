/*
 * serializacion_cliente.h
 *
 *  Created on: 12 nov. 2020
 *      Author: utnso
 */

#ifndef SERIALIZACION_CLIENTE_H_
#define SERIALIZACION_CLIENTE_H_

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
#include"auxiliares_cliente.h"

//manejar codigos de operacion
void enviar_accion(op_code,uint32_t);

void consultar_restaurante(uint32_t);
void seleccionar_restaurante(uint32_t);
void consultar_platos(uint32_t);
void crear_pedido(uint32_t);
void agregar_plato(uint32_t);
void confirmar_pedido(uint32_t);
void consultar_pedido(uint32_t);
//----------------------------

#endif /* SERIALIZACION_CLIENTE_H_ */
