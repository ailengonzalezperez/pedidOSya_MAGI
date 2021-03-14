/*
 * mensajes_comanda.h
 *
 *  Created on: 4 nov. 2020
 *      Author: utnso
 */

#ifndef SERIALIZACION_COMANDA_H_
#define SERIALIZACION_COMANDA_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include"auxiliares_comanda.h"


void atender_request_comanda(uint32_t);

void guardar_pedido(uint32_t);
void guardar_plato(uint32_t);
void confirmar_pedido(uint32_t);
void plato_listo(uint32_t);
void obtener_pedido(uint32_t);
void finalizar_pedido(uint32_t);
/*
t_info_pedido desempaquetar_pedido(t_list*);
t_info_plato desempaquetar_plato(t_list*);
*/

#endif /* SERIALIZACION_COMANDA_H_ */
