#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include "auxiliares_cliente.h"

int main(){

	//----inicializacion de Variables----
	//--- config

	int socket;

	char* decision;
	op_code codigo_operacion;

	// Inicialización de logger y configuración
	config = creacion_de_config_cliente("cliente.config");
	if (config == NULL){
		printf("Algo ha salido mal con la configuración. Por favor intente de nuevo más tarde.\n");
		exit(2);
	}

	logger = creacion_de_logger_cliente(config);

	if (logger == NULL){
		printf("No fue posible generar el logger \n");
		exit(2);
	}

	log_info(logger,"Logger OK. Config OK.\n");


	//cargo la estructura del config
	cargar_config();

	//------------------------------------------

	//PARTE 2: Inicialización de sockets

	//Obtención datos conexion

	log_info(logger,"IP = %s\n",config_c->ip);
	log_info(logger,"Puerto = %s\n",config_c->puerto); //Loggueo la info que obtuve de config

	socket = crear_conexion(config_c->ip, config_c->puerto); //Genero la conexión
	log_info(logger,"Socket vale %d.\n\n",socket);
	if (socket == -1){
		printf("Algo ha salido mal. Por favor intente de nuevo más tarde.\n"); //Enunciado solicita informarlo por pantalla. Lo pongo en modo APP :D
		log_error(logger,"No fue posible establecer la conexión del socket solicitado.\n");
		exit(3);
	}

	//Entro en el apartado de la eleccion de una opcion
	printf("\nBienvenidos a PedidOS Ya!\n\n");

	posiblesOpciones();
	decision = readline(">>");

	while(noEsDecisionPosible(decision)){
		printf("Error! %s no es una opción correcta.\n",decision);
		posiblesOpciones();
		decision = readline(">>");
	}

	log_info(logger,"Decisión tomada = %s.\n",decision);
	codigo_operacion = convertir_codigo_operacion(decision);

	log_info(logger,"Codigo de operacion = %i.\n",codigo_operacion);
	enviar_accion(codigo_operacion, socket);


	/*while(1){


		puts("");

		posiblesOpciones();
		decision = readline(">>");

		while(noEsDecisionPosible(decision)){
			printf("Error! %s no es una opción correcta.\n",decision);
			posiblesOpciones();
			decision = readline(">>");
		}
	}*/


	//Liberacion de memoria

	terminar_cliente(socket, logger, config);

	return EXIT_SUCCESS;

}




