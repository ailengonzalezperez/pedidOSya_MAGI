
#include"comanda.h"
#include"estructuras_memoria.h"
#include <stdio.h>

int main() {
	//inicio variables
	char* archivo_config = "comanda.config";
	char* archivo_log = "comanda.log";
	char* ip_comanda;
	char* puerto_escucha;

	int server_fd;
	int cliente_fd;

	sem_init(&s_memoriaPrincipal, 1, 1);
	sem_init(&s_swap, 1, 1);
	sem_init(&s_listaFrames, 1, 1);
	sem_init(&s_listaRestaurante, 1, 1);
	sem_init(&s_punteroClock, 1, 1);
	sem_init(&s_listaBloquesSwap, 1, 1);

	//inicio logger
	logger = iniciar_logger(archivo_log, "COMANDA");
	if (logger == NULL) {
		printf("No fue posible crear el archivo log.");
		exit(1);
	}
	log_info(logger, "Logger creado para comanda."); //Registro creación de logger

	//inicio config
	config = leer_config(archivo_config);
	if (config == NULL) {
		log_info(logger, "No fue posible generar la configuración desde %s.", archivo_config);
		exit(2);
	}
	log_info(logger, "Configuración creada desde %s", archivo_config);

	cargar_config();

	estados = list_create();
	list_add(estados,"CONFIRMADO");
	list_add(estados,"EN_PROCESO");
	list_add(estados,"CANCELADO");
	list_add(estados,"FINALIZADO");

	lista_restaurantes = list_create();
	log_info(logger, "Lista de restaurantes OK. Comienza vacía");

	tamanioSwap = atoi(config_cm->tamanio_swap);
	log_info(logger, "Tamaño permitido para swap = %d\n", tamanioSwap);

	swap = fopen("memoriaSWAP.txt", "w");
	if (fopen != NULL){
		log_info(logger, "Se creó el archivo memoriaSWAP en el directorio");
	}

	memoriaSwap = (paginaPlato*) mmap(NULL, tamanioSwap, PROT_WRITE, MAP_SHARED, swap, 0); //Chequear offset

	int tamanio = atoi(config_cm->tamanio_memoria);
	memoriaPrincipal = malloc(tamanio);
	log_info(logger, "Se reservó memoria de tamaño %d", memoriaPrincipal);


	punteroClockEn = 0; //Arranca en frame 0
	framesMemoriaPrincipal = list_create();
	bloquesEnSwap = list_create();
	generarListaFrames();
	generarListaBloquesEnSwap();

	//iniciar_servicio_comanda();
	generarRestaurante("hola");
	generarRestaurante("otroResto");

	restaurante* unresto = list_get(lista_restaurantes,0);

	printf("nombre: %s", unresto->nombre);
	//bool guardarPedido(char* nombreRestaurante, int idPedido);
	bool posibilidad = guardarPedido("hola",1);
	//bool guardarPlato(char* nombreRestaurante, int idPedido, char* nombrePlato, int cantidad);
	posibilidad = guardarPlato("hola", 1, "asado", 1);
	//t_paquete* obtenerPedido(char* nombreRestaurante, int idPedido);
	//bool confirmarPedido(char* nombreRestaurante, int idPedido);
	//int platoListo(char* nombreRestaurante, int idPedido, char* platoNombre);
	//int finalizarPedido(char* nombreRestaurante, int idPedido);



	if(posibilidad){
		log_info(logger,"true");
	}else{
		log_info(logger,"false");
	}


	log_info(logger,"a mimir");
	sleep(3);

	//Cierre de módulo
	liberarFrames();
	liberarBloquesEnSwap();
	liberarRestaurantes();
	fclose(swap);
	munmap(memoriaSwap,tamanioSwap);
	free(memoriaPrincipal);
	log_destroy(logger);
	config_destroy(config);

	return EXIT_SUCCESS;
}



