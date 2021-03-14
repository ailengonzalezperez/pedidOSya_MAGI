/*
 * funciones_sindicato.c
 *
 *  Created on: 30 nov. 2020
 *      Author: Gonzalo G.
 */
#include<stdio.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include<pthread.h>
#include<semaphore.h>
#include "sindicato.h"
#include "bitmap.h"
#include "funciones_sindicato.h"


t_log* creacion_de_logger_sindicato(t_config* config) {
	t_log* logger;
	char* nombre_archivo = config_get_string_value(config, "ARCHIVO_LOG");

	logger = iniciar_logger(nombre_archivo, "SINDICATO");

	if (logger == NULL) {
		printf("No fue posible crear el archivo log.\n");
		exit(1);
	}

	log_info(logger, "Logger creado para sindicato.\n"); //Registro creación de logger
	return logger;
}

void crearReceta(char* nombre,char* pasos, char* tiempo_pasos){

	char* ruta_receta = string_new();
	char* directorio = config_s->directorio_recetas;
	string_append(&ruta_receta,directorio);
	string_append(&ruta_receta,"/");
	string_append(&ruta_receta,nombre);
	string_append(&ruta_receta,".AFIP");

	char* datosReceta = generarStringDatosReceta(pasos,tiempo_pasos);
	int tamanioDatos = string_length(datosReceta);

	int bloquesNecesarios = tamanioDatos / espacioDisponibleBloque + 1;

	t_list* bloquesAsignados = pedirBloquesNecesarios(bloquesNecesarios);

	if (list_is_empty(bloquesAsignados)){
		log_info(logger, "No se puede crear la receta porque no hay suficientes bloques disponibles.\n");
	} else{ //Si está bien y tiene suficientes bloques

		FILE * archivoReceta = fopen(ruta_receta,"w");
		int bloqueInicial = list_get(bloquesAsignados,0);
		guardarMetadata(archivoReceta,tamanioDatos,bloqueInicial);
		log_info(logger, "Se creó %s exitosamente. Tamaño = %d / Bloque inicial = %d.",ruta_receta,tamanioDatos,bloqueInicial);
		fclose(archivoReceta);

		log_info(logger, "Comienza escritura de información en los %d bloques asignados.\n",bloquesNecesarios);
		escribirEnMemoria(bloquesAsignados,datosReceta);
		log_info(logger, "Finaliza de forma exitosa la escritura de los bloques.\n");
	}
}


void crearRestaurante(char* nombre, char* cant_cocineros, char* posicion, char* afinidad_cocineros, char* platos, char* precio_platos, char* cant_hornos){

	char* ruta_restaurante = string_new();
	char* directorio = config_s->directorio_restaurante;
	string_append(&ruta_restaurante,directorio);
	string_append(&ruta_restaurante,"/");
	string_append(&ruta_restaurante,nombre);



	char* datosRestaurante = generarStringDatosRestaurante(cant_cocineros,posicion,afinidad_cocineros,platos,precio_platos,cant_hornos);
	int tamanioDatos = string_length(datosRestaurante);

	int bloquesNecesarios = tamanioDatos / espacioDisponibleBloque + 1;

	t_list* bloquesAsignados = pedirBloquesNecesarios(bloquesNecesarios);

	if (list_is_empty(bloquesAsignados)){
		log_info(logger, "No se puede crear el restaurante porque no hay suficientes bloques disponibles.\n");
	} else{ //Si está bien y tiene suficientes bloques

		mkdir(ruta_restaurante,0777);
		string_append(&ruta_restaurante,"/Info.AFIP");

		FILE * archivoRestaurante = fopen(ruta_restaurante,"w");
		int bloqueInicial = list_get(bloquesAsignados,0);

		guardarMetadata(archivoRestaurante,tamanioDatos,bloqueInicial);
		log_info(logger, "Se creo la metadata del restaurante exitosamente con tamaño %d y bloque inicial%d.",tamanioDatos,bloqueInicial);
		fclose(archivoRestaurante);

		log_info(logger, "Comienza escritura de información en los %d bloques asignados.\n",bloquesNecesarios);
		escribirEnMemoria(bloquesAsignados,datosRestaurante);
		log_info(logger, "Finaliza de forma exitosa la escritura de los bloques.\n");
	}
}

void guardarMetadata(FILE* archivo,int tamanioDatos,int bloqueInicial){
	char* metadata = string_new();
	string_append(&metadata,"SIZE=");
	string_append(&metadata,string_itoa(tamanioDatos));
	string_append(&metadata,"\n");
	string_append(&metadata,"INITIAL_BLOCK=");
	string_append(&metadata, string_itoa(bloqueInicial));
	fputs(metadata,archivo);
	log_info(logger,"Se guardó el size e initial block");
}

char* generarStringDatosRestaurante(char* cant_cocineros,char* posicion,char* afinidad_cocineros,char* platos,char* precio_platos,char* cant_hornos){
	char* datosRestaurante = string_new();
	string_append(&datosRestaurante,"CANTIDAD_COCINEROS=");
	string_append(&datosRestaurante,cant_cocineros);
	string_append(&datosRestaurante,"\nPOSICION=");
	string_append(&datosRestaurante,posicion);
	string_append(&datosRestaurante,"\nAFINIDAD_COCINEROS=");
	string_append(&datosRestaurante,afinidad_cocineros);
	string_append(&datosRestaurante,"\nPLATOS=");
	string_append(&datosRestaurante,platos);
	string_append(&datosRestaurante,"\nPRECIO_PLATOS=");
	string_append(&datosRestaurante,precio_platos);
	string_append(&datosRestaurante,"\nCANTIDAD_HORNOS=");
	string_append(&datosRestaurante,cant_hornos);
	string_append(&datosRestaurante,"\nCANTIDAD_PEDIDOS=0000\n");
	return datosRestaurante;
}

char* generarStringDatosReceta(char* pasos, char* tiempo_pasos){
	char* datosReceta = string_new();
	string_append(&datosReceta,"PASOS=");
	string_append(&datosReceta,pasos);
	string_append(&datosReceta,"\nTIEMPO_PASOS=");
	string_append(&datosReceta,tiempo_pasos);
	return datosReceta;
}

void escribirEnMemoria(t_list* bloquesAEscribir, char* datosAEscribir){

	int bloqueActual = list_remove(bloquesAEscribir,0);
	char* rutaBloque = generarRutaBloque(bloqueActual);
	FILE* archivoBloque = fopen(rutaBloque,"w");
	fwrite(datosAEscribir,espacioDisponibleBloque,1,archivoBloque); //Escribo el bloque inicial
	datosAEscribir = datosAEscribir + espacioDisponibleBloque;

	while(list_size(bloquesAEscribir)!=0){
		//Obtengo el siguiente
		uint32_t nuevoActual = (uint32_t) list_remove(bloquesAEscribir,0);

		//Escribo el puntero al siguiente al final del bloque y lo cierro
		fwrite(string_itoa(nuevoActual),4,1,archivoBloque); //REVISAR!!! no sabemos si funciona :D
		log_info(logger, "Finaliza escritura de %s.\n",rutaBloque);
		fclose(archivoBloque);

		//Abrir el proximo bloque a escribir
		rutaBloque = generarRutaBloque(nuevoActual);
		archivoBloque = fopen(rutaBloque,"w");

		fwrite(datosAEscribir,espacioDisponibleBloque,1,archivoBloque);
		datosAEscribir = datosAEscribir + espacioDisponibleBloque;
	}
	fclose(archivoBloque);
}

char* generarRutaBloque(int bloqueActual){
	//char* directorio = strdup(config_s->directorio_bloques);
	char* directorio = config_get_string_value(config,"DIRECTORIO_BLOQUES");
	char* directorioBloque = string_new();
	string_append(&directorioBloque,directorio);
	string_append(&directorioBloque,"/");
	char* numero = string_itoa(bloqueActual);
	string_append(&directorioBloque,numero);
	string_append(&directorioBloque,".AFIP");
	return directorioBloque;
}

void iniciar_servicio_sindicato(t_config* config) {

	iniciar_servidor(atender_request_sindicato, config);

}

void terminar_sindicato() {
	if (logger != NULL) {
		log_destroy(logger);
	}
	if (config != NULL) {
		config_destroy(config);
	}
}

void generarBloques(){
	char* directorio = config_get_string_value(config,"DIRECTORIO_BLOQUES");
	int i;
	log_info(logger, "El directorio de bloques es %s",directorio);
	log_info(logger, "Comienza la creación de los %d bloques",cantBloques);
	for (i=0; i<cantBloques;i++){
		char* directorioActual = string_new();
		string_append(&directorioActual,directorio);
		string_append(&directorioActual,"/");
		string_append(&directorioActual,string_itoa(i));
		string_append(&directorioActual,".AFIP");
		if (!existeArchivo(directorioActual)){
			FILE* block = fopen(directorioActual, "w");
			fclose(block);
		}
	}
	log_info(logger, "Finaliza la creación de los %d bloques",i);
}

t_list* pedirBloquesNecesarios(int bloques){
	t_list* bloquesAsignados = list_create();
	for (int i = 1; i <= bloques; i++){
		int bloqueAsignado = pedirBloqueLibre();
		if (bloqueAsignado == -1){
			anularAsignacionBloques(bloquesAsignados);
			clearBit(miBitmap,bloqueAsignado);
			list_clean(bloquesAsignados);
			return bloquesAsignados;
		} else{
			list_add(bloquesAsignados,bloqueAsignado);
		}
	}
	return bloquesAsignados;
}

void anularAsignacionBloques(t_list* bloquesAsignados){
	for (int i = 0; i < list_size(bloquesAsignados); i++){
		int bloqueActual = list_get(bloquesAsignados,i);
		clearBit(miBitmap, bloqueActual);
	}
}

int pedirBloqueLibre(){
	//Me traigo el archivo config para traerme la ruta dle Bitmap
	bool encontreEspacio = false;
	int numeroBloque = 0;
	//Comienzo a buscar un espacio libre
	while (!encontreEspacio && numeroBloque <= cantBloques){
		if (checkBit(miBitmap,numeroBloque)!=0){
			numeroBloque += 1;
		}else{
			setBit(miBitmap,numeroBloque);
			encontreEspacio= true;
			return numeroBloque;
		}
	}
	return -1;
}

/*Funciones simil config para leer metadata*/

t_metadataSindicato* metadata_sindicato_create(char *path) {
	FILE* file = fopen(path, "r");

	if (file == NULL) {
		return NULL;
	}

	struct stat stat_file;
	stat(path, &stat_file);

	t_metadataSindicato* config = malloc(sizeof(t_metadataSindicato));

	config->path = strdup(path);
	config->properties = dictionary_create();

	char* buffer = calloc(1, stat_file.st_size + 1);
	fread(buffer, stat_file.st_size, 1, file);

	if (strstr(buffer, "\r\n")) {
		printf("\n\nconfig_create - WARNING: the file %s contains a \\r\\n sequence "
		 "- the Windows new line sequence. The \\r characters will remain as part "
		 "of the value, as Unix newlines consist of a single \\n. You can install "
		 "and use `dos2unix` program to convert your files to Unix style.\n\n", path);
	}

	char** lines = string_split(buffer, "\n");

	void add_cofiguration(char *line) {
		if (!string_starts_with(line, "#")) {
			char** keyAndValue = string_n_split(line, 2, "=");
			dictionary_put(config->properties, keyAndValue[0], keyAndValue[1]);
			free(keyAndValue[0]);
			free(keyAndValue);
		}
	}
	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);

	free(lines);
	free(buffer);
	fclose(file);

	return config;
}

char* get_string_metadataSindicato(t_metadataSindicato* self, char *key) {
	char* respuesta = dictionary_get(self->properties, key);
	return respuesta;
}

int get_int_metadataSindicato(t_metadataSindicato *self, char *key) {
	char* value = get_string_metadataSindicato(self, key);
	int valueFinal = atoi(value);
	return valueFinal;
}

void set_value_metadataSindicato(t_metadataSindicato *self, char *key, char *value) {
	t_dictionary* dictionary = self->properties;

	if(dictionary_has_key(dictionary, key)) {
		dictionary_remove_and_destroy(dictionary, key, (void*) free);
	}

    char duplicate_value = string_duplicate(value);

    dictionary_put(self->properties, key, (void*)duplicate_value);

    save_metadataSindicato(self);
}

int save_metadataSindicato(t_metadataSindicato *self) {
    return save_in_file_metadataSindicato(self, self->path);
}

int save_in_file_metadataSindicato(t_metadataSindicato* self, char* path) {
    FILE* file = fopen(path, "w");

    if (file == NULL) {
            return -1;
    }

    char* lines = string_new();
    void add_line(char* key, void* value) {
        string_append_with_format(&lines, "%s=%s\n", key, value);
    }

    dictionary_iterator(self->properties, add_line); //<- Acá falla
    int result = fwrite(lines, strlen(lines), 1, file);
    fclose(file);
    free(lines);
    return result;
}


t_metadataSindicato* leerDesdeBloques(int bloqueInicial,int tamanioDatos){

	int bloqueActual = bloqueInicial;
	int bloquesALeer = (int) (tamanioDatos / espacioDisponibleBloque +1);

	char* temp = string_new();
	char* temporal = string_new();
	char* rutaBloqueActual;
	FILE* f_bloque;
	t_list* bloquesUsados = list_create();

	for (int i=0; i<bloquesALeer-1; i++){
		rutaBloqueActual = generarRutaBloque(bloqueActual);
		f_bloque = fopen(rutaBloqueActual,"r");

		fread(temp, espacioDisponibleBloque, 1, f_bloque);
		temp[espacioDisponibleBloque]='\0';
		string_append(&temporal, temp);

		fseek(f_bloque, espacioDisponibleBloque, SEEK_SET);
		fread(temp, 4, 1, f_bloque);
		bloqueActual = atoi(temp);
		list_add(bloquesUsados, bloqueActual);
		fclose(f_bloque);
	}

	rutaBloqueActual = generarRutaBloque(bloqueActual);
	f_bloque = fopen(rutaBloqueActual,"r");
	int tamFile = tamanioFile(f_bloque);
	fread(temp, tamFile, 1, f_bloque);
	string_append(&temporal, temp);

	FILE* archivoTemporal = fopen("temporal.AFIP","w");
	fwrite(temporal,string_length(temporal),1,archivoTemporal);
	fclose(archivoTemporal);
	t_metadataSindicato* metadata = metadata_sindicato_create("temporal.AFIP");
	return metadata;
}

//ADMINISTRACION DE MENSAJES

t_paquete* consultarPlatos(char* nombreRestaurante){
	t_paquete* respuesta = crear_paquete_personalizado(OK);
	if(existeRestaurante(nombreRestaurante)){
		char * platos;
		char * pathMetadata = metadataRestaurante(nombreRestaurante);
		t_metadataSindicato* metadataRestaurante = metadata_sindicato_create(pathMetadata);
		int bloqueInicial = get_int_metadataSindicato(metadataRestaurante, "INITIAL_BLOCK");
		int tamanio = get_int_metadataSindicato(metadataRestaurante, "SIZE");
		sem_wait(&s_temporalFile);
		t_metadataSindicato* info = leerDesdeBloques(bloqueInicial, tamanio);
		char* platoss = get_string_metadataSindicato(info, "PLATOS");
		platos = strdup(platoss);
		log_info(logger,"los platos son %s",platos);
		sem_post(&s_temporalFile);
		t_list* lista = list_create();
		list_add(lista,platos);
		t_buffer* buffer = serializar_lista_nombres(lista);
		respuesta->buffer = buffer;
		metadataSindicato_destroy(info);
		//metadataSindicato_destroy(metadataRestaurante);
	}else{
		log_info(logger, "El restaurante %s no existe en el FS", nombreRestaurante);
		respuesta->codigo_operacion = FAIL;
	}

	return respuesta;
}

bool guardarPedido(char* nombreRestaurante, int idPedido){
	bool resultado;
	char* id = string_itoa(idPedido);
	if(existeRestaurante(nombreRestaurante)){
		if(existePedido(nombreRestaurante, id)){
			log_info(logger, "El pedido %d existe en el restaurante %s", idPedido,nombreRestaurante);
			resultado = false;
		}else{
			crearArchivoPedido(nombreRestaurante, id);
			//Comento esto para poder testear todo.
			//sumarPedidoARestaurante(nombreRestaurante);
			resultado = true;
		}
	}else{
		log_info(logger, "El restaurante %s no existe en el FS", nombreRestaurante);
		resultado = false;
	}

	return resultado;
}

bool guardarPlato(char* nombreRestaurante, int idPedido, char* plato, int cantidadPlato){
	bool resultado;
		if(existeRestaurante(nombreRestaurante)){
			if(existePedido(nombreRestaurante, idPedido)){
				agregarPlatoAPedido(nombreRestaurante, idPedido, plato, cantidadPlato);
				resultado = true;
			}else{
				log_info(logger, "El pedido %d no existe en el restaurante %s", idPedido,nombreRestaurante);
				resultado = false;
			}
		}else{
			log_info(logger, "El restaurante %s no existe en el FS", nombreRestaurante);
			resultado = false;
		}
	return resultado;
}

bool confirmarPedido(char* nombreRestaurante, int idPedido){
	bool resultado;
	char* idPedidoString = string_itoa(idPedido);
	if(existeRestaurante(nombreRestaurante)){
		if(existePedido(nombreRestaurante, idPedidoString)){
			if(estaEnEstado(nombreRestaurante, idPedido, "Pendiente")){
				cambiarEstadoA(nombreRestaurante, idPedido, "Confirmado");
				resultado = true;
			}else{
				log_info(logger, "El pedido %d no esta pendiente en el restaurante %s", idPedido,nombreRestaurante);
				resultado = false;
			}
		}else{
			log_info(logger, "El pedido %d no existe en el restaurante %s", idPedido,nombreRestaurante);
			resultado = false;
		}
	}else{
		log_info(logger, "El restaurante %s no existe en el FS", nombreRestaurante);
		resultado = false;
	}
	return resultado;
}

t_paquete* obtenerPedido(char* nombreRestaurante, int idPedido){
	t_paquete* respuesta = crear_paquete_personalizado(OK);
	char* idPedidoString = string_itoa(idPedido);
	if(existeRestaurante(nombreRestaurante)){
		if(existePedido(nombreRestaurante, idPedidoString)){
			char * pathPedidoDeResto = rutaPedido(nombreRestaurante, idPedidoString);
			t_metadataSindicato* metadataRestaurante = metadata_sindicato_create(pathPedidoDeResto);
			int bloqueInicial = get_int_metadataSindicato(metadataRestaurante, "INITIAL_BLOCK");
			int tamanio = get_int_metadataSindicato(metadataRestaurante, "SIZE");
			sem_wait(&s_temporalFile);
			t_metadataSindicato* info = leerDesdeBloques(bloqueInicial, tamanio);
			char* estado_pedidoo = get_string_metadataSindicato(info, "ESTADO_PEDIDO");
			char* lista_platoss = get_string_metadataSindicato(info, "LISTA_PLATOS");
			char* cantidad_platoss = get_string_metadataSindicato(info, "CANTIDAD_PLATOS");
			char* cantidad_listaa = get_string_metadataSindicato(info, "CANTIDAD_LISTA");
			char* precio_totall = get_string_metadataSindicato(info, "PRECIO_TOTAL");
			char* estado_pedido = strdup(estado_pedidoo);
			char* lista_platos = strdup(lista_platoss);
			char* cantidad_platos = strdup(cantidad_platoss);
			char* cantidad_lista = strdup(cantidad_listaa);
			char* precio_total = strdup(precio_totall);
			sem_post(&s_temporalFile);
			t_list* lista = list_create();
			list_add(lista,estado_pedido);
			list_add(lista,lista_platos);
			list_add(lista,cantidad_platos);
			list_add(lista,cantidad_lista);
			list_add(lista,precio_total);
			t_buffer* buffer = serializar_lista_nombres(lista);
			respuesta->buffer = buffer;
			metadataSindicato_destroy(info);
			metadataSindicato_destroy(metadataRestaurante);
		}else{
			log_info(logger, "El pedido %d no existe en el restaurante %s", idPedido,nombreRestaurante);
			respuesta->codigo_operacion = FAIL;
		}

	}else{
		log_info(logger, "El restaurante %s no existe en el FS", nombreRestaurante);
		respuesta->codigo_operacion = FAIL;
	}

	return respuesta;
}


t_paquete* obtenerRestaurante(char* nombreRestaurante){
	t_paquete* respuesta = crear_paquete_personalizado(OK);
	if(existeRestaurante(nombreRestaurante)){
		char * pathMetadataResto = rutaRestaurante(nombreRestaurante);
		t_metadataSindicato* metadataRestaurante = metadata_sindicato_create(pathMetadataResto);
		int bloqueInicial = get_int_metadataSindicato(metadataRestaurante, "INITIAL_BLOCK");
		int tamanio = get_int_metadataSindicato(metadataRestaurante, "SIZE");
		sem_wait(&s_temporalFile);
		t_metadataSindicato* info = leerDesdeBloques(bloqueInicial, tamanio);

		char* cantidad_cocineross = get_string_metadataSindicato(info, "CANTIDAD_COCINEROS");
		char* posicionn = get_string_metadataSindicato(info, "POSICION");
		char* afinidad_cocineross = get_string_metadataSindicato(info, "AFINIDAD_COCINEROS");
		char* platoss = get_string_metadataSindicato(info, "LISTA_PLATOS");
		char* precio_platoss = get_string_metadataSindicato(info, "PRECIO_PLATOS");
		char* cantidad_hornoss = get_string_metadataSindicato(info, "CANTIDAD_HORNOS");
		char* cantidad_pedidoss = get_string_metadataSindicato(info, "CANTIDAD_PEDIDOS");

		char* cantidad_cocineros = strdup(cantidad_cocineross);
		char* posicion = strdup(posicionn);
		char* afinidad_cocineros = strdup(afinidad_cocineross);
		char* platos = strdup(platoss);
		char* precio_platos = strdup(precio_platoss);
		char* cantidad_hornos = strdup(cantidad_hornoss);
		char* cantidad_pedidos = strdup(cantidad_pedidoss);

		sem_post(&s_temporalFile);
		t_list* lista = list_create();
		list_add(lista,cantidad_cocineros);
		list_add(lista,posicion);
		list_add(lista,afinidad_cocineros);
		list_add(lista,platos);
		list_add(lista,precio_platos);
		list_add(lista,cantidad_hornos);
		list_add(lista,cantidad_pedidos);

		t_buffer* buffer = serializar_lista_nombres(lista);
		respuesta->buffer = buffer;
		metadataSindicato_destroy(info);
		metadataSindicato_destroy(metadataRestaurante);
	}else{
		log_info(logger, "El restaurante %s no existe en el FS", nombreRestaurante);
		respuesta->codigo_operacion = FAIL;
	}

	return respuesta;
}

bool platoListo(char* nombreRestaurante, int idPedido, char* platoListo){
	bool resultado;
	if(existeRestaurante(nombreRestaurante)){
		if(existePedido(nombreRestaurante, idPedido)){
			if(estaEnEstado(nombreRestaurante, idPedido, "Confirmado")){
				agregarPlatoListo(nombreRestaurante, idPedido, platoListo);
				resultado = true;
			}
			log_info(logger, "El pedido %d no existe en el restaurante %s", idPedido,nombreRestaurante);
			resultado = false;
		}else{
			log_info(logger, "El pedido %d no existe en el restaurante %s", idPedido,nombreRestaurante);
			resultado = false;
		}
	}else{
		log_info(logger, "El restaurante %s no existe en el FS", nombreRestaurante);
		resultado = false;
	}
	return resultado;
}


t_paquete* obtenerReceta(char* nombrePlato){
	t_paquete* respuesta = crear_paquete_personalizado(OK);
	if(existeReceta(nombrePlato)){
		char * pathMetadataReceta = rutaReceta(nombrePlato);
		t_metadataSindicato* metadataReceta = metadata_sindicato_create(pathMetadataReceta);
		int bloqueInicial = get_int_metadataSindicato(metadataReceta, "INITIAL_BLOCK");
		int tamanio = get_int_metadataSindicato(metadataReceta, "SIZE");
		sem_wait(&s_temporalFile);
		t_metadataSindicato* info = leerDesdeBloques(bloqueInicial, tamanio);

		char* pasoss = get_string_metadataSindicato(info, "PASOS");
		char* tiempo_pasoss = get_string_metadataSindicato(info, "TIEMPO_PASOS");

		char* pasos = strdup(pasoss);
		char* tiempo_pasos = strdup(tiempo_pasoss);

		sem_post(&s_temporalFile);
		t_list* lista = list_create();
		list_add(lista,pasos);
		list_add(lista,tiempo_pasos);

		t_buffer* buffer = serializar_lista_nombres(lista);
		respuesta->buffer = buffer;
		metadataSindicato_destroy(info);
		metadataSindicato_destroy(metadataRestaurante);
	}else{
		log_info(logger, "La receta %s no existe en el FS", nombrePlato);
		respuesta->codigo_operacion = FAIL;
	}

	return respuesta;
}

bool terminarPedido(char* nombreRestaurante, int idPedido){
	bool resultado;
	if(existeRestaurante(nombreRestaurante)){
		if(existePedido(nombreRestaurante, idPedido)){
			if(estaEnEstado(nombreRestaurante, idPedido, "Confirmado")){
				cambiarEstadoA(nombreRestaurante, idPedido, "Terminado");
				resultado = true;
			}else{
				log_info(logger, "El pedido %d no esta CONFIRMADO en el restaurante %s", idPedido,nombreRestaurante);
				resultado = false;
			}
		}else{
			log_info(logger, "El pedido %d no existe en el restaurante %s", idPedido,nombreRestaurante);
			resultado = false;
		}
	}else{
		log_info(logger, "El restaurante %s no existe en el FS", nombreRestaurante);
		resultado = false;
	}
	return resultado;
}


//Auxiliares

bool existeReceta(char* nombrePlato){
	char* directorioRecetas = config_s->directorio_recetas;
	char* pathReceta = string_new();
	string_append(&pathReceta, directorioRecetas);
	string_append(&pathReceta, "/");
	string_append(&pathReceta, nombrePlato);
	string_append(&pathReceta, ".AFIP");
	return existeArchivo(pathReceta);
}

char* rutaReceta(char* nombrePlato){
	char* directorioRecetas = config_s->directorio_recetas;
	char* pathReceta = string_new();
	string_append(&pathReceta, directorioRecetas);
	string_append(&pathReceta, "/");
	string_append(&pathReceta, nombrePlato);
	string_append(&pathReceta, ".AFIP");
	return pathReceta;
}


bool existeRestaurante(char* nombreRestaurante){
	char* directorioRestos = config_s->directorio_restaurante;
	char* pathResto = string_new();
	string_append(&pathResto, directorioRestos);
	string_append(&pathResto, "/");
	string_append(&pathResto, nombreRestaurante);
	string_append(&pathResto, "/Info.AFIP");
	return existeArchivo(pathResto);
}


bool existePedido(char* nombreRestaurante, char* idPedido){
	char* directorioRestos = config_s->directorio_restaurante;
	char* pathPedido = string_new();
	string_append(&pathPedido, directorioRestos);
	string_append(&pathPedido, "/");
	string_append(&pathPedido, nombreRestaurante);
	string_append(&pathPedido, "/Pedido");
	string_append(&pathPedido, idPedido);
	string_append(&pathPedido, ".AFIP");
	return existeArchivo(pathPedido);
}

char* rutaPedido(char* nombreRestaurante, char* idPedido){
	char* directorioRestos = config_s->directorio_restaurante;
	char* pathPedido = string_new();
	string_append(&pathPedido, directorioRestos);
	string_append(&pathPedido, "/");
	string_append(&pathPedido, nombreRestaurante);
	string_append(&pathPedido, "/Pedido");
	string_append(&pathPedido, idPedido);
	string_append(&pathPedido, ".AFIP");
	return pathPedido;
}

int tamanioFile(FILE* f){
	int posOriginal = ftell(f);
	fseek(f, 0 , SEEK_END);//Me muevo al final
	int fileSize = ftell(f); // me fijo donde está
	fseek(f, 0 , SEEK_SET); // vuelvo al inicio
	return fileSize;
}

char* rutaRestaurante(char* nombreRestaurante){
	char* directorioRestos = config_s->directorio_restaurante;
	char* pathResto = string_new();
	string_append(&pathResto, directorioRestos);
	string_append(&pathResto, "/");
	string_append(&pathResto, nombreRestaurante);
	return pathResto;
}

char* metadataRestaurante(char* nombreRestaurante){
	char* path = string_new();
	char* restaurante = rutaRestaurante(nombreRestaurante);
	string_append(&path, restaurante);
	string_append(&path, "/Info.AFIP");
	return path;
}

void crearArchivoPedido(char* nombreRestaurante, int idPedido){
	char* ruta_pedido = rutaPedido(nombreRestaurante, idPedido);
	char* datosPedido = "ESTADO_PEDIDO=Pendiente\nLISTA_PLATOS=[]\nCANTIDAD_PLATOS=[]\nCANTIDAD_LISTA=[]\nPRECIO_TOTAL=0";

	int tamanioDatos = string_length(datosPedido);

	int bloquesNecesarios = tamanioDatos / espacioDisponibleBloque + 1;

	t_list* bloquesAsignados = pedirBloquesNecesarios(bloquesNecesarios);

	if (list_is_empty(bloquesAsignados)){
		log_info(logger, "No se puede crear la receta porque no hay suficientes bloques disponibles.\n");
	} else{ //Si está bien y tiene suficientes bloques

		FILE * archivoPedido = fopen(ruta_pedido,"w");
		int bloqueInicial = list_get(bloquesAsignados,0);
		guardarMetadata(archivoPedido,tamanioDatos,bloqueInicial);
		log_info(logger, "Se creó %s exitosamente. Tamaño = %d / Bloque inicial = %d.",ruta_pedido,tamanioDatos,bloqueInicial);
		fclose(archivoPedido);

		log_info(logger, "Comienza escritura de información en los %d bloques asignados.\n",bloquesNecesarios);
		escribirEnMemoria(bloquesAsignados,datosPedido);
		log_info(logger, "Finaliza de forma exitosa la escritura de los bloques.\n");
	}
}

void cambiarEstadoA(char* nombreRestaurante, int idPedido, char* nuevoEstado){
	char* idPedido_char = string_itoa(idPedido);
	char* rutaDelPedido = rutaPedido(nombreRestaurante, idPedido_char);
	t_metadataSindicato* metadataPedido = metadata_sindicato_create(rutaDelPedido);
	int bloqueInicial = get_int_metadataSindicato(metadataPedido, "INITIAL_BLOCK");
	int tamanio = get_int_metadataSindicato(metadataPedido, "SIZE");
	sem_wait(&s_temporalFile);
	t_metadataSindicato* pedido = leerDesdeBloques(bloqueInicial, tamanio);

	t_list* bloquesUsados = obtenerBloquesUsados(bloqueInicial, tamanio);

	liberarBloques(bloquesUsados);

	set_value_metadataSindicato(pedido, "ESTADO_PEDIDO", nuevoEstado);

	char* platoActualizado;
	FILE* miFile = fopen(pedido->path, "r");
	int tamFile = tamanioFile(miFile);
	fread((void*)platoActualizado, (size_t) tamFile, 1, miFile);
	fclose(miFile);

	int bloquesNecesarios = tamFile / espacioDisponibleBloque + 1;

    t_list* bloquesAsignados = pedirBloquesNecesarios(bloquesNecesarios);
    bloqueInicial = list_get(bloquesAsignados,0);
    char* bloqueInicial_str = string_itoa(bloqueInicial);
    char* tam = string_itoa(tamFile);
    set_value_metadataSindicato(metadataPedido, "INITIAL_BLOCK", bloqueInicial_str);
    set_value_metadataSindicato(metadataPedido, "SIZE", tam);

	log_info(logger, "Comienza escritura de información en los %d bloques asignados.\n",bloquesNecesarios);
	escribirEnMemoria(bloquesAsignados,platoActualizado);
	log_info(logger, "Finaliza de forma exitosa la escritura de los bloques.\n");

	sem_post(&s_temporalFile);

	metadataSindicato_destroy(pedido);
	metadataSindicato_destroy(metadataPedido);
}

bool estaEnEstado(char* nombreRestaurante, int idPedido, char* estado){
	char* idPedido_char = string_itoa(idPedido);
	char* rutaDelPedido = rutaPedido(nombreRestaurante, idPedido_char);
	t_metadataSindicato* metadataPedido = metadata_sindicato_create(rutaDelPedido);
	int bloqueInicial = get_int_metadataSindicato(metadataPedido, "INITIAL_BLOCK");
	int tamanio = get_int_metadataSindicato(metadataPedido, "SIZE");
	sem_wait(&s_temporalFile);
	t_metadataSindicato* pedido = leerDesdeBloques(bloqueInicial, tamanio);

	char* estadoActual = get_string_metadataSindicato(pedido, "ESTADO_PEDIDO");
	bool resultado = (strcmp(estadoActual, estado)==0);
	sem_post(&s_temporalFile);

	return resultado;
}


void agregarPlatoAPedido(char* nombreRestaurante, int idPedido, char* plato, int cantidadPlato){
	char* idPedido_char = string_itoa(idPedido);
	char* rutaDelPedido = rutaPedido(nombreRestaurante, idPedido_char);
	t_metadataSindicato* metadataPedido = metadata_sindicato_create(rutaDelPedido);
	int bloqueInicial = get_int_metadataSindicato(metadataRestaurante, "INITIAL_BLOCK");
	int tamanio = get_int_metadataSindicato(metadataRestaurante, "SIZE");
	sem_wait(&s_temporalFile);
	t_metadataSindicato* pedido = leerDesdeBloques(bloqueInicial, tamanio);
	char* platos = get_string_metadataSindicato(pedido, "LISTA_PLATOS");
	char* cantidad_platos = get_string_metadataSindicato(pedido, "CANTIDAD_PLATOS");
	char* precio_total = get_string_metadataSindicato(pedido, "PRECIO_TOTAL");

	t_list* bloquesUsados = obtenerBloquesUsados(bloqueInicial, tamanio);

	liberarBloques(bloquesUsados);

	modificarPlatoYPrecio(platos, cantidad_platos, precio_total, plato, cantidadPlato, nombreRestaurante);

	set_value_metadataSindicato(pedido, "LISTA_PLATOS", platos);
	set_value_metadataSindicato(pedido, "CANTIDAD_PLATOS", cantidad_platos);
	set_value_metadataSindicato(pedido, "PRECIO_TOTAL", precio_total);

	char* platoActualizado;
	FILE* miFile = fopen(pedido->path, "r");
	int tamFile = tamanioFile(miFile);
	fread((void*)platoActualizado, (size_t) tamFile, 1, miFile);
	fclose(miFile);

	int bloquesNecesarios = tamFile / espacioDisponibleBloque + 1;

    t_list* bloquesAsignados = pedirBloquesNecesarios(bloquesNecesarios);
    bloqueInicial = list_get(bloquesAsignados,0);
    char* bloqueInicial_str = string_itoa(bloqueInicial);
    char* tam = string_itoa(tamFile);
    set_value_metadataSindicato(metadataPedido, "INITIAL_BLOCK", bloqueInicial_str);
    set_value_metadataSindicato(metadataPedido, "SIZE", tam);

	log_info(logger, "Comienza escritura de información en los %d bloques asignados.\n",bloquesNecesarios);
	escribirEnMemoria(bloquesAsignados,platoActualizado);
	log_info(logger, "Finaliza de forma exitosa la escritura de los bloques.\n");

	sem_post(&s_temporalFile);

	metadataSindicato_destroy(pedido);
	metadataSindicato_destroy(metadataPedido);
}

void modificarPlatoYPrecio(char* platos, char* cantidadPlatos, char* precioTotal, char* platoAModificar, int cantidadASumar, char* nombreRestaurante){
	int precio = atoi(precioTotal);
	t_list* listaPlatos = generarListaDesdeString(platos);
	t_list* listaCantidadPlatos = generarListaDesdeString(cantidadPlatos);
	int ubicacion = posicionPlatoDelPlatoAModificar(listaPlatos, platoAModificar);
	if(ubicacion == -1){
		list_add(listaPlatos, platoAModificar);
		list_add(listaCantidadPlatos, cantidadASumar);
	}else{
		int cantidadActual = list_get(listaCantidadPlatos, ubicacion);
		cantidadActual += cantidadASumar;
		list_add_in_index(listaCantidadPlatos, ubicacion, cantidadActual);
	}
	precio += precioReceta(ubicacion, nombreRestaurante) * cantidadASumar;

	precioTotal = string_itoa(precio);
	platos = listaAArray(listaPlatos);
	cantidadPlatos = listaAArray(listaCantidadPlatos);
}

int posicionPlatoDelPlatoAModificar(t_list* listaPlatos,char* platoAModificar){
	for(int i=0; i<list_size(listaPlatos); i++){
		char * actual = list_get(listaPlatos, i);
		if(strcmp(platoAModificar,actual)==0){
			return i;
		}
	}
	return -1;
}

int precioReceta(int ubicacionPlatoEnLista, char* nombreRestaurante){
	char* pathMetadataResto = metadataRestaurante(nombreRestaurante);
	t_metadataSindicato* datosRestaurante = metadata_sindicato_create(pathMetadataResto);
	int bloqueInicial = get_int_metadataSindicato(datosRestaurante, "INITIAL_BLOCK");
	int tamanio = get_int_metadataSindicato(datosRestaurante, "SIZE");
	t_metadataSindicato* info =leerDesdeBloques(bloqueInicial, tamanio);
	char* precios = get_string_metadataSindicato(info, "PRECIO_PLATOS");
	t_list* preciosLista = generarListaDesdeString(precios);
	metadataSindicato_destroy(info);
	//metadataSindicato_destroy(datosRestaurante);
	char* resultado = list_get(preciosLista, ubicacionPlatoEnLista);
	return atoi(resultado);
}

char* listaAArray(t_list* lista){
	char* resultado = string_new();
	string_append(&resultado, "[");
	int i=0;
	for(i=0; i<list_size(lista)-1; i++){
		char* add = list_get(lista, i);
		string_append(&resultado, add);
		string_append(&resultado, ",");
	}
	char* add = list_get(lista, i);
	string_append(&resultado, add);
	string_append(&resultado, "]");
	return resultado;
}

t_list* generarListaDesdeString(char* text) {
    char** array = string_get_string_as_array(text);
    t_list* lista = list_create();
    copiar_array_a_lista(lista, array);
    return lista;
}

void metadataSindicato_destroy(t_metadataSindicato *config) {
    dictionary_destroy_and_destroy_elements(config->properties, (void*)free);
    free(config->path);
    free(config);
}

t_list* obtenerBloquesUsados(int bloqueInicial, int tamanio){

	int bloqueActual = bloqueInicial;
	int bloquesALeer = (int) (tamanio / espacioDisponibleBloque +1);

	char* temp = string_new();
	char* temporal = string_new();
	char* rutaBloqueActual;
	FILE* f_bloque;
	t_list* bloquesUsados = list_create();

	for (int i=0; i<bloquesALeer-1; i++){
		rutaBloqueActual = generarRutaBloque(bloqueActual);
		f_bloque = fopen(rutaBloqueActual,"r");

		fread(temp, espacioDisponibleBloque, 1, f_bloque);
		temp[espacioDisponibleBloque]='\0';
		string_append(&temporal, temp);

		fseek(f_bloque, espacioDisponibleBloque, SEEK_SET);
		fread(temp, 4, 1, f_bloque);
		bloqueActual = atoi(temp);
		list_add(bloquesUsados, bloqueActual);
		fclose(f_bloque);
	}

	rutaBloqueActual = generarRutaBloque(bloqueActual);
	f_bloque = fopen(rutaBloqueActual,"r");
	int tamFile = tamanioFile(f_bloque);
	fread(temp, tamFile, 1, f_bloque);
	string_append(&temporal, temp);

	return bloquesUsados;
}

void liberarBloques(t_list* listaBloques){
	for(int i = 0; i<list_size(listaBloques); i++){
		int bloqueActual = list_get(listaBloques, i);
		clearBit(miBitmap, bloqueActual);
	}
}

void agregarPlatoListo(char* nombreRestaurante, int idPedido, char* plato){
	char* idPedido_char = string_itoa(idPedido);
	char* rutaDelPedido = rutaPedido(nombreRestaurante, idPedido_char);
	t_metadataSindicato* metadataPedido = metadata_sindicato_create(rutaDelPedido);
	int bloqueInicial = get_int_metadataSindicato(metadataRestaurante, "INITIAL_BLOCK");
	int tamanio = get_int_metadataSindicato(metadataRestaurante, "SIZE");
	sem_wait(&s_temporalFile);
	t_metadataSindicato* pedido = leerDesdeBloques(bloqueInicial, tamanio);
	char* platos = get_string_metadataSindicato(pedido, "LISTA_PLATOS");
	char* cantidad_lista = get_string_metadataSindicato(pedido, "CANTIDAD_LISTA");

	t_list* bloquesUsados = obtenerBloquesUsados(bloqueInicial, tamanio);

	liberarBloques(bloquesUsados);

	aumentarCantidadListaDePlato(platos,cantidad_lista, plato);

	set_value_metadataSindicato(pedido, "CANTIDAD_LISTA", cantidad_lista);

	char* platoActualizado;
	FILE* miFile = fopen(pedido->path, "r");
	int tamFile = tamanioFile(miFile);
	fread((void*)platoActualizado, (size_t) tamFile, 1, miFile);
	fclose(miFile);

	int bloquesNecesarios = tamFile / espacioDisponibleBloque + 1;

    t_list* bloquesAsignados = pedirBloquesNecesarios(bloquesNecesarios);
    bloqueInicial = list_get(bloquesAsignados,0);
    char* bloqueInicial_str = string_itoa(bloqueInicial);
    char* tam = string_itoa(tamFile);
    set_value_metadataSindicato(metadataPedido, "INITIAL_BLOCK", bloqueInicial_str);
    set_value_metadataSindicato(metadataPedido, "SIZE", tam);

	log_info(logger, "Comienza escritura de información en los %d bloques asignados.\n",bloquesNecesarios);
	escribirEnMemoria(bloquesAsignados,platoActualizado);
	log_info(logger, "Finaliza de forma exitosa la escritura de los bloques.\n");

	sem_post(&s_temporalFile);

	metadataSindicato_destroy(pedido);
	metadataSindicato_destroy(metadataPedido);
}

bool aumentarCantidadListaDePlato(char* platos, char* cantidad_lista, char* platoAModificar){
	bool respuesta = true;
	t_list* listaPlatos = generarListaDesdeString(platos);
	t_list* listaCantidadListaPlatos = generarListaDesdeString(cantidad_lista);
	int ubicacion = posicionPlatoDelPlatoAModificar(listaPlatos, platoAModificar);
	if(ubicacion == -1){
		respuesta = false;
	}else{
		int cantidadActual = list_get(listaCantidadListaPlatos, ubicacion);
		cantidadActual += 1;
		list_add_in_index(listaCantidadListaPlatos, ubicacion, cantidadActual);
	}

	cantidad_lista = listaAArray(listaCantidadListaPlatos);

	return respuesta;
}

void sumarPedidoARestaurante(char* nombreRestaurante){
	char* rutaMetadataResto = metadataRestaurante(nombreRestaurante);
	t_metadataSindicato* metadataRestaurante = metadata_sindicato_create(rutaMetadataResto);
	int bloqueInicial = get_int_metadataSindicato(metadataRestaurante, "INITIAL_BLOCK");
	int tamanio = get_int_metadataSindicato(metadataRestaurante, "SIZE");
	sem_wait(&s_temporalFile);
	t_metadataSindicato* restaurante = leerDesdeBloques(bloqueInicial, tamanio);
	int cantidadPedidos = get_int_metadataSindicato(restaurante, "CANTIDAD_PEDIDOS");

	t_list* bloquesUsados = obtenerBloquesUsados(bloqueInicial, tamanio);

	liberarBloques(bloquesUsados);

	cantidadPedidos++;

	char* nuevaCantidad = string_itoa(cantidadPedidos);

	set_value_metadataSindicato(restaurante, "CANTIDAD_PEDIDOS", nuevaCantidad);

	get_int_metadataSindicato(restaurante, "CANTIDAD_PEDIDOS");

	char* restauranteActualizado;
	FILE* miFile = fopen(restaurante->path, "r");
	int tamFile = tamanioFile(miFile);
	fread((void*)restauranteActualizado, (size_t) tamFile, 1, miFile);
	fclose(miFile);

	int bloquesNecesarios = tamFile / espacioDisponibleBloque + 1;

	t_list* bloquesAsignados = pedirBloquesNecesarios(bloquesNecesarios);
	bloqueInicial = list_get(bloquesAsignados,0);
	char* bloqueInicial_str = string_itoa(bloqueInicial);
	char* tam = string_itoa(tamFile);
	set_value_metadataSindicato(metadataRestaurante, "INITIAL_BLOCK", bloqueInicial_str);
	set_value_metadataSindicato(metadataRestaurante, "SIZE", tam);

	log_info(logger, "Comienza escritura de información en los %d bloques asignados.\n",bloquesNecesarios);
	escribirEnMemoria(bloquesAsignados,restauranteActualizado);
	log_info(logger, "Finaliza de forma exitosa la escritura de los bloques.\n");

	sem_post(&s_temporalFile);

	metadataSindicato_destroy(restaurante);
	metadataSindicato_destroy(metadataRestaurante);
}

