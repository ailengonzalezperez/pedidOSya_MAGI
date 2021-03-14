/*
 * auxiliares_comanda.h
 *
 *  Created on: 4 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_COMANDA_H_
#define AUXILIARES_COMANDA_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include "comanda.h"
#include "serializacion_comanda.h"

void cargar_config();
void iniciar_servicio_app();

#endif /* AUXILIARES_COMANDA_H_ */
