/*
 * comanda.h
 *
 *  Created on: 4 nov. 2020
 *      Author: utnso
 */

#ifndef COMANDA_H_
#define COMANDA_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<readline/readline.h>
#include<magi-library.h>
#include<sys/mman.h>
#include "auxiliares_comanda.h"
#include "estructuras_memoria.h"


FILE* swap;


t_list* framesMemoriaPrincipal;
t_list* bloquesEnSwap;

int tamanioSwap;

t_log* logger;
t_config* config;

typedef struct{
	char* puerto;
	char* tamanio_memoria;
	char* tamanio_swap;
	char* algoritmo;

}t_config_comanda;

t_config_comanda* config_cm;

t_list* lista_restaurantes;


#endif /* COMANDA_H_ */
