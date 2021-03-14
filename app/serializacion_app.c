/*
 * serializacion_app.c
 *
 *  Created on: 5 nov. 2020
 *      Author: utnso
 */

#include "serializacion_app.h"



void atender_request_app(uint32_t request_fd){

	// CONSIGO EL CODIGO DE OPERACION DEL PAQUETE
	op_code codigo_operacion = recibir_operacion(request_fd);

	switch(codigo_operacion){

	case CONSULTAR_RESTAURANTE:
		log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

		consulta_restaurante(request_fd);

		break;

	case SELECCIONAR_RESTAURANTE:
		log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

		seleccionar_restaurante(request_fd);

		break;
	case CONSULTAR_PLATOS:
		log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

		consultar_platos(request_fd);

		break;
	case CREAR_PEDIDO:
		log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

		crear_pedido(request_fd);

		break;
	case AGREGAR_PLATO:
		log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

		agregar_plato(request_fd);

		break;
	case PLATO_LISTO:
		log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

		plato_listo(request_fd);

		break;
	case CONFIRMAR_PEDIDO:
		log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

		confirmar_pedido(request_fd);

		break;
	case CONSULTAR_PEDIDO:
		log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

		conslutar_pedido(request_fd);

		break;
	case SUSCRIPCION:
		log_info(logger,"Entre al case de op_code %i.\n", codigo_operacion);

		suscripcion(request_fd);

		break;
	default:
		log_error(logger,"Codigo de operacion erroneo");
		break;
	}

}

void consulta_restaurante(uint32_t request_fd){
	log_info(logger,"Funcion de CONSULTAR_RESTAURANTE");

	t_list* lista_respuesta = list_create();
	t_paquete* paquete_respuesta = crear_paquete_personalizado(OK);

	if(list_is_empty(lista_restaurantes) == TRUE){
		list_add(lista_respuesta, "RestoDefault");
	}else{
		lista_respuesta = list_map(lista_restaurantes, (void*)get_nombre_restaurante);
	}

	//--------------------- envio OK con lista de restaurantes
	paquete_respuesta->buffer = serializar_lista_nombres(lista_respuesta);
	enviar_paquete(paquete_respuesta, request_fd);

	log_info(logger,"  'CONSULTAR RESUELTA CORRECTAMENTE");

	list_destroy(lista_respuesta);

	close(request_fd);
}

void seleccionar_restaurante(uint32_t request_fd){
	log_info(logger,"Funcion de SELECCIONAR_RESTAURANTE");

	//--------recivo SELECCIONAR_RESTAURANTE con
	//-------- id, nombre_restaurante, pos_x y pos_y

	t_buffer* buffer;
	t_paquete* paquete_cliente;
	t_list* lista_cliente_nuevo;

	buffer = recibir_buffer(request_fd);
	lista_cliente_nuevo = deserializar_lista_nombres(buffer);
	log_info(logger,"Recibio el buffer y deserializo la lista");

	//asocio al cliente con el restaurante

	char* idCliente = list_get(lista_cliente_nuevo,0);
	char* pos_x = list_get(lista_cliente_nuevo,1);
	char* pos_y = list_get(lista_cliente_nuevo,2);
	char* nombre_restaurante = list_get(lista_cliente_nuevo,3);

	if(list_is_empty(lista_restaurantes) == TRUE){
		log_info(logger, "No hay restaurantes conectados, se asociara al cliente con el RestoDefault");
		nombre_restaurante = "RestoDefault";
	}

	if(esClienteNuevo((char*)list_get(lista_cliente_nuevo,0))){
		t_cliente* cliente_nuevo = malloc(sizeof(t_cliente));
		cliente_nuevo->nombre = idCliente;
		cliente_nuevo->pos_x = atoi(pos_x);
		cliente_nuevo->pos_y = atoi(pos_y);
		cliente_nuevo->nombre_restaurante = nombre_restaurante;
		log_info(logger, "Se agrega cliente %s, con posiciones: [%d,%d]", cliente_nuevo->nombre, cliente_nuevo->pos_x, cliente_nuevo->pos_y);
		log_info(logger, "Se vincula con %s", cliente_nuevo->nombre_restaurante);
		list_add(lista_clientes,cliente_nuevo);
	}else{
		modificarSeleccionCliente(idCliente, nombre_restaurante);
	}
	//respondo OK

	paquete_cliente = crear_paquete_personalizado(OK);
	enviar_paquete(paquete_cliente, request_fd);

	//t_cliente* cliente = (t_cliente*)list_get(lista_clientes,0);
	log_info(logger,"CONSULTAR RESUELTA CORRECTAMENTE");
	//list_destroy(lista_cliente);
	close(request_fd);
	//free(cliente_nuevo);
}
void consultar_platos(uint32_t request_fd){
	log_info(logger,"Funcion de CONSULTAR_PLATOS");

	t_buffer* buffer_cliente = recibir_buffer(request_fd);
	char* id_cliente = list_get(deserializar_lista_nombres(buffer_cliente),0);
	t_cliente* cliente_datos = datosCliente(id_cliente);
	log_info(logger, "Se recibio consulta del cliente %s", id_cliente);

	//genero respuesta
	t_paquete* paquete_respuesta = crear_paquete_personalizado(OK);
	log_info(logger, "Paquete de respuesta creado");
	//recibir lista de platos
	if(list_size(lista_restaurantes)!=0){

		log_info(logger,"hay restaurantes conectados");
		//enviar CONSULTAR_PLATOS a restaurante

		t_restaurantes* restaurante = datosRestaurante(cliente_datos->nombre_restaurante);
		log_info(logger, "Hago consulta de platos a %s", restaurante->nombre);

		t_paquete* paquete_restaurante = crear_paquete_personalizado(CONSULTAR_PLATOS);
		t_list* lista = list_create();
		list_add(lista,id_cliente);
		t_buffer* buffer_paquete = serializar_lista_nombres(lista);
		paquete_restaurante->buffer = buffer_paquete;
		log_info(logger, "Creo conexion a puerto: %s y IP: %s", restaurante->ip, restaurante->puerto);
		int fd_restaurante = crear_conexion(restaurante->ip, restaurante->puerto);
		enviar_paquete(paquete_restaurante,fd_restaurante);

		log_info(logger, "envio consulta por el socket %d", fd_restaurante);

		//recibo la respuesta del restaurante
		if(recibir_operacion(fd_restaurante)==OK){
			//agarro la lista de nombres y se la mando al cliente
			t_buffer* buffer_restaurante = recibir_buffer(fd_restaurante);
			paquete_respuesta->buffer = buffer_restaurante;

		}else{
			paquete_respuesta->codigo_operacion = FAIL;
		}
	}else{
		t_list* lista_cliente = list_create();
		char** array_platos= string_get_string_as_array(config_a->platosDefault);
		copiar_array_a_lista(lista_cliente, array_platos);
		t_buffer* buffer = serializar_lista_nombres(lista_cliente);
		paquete_respuesta->buffer = buffer;
	}

	enviar_paquete(paquete_respuesta, request_fd);

	log_info(logger,"CONSULTAR RESUELTA CORRECTAMENTE");
	close(request_fd);
}
void crear_pedido(uint32_t request_fd){
	log_info(logger,"Funcion de CREAR_PEDIDO");
	t_paquete* paquete_respuesta;
	op_code respuesta_restaurante;
	int fd_restaurante;

	//Recibo del cliente
	t_buffer* buffer_cliente = recibir_buffer(request_fd);
	char* id_cliente = list_get(deserializar_lista_nombres(buffer_cliente),0);
	//----------------- CREO PAQUETE
	t_paquete* paquete = crear_paquete_personalizado(CREAR_PEDIDO);
	paquete->buffer = buffer_cliente;
	t_cliente* cliente_datos = datosCliente(id_cliente);

	if(strcmp(cliente_datos->nombre_restaurante,"RestoDefault")!= 0){
		t_restaurantes* restaurante = datosRestaurante(cliente_datos->nombre_restaurante);

		//ENVIO DATOS AL RESTAURANTE
		fd_restaurante = crear_conexion(restaurante->ip, restaurante->puerto);
		enviar_paquete(paquete, fd_restaurante);//enviar a restuarante
		log_info(logger, "Datos enviados al restaurante %s",cliente_datos->nombre_restaurante);

		respuesta_restaurante = recibir_operacion(fd_restaurante);
	}else{
		respuesta_restaurante = OK;
	}

	if(respuesta_restaurante == OK){
		//----------------- RECOJO LA RESPUESTA
		t_buffer* buffer;
		char* id_pedido;

		if(strcmp(cliente_datos->nombre_restaurante,"RestoDefault")!= 0){
			buffer = recibir_buffer(fd_restaurante);
			id_pedido = list_get(deserializar_lista_nombres(buffer),0);
		}else{
			id_pedido = string_itoa(generarIdPedido());
		}

		//----------------- MUESTRO LA RESPUESTA
		log_info(logger,"mi pedido se creo correctamente con el id: %s", id_pedido);

		//------------------------------------GUARDO EN COMANDA
		t_list* lista = list_create();
		list_add(lista, id_pedido);
		list_add(lista, cliente_datos->nombre_restaurante);

		//----------------- CREO PAQUETE
		t_paquete* paquete_comanda = crear_paquete_personalizado(GUARDAR_PEDIDO);
		t_buffer* buffer_comanda = serializar_lista_nombres(lista);
		paquete_comanda->buffer = buffer_comanda;

		//----- ABRO LA CONEXION CON LA COMANDA
		int fd_comanda = crear_conexion(config_a->ip_comanda, config_a->puerto_comanda);
		log_info(logger, "Envia mensaje a comanda por socket %d IP: %s PUERTO: %s", fd_comanda, config_a->ip_comanda, config_a->puerto_comanda);

		enviar_paquete(paquete_comanda, fd_comanda);//enviar a comanda
		op_code respuesta_comanda = recibir_operacion(fd_comanda);//recibir de comanda

		if(respuesta_comanda==OK){
			log_info(logger,"Se guardo el pedido en comanda correctamente");

		}else{
			log_info(logger,"No se guardo el pedido en comanda correctamente");
		}

		paquete_respuesta = crear_paquete_personalizado(OK);
		t_buffer* buffer_cliente;
		t_list* lista_cliente = list_create();
		list_add(lista_cliente, id_pedido);
		buffer_cliente = serializar_lista_nombres(lista_cliente);
		paquete_respuesta->buffer = buffer_cliente;
	}else{
		log_info(logger,"La operacion no fue efectuada correctamente");
		paquete_respuesta = crear_paquete_personalizado(FAIL);
	}
	//------------------------------------RESPONDO AL CLIENTE

	enviar_paquete(paquete_respuesta, request_fd);
	close(request_fd);
}
void agregar_plato(uint32_t request_fd){
	log_info(logger,"Funcion de AGREGAR_PLATO");

	t_list* lista;
	t_paquete* paquete_respuesta;
	int fd_restaurante;
	op_code respuesta_restaurante;
	//op_code respuesta_comanda = FAIL;

	//------------------RECIBO RESPUESTA
	t_buffer* buffer_cliente;
	buffer_cliente = recibir_buffer(request_fd);
	lista = deserializar_lista_nombres(buffer_cliente);


	//------------------PASO EL MENSAJE A RESTAURANTE y recibo
	t_cliente* cliente_datos = datosCliente((char*)list_remove(lista,2));
	t_restaurantes* restaurante;
	if(strcmp(cliente_datos->nombre_restaurante,"RestoDefault")!= 0){
		restaurante = datosRestaurante(cliente_datos->nombre_restaurante);
		t_paquete* paquete_restaurante = crear_paquete_personalizado(AGREGAR_PLATO);
		t_buffer* buffer_restaurante = serializar_lista_nombres(lista);
		paquete_restaurante->buffer = buffer_restaurante;

		fd_restaurante = crear_conexion(restaurante->ip, restaurante->puerto);
		enviar_paquete(paquete_restaurante, fd_restaurante);//enviar a restuarante
		log_info(logger, "Datos enviados al restaurante %s",cliente_datos->nombre_restaurante);

		respuesta_restaurante = recibir_operacion(fd_restaurante);
	}else{
		respuesta_restaurante = OK;
	}


	// la restaurante
	if(respuesta_restaurante == OK){

		t_paquete* paquete_comanda = crear_paquete_personalizado(GUARDAR_PLATO);
		t_list* lista_comanda = list_create();
		t_buffer* buffer_comanda;

		// TODO lleno ese paquete y se lo mando a la comanda
		list_add(lista_comanda,  cliente_datos->nombre_restaurante);
		list_add(lista_comanda, (char*)list_get(lista,0));
		list_add(lista_comanda, (char*)list_get(lista,1));
		list_add(lista_comanda, "1");

		buffer_comanda = serializar_lista_nombres(lista_comanda);
		paquete_comanda->buffer = buffer_comanda;

		int fd_comanda = crear_conexion(config_a->ip_comanda, config_a->puerto_comanda);
		log_info(logger, "Envia mensaje a comanda por socket %d IP: %s PUERTO: %s", fd_comanda, config_a->ip_comanda, config_a->puerto_comanda);

		enviar_paquete(paquete_comanda,fd_comanda);

		op_code respuesta_comanda = recibir_operacion(fd_comanda);

		if(respuesta_comanda==OK){
			log_info(logger,"Se guardo el plato en comanda correctamente");

		}else{
			log_info(logger,"No se guardo el plato en comanda correctamente");
		}


		paquete_respuesta = crear_paquete_personalizado(OK);
		t_buffer* buffer_cliente;


	}else{
		log_info(logger,"La operacion no fue efectuada correctamente");
		paquete_respuesta = crear_paquete_personalizado(FAIL);
	}

	enviar_paquete(paquete_respuesta, request_fd);

	close(request_fd);

}

void confirmar_pedido(uint32_t request_fd){
	log_info(logger,"Funcion de CONFIRMAR_PEDIDO");
	t_paquete* paquete_respuesta;
	op_code respuesta_restaurante;
	//------------------------------------ RECIBO ID
	t_buffer* buffer_cliente = recibir_buffer(request_fd);
	t_list* list_cliente = deserializar_lista_nombres(buffer_cliente);
	char* id_pedido = list_get(list_cliente,0);
	char* id_cliente = list_remove(list_cliente,1);
	log_info(logger, "Se hizo un pedido de confirmacion del pedido %s", id_pedido);
	t_cliente* cliente_datos = datosCliente(id_cliente);

	if(strcmp(cliente_datos->nombre_restaurante,"RestoDefault")!= 0){
		//--------------------------CREO RESPUESTA RESTAURANTE
		t_paquete* paquete_restaurante = crear_paquete_personalizado(CONFIRMAR_PEDIDO);
		t_buffer* buffer_restaurante = serializar_lista_nombres(list_cliente);
		paquete_restaurante->buffer = buffer_restaurante;

		log_info(logger, "Generado mensaje de confirmacion para el restaurante");


		t_restaurantes* restaurante = datosRestaurante(cliente_datos->nombre_restaurante);

		int fd_restaurante = crear_conexion(restaurante->ip, restaurante->puerto);
		enviar_paquete(paquete_restaurante, fd_restaurante);//enviar a restuarante
		log_info(logger, "Datos enviados al restaurante %s",cliente_datos->nombre_restaurante);

		respuesta_restaurante = recibir_operacion(fd_restaurante);//recibir de restaurante
	}else{
		respuesta_restaurante = OK;
	}

	if(respuesta_restaurante == OK){
		//------------MANDAR MENSAJE DE CONFIRMACION A LA COMANDA

		t_paquete* paquete_comanda = crear_paquete_personalizado(CONFIRMAR_PEDIDO);
		t_list* lista_comanda = list_create();
		t_buffer* buffer_comanda;

		// TODO lleno ese paquete y se lo mando a la comanda
		list_add(lista_comanda, cliente_datos->nombre_restaurante);
		list_add(lista_comanda, id_pedido);


		buffer_comanda = serializar_lista_nombres(lista_comanda);
		paquete_comanda->buffer = buffer_comanda;

		int fd_comanda = crear_conexion(config_a->ip_comanda, config_a->puerto_comanda);
		enviar_paquete(paquete_comanda, fd_comanda);//enviar a comanda

		op_code respuesta_comanda = recibir_operacion(fd_comanda);//recibir de comanda
		if(respuesta_comanda == OK){
			log_info(logger,"Pedido confirmado por la comanda correctamente");

			if(confirmarPedido(id_pedido, cliente_datos)){
				log_info(logger,"Se agrego a las colas de planificacion correctamente");
				paquete_respuesta = crear_paquete_personalizado(OK);
			}else{
				log_info(logger,"No se agrego a las colas de planificacion correctamente");
				paquete_respuesta = crear_paquete_personalizado(FAIL);
			}

		}else{
				paquete_respuesta = crear_paquete_personalizado(FAIL);
				log_info(logger,"Se produjo un error en la creacion del pedido");
		}
	}else{
		log_info(logger,"La operacion no fue efectuada correctamente");
		paquete_respuesta = crear_paquete_personalizado(FAIL);
	}

	//------------------------------------RESPONDO AL CLIENTE
	enviar_paquete(paquete_respuesta, request_fd);

}
void conslutar_pedido(uint32_t request_fd){
	log_info(logger,"Funcion de CONSULTAR_PEDIDO");

	//---------------------------recibe CONSULTAR_PEDIDO (id_pedido)
	t_buffer* buffer_cliente = recibir_buffer(request_fd);
	t_list* lista_cliente = deserializar_lista_nombres(buffer_cliente);
	char* id_pedido = list_get(lista_cliente,0);
	char* id_cliente = list_remove(lista_cliente,1);

	t_cliente* cliente_datos = datosCliente(id_cliente);

	//envia OBTENER_PEDIDO (id_pedido, nombre_restaurante) a comanda
	t_paquete* paquete_comanda = crear_paquete_personalizado(OBTENER_PEDIDO);
	t_list* lista_comanda = list_create();
	list_add(lista_comanda, id_pedido);
	list_add(lista_comanda, cliente_datos->nombre_restaurante);
	t_buffer* buffer_comanda = serializar_lista_nombres(lista_comanda);
	paquete_comanda->buffer = buffer_comanda;

	int fd_comanda = crear_conexion(config_a->ip_comanda, config_a->puerto_comanda);
	enviar_paquete(paquete_comanda, fd_comanda);
	log_info(logger, "Datos del pédido consultados a comanda");

	//---------------------------------------recibe lista de comanda
	//---------------------------------------y envia lista al cliente
	op_code codigo_comanda = recibir_operacion(fd_comanda); // que lo reciba de comanda
	log_info(logger, "Recibo codigo de operacion de comanda");
	t_paquete* paquete_cliente = crear_paquete_personalizado(codigo_comanda);

	if (codigo_comanda == OK){
		buffer_comanda = recibir_buffer(fd_comanda);
		log_info(logger, "Recibo buffer de comanda");
		paquete_cliente->buffer = buffer_comanda;
		enviar_paquete(paquete_cliente, request_fd);
		log_info(logger, "Datos del pédido enviados");
	}else{
		log_info(logger, "error consultando pedido");
		enviar_paquete(paquete_cliente, request_fd);
	}


}
void plato_listo(uint32_t request_fd){
	log_info(logger,"Funcion de PLATO_LISTO");
	t_paquete* paquete_respuesta;

	//recibe PLATO_LISTO(nombre_resto, id pedido, nombre_plato) del restaurante
	t_buffer* buffer_restaurante = recibir_buffer(request_fd);
	t_list* lista_resto = deserializar_lista_nombres(buffer_restaurante);
	char* nombre_resto = list_get(lista_resto, 0);
	char* id_pedido = list_get(lista_resto, 1);

	//informa PLATO_LISTO(nombre_resto, id pedido, nombre_plato) a la comanda
	t_paquete* paquete_comanda = crear_paquete_personalizado(PLATO_LISTO);
	t_buffer* buffer_comanda = serializar_lista_nombres(lista_resto);
	paquete_comanda->buffer = buffer_comanda;

	int fd_comanda = crear_conexion(config_a->ip_comanda, config_a->puerto_comanda);
	enviar_paquete(paquete_comanda, fd_comanda);

	op_code codigo_comanda = recibir_operacion(fd_comanda);
	if(codigo_comanda==OK){
		//hace OBTENER_PEDIDO(nombre_resto, id pedido) a la comanda
		list_remove(lista_resto, 2);
		t_paquete* paquete_comanda_obtener = crear_paquete_personalizado(OBTENER_PEDIDO);
		t_buffer* buffer_comanda_obtener = serializar_lista_nombres(lista_resto);
		paquete_comanda_obtener->buffer = buffer_comanda_obtener;

		fd_comanda = crear_conexion(config_a->ip_comanda, config_a->puerto_comanda);
		enviar_paquete(paquete_comanda_obtener, fd_comanda);

		codigo_comanda = recibir_operacion(fd_comanda);

		if(codigo_comanda==OK){
			buffer_comanda_obtener = recibir_buffer(fd_comanda);
			t_list* lista_comanda = deserializar_lista_nombres(buffer_comanda_obtener);
			char* estado = list_get(lista_comanda,0);

			paquete_respuesta = crear_paquete_personalizado(OK);
			if(strcmp(estado, "FINALIZADO")==0){
				finalizarPedido(atoi(id_pedido));
			}

		}else{
			paquete_respuesta = crear_paquete_personalizado(FAIL);
		}
		//verifica si el pedido esta listo y
		//en base a eso cambia el pedido de la lista de pedidos
		//finalizarPedido(id_pedido);
	}else{
		paquete_respuesta = crear_paquete_personalizado(FAIL);
	}

	enviar_paquete(paquete_respuesta,request_fd);

}
void suscripcion(uint32_t request_fd){
	log_info(logger,"Funcion de SUSCRIPCION");

	t_buffer* buffer = recibir_buffer(request_fd);
	t_list* lista_restaurante = deserializar_lista_nombres(buffer);

	log_info(logger, "El restaurante %s se suscribio", (char*)list_get(lista_restaurante,0));

	t_restaurantes* restaurante = malloc(sizeof(t_restaurantes));
	restaurante->nombre = list_get(lista_restaurante,0);
	restaurante->posicion_x = atoi(list_get(lista_restaurante,1));
	restaurante->posicion_y = atoi(list_get(lista_restaurante,2));
	restaurante->puerto = list_get(lista_restaurante,3);
	restaurante->ip = list_get(lista_restaurante,4);

	//log_info(logger, "Conexion con el restaurante por socket %d establecida y almacenada", restaurante->restaurante_fd);

	list_add(lista_restaurantes, restaurante);
}



