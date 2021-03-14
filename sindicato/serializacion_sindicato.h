/*
 * serializacion_sindicato.h
 *
 *  Created on: 22 nov. 2020
 *      Author: utnso
 */

#ifndef SERIALIZACION_SINDICATO_H_
#define SERIALIZACION_SINDICATO_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include"sindicato.h"

void atender_request_sindicato(uint32_t);

void consultar_platos(uint32_t request_fd);
void obtener_restaurante(uint32_t request_fd);
void guardar_pedido(uint32_t request_fd);
void guardar_plato(uint32_t request_fd);
void confirmar_pedido(uint32_t request_fd);
void plato_listo(uint32_t request_fd);
void obtener_pedido(uint32_t request_fd);
void terminar_pedido(uint32_t request_fd);
void obtener_receta(uint32_t request_fd);
#endif /* SERIALIZACION_SINDICATO_H_ */
