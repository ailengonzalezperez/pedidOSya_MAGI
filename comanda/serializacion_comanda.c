/*
 *  Created on: 4 nov. 2020
 *      Author: utnso
 */

#include "serializacion_comanda.h"


void atender_request_comanda(uint32_t request_fd){

	op_code codigo_operacion = recibir_operacion(request_fd);

	switch(codigo_operacion){
	case GUARDAR_PEDIDO:

		printf("Me llego operacion: GUARDAR PEDIDO\n");
		log_info(logger, "Me llego operacion: GUARDAR PEDIDO\n");

		guardar_pedido(request_fd);

		break;
	case GUARDAR_PLATO:

		printf("Me llego operacion: GUARDAR PLATO\n");
		log_info(logger, "Me llego operacion: GUARDAR PLATO\n");

		guardar_plato(request_fd);

		break;
	case CONFIRMAR_PEDIDO:

		printf("Me llego operacion: CONFIRMAR PEDIDO\n");
		log_info(logger, "Me llego operacion: CONFIRMAR PEDIDO\n");

		confirmar_pedido(request_fd);

		break;
	case PLATO_LISTO:

		printf("Me llego operacion: PLATO_LISTO\n");
		log_info(logger, "Me llego operacion: PLATO_LISTO\n");

		plato_listo(request_fd);

		break;
	case OBTENER_PEDIDO:

		printf("Me llego operacion: OBTENER_PEDIDO\n");
		log_info(logger, "Me llego operacion: OBTENER_PEDIDO\n");

		obtener_pedido(request_fd);

		break;
	case FINALIZAR_PEDIDO:

		printf("Me llego operacion: FINALIZAR_PEDIDO\n");
		log_info(logger, "Me llego operacion: FINALIZAR_PEDIDO\n");

		finalizar_pedido(request_fd);

		break;
	default:

		printf("Me llego operacion: FINALIZAR_PEDIDO\n");
		log_info(logger, "Me llego operacion: FINALIZAR_PEDIDO\n");

		break;
	}
}

void guardar_pedido(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion guardar_pedido\n");

	t_paquete* paquete_respuesta;
	//recibe GUARDAR_PEDIDO(id_pedido, nombre_restaurante);
	t_buffer* buffer_app = recibir_buffer(request_fd);
	t_list* lista_app = deserializar_lista_nombres(buffer_app);

	char* id_pedido = list_get(lista_app,0);
	char* nombre_restaurante = list_get(lista_app,1);

	//bool guardar_pedido = guardarPedido(nombre_restaurante, atoi(id_pedido));
	bool guardar_pedido = true;

	if(guardar_pedido){
		log_info(logger, "Se guardo el pedido %s del restaurante %s correctamente", id_pedido, nombre_restaurante);
		paquete_respuesta = crear_paquete_personalizado(OK);
	}else{
		log_info(logger, "No guardo el pedido %s del restaurante %s correctamente", id_pedido, nombre_restaurante);
		paquete_respuesta = crear_paquete_personalizado(FAIL);
	}

	enviar_paquete(paquete_respuesta,request_fd);
}
void guardar_plato(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion guardar_plato\n");


	t_paquete* paquete_respuesta;

	//recibe GUARDAR_PEDIDO(id_pedido, nombre_restaurante, nombre_plato, cantidad)
	t_buffer* buffer_app = recibir_buffer(request_fd);
	t_list* lista_app = deserializar_lista_nombres(buffer_app);

	char* nombre_resto = list_get(lista_app, 0);
	char* id_pedido = list_get(lista_app, 1);
	char* nombre_plato =list_get(lista_app, 2);
	char* cantidad = list_get(lista_app, 3);

	//guardar_plato = guardarPlato();
	bool guardar_plato = true;

	//devuelve ok/fail
	if(guardar_plato){
		log_info(logger, "Se guardo el plato %s del pedido %s del resto %s",nombre_plato, id_pedido, nombre_resto);
		paquete_respuesta = crear_paquete_personalizado(OK);
	}else{
		log_info(logger, "No se guardo el plato %s del pedido %s del resto %s",nombre_plato, id_pedido, nombre_resto);
		paquete_respuesta = crear_paquete_personalizado(FAIL);
	}
	enviar_paquete(paquete_respuesta, request_fd);
}
void confirmar_pedido(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion confirmar_pedido\n");
	t_paquete* paquete_respuesta;

	//recibe CONFIRMAR_PEDIDO(id_pedido, nombre_restaurante)
	t_buffer* buffer_app = recibir_buffer(request_fd);
	t_list* lista_app = deserializar_lista_nombres(buffer_app);

	char* nombre_restaurante = list_get(lista_app,0);
	char* id_pedido = list_get(lista_app,1);

	bool confirmacion = true;
	//confirmacion = confirmarPedido(nombre_restaurante, id_pedido)

	// devuelve ok/fail
	if(confirmacion){
		log_info(logger, "Se confirmo el pedido %s del resto %s",id_pedido, nombre_restaurante);
		paquete_respuesta = crear_paquete_personalizado(OK);
	}else{
		log_info(logger, "No se confirmo el pedido %s del resto %s",id_pedido, nombre_restaurante);
		paquete_respuesta = crear_paquete_personalizado(FAIL);
	}
	enviar_paquete(paquete_respuesta, request_fd);
}
void plato_listo(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion plato_listo\n");

	//recibe GUARDAR_PLATO(id_pedido, nombre_restaurante, nombre_plato)
	t_buffer* buffer_app = recibir_buffer(request_fd);
	t_list* lista_app = deserializar_lista_nombres(buffer_app);

	// devuelve ok/fail
	t_paquete* paquete_app = crear_paquete_personalizado(OK);
	enviar_paquete(paquete_app, request_fd);
}
void obtener_pedido(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion obtener_pedido\n");
	t_paquete* paquete_respuesta;
	//recibe OBTENER_PEDIDO(id_pedido, nombre_restaurante)
	t_buffer* buffer_app = recibir_buffer(request_fd);
	t_list* lista_app = deserializar_lista_nombres(buffer_app);

	char* id_pedido = list_get(lista_app,0);
	char* nombre_restaurante = list_get(lista_app,1);

	//paquete_respuesta = obtenerPedido(nombre_restaurante, atoi(id_pedido));
	paquete_respuesta = crear_paquete_personalizado(OK);
	t_list* lista = list_create();
	list_add(lista, "CONFIRMADO");
	list_add(lista, "[milanesa,3,2]");
	list_add(lista, "[wokdeverduras,2,2]");
	t_buffer* buffer_respuesta = serializar_lista_nombres(lista);
	paquete_respuesta->buffer = buffer_respuesta;

	//devuelve el pedido
	if(paquete_respuesta->codigo_operacion == OK){
		log_info(logger, "Se obtuvo los datos del pedido %s",id_pedido);
	}else{
		log_info(logger, "No se obtuvo los datos del pedido %s",id_pedido);
	}
	enviar_paquete(paquete_respuesta, request_fd);
	log_info(logger, "Respuesta enviada a app");

}
void finalizar_pedido(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion finalizar_pedido\n");

	//recibe FINALIZAR_PEDIDO(id_pedido, nombre_restaurante)
	t_buffer* buffer_app = recibir_buffer(request_fd);
	t_list* lista_app = deserializar_lista_nombres(buffer_app);

	// devuelve ok/fail
	t_paquete* paquete_app = crear_paquete_personalizado(OK);
	enviar_paquete(paquete_app, request_fd);
}


/*
t_info_pedido desempaquetar_pedido(t_list* lista_mensajes) {

	t_info_pedido datos_pedido;

	//Los mensajes se "empaquetan" en orden,
	//por lo que el primer valor enviado estara en la
	//primera posicion de la lista y asi sucesivamente

	datos_pedido.resto_nombre = list_get(lista_mensajes,0);
	datos_pedido.id_pedido = list_get(lista_mensajes,1);

	return datos_pedido;
}

t_info_plato desempaquetar_plato(t_list* lista_mensajes) {
	t_info_plato datos_plato;

	//Los mensajes se "empaquetan" en orden,
	//por lo que el primer valor enviado estara en la
	//primera posicion de la lista y asi sucesivamente

	datos_plato.resto_nombre = list_get(lista_mensajes,0);
	datos_plato.id_pedido = list_get(lista_mensajes,1);
	datos_plato.id_plato = list_get(lista_mensajes,2);
	datos_plato.cantidad_plato = atoi(list_get(lista_mensajes,3));

	return datos_plato;
}*/
