/*
 * serializacion_restaurante.h
 *
 *  Created on: 5 nov. 2020
 *      Author: utnso
 */

#ifndef SERIALIZACION_APP_H_
#define SERIALIZACION_APP_H_

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
#include"app.h"
#include"auxiliares_app.h"

// manejar codigos de operacion
void atender_request_app(uint32_t);

void consulta_restaurante(uint32_t);
void seleccionar_restaurante(uint32_t);
void consultar_platos(uint32_t);
void crear_pedido(uint32_t);
void agregar_plato(uint32_t);
void plato_listo(uint32_t);
void confirmar_pedido(uint32_t);
void conslutar_pedido(uint32_t);
void suscripcion(uint32_t);
//---------------------------

#endif /* SERIALIZACION_APP_H_ */
