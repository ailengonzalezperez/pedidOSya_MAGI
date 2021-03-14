/*
 * funciones_sindicato.h
 *
 *  Created on: 30 nov. 2020
 *      Author: utnso
 */

#ifndef FUNCIONES_SINDICATO_H_
#define FUNCIONES_SINDICATO_H_

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<magi-library.h>
#include<semaphore.h>
#include "serializacion_sindicato.h"

t_log* creacion_de_logger_sindicato(t_config* config);

void crearReceta(char* nombre,char* pasos, char* tiempo_pasos);
void crearRestaurante(char* nombre, char* cant_cocineros, char* posicion, char* afinidad_cocineros, char* platos, char* precio_platos, char* cant_hornos);
void guardarMetadata(FILE* archivo,int tamanioDatos,int bloqueInicial);
void escribirEnMemoria(t_list* bloquesAEscribir, char* datosAEscribir);
void iniciar_servicio_sindicato(t_config* config);
void terminar_sindicato();
void generarBloques();
void anularAsignacionBloques(t_list* bloquesAsignados);

char* generarStringDatosRestaurante(char* cant_cocineros,char* posicion,char* afinidad_cocineros,char* platos,char* precio_platos,char* cant_hornos);
char* generarStringDatosReceta(char* pasos, char* tiempo_pasos);
char* generarRutaBloque(int bloqueActual);
char* get_string_metadataSindicato(t_metadataSindicato* self, char *key);

bool existeRestaurante(char* nombreRestaurante);
bool existePedido(char* nombreRestaurante, char* idPedido);
bool existeReceta(char* nombrePlato);
bool estaEnEstado(char* nombreRestaurante, int idPedido, char* estado);
char* rutaReceta(char* nombrePlato);
char* rutaPedido(char* nombreRestaurante, char* idPedido);
int tamanioFile(FILE* f);
char* rutaRestaurante(char* nombreRestaurante);
char* metadataRestaurante(char* nombreRestaurante);
void crearArchivoPedido(char* nombreRestaurante, int idPedido);
t_list* generarListaDesdeString(char* text);
t_list* obtenerBloquesUsados(int bloqueInicial, int tamanio);
void agregarPlatoAPedido(char* nombreRestaurante, int idPedido, char* plato, int cantidadPlato);
char* listaAArray(t_list* lista);
bool aumentarCantidadListaDePlato(char* platos, char* cantidad_lista, char* platoAModificar);


t_list* pedirBloquesNecesarios(int bloques);
t_metadataSindicato* metadata_sindicato_create(char *path);

int pedirBloqueLibre();
int get_int_metadataSindicato(t_metadataSindicato *self, char *key);


#endif /* FUNCIONES_SINDICATO_H_ */
