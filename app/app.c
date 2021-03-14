/*
 * app.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */


#include<string.h>
#include<math.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>

#include "app.h"
#include "auxiliares_app.h"
#include "planificacion.h"
#include "serializacion_app.h"

/*
int main(void){

	char* archivo_config = "app.config";

	sem_init(&m_planif_exec, 1, 0);
	sem_init(&m_planif_ready, 1, 1);
	sem_init(&m_planif_blocked, 1, 0);
	sem_init(&hay_alguien,1,0);
	sem_init(&mutexIdPedido, 0, 1);

	sem_init(&m_cola_NEW,1,1);
	sem_init(&m_cola_READY,1,1);
	sem_init(&m_cola_EXEC,1,1);
	sem_init(&m_cola_BLOCKED,1,1);
	sem_init(&m_cola_EXIT,1,1);
	sem_init(&pedidoEntregado,1,0);

	//CREACION DE CONFIG Y LOGGER
	config = creacion_de_config_app(archivo_config);
	if (config == NULL){
		printf("No fue posible generar la configuración desde %s.\n",archivo_config);
		exit(2);
	}

	logger = creacion_de_logger_app(config);

	if (logger == NULL){
		printf("No fue posible generar el logger \n");
		exit(2);
	}

	log_info(logger,"Configuración y logger OK.");

	cargar_config();

	//LISTAS DE PLANIFICACION
	lista_repartidores = generarRepartidores();
	log_info(logger,"Inicia creación de listas para colas de planificación");
	cola_new = list_duplicate(lista_repartidores);
	log_info(logger,"Cola new: Se ha generado la lista de repartidores generados desde config");
	cola_ready = list_create();
	log_info(logger,"Cola ready: Vacía");
	cola_blocked = list_create();
	log_info(logger,"Cola blocked: Vacía");
	cola_exec = list_create();
	log_info(logger,"Cola exec: Vacía");
	cola_exit = list_create();
	log_info(logger,"Cola exit: Vacía");
	log_info(logger,"Finaliza creación de listas para colas de planificación");

	lista_restaurantes = list_create();
	log_info(logger,"Creación de lista de restaurantes");

	lista_clientes = list_create();
	log_info(logger,"Creación de lista de clientes");

	lista_pedidos = list_create();
	log_info(logger,"Creación de lista de pedidos");
	//-------------------------------
	//-----INICIALIZACION DE SEMAFOROS

	if (pthread_create(&threadREADY,NULL,(void*)planificadorEXEC,NULL) != 0){
	       printf("Error READY\n");
	}
	if (pthread_create(&threadEXEC,NULL,(void*)planificadorREADY,NULL) != 0){
		       printf("Error EXEC\n");
	}
	if (pthread_create(&threadBLOCK,NULL,(void*)planificadorBLOCKED,NULL) != 0){
		       printf("Error EXEC\n");
	}
	if (pthread_create(&threadEXIT,NULL,(void*)entregaPedidos,NULL) != 0){
			      printf("Error finalizacionPedidos\n");
	}

	handshakeComanda();

	algoritmoActual = planificador_actual_APP();
	iniciar_servicio_app();
	terminar_app();


	return EXIT_SUCCESS;
}
*/

//probando funciones de serializacion
int main(void){

	char* archivo_config = "app.config";


	//CREACION DE CONFIG Y LOGGER
	config = creacion_de_config_app(archivo_config);
	if (config == NULL){
		printf("No fue posible generar la configuración desde %s.\n",archivo_config);
		exit(2);
	}
}


