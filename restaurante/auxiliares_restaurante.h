/*
 * auxiliares_restaurante.h
 *
 *  Created on: 17 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_RESTAURANTE_H_
#define AUXILIARES_RESTAURANTE_H_

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
#include "planificacion_platos.h"


t_log* creacion_de_logger_restaurante(t_config* config);
t_config* creacion_de_config_restaurante(char* archivo_config);
void* iniciar_servicio_resto();
void* atender_request(int request_fd);
void pedirMetadata(char* nombre_resto, int conexion_sindicato);
void pedir_recetas(int conexion_sindicato);

//Funciones para liberar memoria
void terminar_resto();
void destruir_receta(t_receta*);
void destruir_plato(t_plato*);
void destruir_pedido(t_pedido*);
void destruir_metadata();

int generarIdPedido();
t_plato* desempaquetarPlato(t_list* lista);

bool sePuedePrepararPlato(char* plato);

bool existeReceta(char* plato);

#endif /* AUXILIARES_RESTAURANTE_H_ */
