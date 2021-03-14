/*
 * serializacion_restaurante.c
 *
 *  Created on: 5 nov. 2020
 *      Author: utnso
 */

#include "serializacion_restaurante.h"
#include "planificacion_platos.h"
#include "auxiliares_restaurante.h"
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

void atender_request_restaurante(uint32_t request_fd) {

	// CONSIGO EL CODIGO DE OPERACION DEL PAQUETE
	op_code codigo_operacion = recibir_operacion(request_fd);
	switch (codigo_operacion) {
	case CONSULTAR_PLATOS:
		sem_wait(&mutexLOGGER);
		log_info(logger, "Entre al case de op_code %i.\n", codigo_operacion);
		sem_post(&mutexLOGGER);

		consultar_platos(request_fd);

		break;
	case CREAR_PEDIDO:
		sem_wait(&mutexLOGGER);
		log_info(logger, "Entre al case de op_code %i.\n", codigo_operacion);
		sem_post(&mutexLOGGER);

		crear_pedido(request_fd);

		break;
	case AGREGAR_PLATO:
		sem_wait(&mutexLOGGER);
		log_info(logger, "Entre al case de op_code %i.\n", codigo_operacion);
		sem_post(&mutexLOGGER);

		agregar_plato(request_fd);

		break;
	case CONFIRMAR_PEDIDO:
		sem_wait(&mutexLOGGER);
		log_info(logger, "Entre al case de op_code %i.\n", codigo_operacion);
		sem_post(&mutexLOGGER);

		confirmar_pedido(request_fd);

		break;
	case CONSULTAR_PEDIDO:
		sem_wait(&mutexLOGGER);
		log_info(logger, "Entre al case de op_code %i.\n", codigo_operacion);
		sem_post(&mutexLOGGER);

		consultar_pedido(request_fd);

		break;
	default:
		sem_wait(&mutexLOGGER);
		log_warning(logger, "Operacion desconocida. No quieras meter la pata");
		sem_post(&mutexLOGGER);
		break;
	}

}

void consultar_platos(uint32_t request_fd) {
	sem_wait(&mutexLOGGER);
	log_info(logger, "Funcion de CONSULTAR_PLATOS");
	sem_post(&mutexLOGGER);
	//recibe CONSULTAR_PLATOS de la app
	recibir_buffer(request_fd);
	//envia CONSULTAR_PLATOS (nombre_restaurante) al sindicato
	t_paquete* paquete_sindicato = crear_paquete_personalizado(CONSULTAR_PLATOS);

	t_buffer* buffer_sindicato;
	buffer_sindicato = serializar_nombre(nombre_resto);
	paquete_sindicato->buffer = buffer_sindicato;
	socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	enviar_paquete(paquete_sindicato, socket_sindicato);

	sem_wait(&mutexLOGGER);
	log_info(logger,"Paquete enviado a sindicato");
	sem_post(&mutexLOGGER);

	//recibe lista de comidas
	op_code codigo_operacion = recibir_operacion(socket_sindicato);

	sem_wait(&mutexLOGGER);
	log_info(logger,"Recibido codigo de operacion");
	sem_post(&mutexLOGGER);

	t_paquete* paquete_respuesta = crear_paquete_personalizado(codigo_operacion);
	if (codigo_operacion == OK) {
		buffer_sindicato = recibir_buffer(socket_sindicato);
		t_list* lista_platos = deserializar_lista_nombres(buffer_sindicato);

		sem_wait(&mutexLOGGER);
		loggear_lista_nombres(lista_platos, logger);
		sem_post(&mutexLOGGER);

		t_buffer* buffer_app = serializar_lista_nombres(lista_platos);
		paquete_respuesta->buffer = buffer_app;
	} else {
		sem_wait(&mutexLOGGER);
		log_info(logger, "No se obtuvo la lista de comidas correctamente");
		sem_post(&mutexLOGGER);
	}
	enviar_paquete(paquete_respuesta, request_fd);
}

void crear_pedido(uint32_t request_fd) {
	sem_wait(&mutexLOGGER);
	log_info(logger, "Funcion de CREAR_PEDIDO");
	sem_post(&mutexLOGGER);

	recibir_buffer(request_fd);
	// genera el id
	char* id_pedido = string_itoa(generarIdPedido());
	//envia GUARDAR_PEDIDO (id_pedido, nombre_restaurante) al sindicato
	t_paquete* paquete_sindicato = crear_paquete_personalizado(GUARDAR_PEDIDO);
	t_buffer* buffer_sindicato;
	t_list* lista_sindicato = list_create();

	list_add(lista_sindicato, id_pedido);
	list_add(lista_sindicato, nombre_resto);

	sem_wait(&mutexLOGGER);
	log_info(logger,"Pedido nro: %s generado",id_pedido);
	sem_post(&mutexLOGGER);

	buffer_sindicato = serializar_lista_nombres(lista_sindicato);
	paquete_sindicato->buffer = buffer_sindicato;

	int fd_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	enviar_paquete(paquete_sindicato, fd_sindicato);
	//enviar_paquete(paquete_sindicato, socket_sindicato);
	//eliminar_paquete(paquete_sindicato);

	op_code codigo_operacion = recibir_operacion(fd_sindicato);
	if (codigo_operacion == OK) {
		sem_wait(&mutexLOGGER);
		log_info(logger, "Se guardo el pedido correctamete");
		sem_post(&mutexLOGGER);

		t_paquete* paquete_app = crear_paquete_personalizado(OK);

		sem_wait(&mutexLOGGER);
		log_info(logger,"Pedido nro: %s enviado a la app",id_pedido);
		sem_post(&mutexLOGGER);

		list_remove(lista_sindicato,1);
		t_buffer* buffer_app = serializar_lista_nombres(lista_sindicato);
		//t_buffer* buffer_app = serializar_nombre(id_pedido);
		paquete_app->buffer = buffer_app;
		enviar_paquete(paquete_app, request_fd);

		sem_wait(&mutexLOGGER);
		log_info(logger, "Se creo el pedido %s correctamente",id_pedido);
		sem_post(&mutexLOGGER);
		//eliminar_paquete(paquete_app);
	} else {
		sem_wait(&mutexLOGGER);
		log_info(logger, "No guardo el pedido correctamete");
		sem_post(&mutexLOGGER);

		t_paquete* paquete_app = crear_paquete_personalizado(FAIL);
		enviar_paquete(paquete_app, request_fd);
		//eliminar_paquete(paquete_app);
	}


}

void agregar_plato(uint32_t request_fd) {
	sem_wait(&mutexLOGGER);
	log_info(logger, "Funcion de AGREGAR_PLATO");
	sem_post(&mutexLOGGER);

	// recibe AGREGAR_PLATO (nombre_plato, id_pedido)
	t_buffer* buffer_app = recibir_buffer(request_fd);
	t_list* lista_app;
	lista_app = deserializar_lista_nombres(buffer_app);

	char* id_pedido = list_get(lista_app, 0);
	char* afinidad_plato = list_get(lista_app, 1);

	//primero chequeo que el plato pedido pueda ser preparado por el restaurante
	if(!sePuedePrepararPlato(afinidad_plato)){
		sem_wait(&mutexLOGGER);
		log_error(logger, "El restaurante no realiza el plato pedido");
		sem_post(&mutexLOGGER);

		t_paquete* paquete_app = crear_paquete_personalizado(FAIL);
		enviar_paquete(paquete_app, request_fd);

	}else{

	// envia GUARDAR_PLATO (nombre_restaurante, id_pedido, nombre_plato, cantidad)
	t_paquete* paquete_sindicato = crear_paquete_personalizado(GUARDAR_PLATO);
	t_list* lista_sindicato = list_create();
	list_add(lista_sindicato, nombre_resto);
	list_add(lista_sindicato, id_pedido);
	list_add(lista_sindicato, afinidad_plato);
	list_add(lista_sindicato, "1"); //cantidad del plato
	t_buffer* buffer_sindicato = serializar_lista_nombres(lista_sindicato);
	paquete_sindicato->buffer = buffer_sindicato;

	int fd_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	enviar_paquete(paquete_sindicato, fd_sindicato);
	//enviar_paquete(paquete_sindicato, socket_sindicato);
	//eliminar_paquete(paquete_sindicato);

	//recibe ok/fail
	t_paquete* paquete_app;
	op_code codigo_operacion = recibir_operacion(fd_sindicato);
	if (codigo_operacion == OK) {
		sem_wait(&mutexLOGGER);
		log_info(logger, "Se guardo el pedido correctamete");
		sem_post(&mutexLOGGER);

		paquete_app = crear_paquete_personalizado(OK);

	} else {
		sem_wait(&mutexLOGGER);
		log_info(logger, "No guardo el pedido correctamete");
		sem_post(&mutexLOGGER);

		paquete_app = crear_paquete_personalizado(FAIL);
	}
	//envia el ok/fail a la app
	enviar_paquete(paquete_app, request_fd);
	//eliminar_paquete(paquete_app);
	}
}

void confirmar_pedido(uint32_t request_fd) {
	sem_wait(&mutexLOGGER);
	log_info(logger, "Funcion de CONFIRMAR_PEDIDO");
	sem_post(&mutexLOGGER);

	t_paquete* paquete_respuesta;

	//recibe  CONFIRMAR_PEDIDO (id_pedido) de la app
	t_buffer* buffer_app = recibir_buffer(request_fd);
	char* id_pedido = list_get(deserializar_lista_nombres(buffer_app),0);

	sem_wait(&mutexLOGGER);
	log_info(logger,"Recibi pedido de confirmacion del pedido %s" ,id_pedido);
	sem_post(&mutexLOGGER);

	//envia CONFIRMAR_PEDIDO (id_pedido, nombre_restaurante) al sindicato
	t_paquete* paquete_sindicato = crear_paquete_personalizado(CONFIRMAR_PEDIDO);
	t_list* lista_sindicato = list_create();

	list_add(lista_sindicato, id_pedido);
	list_add(lista_sindicato, nombre_resto);

	paquete_sindicato->buffer = serializar_lista_nombres(lista_sindicato);

	int fd_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	enviar_paquete(paquete_sindicato, fd_sindicato);

	sem_wait(&mutexLOGGER);
	log_info(logger,"Se envio el paquete al sindicato");
	sem_post(&mutexLOGGER);
	//enviar_paquete(paquete_sindicato, socket_sindicato);
	//eliminar_paquete(paquete_sindicato);

	//recibe ok/fail del sindicato

	op_code codigo_operacion = recibir_operacion(fd_sindicato);
	if (codigo_operacion == OK) {
		if(obtener_pedido(id_pedido)){
			sem_wait(&mutexLOGGER);
			log_info(logger,"Se Confirmo el pedido correctamente");
			sem_post(&mutexLOGGER);

			paquete_respuesta = crear_paquete_personalizado(OK);
		}else{
			log_info(logger,"Error al confirmar pedido");
			paquete_respuesta = crear_paquete_personalizado(FAIL);
		}
	} else {
		sem_wait(&mutexLOGGER);
		log_info(logger,"Error guardando la confirmacion del pedido en sindicato");
		sem_post(&mutexLOGGER);

		paquete_respuesta = crear_paquete_personalizado(FAIL);
	}
	enviar_paquete(paquete_respuesta, request_fd);
}

//plato listo la ejecuta el propio restaurante
void plato_listo(char* id_pedido, char* nombre_plato) {
	sem_wait(&mutexLOGGER);
	log_info(logger, "Funcion de PLATO_LISTO");
	sem_post(&mutexLOGGER);

	//envia PLATO_LISTO (nombre_restaurante, id_pedido, nombre_plato) al sindicato
	t_paquete* paquete_sindicato = crear_paquete_personalizado(PLATO_LISTO);
	t_list* lista_sindicato = list_create();

	list_add(lista_sindicato, nombre_resto);
	list_add(lista_sindicato, id_pedido);
	list_add(lista_sindicato, nombre_plato);

	t_buffer* buffer_sindicato;
	buffer_sindicato = serializar_lista_nombres(lista_sindicato);

	paquete_sindicato->buffer = buffer_sindicato;
	enviar_paquete(paquete_sindicato, socket_sindicato);
	//recibe ok/fail
	op_code codigo_operacion = recibir_operacion(socket_sindicato);
	if (codigo_operacion == OK) {
		//le envio la info al modulo app
		t_paquete* paquete_app = crear_paquete_personalizado(PLATO_LISTO);
		t_list* lista_app = list_create();

		list_add(lista_app, nombre_plato);
		list_add(lista_app, id_pedido);
		list_add(lista_app, nombre_plato);

		t_buffer* buffer_app;
		buffer_app = serializar_lista_nombres(lista_app);
		paquete_app->buffer = buffer_app;
		//TODO: Crear conexion socket_app
		fd_app = crear_conexion(ip_app,puerto_app);
		enviar_paquete(paquete_app, fd_app);
		//eliminar_paquete(paquete_app);
		codigo_operacion = recibir_operacion(fd_app);
		if(codigo_operacion == OK){
			sem_wait(&mutexLOGGER);
			log_info(logger,"Respuesta de la app a plato listo OK");
			sem_post(&mutexLOGGER);
		}else{
			sem_wait(&mutexLOGGER);
			log_info(logger,"Respuesta de la app a plato listo FAIL");
			sem_post(&mutexLOGGER);
		}
		sem_wait(&mutexLOGGER);
		log_info(logger, "Finaliza el PCB con ID: %s del PLATO: %s",id_pedido,nombre_plato);
		sem_post(&mutexLOGGER);
	} else {
		sem_wait(&mutexLOGGER);
		log_info(logger, "No finaliza el PCB con ID: %s del PLATO: %s",id_pedido,nombre_plato);
		sem_post(&mutexLOGGER);
	}

}

void consultar_pedido(uint32_t request_fd) {
	sem_wait(&mutexLOGGER);
	log_info(logger, "Funcion de CONSULTAR_PEDIDO");
	sem_post(&mutexLOGGER);
//recibo CONSULTAR_PEDIDO (id_pedido) del cliente
//envio OBTENER_PEDIDO (nombre_restaurante, id_pedido) al sindicato
//recibo el pedido
//envio el pedido al cliente
	t_buffer* buffer_cliente = recibir_buffer(request_fd);
	char* id_pedido = list_get(deserializar_lista_nombres(buffer_cliente),0);

	//envia OBTENER_PEDIDO (id_pedido, nombre_restaurante) al sindicato
	t_paquete* paquete_sindicato = crear_paquete_personalizado(OBTENER_PEDIDO);
	t_list* lista_sindicato = list_create();

	list_add(lista_sindicato, id_pedido);
	list_add(lista_sindicato, nombre_resto);

	paquete_sindicato->buffer = serializar_lista_nombres(lista_sindicato);


	int fd_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	enviar_paquete(paquete_sindicato, fd_sindicato);
	//eliminar_paquete(paquete_sindicato);

	t_paquete* paquete_cliente;
	op_code codigo_operacion = recibir_operacion(fd_sindicato);
	if (codigo_operacion == OK) {
		t_buffer* buffer_sindicato = recibir_buffer(fd_sindicato);
		t_list* lista_cliente = deserializar_lista_nombres(buffer_sindicato);

		paquete_cliente = crear_paquete_personalizado(OK);
		buffer_cliente = serializar_lista_nombres(lista_cliente);
		paquete_cliente->buffer = buffer_cliente;
		enviar_paquete(paquete_cliente, request_fd);

		sem_wait(&mutexLOGGER);
		log_info(logger, "Me llego la info del pedido correctamete");
		sem_post(&mutexLOGGER);
	} else {
		paquete_cliente = crear_paquete_personalizado(FAIL);
		enviar_paquete(paquete_cliente, request_fd);

		sem_wait(&mutexLOGGER);
		log_info(logger, "No llego la info del pedido correctamete");
		sem_post(&mutexLOGGER);
	}
}

void terminar_pedido(char* id_pedido) {
	sem_wait(&mutexLOGGER);
	log_info(logger, "Funcion de TERMINAR_PEDIDO");
	sem_post(&mutexLOGGER);

//envio TERMINAR_PEDIDO (id_pedido, nombre_restaurante)
	t_paquete* paquete_sindicato = crear_paquete_personalizado(TERMINAR_PEDIDO);
	t_list* lista_sindicato = list_create();

	list_add(lista_sindicato, id_pedido);
	list_add(lista_sindicato, nombre_resto);

	paquete_sindicato->buffer = serializar_lista_nombres(lista_sindicato);
	int fd_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	enviar_paquete(paquete_sindicato, fd_sindicato);
	//eliminar_paquete(paquete_sindicato);
//recibo ok/fail
	op_code codigo_operacion = recibir_operacion(socket_sindicato);
	if (codigo_operacion == OK) {
		sem_wait(&mutexLOGGER);
		log_info(logger, "Se termino el pedido correctamente");
		sem_post(&mutexLOGGER);
	} else {
		sem_wait(&mutexLOGGER);
		log_info(logger, "No se termino el pedido correctamente");
		sem_post(&mutexLOGGER);
	}
}

void obtener_receta(char* nombre_plato) {
	sem_wait(&mutexLOGGER);
	log_info(logger, "Funcion de OBTENER_RECETA");
	sem_post(&mutexLOGGER);

//envio OBTENER_RECETA (nombre_plato) al sindicato
	t_paquete* paquete_sindicato = crear_paquete_personalizado(OBTENER_RECETA);
	t_list* lista_sindicato = list_create();
	list_add(lista_sindicato, nombre_plato);
	t_buffer* buffer_sindicato = serializar_lista_nombres(lista_sindicato);
	paquete_sindicato->buffer = buffer_sindicato;

	int fd_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	enviar_paquete(paquete_sindicato, fd_sindicato);

	//obtengo respuesta de sindicato
	op_code codigo_operacion = recibir_operacion(fd_sindicato);
	if (codigo_operacion == OK) {
		t_buffer* buffer = recibir_buffer(fd_sindicato);
		t_list* lista = deserializar_lista_nombres(buffer);

		t_receta* nuevaReceta = malloc(sizeof(t_receta)); //TODO: hacer free
		nuevaReceta->nombre_plato = (char*) list_get(lista, 0);
		char* pasos_receta = list_get(lista, 1);
		char* ciclos_receta = list_get(lista, 2);

		char** array_pasos = string_get_string_as_array(pasos_receta);
		copiar_array_a_lista(nuevaReceta->pasos, array_pasos);

		char** array_ciclos = string_get_string_as_array(ciclos_receta);
		copiar_array_a_lista(nuevaReceta->ciclos, array_ciclos);

		//si la nueva receta no se encuentra en la lista global, la agrego
		if(!existeReceta(nuevaReceta->nombre_plato)){
		sem_wait(&mutexRECETAS);
		list_add(recetas, nuevaReceta);
		sem_post(&mutexRECETAS);
		}
		sem_wait(&mutexLOGGER);
		log_info(logger, "Recibi la receta correctamente");
		sem_post(&mutexLOGGER);
	} else {
		sem_wait(&mutexLOGGER);
		log_info(logger, "No recibi la receta correctamente");
		sem_post(&mutexLOGGER);
	}

}

bool obtener_pedido(char* idPedido) {
	log_info(logger,"Llegue a obtener pedido");
	t_paquete* paquete_sindicato = crear_paquete_personalizado(OBTENER_PEDIDO);
	t_buffer* buffer_sindicato;
	t_list* lista_sindicato = list_create();
	list_add(lista_sindicato, nombre_resto);
	list_add(lista_sindicato, idPedido);
	buffer_sindicato = serializar_lista_nombres(lista_sindicato);
	paquete_sindicato->buffer = buffer_sindicato;

	log_info(logger, "Envio mensaje de obtencion de pedido a sindicato, con el pedido: %s", idPedido);
	int fd_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	enviar_paquete(paquete_sindicato, fd_sindicato);

	//obtengo respuesta de sindicato
	op_code codigo_operacion = recibir_operacion(fd_sindicato);
	if (codigo_operacion == OK) {
		t_buffer* buffer = recibir_buffer(fd_sindicato);
		t_list* lista = deserializar_lista_nombres(buffer);
		char* estado_pedido = list_get(lista, 0); //buscar pedido y cambiarle estado
		char* platos_pedido = list_get(lista, 1);
		char* platos_totales = list_get(lista, 2);
		char* cantidades_listas = list_get(lista, 3);

		char** array_platos = string_get_string_as_array(platos_pedido);
		t_list* nombres_platos = list_create();
		copiar_array_a_lista(nombres_platos, array_platos);

		char** array_totales = string_get_string_as_array(platos_totales);
		t_list* cantidades_platos = list_create();
		copiar_array_a_lista(cantidades_platos, array_totales);

		char** array_listos = string_get_string_as_array(cantidades_listas);
		t_list* platos_listos = list_create();
		copiar_array_a_lista(platos_listos, array_listos);

		//creo el pedido y le asigno los platos correspondientes
		t_pedido* nuevoPedido = malloc(sizeof(t_pedido)); //TODO: free
		nuevoPedido->id_pedido = atoi(idPedido);
		nuevoPedido->platos = list_create();
		for (int i = 0; i < list_size(nombres_platos); i++) {
			//creo PCB de cada plato
			t_plato* nuevoPlato = malloc(sizeof(t_plato));
			nuevoPlato->id_pedido = atoi(idPedido); //el id es un char*
			nuevoPlato->afinidad = (char*) list_get(nombres_platos, i);
			nuevoPlato->cantidad_total = atoi((char*) list_get(cantidades_platos, i));
			nuevoPlato->cantidad_lista = atoi((char*) list_get(platos_listos, i));
			list_add(nuevoPedido->platos,nuevoPlato);
			//pido receta
			obtener_receta(nuevoPlato->afinidad);
			//agrego el nuevo plato a la lista new para luego ser procesado
			sem_wait(&mutexLOGGER);
			log_info(logger,"Se crea PCB con ID: %s del PLATO: %s",idPedido,nuevoPlato->afinidad);
			sem_post(&mutexLOGGER);

			sem_wait(&mutexNEW);
			list_add(cola_new, nuevoPlato);
			sem_post(&mutexNEW);
			//free(nuevoPlato);
		}

		bool existePedidoEnLista(t_pedido* pedido){
			return pedido->id_pedido = atoi(idPedido);
		}
		//si el pedido no esta en la lista global de pedidos, lo agrega
		sem_wait(&mutexPEDIDOS);
		if(!list_any_satisfy(pedidos,(void*)existePedidoEnLista)){
		list_add(pedidos,nuevoPedido);
		}
		sem_post(&mutexPEDIDOS);

		sem_post(&nuevoPLATO); //le aviso al modulo planificador de platos que tiene
		//nuevos platos para procesar
		sem_wait(&mutexLOGGER);
		log_info(logger, "Obtuve el pedido correctamente");
		sem_post(&mutexLOGGER);
		//le aviso al modulo que se comunico con restaurante que el pedido fue confirmado
		return true;
		//t_paquete* paquete_cliente;
		//paquete_cliente = crear_paquete_personalizado(OK);
		//enviar_paquete(paquete_cliente, request_fd);
		//eliminar_paquete(paquete_cliente);

	} else {
		sem_wait(&mutexLOGGER);
		log_info(logger, "No recibi el pedido correctamente");
		sem_post(&mutexLOGGER);
		//t_paquete* paquete_cliente;
		//paquete_cliente = crear_paquete_personalizado(FAIL);
		//enviar_paquete(paquete_cliente, request_fd);
		//eliminar_paquete(paquete_cliente);
		return false;
	}

}
