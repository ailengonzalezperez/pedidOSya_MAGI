/*
 * modoOculto.c
 *
 *  Created on: 13 dic. 2020
 *      Author: utnso
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

void modoOculto(t_config* config){
	log_info(logger,"Bienvenido al modo oculto de sindicato");

	//log_info(logger,"Guardar pedido 1 en mostaza");
	//t_metadataSindicato* la = leerDesdeBloques(0,156);
	crearRestaurante("Mostaza", "4", "[1,1]", "[mila]", "[mila, asado]", "[200,400]", "4");
	guardarPedido("Mostaza", 2);

	//obtenerPedido("Mostaza", 2);

	//confirmarPedido("Mostaza", 2);

	consultarPlatos("Mostaza");
	//t_paquete* consultarPlatos(char* nombreRestaurante);

	//bool guardarPlato(char* nombreRestaurante, int idPedido, char* plato, int cantidadPlato);
	//bool confirmarPedido(char* nombreRestaurante, int idPedido);
	//t_paquete* obtenerPedido(char* nombreRestaurante, int idPedido);
	//t_paquete* obtenerRestaurante(char* nombreRestaurante);
	//bool platoListo(char* nombreRestaurante, int idPedido, char* platoListo);
	//t_paquete* obtenerReceta(char* nombrePlato);
	//bool terminarPedido(char* nombreRestaurante, int idPedido);

}
