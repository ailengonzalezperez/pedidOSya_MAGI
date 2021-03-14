/*
 * serializacion_cliente.c
 *
 *  Created on: 12 nov. 2020
 *      Author: utnso
 */

#include "serializacion_cliente.h"



void enviar_accion(op_code codigo_operacion, uint32_t socket){

	//----------------- CREO EL PAQUETE

	switch (codigo_operacion){
		case CONSULTAR_RESTAURANTE:
			log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

			consultar_restaurante(socket);

			break;
		case SELECCIONAR_RESTAURANTE:;
			log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

			seleccionar_restaurante(socket);

			break;
		case CONSULTAR_PLATOS:;
			log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

			consultar_platos(socket);

			break;
		case CREAR_PEDIDO:;
			log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

			crear_pedido(socket);

			break;
		case AGREGAR_PLATO:;
			log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

			agregar_plato(socket);

			break;
		case CONFIRMAR_PEDIDO:;

			confirmar_pedido(socket);

			break;
		case CONSULTAR_PEDIDO:;
			log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

			consultar_pedido(socket);

			break;
		default:
				log_info(logger,"CODIGO DE OPERACION INCORRECTO");
			break;
	}


	close(socket);
}

void consultar_restaurante(uint32_t socket){

	log_info(logger,"Funcion de CONSULTAR_RESTAURANTE");

	t_paquete* paquete = crear_paquete_personalizado(CONSULTAR_RESTAURANTE);

	//----------------- ENVIO EL CODIGO
	enviar_paquete(paquete , socket);
	op_code respuesta = recibir_operacion(socket);
	if(respuesta == OK){
		//----------------- RECOJO LA RESPUESTA
		t_buffer* buffer;
		t_list* lista;

		buffer = recibir_buffer(socket);
		lista = deserializar_lista_nombres(buffer);
		//----------------- MUESTRO LA RESPUESTA
		loggear_lista_nombres(lista,logger);
	}else{
		log_info(logger,"La operacion no fue efectuada correctamente");
	}
}

void seleccionar_restaurante(uint32_t socket){
	log_info(logger,"Funcion de SELECCIONAR_RESTAURANTE");

	t_paquete* paquete = crear_paquete_personalizado(SELECCIONAR_RESTAURANTE);

	//----------------- PREGUNTO Q RESTAURANTE QUIERE
	printf("\n\tQue restaurante desea seleccionar ");
	char* restaurante_seleccionado;
	restaurante_seleccionado  = readline(">>");

	//----------------- envio SELECCIONAR_RESTAURANTE con la lista
	t_buffer* buffer_app;
	t_list* lista_app = list_create();

	//lleno

	list_add(lista_app,	(char*)config_c->id_cliente);
	list_add(lista_app,	(char*)config_c->pos_x);
	list_add(lista_app,	(char*)config_c->pos_y);
	list_add(lista_app, restaurante_seleccionado);

	loggear_lista_nombres(lista_app,logger);

	//buffer_app = serializar_lista_nombres(lista_app);
	buffer_app = serializar_lista_nombres(lista_app);
	paquete->buffer = buffer_app;
	enviar_paquete(paquete, socket);

	//---------------------RECIBO RESPUESTA
	op_code respuesta = recibir_operacion(socket);
	if(respuesta == OK){
		log_info(logger,"La operacion fue efectuada correctamente");
	}else{
		log_info(logger,"La operacion no fue efectuada correctamente");
	}

	free(buffer_app);
}
void consultar_platos(uint32_t socket){
	log_info(logger,"Funcion de CONSULTAR_PLATOS");

	//envio CONSULTAR_PLATOS a la app
	t_paquete* paquete = crear_paquete_personalizado(CONSULTAR_PLATOS);
	t_buffer* buffer_app;
	char* id_cliente = config_c->id_cliente;
	t_list* lista = list_create();
	list_add(lista, id_cliente);
	buffer_app = serializar_lista_nombres(lista);
	paquete->buffer = buffer_app;
	enviar_paquete(paquete , socket);

	//recivo la lista de platos
	op_code respuesta = recibir_operacion(socket);
	if(respuesta == OK){
		//----------------- RECOJO LA RESPUESTA
		t_buffer* buffer_app;
		t_list* lista_app;

		buffer_app = recibir_buffer(socket);
		lista_app = deserializar_lista_nombres(buffer_app);
		//----------------- MUESTRO LA RESPUESTA
		loggear_lista_nombres(lista_app,logger);//lista de platos
	}else{
		log_info(logger,"La operacion no fue efectuada correctamente");
	}

}
void crear_pedido(uint32_t socket){
	log_info(logger,"Funcion de CREAR_PEDIDO");

	t_paquete* paquete = crear_paquete_personalizado(CREAR_PEDIDO);
	t_buffer* buffer_app;
	char* id_cliente = config_c->id_cliente;
	t_list* lista = list_create();
	log_info(logger, "Informacion del cliente %s enviada", id_cliente);
	list_add(lista, id_cliente);
	buffer_app = serializar_lista_nombres(lista);
	paquete->buffer = buffer_app;
	enviar_paquete(paquete , socket);

	//----------------- Recibo EL CODIGO
	op_code respuesta = recibir_operacion(socket);
	if(respuesta == OK){
		//----------------- RECOJO LA RESPUESTA
		t_buffer* buffer;
		char* id_pedido;

		buffer = recibir_buffer(socket);
		id_pedido = list_get(deserializar_lista_nombres(buffer),0);
		//----------------- MUESTRO LA RESPUESTA
		log_info(logger,"mi pedido se creo con el id: %s", id_pedido);
	}else{
		log_info(logger,"La operacion no fue efectuada correctamente");
	}
}
void agregar_plato(uint32_t socket){
	log_info(logger,"Funcion de AGREGAR_PLATO");

	t_paquete* paquete = crear_paquete_personalizado(AGREGAR_PLATO);
	t_buffer* buffer_app;
	t_list* lista = list_create();

	//------------------PIDO DATOS
	printf("\n\tId del pedido al que quiere sumar el plato");
	char* id_pedido;
	id_pedido  = readline(">>");
	list_add(lista, id_pedido);

	printf("\n\tNombre del plato que quiere sumar al pedido");
	char* nombre_plato;
	nombre_plato  = readline(">>");
	list_add(lista,nombre_plato);

	list_add(lista,config_c->id_cliente);

	//------------------ ENVIO

	paquete->buffer = serializar_lista_nombres(lista);
	enviar_paquete(paquete , socket);

	//------------------ RECIBO
	op_code respuesta = recibir_operacion(socket);
	if(respuesta == OK){
		log_info(logger,"Se agrego el plato correctamente");
	}else{
		log_info(logger,"La operacion no fue efectuada correctamente");
	}

}
void confirmar_pedido(uint32_t socket){
	log_info(logger,"Funcion de Confirmar Pedido");

	t_paquete* paquete = crear_paquete_personalizado(CONFIRMAR_PEDIDO);

	//------------------PIDO DATOS
	printf("\n\tId del pedido a confirmar");
	char* id_pedido;
	id_pedido  = readline(">>");
	//------------------ ENVIO
	t_list* lista = list_create();
	list_add(lista,id_pedido);
	list_add(lista,config_c->id_cliente);
	paquete->buffer = serializar_lista_nombres(lista);
	enviar_paquete(paquete , socket);

	//------------------ RECIBO

	op_code respuesta = recibir_operacion(socket);
	if(respuesta == OK){
		log_info(logger,"Se confirmo el pedido correctamente");
	}else{
		log_info(logger,"La operacion no fue efectuada correctamente");
	}
}
void consultar_pedido(uint32_t socket){
	log_info(logger,"Funcion de Consultar Plato");

	t_paquete* paquete = crear_paquete_personalizado(CONSULTAR_PEDIDO);

	//------------------PIDO DATOS
	printf("\n\tId del pedido a consultar");
	char* id_pedido;
	id_pedido  = readline(">>");
	//------------------ ENVIO
	t_list* lista = list_create();
	list_add(lista,id_pedido);
	list_add(lista,config_c->id_cliente);
	paquete->buffer = serializar_lista_nombres(lista);
	enviar_paquete(paquete , socket);
	//------------------ RECIBO

	op_code respuesta = recibir_operacion(socket);
	if(respuesta == OK){
	//----------------- RECOJO LA RESPUESTA
		t_list* lista_respuesta;
		t_buffer* buffer;

		buffer = recibir_buffer(socket);
		lista = deserializar_lista_nombres(buffer);

		loggear_lista_nombres(lista,logger);
	//----------------- MUESTRO LA RESPUESTA
		log_info(logger,"La operacion fue efectuada correctamente");

	}else{
		log_info(logger,"La operacion no fue efectuada correctamente");
	}
}
