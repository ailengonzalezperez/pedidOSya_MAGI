/*
 * auxiliares_cliente.h
 *
 *  Created on: 4 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CLIENTE_H_
#define AUXILIARES_CLIENTE_H_

t_log* logger;
t_config* config;


typedef struct{
	char* ip;//IP;
	char* puerto;//PUERTO;
	char* log;//ARCHIVO_LOG;
	char* pos_x;//POSICION_X;
	char* pos_y;//POSICION_Y;
	char* id_cliente;//ID_CLIENTE;
}t_config_cliente;

t_config_cliente* config_c;

t_config* creacion_de_config_cliente(char* archivo_config);
void terminar_cliente(int conexion, t_log* logger, t_config* config);
t_log* creacion_de_logger_cliente(t_config* config);
void enviarDatosAComanda(char* decision, int conexion, t_log* logger);
void menuComanda();
bool noEsDecisionPosible(char* decision);
void posiblesOpciones();



op_code convertir_codigo_operacion(char *codigo);
#endif /* AUXILIARES_CLIENTE_H_ */
