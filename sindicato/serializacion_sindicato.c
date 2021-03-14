/*
 * serializacion_sindicato.c
 *
 *  Created on: 22 nov. 2020
 *      Author: utnso
 */

#include"serializacion_sindicato.h"


void atender_request_sindicato(uint32_t request_fd){

	op_code codigo_operacion = recibir_operacion(request_fd);

	switch(codigo_operacion){
	case OBTENER_RESTAURANTE:

		printf("Me llego operacion: OBTENER_RESTAURANTE \n");
		log_info(logger, "Me llego operacion: OBTENER_RESTAURANTE \n");

		obtener_restaurante(request_fd);

	break;
	case CONSULTAR_PLATOS:

		printf("Me llego operacion: CONSULTAR_PLATOS \n");
		log_info(logger, "Me llego operacion: CONSULTAR_PLATOS \n");

		consultar_platos(request_fd);

	break;
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
	case TERMINAR_PEDIDO:

		printf("Me llego operacion: TERMINAR_PEDIDO\n");
		log_info(logger, "Me llego operacion: TERMINAR_PEDIDO\n");

		terminar_pedido(request_fd);

		break;
	case OBTENER_RECETA:

		printf("Me llego operacion: OBTENER_RECETA\n");
		log_info(logger, "Me llego operacion: OBTENER_RECETA\n");

		obtener_receta(request_fd);

		break;
	default:

		printf("Me llego operacion: FINALIZAR_PEDIDO\n");
		log_info(logger, "Me llego operacion: FINALIZAR_PEDIDO\n");

		break;
	}
}

void consultar_platos(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion consultar_platos\n");

	//recibe CONSULTAR_PLATOS(nombre_restaurante)
	t_buffer* buffer_restaurante = recibir_buffer(request_fd);
	char* nombre_restaurante;
	nombre_restaurante = deserializar_nombre(buffer_restaurante);

	log_info(logger, "Recibi una consulta de platos del restaurante %s",nombre_restaurante);

	//devuelve lista de comidas
	t_paquete* paquete_restaurante = crear_paquete_personalizado(OK);

	char* lista_platos = "[wokverduras,asado,milanesa]";
	t_list* lista_comidas = list_create();
	char** array_platos= string_get_string_as_array(lista_platos);
	copiar_array_a_lista(lista_comidas, array_platos);


	buffer_restaurante = serializar_lista_nombres(lista_comidas);
	paquete_restaurante->buffer = buffer_restaurante;
	enviar_paquete(paquete_restaurante, request_fd);
}
void obtener_restaurante(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion obtener_restaurante\n");

	//recibe OBTENER_RESTAURANTE(nombre_restaurante)
	t_buffer* buffer_restaurante = recibir_buffer(request_fd);
	char* nombre = deserializar_nombre(buffer_restaurante);

	log_info(logger, "restaurante: %s", nombre);

	//TODO buscar los datos en los bloques

	char* cantidad_cocineros = "2";
	char* posicion = "[1,2]";
	char* afinidades = "[milanesa,asado]";
	char* platos = "[wokverduras,asado,milanesa]";
	char* precios = "[300,500,400]";
	char* cantidad_hornos = "3";


	//prepara datos para el envio
	t_list* lista_restaurante = list_create();
	list_add(lista_restaurante,cantidad_cocineros);
	list_add(lista_restaurante,posicion);
	list_add(lista_restaurante,afinidades);
	list_add(lista_restaurante,platos);
	list_add(lista_restaurante,precios);
	list_add(lista_restaurante,cantidad_hornos);

	//devuelve datos del restaurante
	t_paquete* paquete_restaurante = crear_paquete_personalizado(OK);
	buffer_restaurante = serializar_lista_nombres(lista_restaurante);
	paquete_restaurante->buffer = buffer_restaurante;
	enviar_paquete(paquete_restaurante, request_fd);
}
void guardar_pedido(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion guardar_pedido\n");

	t_paquete* paquete_respuesta;
	//recibe GUARDAR_PEDIDO(id_pedido, nombre_restaurante)
	t_buffer* buffer_restaurante = recibir_buffer(request_fd);
	t_list* lista_restaurante = deserializar_lista_nombres(buffer_restaurante);

	char* id_pedido = list_get(lista_restaurante,0);
	char* nombre_restaurante = list_get(lista_restaurante,1);

	//guardar_pedido = guardarPedido(char* id_pedido, char* nombre_restaurante);
	bool guardar_pedido = true;

	if(guardar_pedido){
		log_info(logger, "Se creo el pedido %s para el restaurante %s correctamente", id_pedido, nombre_restaurante);
		paquete_respuesta = crear_paquete_personalizado(OK);
	}else{
		log_info(logger, "No se creo el pedido %s para el restaurante %s correctamente", id_pedido, nombre_restaurante);
		paquete_respuesta = crear_paquete_personalizado(FAIL);
	}

	//devuelve ok/fail
	enviar_paquete(paquete_respuesta, request_fd);
}
void guardar_plato(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion guardar_plato\n");

	t_paquete* paquete_respuesta;

	//recibe GUARDAR_PEDIDO(id_pedido, nombre_restaurante, nombre_plato, cantidad)
	t_buffer* buffer_restaurante = recibir_buffer(request_fd);
	t_list* lista_restaurante = deserializar_lista_nombres(buffer_restaurante);

	char* nombre_resto = list_get(lista_restaurante, 0);
	char* id_pedido = list_get(lista_restaurante, 1);
	char* nombre_plato =list_get(lista_restaurante, 2);
	char* cantidad = list_get(lista_restaurante, 3);


	//guardar_plato = guardarPlato(char* id_pedido, char* nombre_restaurante);
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
	t_buffer* buffer_restaurante = recibir_buffer(request_fd);
	t_list* lista_restaurante = deserializar_lista_nombres(buffer_restaurante);

	char* id_pedido = list_get(lista_restaurante,0);
	char* nombre_resto = list_get(lista_restaurante,1);

	//devuelve ok/fail
	//confirmar_pedido = confirmarPedido(char* id_pedido, char* nombre_restaurante);
	bool confirmar_pedido = true;

	//devuelve ok/fail
	if(confirmar_pedido){
		log_info(logger, "Se confirmo pedido %s del resto %s",id_pedido, nombre_resto);
		paquete_respuesta = crear_paquete_personalizado(OK);
	}else{
		log_info(logger, "No se confirmo pedido %s del resto %s", id_pedido, nombre_resto);
		paquete_respuesta = crear_paquete_personalizado(FAIL);
	}
	enviar_paquete(paquete_respuesta, request_fd);
}
void plato_listo(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion plato_listo\n");

	//recibe PLATO_LISTO(id_pedido, nombre_restaurante, nombre_plato)
	t_buffer* buffer_restaurante = recibir_buffer(request_fd);
	t_list* lista_restaurante = deserializar_lista_nombres(buffer_restaurante);

	//devuelve ok/fail
	t_paquete* paquete_restaurante = crear_paquete_personalizado(OK);
	enviar_paquete(paquete_restaurante, request_fd);
}
void obtener_pedido(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion obtener_pedido\n");
	t_paquete* paquete_respuesta;
	//recibe OBTENER_PEDIDO(id_pedido, nombre_restaurante)
	t_buffer* buffer_restaurante = recibir_buffer(request_fd);
	t_list* lista_restaurante = deserializar_lista_nombres(buffer_restaurante);

	char* id_pedido = list_get(lista_restaurante,0);
	char* nombre_restaurante = list_get(lista_restaurante,1);

	//paquete_respuesta = obtenerPedido(nombre_restaurante, atoi(id_pedido));
	paquete_respuesta = crear_paquete_personalizado(OK);
	t_list* lista = list_create();
	list_add(lista, "CONFIRMADO");
	list_add(lista, "[wokdeverduras,milanesa,asado]");
	list_add(lista, "[3,3,2]");
	list_add(lista, "[2,2,2]");
	t_buffer* buffer_respuesta = serializar_lista_nombres(lista);
	paquete_respuesta->buffer = buffer_respuesta;

	//devuelve el pedido
	if(paquete_respuesta->codigo_operacion == OK){
		log_info(logger, "Se obtuvo los datos del pedido %s",id_pedido);
	}else{
		log_info(logger, "No se obtuvo los datos del pedido %s",id_pedido);
	}
	enviar_paquete(paquete_respuesta, request_fd);
	log_info(logger, "Respuesta enviada a restaurante");
}
void terminar_pedido(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion terminar_pedido\n");

	//recibe TERMINAR_PEDIDO(id_pedido, nombre_restaurante)
	t_buffer* buffer_restaurante = recibir_buffer(request_fd);
	t_list* lista_restaurante = deserializar_lista_nombres(buffer_restaurante);

	//devuelve ok/fail
	t_paquete* paquete_restaurante = crear_paquete_personalizado(OK);
	enviar_paquete(paquete_restaurante, request_fd);
}
void obtener_receta(uint32_t request_fd){
	log_info(logger, "ingresa a la funcion obtener_receta\n");

	//recibe OBTENER_RECETA(nombre_plato)
	t_buffer* buffer_restaurante = recibir_buffer(request_fd);
	char* nombre_plato = list_get(deserializar_lista_nombres(buffer_restaurante),0);
	//devuelve receta

	//t_paquete* paquete_restaurante = obtenerReceta(nombre_plato);
	//prueba
	t_paquete* paquete_restaurante = crear_paquete_personalizado(OK);
	t_list* lista_restaurante = list_create();
	list_add(lista_restaurante, nombre_plato);
	list_add(lista_restaurante,"[cortar,asar,freir]");
	list_add(lista_restaurante,"[2,3,4]");
	//----
	paquete_restaurante->buffer = buffer_restaurante;
	enviar_paquete(paquete_restaurante, request_fd);

}
