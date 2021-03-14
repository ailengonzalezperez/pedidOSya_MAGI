/*
 * auxiliares_comanda.c
 *
 *  Created on: 4 nov. 2020
 *      Author: utnso
 */

#include "auxiliares_comanda.h"

void cargar_config(){
	config_cm = malloc(sizeof(t_config_comanda));
	config_cm->puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
	config_cm->tamanio_swap = config_get_string_value(config,"TAMANIO_SWAP");
	config_cm->algoritmo = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	config_cm->tamanio_memoria = config_get_string_value(config,"TAMANIO_MEMORIA");
}
void iniciar_servicio_comanda(){

	iniciar_servidor(atender_request_comanda, config);

}



