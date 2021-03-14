/*
 * sindicato.h
 *
 *  Created on: 24 oct. 2020
 *      Author: Gonzalo G
 */

#ifndef SINDICATO_H_
#define SINDICATO_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include<semaphore.h>
#include "commons/collections/dictionary.h"

t_config* config;
t_log* logger;
int tamanioBloque;
int espacioDisponibleBloque;
int cantBloques;



typedef struct{
	char* puntoMontaje;
	char* directorio_restaurante;
	char* directorio_recetas;
	char* directorio_bloques;
	char* archivo_metadata;
	char* archivo_bitmap;
}t_config_sindicato;

t_config_sindicato* config_s;

//t_config* creacion_de_config_sindicato(char* archivo_config);
t_log* creacion_de_logger_sindicato(t_config* config);
void iniciar_servicio_sindicato(t_config* config);
void posiblesOpciones(void);
bool noEsDecisionPosible(char*);
void consola_sindicato(int decision);
void terminar_sindicato();
void crearDirectorios();
void cargar_config();


/*Funciones simil config para leer metadata*/

typedef struct{
	char* path;
	t_dictionary* properties;
}t_metadataSindicato;

t_metadataSindicato* metadata_sindicato_create(char *path);
char* get_string_metadataSindicato(t_metadataSindicato* self, char *key);
int get_int_metadataSindicato(t_metadataSindicato *self, char *key);

#endif /* SINDICATO_H_ */
