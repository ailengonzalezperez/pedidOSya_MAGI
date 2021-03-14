/*
 * auxiliares_restaurante.c
 *
 *  Created on: 17 oct. 2020
 *      Author: utnso
 */

#include "auxiliares_restaurante.h"
#include "planificacion_platos.h"
#include "restaurante.h"

t_log* creacion_de_logger_restaurante(t_config* config) {
	char* nombre_archivo;
	t_log* logger;

	nombre_archivo = config_get_string_value(config, "ARCHIVO_LOG");
	logger = iniciar_logger(nombre_archivo, "RESTAURANTE");

	if (logger == NULL) {
		printf("No fue posible crear el archivo log.\n");
		exit(1);
	}

	log_info(logger, "Logger creado para cliente.\n"); //Registro creación de logger
	return logger;
}

t_config* creacion_de_config_restaurante(char* archivo_config) {
	t_config* config = leer_config(archivo_config);
	return config;
}

void* iniciar_servicio_resto() {

	iniciar_servidor(atender_request_restaurante, config);
	return NULL;
}

void pedirMetadata(char* nombre_resto, int conexion_sindicato) {

	//Pedir Datos Restaurante a Sindicato
	t_paquete* paquete_sindicato = crear_paquete_personalizado(
			OBTENER_RESTAURANTE);
	t_buffer* buffer_sindicato;

	log_info(logger, "restaurante: %s", nombre_resto);
	buffer_sindicato = serializar_nombre(nombre_resto);

	paquete_sindicato->buffer = buffer_sindicato;
	enviar_paquete(paquete_sindicato, conexion_sindicato);

	//recibo datos del sindicato
	op_code codigo_operacion = recibir_operacion(conexion_sindicato);
	if (codigo_operacion == OK) {

		t_buffer* buffer = recibir_buffer(conexion_sindicato);
		t_list* lista = deserializar_lista_nombres(buffer);

		loggear_lista_nombres(lista, logger);

		//cantidad cocineros
		metadata->cantidad_cocineros = atoi(list_get(lista, 0));

		//Posicion
		char* posicion = list_get(lista, 1);
		char** posicion_array = string_get_string_as_array(posicion);
		metadata->pos_x = atoi(posicion_array[0]);
		metadata->pos_y = atoi(posicion_array[1]);

		//afinidades
		char* afinidades = list_get(lista, 2);
		char** lista_afinidades = string_get_string_as_array(afinidades);
		copiar_array_a_lista(metadata->afinidades, lista_afinidades);

		//platos
		char* platos = list_get(lista, 3);
		char** lista_platos = string_get_string_as_array(platos);
		copiar_array_a_lista(metadata->platos, lista_platos);

		//Precios
		char* precios = list_get(lista, 4);
		char** lista_precios = string_get_string_as_array(precios);
		copiar_array_a_lista(metadata->precios, lista_precios);

		//cantidad hornos
		metadata->cantidad_hornos = atoi(list_get(lista, 5));

		log_info(logger, "Recibi la metadata correctamente");
	} else {
		log_info(logger, "No recibi la metadata correctamente");
	}
	//eliminar_paquete(paquete_sindicato);

}
/*
 void pedir_recetas(int conexion_sindicato) {
 //Pedir Datos Restaurante a Sindicato
 t_paquete* paquete_sindicato = crear_paquete_personalizado(OBTENER_RECETA);
 t_buffer* buffer_sindicato;
 buffer_sindicato = serializar_lista_nombres(metadata->platos);

 paquete_sindicato->buffer = buffer_sindicato;
 enviar_paquete(paquete_sindicato, conexion_sindicato);

 //recibo datos del sindicato
 op_code codigo_operacion = recibir_operacion(conexion_sindicato);
 if (codigo_operacion == OK) {

 t_buffer* buffer = recibir_buffer(conexion_sindicato);
 t_list* lista = deserializar_lista_nombres(buffer);

 loggear_lista_nombres(lista, logger);

 //copio las recetas
 list_add_all(recetas,lista);
 log_info(logger, "Recibi las recetas correctamente");

 } else {
 log_info(logger, "No recibi las recetas correctamente");
 }
 //eliminar_paquete(paquete_sindicato);

 }
 */
t_plato* desempaquetarPlato(t_list* lista) {
	t_plato * nuevo_plato;
	nuevo_plato = list_get(lista, 0); //el id esta dentro del plato
	return nuevo_plato;
}

int generarIdPedido() {
	int id_generado;
	sem_wait(&mutexIdPedido);
	aux_id_pedido++;
	id_generado = aux_id_pedido;
	sem_post(&mutexIdPedido);
	return id_generado;
}

int obtenerIdPedido(t_list* lista) {
	return list_get(lista, 0);
}

void terminar_resto() {
	if (logger != NULL) {
		log_destroy(logger);
	}
	if (config != NULL) {
		config_destroy(config);
	}

	list_destroy_and_destroy_elements(recetas, (void*) destruir_receta);

	list_destroy_and_destroy_elements(pedidos, (void*) destruir_pedido);

	destruir_metadata();

	//destruyo colas de planificacion
	if(!list_is_empty(colas_ready)){
		list_destroy_and_destroy_elements(colas_ready, (void*) destruir_cola_ready);
	}
	if(!list_is_empty(cola_new)){
			list_destroy_and_destroy_elements(cola_new, (void*) destruir_plato);
		}
	if(!list_is_empty(cola_exec)){
				list_destroy_and_destroy_elements(cola_exec, (void*) destruir_plato);
			}
	if(!list_is_empty(cola_exit)){
				list_destroy_and_destroy_elements(cola_exit, (void*) destruir_plato);
			}
	if(!list_is_empty(cola_block)){
				list_destroy_and_destroy_elements(cola_block, (void*) destruir_plato);
			}

}

void destruir_receta(t_receta* receta) {
	if(receta->nombre_plato != NULL){
	free(receta->nombre_plato);
	}
	if(!list_is_empty(receta->pasos)){
	list_destroy_and_destroy_elements(receta->pasos, (void*) free);
	}
	if(!list_is_empty(receta->ciclos)){
	list_destroy_and_destroy_elements(receta->ciclos, (void*) free);
	}
	free(receta);
}

void destruir_plato(t_plato* plato) {
	if(plato->afinidad!=NULL){
	free(plato->afinidad);
	}
	if(!list_is_empty(plato->pasos)){
	list_destroy_and_destroy_elements(plato->pasos, (void*) free);
	}
	if(!list_is_empty(plato->ciclos_pendientes)){
	list_destroy_and_destroy_elements(plato->ciclos_pendientes, (void*) free);
	}
	free(plato);
}

void destruir_pedido(t_pedido* pedido) {
	if(!list_is_empty(pedido->platos)){
	list_destroy_and_destroy_elements(pedido->platos, (void*) destruir_plato);
	}
	free(pedido);
}

void destruir_metadata(){
	if(!list_is_empty(metadata->afinidades)){
		list_destroy_and_destroy_elements(metadata->afinidades, (void*) free);
	}
	if(!list_is_empty(metadata->platos)){
			list_destroy_and_destroy_elements(metadata->platos, (void*) free);
		}
	if(!list_is_empty(metadata->afinidades)){
			list_destroy_and_destroy_elements(metadata->precios, (void*) free);
		}
	free(metadata);

}

void destruir_cola_ready(cola_ready* cola){
	if(cola->afinidad!=NULL){
		free(cola->afinidad);
	}
	if(!list_is_empty(cola->platos)){
		list_destroy_and_destroy_elements(cola->platos, (void*) destruir_plato);
	}
	free(cola);
}

void conexionApp() {
	log_info(logger, "Inicio la conexion con APP");

	t_paquete* paquete = crear_paquete_personalizado(SUSCRIPCION);
	t_list* lista_app = list_create();
	t_buffer* buffer_app;

	list_add(lista_app, nombre_resto);
	char* pos_x_nom = string_itoa(metadata->pos_x);
	list_add(lista_app, pos_x_nom);
	char* pos_y_nom = string_itoa(metadata->pos_y);
	list_add(lista_app, pos_y_nom);

	list_add(lista_app, puerto_escucha);
	list_add(lista_app, ip_restaurante);

	//log_info(logger, "nombre: %s \t pos_x: %s \t pos_y: %s \n", nombre_resto, pos_x_nom, pos_y_nom);
	//log_info(logger, "n: %s x: %s y: %s \n", (char*)list_get(lista_app,0), (char*)list_get(lista_app,1), (char*)list_get(lista_app,2));

	fd_app = crear_conexion(ip_app, puerto_app);

	log_info(logger, "Socket de conexion: %d, puerto: %s ip: %s", fd_app,
			puerto_escucha, ip_restaurante);
	buffer_app = serializar_lista_nombres(lista_app);
	paquete->buffer = buffer_app;

	enviar_paquete(paquete, fd_app);

}

bool sePuedePrepararPlato(char* plato) {
	bool existe_plato = false;
	for (int i = 0; i < list_size(metadata->platos); i++) {
		char* plato_actual = list_get(metadata->platos, i);
		if (strcmp(plato, plato_actual) == 0) {
			existe_plato = true;
		}
	}
	return existe_plato;
}

bool existeReceta(char* plato) {
	bool existe_receta = false;
	for (int i = 0; i < list_size(recetas); i++) {
		t_receta* receta_actual = list_get(recetas, i);
		char* plato_actual = receta_actual->nombre_plato;
		if (strcmp(plato, plato_actual) == 0) {
			existe_receta = true;
		}
	}
	return existe_receta;
}
