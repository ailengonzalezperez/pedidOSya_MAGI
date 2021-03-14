/*
 * auxiliares_app.h
 *
 *  Created on: 17 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_APP_H_
#define AUXILIARES_APP_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>

#include<readline/readline.h>
#include<magi-library.h>
#include<semaphore.h>
#include"app.h"


int movimientosRestantes(repartidores r, int x_dest, int y_dest);
void hacerUnMovimiento(repartidores* r);
void cargar_config();
void iniciarRepartidor(repartidores r, int x_dest, int y_dest);
int cantidadRepartidores(t_config*  config);
repartidores* generarRepartidor(int n);
t_list* generarRepartidores();
int distanciaARestaurante(repartidores* repartidor, int x_resto, int y_resto);
int distanciaADestino(repartidores* repartidor, int x, int y);
repartidores masCercanoAlRestaurante(repartidores* repartidor1, repartidores* repartidor2, int x_resto, int y_resto);
repartidores* repartidor_mas_cercano(int x_resto, int y_resto, t_list* estadoReady);
int generarIdPedido();

t_log* creacion_de_logger_app(t_config* config);
t_config* creacion_de_config_app(char* archivo_config);

void iniciar_servicio_app();
void atender_request_app(uint32_t request_fd);

bool esClienteNuevo(char* idCliente);
void modificarSeleccionCliente(char* idCliente, char* nombreRestaurante);

void terminar_app();
char* get_nombre_restaurante(t_restaurantes*);
t_list* recibir_paquete_app(int socket_cliente);
void handshakeComanda(void);
#endif /* AUXILIARES_APP_H_ */
