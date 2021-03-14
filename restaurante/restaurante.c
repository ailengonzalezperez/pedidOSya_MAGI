#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include<pthread.h>
#include<semaphore.h>
#include "restaurante.h"
#include "auxiliares_restaurante.h"
#include "planificacion_platos.h"


int main(void) {

	char* archivo_config = "restaurante.config";

	config = creacion_de_config_restaurante(archivo_config);
	if (config == NULL) {
		printf("No fue posible generar la configuración desde %s.\n",
				archivo_config);
		exit(2);
	}

	logger = creacion_de_logger_restaurante(config);
	log_info(logger, "Logger OK. Config OK.");

	ip_restaurante = config_get_string_value(config, "IP_RESTAURANTE");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");

	//Obtención datos para conexion con sindicato
	ip_sindicato = config_get_string_value(config, "IP_SINDICATO"); //IP_SINDICATO
	puerto_sindicato = config_get_string_value(config, "PUERTO_SINDICATO"); //PUERTO_SINDICATO
	log_info(logger, "%s\n", ip_sindicato);
	log_info(logger, "%s\n", puerto_sindicato); //Loggueo la info que obtuve de config

	socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	log_info(logger, "Socket vale %d.\n", socket_sindicato);
	if (socket_sindicato == -1) {
		printf("Algo ha salido mal. Por favor intente de nuevo más tarde.\n"); //Enunciado solicita informarlo por pantalla. Lo pongo en modo APP :D
		log_error(logger,
				"No fue posible establecer la conexión del socket solicitado.\n");
		exit(3);
	}

	quantum = 0; //si el algoritmo es RR, este valor se modifica por el que corresponde

	//Inicializo variable ALGORITMO
	algoritmo = planificador_actual_restaurante();

	//1. Envio OBTENER_RESTAURANTE a sindicato
	nombre_resto = config_get_string_value(config, "NOMBRE_RESTAURANTE");
	metadata = malloc(sizeof(t_metadata));
	//TODO: liberar en terminar_resto()
	metadata->afinidades = list_create();
	metadata->precios = list_create();
	metadata->platos = list_create();
	recetas = list_create();
	pedidos = list_create();
	pedirMetadata(nombre_resto, socket_sindicato);

	//2. Creo colas de planificacion
	cola_new = list_create();
	cola_exec = list_create();
	cola_block = list_create();
	cola_exit = list_create();
	colas_ready = list_create();

	//Creo colas de ready (una por cada cocinero)
	crearColasReady();

	aux_id_pedido = 0;

	ip_app = config_get_string_value(config, "IP_APP"); //IP_SINDICATO
	puerto_app = config_get_string_value(config, "PUERTO_APP"); //PUERTO_SINDICATO
	log_info(logger,"Se configuro los datos de APP");

	conexionApp();



	//Inicializo semaforos
	sem_init(&mutexRECETAS, 0, 1);
	sem_init(&mutexNEW, 0, 1);
	sem_init(&mutexREADY, 0, 1);
	sem_init(&mutexEXEC, 0, 1);
	sem_init(&mutexBLOCK, 0, 1);
	sem_init(&mutexEXIT, 0, 1);
	sem_init(&mutexIdPedido, 0, 1);
	sem_init(&mutexLOGGER, 0, 1);
	sem_init(&nuevoPLATO,0,0);
	sem_init(&mutexPEDIDOS,0,1);
	sem_init(&platoLISTO,0,0);

	int cant_hornos = metadata->cantidad_hornos;
	sem_init(&contHORNOS, 0, cant_hornos);

	//Creo hilos
	int cocineros = metadata->cantidad_cocineros;

	//Habra un hilo por cada cocinero o cola de ready.
	pthread_t threadCOCINA[cocineros];

	if (pthread_create(&threadSERVER, NULL, (void*) iniciar_servicio_resto,
			NULL) != 0) {
		printf("Error iniciando servidor/n");
	}


	if (pthread_create(&threadPLANIFICADOR, NULL, (void*) iniciar_planificador_platos,
				NULL) != 0) {
			printf("Error iniciando planificador/n");
		}

	if (pthread_create(&threadFinalizadorPedidos, NULL, (void*) finalizadorPedidos,
				NULL) != 0) {
			printf("Error iniciando finalizador de pedidos/n");
		}

	for(int i = 0; i < metadata->cantidad_cocineros;i++){
	if (pthread_create(&(threadCOCINA[i]), NULL,
			(void*) iniciar_cocina, (void*)list_get(colas_ready,i)) != 0) {
		printf("Error iniciando thread de cocineros/n");
	}
	}

	pthread_join(threadSERVER, NULL);

	pthread_join(threadPLANIFICADOR, NULL);

	pthread_join(threadFinalizadorPedidos, NULL);

	for(int i = 0; i < metadata->cantidad_cocineros;i++){
		pthread_join(threadCOCINA[i], NULL);
		}

	sem_destroy(&mutexRECETAS);
	sem_destroy(&mutexNEW);
	sem_destroy(&mutexREADY);
	sem_destroy(&mutexEXEC);
	sem_destroy(&mutexBLOCK);
	sem_destroy(&mutexEXIT);
	sem_destroy(&mutexIdPedido);
	sem_destroy(&mutexLOGGER);
	sem_destroy(&nuevoPLATO);
	sem_destroy(&mutexPEDIDOS);
	sem_destroy(&platoLISTO);

	close(socket_sindicato);

	terminar_resto();

	return EXIT_SUCCESS;
}

void crearColasReady(){
	int cantidad_cocineros = (int) metadata->cantidad_cocineros;
	//creo una nueva cola ready y la agrego a la var global colas_ready por cada cocinero que exista
	for (int i = 0; i < cantidad_cocineros; i++) {
		cola_ready *nuevaCola = malloc(sizeof(cola_ready)); //TODO: hacer free
		nuevaCola->platos = list_create();
		sem_init((&nuevaCola->cpuLIBRE),0,1);
		nuevaCola->quantum_actual = quantum;
		sem_init((&nuevaCola->platoNUEVO),0,0);
		list_add(colas_ready, nuevaCola);
	}
	//a las colas ya creadas les agrego las afinidades existentes
	for (int i = 0; i < list_size(metadata->afinidades); i++) {
		agregarAfinidadACola((char*) list_get(metadata->afinidades, i),
				(cola_ready*) list_get(colas_ready, i));
	}
}

void agregarAfinidadACola(char* afinidad, cola_ready* cola){
	cola->afinidad = afinidad;
}

