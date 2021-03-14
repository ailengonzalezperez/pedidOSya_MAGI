/*
 * auxiliares_cliente.c
 *
 *  Created on: 4 oct. 2020
 *      Author: utnso
 */

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include "auxiliares_cliente.h"

void terminar_cliente(int conexion, t_log* logger, t_config* config){
	if (logger != NULL){
		log_destroy(logger);
	}
	if (config != NULL){
		config_destroy(config);
	}

	liberar_conexion(conexion);
}

t_log* creacion_de_logger_cliente(t_config* config){
	char* nombre_archivo = config_get_string_value(config, "ARCHIVO_LOG");
	t_log* logger = iniciar_logger(nombre_archivo,"CLIENTE");

	if (logger == NULL){
		printf("No fue posible crear el archivo log.\n");
		exit(1);
	}

	log_info(logger,"Logger creado para cliente.\n"); //Registro creación de logger
	return logger;
}

t_config* creacion_de_config_cliente(char* archivo_config){
	t_config* config = leer_config(archivo_config);
	return config;
}
void cargar_config(){
	config_c = malloc(sizeof(t_config_cliente));
	config_c->ip = config_get_string_value(config, "IP");
	config_c->puerto = config_get_string_value(config, "PUERTO");
	config_c->log = config_get_string_value(config, "ARCHIVO_LOG");
	config_c->pos_x = config_get_string_value(config,"POSICION_X");
	config_c->pos_y = config_get_string_value(config,"POSICION_Y");
	config_c->id_cliente = config_get_string_value(config,"ID_CLIENTE");

}

bool noEsDecisionPosible(char* decision){
	return ((strncmp(decision,"1",1)!=0) && (strncmp(decision,"2",1)!=0) &&
			(strncmp(decision,"3",1)!=0) && (strncmp(decision,"4",1)!=0) &&
			(strncmp(decision,"5",1)!=0) && (strncmp(decision,"6",1)!=0) &&
			(strncmp(decision,"7",1)!=0) && (strncmp(decision,"8",1)!=0));
}

void posiblesOpciones(){
	printf("Te mostramos a continuación las opciones disponibles:\n\n");
	printf("1) Consultar Restaurantes\n");
	printf("2) Seleccionar Restaurante\n");
	printf("3) Consultar Platos\n");
	printf("4) Crear Pedido\n");
	printf("5) Añadir Plato\n");
	printf("6) Confirmar Pedido\n");
	printf("7) Consultar Pedido\n");
	printf("\n¿Qué querés hacer? Ingresá el número correspondiente\n\n");
}


void menuComanda(){
	printf("Te mostramos a continuación las opciones disponibles:\n\n");
		printf("1) GUARDAR PEDIDO\n");
		printf("2) GUARDAR PLATO\n");
		printf("3) OBTENER PEDIDO\n");
		printf("4) CONFIRMAR PEDIDO\n");
		printf("5) PLATO LISTO\n");
		printf("6) FINALIZAR PEDIDO\n");
		printf("\n¿Qué querés hacer? Ingresá el número correspondiente\n\n");
}

/*void enviarDatosAComanda(char* decision, int conexion, t_log* logger){
	char* nombre_resto;
	char* id_pedido;
	char* id_plato;
	char* cantidad_plato;
	t_paquete* paquete = crear_paquete();

	if((strncmp(decision,"1",1)==0)){
		paquete->codigo_operacion = GUARDAR_PEDIDO;

		printf("Seleccionaste GUARDAR PEDIDO!\n");
		printf("Por favor, ingresa el nombre del restaurante:\n");
		nombre_resto = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",nombre_resto);
		agregar_a_paquete(paquete,nombre_resto,strlen(nombre_resto)+1);
		free(nombre_resto);

		printf("\nPor favor, ingresa el ID del pedido:\n");
		id_pedido = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",id_pedido);
		agregar_a_paquete(paquete,id_pedido,strlen(id_pedido)+1);
		free(id_pedido);
	}
	else if((strncmp(decision,"2",1)==0)){
		paquete->codigo_operacion = GUARDAR_PLATO;

		printf("Seleccionaste GUARDAR PLATO!\n");
		printf("Por favor, ingresa el nombre del restaurante:\n");
		nombre_resto = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",nombre_resto);
		agregar_a_paquete(paquete,nombre_resto,strlen(nombre_resto)+1);
		free(nombre_resto);

		printf("\nPor favor, ingresa el ID del pedido:\n");
		id_pedido = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",id_pedido);
		agregar_a_paquete(paquete,id_pedido,strlen(id_pedido)+1);
		free(id_pedido);

		printf("\nPor favor, ingresa el ID del plato:\n");
		id_plato = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",id_plato);
		agregar_a_paquete(paquete,id_plato,strlen(id_plato)+1);
		free(id_plato);

		printf("\nPor favor, ingresa la cantidad de platos:\n");
		cantidad_plato = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",cantidad_plato);
		agregar_a_paquete(paquete,cantidad_plato,strlen(cantidad_plato)+1);
		free(cantidad_plato);
	}else if((strncmp(decision,"3",1)==0)){
		paquete->codigo_operacion = OBTENER_PEDIDO;

		printf("Seleccionaste OBTENER PEDIDO!\n");
		printf("Por favor, ingresa el nombre del restaurante:\n");
		nombre_resto = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",nombre_resto);
		agregar_a_paquete(paquete,nombre_resto,strlen(nombre_resto)+1);
		free(nombre_resto);

		printf("\nPor favor, ingresa el ID del pedido:\n");
		id_pedido = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",id_pedido);
		agregar_a_paquete(paquete,id_pedido,strlen(id_pedido)+1);
		free(id_pedido);
	}else if((strncmp(decision,"4",1)==0)){
		paquete->codigo_operacion = CONFIRMAR_PEDIDO;

		printf("Seleccionaste CONFIRMAR PEDIDO!\n");
		printf("Por favor, ingresa el nombre del restaurante:\n");
		nombre_resto = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",nombre_resto);
		agregar_a_paquete(paquete,nombre_resto,strlen(nombre_resto)+1);
		free(nombre_resto);

		printf("\nPor favor, ingresa el ID del pedido:\n");
		id_pedido = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",id_pedido);
		agregar_a_paquete(paquete,id_pedido,strlen(id_pedido)+1);
		free(id_pedido);
	}else if((strncmp(decision,"5",1)==0)){
		paquete->codigo_operacion = PLATO_LISTO;

		printf("Seleccionaste PLATO LISTO!\n");
		printf("Por favor, ingresa el nombre del restaurante:\n");
		nombre_resto = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",nombre_resto);
		agregar_a_paquete(paquete,nombre_resto,strlen(nombre_resto)+1);
		free(nombre_resto);

		printf("\nPor favor, ingresa el ID del pedido:\n");
		id_pedido = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",id_pedido);
		agregar_a_paquete(paquete,id_pedido,strlen(id_pedido)+1);
		free(id_pedido);

		printf("\nPor favor, ingresa el ID del plato:\n");
		id_plato = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",id_plato);
		agregar_a_paquete(paquete,id_plato,strlen(id_plato)+1);
		free(id_plato);
	}else if((strncmp(decision,"6",1)==0)){
		paquete->codigo_operacion = FINALIZAR_PEDIDO;

		printf("Seleccionaste OBTENER PEDIDO!\n");
		printf("Por favor, ingresa el nombre del restaurante:\n");
		nombre_resto = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",nombre_resto);
		agregar_a_paquete(paquete,nombre_resto,strlen(nombre_resto)+1);
		free(nombre_resto);

		printf("\nPor favor, ingresa el ID del pedido:\n");
		id_pedido = readline(">>");
		log_info(logger,"Se agrega a paquete %s\n",id_pedido);
		agregar_a_paquete(paquete,id_pedido,strlen(id_pedido)+1);
		free(id_pedido);
	}else{
		printf("La opcion ingresada es invalida.\n");

	}


	enviar_paquete(paquete,conexion);

	eliminar_paquete(paquete); //recordar eliminar paquete despues de usar
}*/

op_code convertir_codigo_operacion(char *codigo){
	op_code codigo_operacion;

	if(strncmp(codigo,"1",1)==0){
		codigo_operacion=CONSULTAR_RESTAURANTE;
	}else if(strncmp(codigo,"2",1)==0){
		codigo_operacion=SELECCIONAR_RESTAURANTE;
	}else if(strncmp(codigo,"3",1)==0){
		codigo_operacion = CONSULTAR_PLATOS;
	}else if(strncmp(codigo,"4",1)==0){
		codigo_operacion = CREAR_PEDIDO;
	}else if(strncmp(codigo,"5",1)==0){
		codigo_operacion = AGREGAR_PLATO;
	}else if(strncmp(codigo,"6",1)==0){
		codigo_operacion = CONFIRMAR_PEDIDO;
	}else if(strncmp(codigo,"7",1)==0){
		codigo_operacion = CONSULTAR_PEDIDO;
	}


	return codigo_operacion;
}





