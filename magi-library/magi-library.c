#include "magi-library.h"
#include<commons/log.h>

/**************************************************************************************
 * Aca arranca toda la estructura de los sockets que se van a usar para
 * los clientes, la app y los restaurantes.
 *
 **************************************************************************************/
//Estructura Cliente



int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		freeaddrinfo(server_info);
		return -1;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}


void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}



//Estructura Servidor


void iniciar_servidor(void (*atender_request)(uint32_t), t_config* config)
{
	struct sockaddr_in direccionServidor;
		struct sockaddr_storage direccionRequest;
		socklen_t tamanioDireccion;
		int servidor_fd, puerto_app;

		puerto_app = config_get_int_value(config,"PUERTO_ESCUCHA");

		direccionServidor.sin_family = AF_INET;
		direccionServidor.sin_addr.s_addr = INADDR_ANY;
		direccionServidor.sin_port = htons(puerto_app);

		servidor_fd = socket(AF_INET, SOCK_STREAM, 0);

		int activado = 1;
		setsockopt(servidor_fd,SOL_SOCKET,SO_REUSEADDR, &activado, sizeof(activado));

		if(bind(servidor_fd, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
			perror("fallo el bind");
		}

		if(listen(servidor_fd, 10) == -1){
			perror("fallo el listen");
		}

		log_info(logger, "(Esperando conexiones en Direccion: %i, Puerto: %i)",INADDR_ANY,puerto_app);
			//-----
		while(1){
			tamanioDireccion = sizeof(direccionRequest);
			pthread_t trequest;

			int request_fd;
			request_fd = accept(servidor_fd, (void*) &direccionRequest, &tamanioDireccion);

			log_info(logger, "(Recibi request del puerto: %i)",request_fd);

			int thread_status = pthread_create(&trequest, NULL, (void*) atender_request,(void*) request_fd);
			if( thread_status != 0 ){
					log_error(logger, "Thread create returno %d", thread_status );
					log_error(logger, "Thread create returno %s", strerror( thread_status ) );
			} else {
					pthread_detach( trequest );
			}
		}
}

int esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	//log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

/*
char* recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	//log_info(logger, "Me llego el mensaje %s", buffer);
	//printf("Me llegó el mensaje: %s\n",buffer);
	//free(buffer);
	return buffer;
}*/


//Logger y config (despues pasarlo)

t_log* iniciar_logger(char* logFile, char* proceso) {
	// char* file, char *program_name  bool is_active_console, t_log_level
	t_log *logger;
	logger = log_create(logFile,proceso,1,LOG_LEVEL_INFO);// 0 porque Log no va por consola
	/*if ((logger = log_create(logFile,proceso,0,LOG_LEVEL_INFO)) == NULL)
	{
		printf("No se pudo crear el logger \n");
		exit(1);
	} Paso esta lógica a cada módulo para que lo trate como prefiera*/
	return logger;
}

t_config* leer_config(char* nombre){
	t_config* configuracion;
	configuracion = config_create(nombre);
	/*if ((configuracion = config_create(nombre)) == NULL){
		printf("No se pudo crear configuración");
		exit(2);
	}Paso esta lógica a cada módulo para que lo trate como prefiera*/
	return configuracion;
}

void leer_consola(t_log* logger){
	char* leido;
	leido = readline(">");
	while(*leido != '\0')
	{
		log_info(logger,leido);
		free(leido);
		leido = readline(">");
	}
	free(leido);
}


//FUNCIONES GENERALES

int min(int num1, int num2) {
   int result;
   if (num1 < num2)
      result = num1;
   else
      result = num2;
   return result;
}
int max(int num1, int num2) {
   int result;
   if (num1 > num2)
      result = num1;
   else
      result = num2;
   return result;
}

void loggear_lista_nombres(t_list* lista, t_log* logger){

	if(list_size(lista)!=0){
		log_info(logger, "El contenido de la lista es el siguiente\n");
		for(int i = 0; i < list_size(lista); i++){
			log_info(logger, "\tEl elemento %d es : %s\n", i+1, (char*)list_get(lista,i));
		}
	}else{
		log_info(logger, "Lista vacia\n");
	}
}

//-----------------------MENSAJES----------------------

//------------------------ ENVIAR

void enviar_paquete(t_paquete* paquete, int fd_socket){

	void* a_enviar = malloc(paquete->buffer->size + sizeof(op_code) + sizeof(uint32_t));
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(op_code));
	offset += sizeof(op_code);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	send(fd_socket, a_enviar, paquete->buffer->size + sizeof(op_code) + sizeof(uint32_t),0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
/*
void enviar_paquete(t_paquete *paquete, uint32_t socket_cliente)
{
	uint32_t sizePaquete = paquete->buffer->size + 2 * sizeof(uint32_t);
	void *stream = serializar_paquete(paquete, sizePaquete);
	send(socket_cliente, stream, sizePaquete, 0);
	eliminar_paquete(paquete);
	free(stream);
}

void* serializar_paquete(t_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream,
			paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}*/
//------------------------ DES/SERIALIZAR


t_buffer* serializar_lista_nombres(t_list* lista){

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) //cantidad de elementos de la lista
			+ peso_de_una_lista(lista); //peso de los elemetos de la lista

	uint32_t cantElementos,tamanio;
	char* char_auxiliar;
	cantElementos = list_size(lista);

	void* stream = malloc(buffer->size);
	int offset = 0;

	// memcpy
	memcpy(stream + offset, &cantElementos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for(int i = 0; i < cantElementos; i++){
		char_auxiliar = list_get(lista,i);
		tamanio = strlen(char_auxiliar);

		memcpy(stream + offset, &tamanio, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset, char_auxiliar, tamanio+1);
		offset += tamanio+1;
	}

 	buffer->stream = stream;

 	return buffer;
}

t_list* deserializar_lista_nombres(t_buffer* buffer){

	uint32_t cantElementos, tamanio;
	char* char_auxiliar;
	char* a_enviar;
	t_list* lista = list_create();

 	void* stream = buffer->stream;

 	// memcpy  -------
 	memcpy(&cantElementos, stream, sizeof(uint32_t));
 	stream += sizeof(uint32_t);

 	//chars
 	for(int i = 0; i < cantElementos; i++){
 		memcpy(&tamanio, stream, sizeof(uint32_t));
 		stream += sizeof(uint32_t);
 		char_auxiliar = malloc(tamanio+1);
 		memcpy(char_auxiliar, stream, tamanio+1);
 		stream += tamanio+1;

 		list_add(lista,char_auxiliar);
 	}
	return lista;
}

t_buffer* serializar_numero(uint32_t numero){

 	t_buffer* buffer = malloc(sizeof(t_buffer));
 	buffer->size = sizeof(uint32_t);

 	void* stream = malloc(buffer->size);
 	int offset = 0;

 	memcpy(stream + offset, &numero, sizeof(uint32_t));
 	offset += sizeof(uint32_t);

 	buffer->stream = stream;

 	return buffer;
 }

uint32_t deserializar_numero(t_buffer* buffer){
 	uint32_t numero;

 	void* stream = buffer->stream;

 	memcpy(&numero, stream, sizeof(uint32_t));


	return numero;
 }


t_buffer* serializar_nombre(char* nombre){

 	t_buffer* buffer = malloc(sizeof(t_buffer));
 	buffer->size = sizeof(uint32_t) + strlen(nombre) + 1;
 	uint32_t tamanio = strlen(nombre);

 	void* stream = malloc(buffer->size);
 	int offset = 0;

 	// memcpy
 	memcpy(stream + offset, &tamanio, sizeof(uint32_t));
 	offset += sizeof(uint32_t);
 	memcpy(stream + offset, nombre, strlen(nombre)+1);
 	offset += strlen(nombre)+1;
 	//--------

 	buffer->stream = stream;

 	return buffer;
 }

char* deserializar_nombre(t_buffer* buffer){
 	uint32_t tamanio;
 	char* nombre;

 	void* stream = buffer->stream;

 	// memcpy  -------
 	memcpy(&tamanio, stream, sizeof(uint32_t));
 	stream += sizeof(uint32_t);

 	nombre = malloc(tamanio);

 	memcpy(nombre, stream, tamanio);

 	//-----------------


	return nombre;
 }

//t_buffer* serializar_Pedido_completo(t_pedido* pedido);
//t_pedido* deserializar_Pedido_completo(t_buffer* buffer);
//------------------------ RECIBIR

int recibir_operacion(int fd_entrada)
{
	int cod_op;
	if(recv(fd_entrada, &cod_op, sizeof(op_code), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(fd_entrada);
		log_error(logger, "Error obteniendo codigo de operacion");
		return -1;
	}
}
void* recibir_buffer(int fd_entrada)
{
	t_buffer* buffer;
	buffer = malloc(sizeof(t_buffer));

	recv(fd_entrada, &(buffer->size), sizeof(uint32_t), MSG_WAITALL);
	buffer->stream = malloc(buffer->size);
	recv(fd_entrada, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}

//------------------------ EXTRAS
void copiar_array_a_lista(t_list* lista, char** array){

	void _agregar_a_lista(char* string){
		list_add(lista, string);
	}
	string_iterate_lines(array, (void*)_agregar_a_lista);
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete_personalizado(op_code codigo_operacion)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_operacion;
	crear_buffer(paquete);
	return paquete;
}


size_t peso_de_una_lista(t_list* lista){

	t_list* lista_auxiliar;
	char* char_auxiliar;
	size_t size = 0;

	lista_auxiliar = list_duplicate(lista);

	for(int i = 0 ; i < list_size(lista_auxiliar); i++){

		char_auxiliar = list_get(lista_auxiliar,i);
		size = size + (strlen(char_auxiliar)+1);

	}

	size = size + (sizeof(uint32_t)*list_size(lista));

	return size;
}

// ------------------------------ ELIMINAR

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

//-----------------------MENSAJES---------------------















//no sirven por ahora
/*void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	if(send(socket_cliente, a_enviar, bytes,0) == -1){
		log_error(logger, "Error en envio de mensaje");
	}else{
		log_info(logger, "Envio de mensaje exitoso");
	}

	free(a_enviar);
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}*/


/*
void* recibir_buffer(int socket_cliente)
{
	int* size;
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}*/
/*
t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);

	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
	return NULL;
}


t_paquete* armar_paquete(){
	t_paquete* paquete = crear_paquete();
	char* leido = readline(">");
	while(*leido != '\0')
	{
		agregar_a_paquete(paquete, leido, strlen(leido)+1);
		free(leido);
		leido = readline(">");
	}
	free(leido);
	return paquete;
}*/





