/*
 * sindicato.c
 *	Created on: 24 sep. 2020
 *  Author: Gonzalo G
 */

#include<stdio.h>
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
//#include "funciones_sindicato.h"
#include "serializacion_sindicato.h"



int main(void) {

	char* archivo_config = "sindicato.config";
	sem_init(&s_bitmap,1,1);
	sem_init(&s_temporalFile,1,1);
	//cargando archivo de config
	config = leer_config(archivo_config);
	cargar_config();
	// Creando archivo para Log:

	logger = creacion_de_logger_sindicato(config);
	log_info(logger, "Escribiendo el primer log.");


	char* direccionMetadata = config_get_string_value(config, "ARCHIVO_METADATA");
	t_metadataSindicato* metadata = metadata_sindicato_create(direccionMetadata);
	tamanioBloque = get_int_metadataSindicato(metadata, "BLOCK_SIZE");
	cantBloques = get_int_metadataSindicato(metadata, "BLOCKS");
	espacioDisponibleBloque = tamanioBloque - 4;
	log_info(logger,"BLOCK_SIZE = %d // BLOCKS = %d",tamanioBloque, cantBloques);

	//ruta bloques
	//char* ruta_bloque = config_get_string_value(config, "DIRECTORIO_BLOQUES");

	//Luego de esto procedemos a invocar a la funcion para crear el archivo bitmap y asi ya separarnos el espacio y dejarlo listo.
	direccionBitmap = config_get_string_value(config, "ARCHIVO_BITMAP");
	contenidoBitmap = malloc(cantBloques/8);
	miBitmap = crearBitmap(cantBloques);

	crearDirectorios();
	generarBloques();

	printf("\nCreación de Archivos sindicato\n\n");

	posiblesOpciones();
	char* decision = readline(">>");

	while(noEsDecisionPosible(decision)){
		printf("Error! %s no es una opción correcta.\n",decision);
		posiblesOpciones();
		decision = readline(">>");
	}

	int decisionNro = atoi(decision);
	while(1){
		consola_sindicato(decisionNro);

		posiblesOpciones();
		decision = readline(">>");

		while(noEsDecisionPosible(decision)){
			printf("Error! %s no es una opción correcta.\n",decision);
			posiblesOpciones();
			decision = readline(">>");
		}
		decisionNro = atoi(decision);
	}


	/*No estoy seguro que se tenga que poder terminar en base a un pedido .
	 *Pero por las dudas lo dejo armados.*/

	free(decision);
	terminar_sindicato(config);

	return EXIT_SUCCESS;
}

void posiblesOpciones(void){
	printf("Opciones de Creacion disponibles:\n\n");
	printf("1) Crear Restaurante\n");
	printf("2) Crear Receta\n");
	printf("3) Iniciar servicio de sindicato\n");
	printf("\n¿Qué querés hacer? Ingresá el número correspondiente\n\n");
}

bool noEsDecisionPosible(char* decision){
	return ((strncmp(decision,"1",1)!=0) &&
			(strncmp(decision,"2",1)!=0) &&
			(strncmp(decision,"3",1)!=0)&&
			(strncmp(decision,"4",1)!=0));
}
void consola_sindicato(int decision){
	char *nombre_restaurante,
		 *cant_cocineros, *posicion,
		 *afinidad_cocineros, *receta, *precio_receta,
		 *cant_hornos, *nombre_receta, *pasos, *tiempo_pasos;


	switch(decision){
		case 1:
			printf("Completar los datos del Restaurante:\n\n");

			printf("\n Nombre del restaurante\n Ejemplo: MAGI Dinner\n\n");
			nombre_restaurante = readline(">>");
			printf("\n Cantidad de cocineros\n Ejemplo: 5\n\n");
			cant_cocineros = readline(">>");
			printf("\n Ubicación del restaurante n \n Formato [x,y] Ejemplo: [1,1] \n\n");
			posicion = readline(">>");
			printf("\n Afinidad de los cocineros \n Formato [x,y,z] Ejemplo: [Asado]\n\n");
			afinidad_cocineros = readline(">>");
			printf("\n Recetas \n Formato [x,y,z] Ejemplo: [wokvegetales, asado]\n\n");
			receta = readline(">>");
			printf("\n Precio de las recetas \n Formato [x,y,z] Ejemplo: [200,500]\n\n");
			precio_receta = readline(">>");
			printf("\n Cantidad de hornos:\n Ejemplo: 5\n");
			cant_hornos = readline(">>");

			//AGREGAR CANTIDAD DE PEDIDOS!!!

			log_info(logger,"Restaurante : %s \t/\t %d \t/\t %s \t/\t %s \t/\t %s \t/\t %s \t/\t %d",
								nombre_restaurante, atoi(cant_cocineros), posicion, afinidad_cocineros, receta,
								precio_receta, atoi(cant_hornos));

			crearRestaurante(nombre_restaurante,cant_cocineros,posicion,
			afinidad_cocineros,receta,precio_receta,cant_hornos);

			break;
		case 2:
			printf("Completar Receta:\n\n");

			printf("\n Nombre Receta\n Ejemplo: Milanesa\n\n");
			nombre_receta = readline(">>");
			printf("\nPasos de preparación\nFormato [x,y,z] Ejemplo: [cortar,empanar,freir]\n\n");
			pasos = readline(">>");
			printf("\nTiempo de los pasos\nFormato [x,y,z] Ejemplo: [1,4,2]\n\n");
			tiempo_pasos = readline(">>");

			log_info(logger,"\nReceta :\t %s \t/\t %s \t/\t %s\n",
					nombre_receta, pasos, tiempo_pasos);
			crearReceta(nombre_receta, pasos, tiempo_pasos);


			break;
		case 3:
			iniciar_servicio_sindicato(config);
			break;
		case 4:
			modoOculto(config);
			break;
		default:
			log_info(logger, "Error de consola");
			break;
		}
}
/*
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
}*/


void crearDirectorios(){
	struct stat stats;
	//Crea directorio recetas
	char* directorio_de_recetas = config_s->directorio_recetas;
		if (stat(directorio_de_recetas, &stats) == -1){
			mkdir(directorio_de_recetas,0777);
			log_info(logger, "Se creó el directorio de recetas.");
			}

	//Crea directorio restaurantes
		char* directorio_de_restaurantes = config_s->directorio_restaurante;//config_get_string_value(config, "DIRECTORIO_RESTAURANTES");
		if (stat(directorio_de_restaurantes, &stats) == -1){
			mkdir(directorio_de_restaurantes,0777);
			log_info(logger, "Se creó el directorio de restaurantes.");
			}


	//Crea directorio bloques
		char* directorio_de_bloques = config_s->directorio_bloques;//config_get_string_value(config, "DIRECTORIO_BLOQUES");
		if (stat(directorio_de_bloques, &stats) == -1){
			mkdir(directorio_de_bloques,0777);
			log_info(logger, "Se creó el directorio de bloques.");
			}
	//NOTA: Los pedidos se crean dentro de restaurante.

}


/*

void terminar_sindicato(config) {
	if (logger != NULL) {
		log_destroy(logger);
	}
	if (config != NULL) {
		config_destroy(config);
	}
}*/

void cargar_config(){
	config_s = malloc(sizeof(t_config_sindicato));
	config_s->archivo_bitmap = config_get_string_value(config,"ARCHIVO_BITMAP");
	config_s->archivo_metadata = config_get_string_value(config,"ARCHIVO_METADATA");
	config_s->directorio_bloques = config_get_string_value(config,"DIRECTORIO_BLOQUES");
	config_s->directorio_recetas = config_get_string_value(config,"DIRECTORIO_RECETAS");
	config_s->directorio_restaurante = config_get_string_value(config,"DIRECTORIO_RESTAURANTES");
	config_s->puntoMontaje = config_get_string_value(config,"PUNTO_MONTAJE");
}
