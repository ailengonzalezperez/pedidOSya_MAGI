/*
 * planificacion.c
 *
 *  Created on: 24 oct. 2020
 *      Author: utnso
 */
#include "magi-library.h"
#include "auxiliares_app.h"
#include "planificacion.h"
#include "app.h"

bool confirmarPedido(char* id_pedido, t_cliente* cliente_datos){
	//pedido* pedido = malloc(sizeof(pedido));
	t_restaurantes* restaurante_destino;

	//defino el restaurante, exepcion para resto default
	if(strcmp(cliente_datos->nombre_restaurante,"RestoDefault")!= 0){
		restaurante_destino = datosRestaurante(cliente_datos->nombre_restaurante);
	}else{
		t_restaurantes* restoDefault = malloc(sizeof(t_restaurantes));
		restoDefault->nombre = "RestoDefault";
		restoDefault->posicion_x = atoi(config_a->pos_x_rest);
		restoDefault->posicion_y = atoi(config_a->pos_y_rest);
		restaurante_destino = restoDefault;
	}

	//genero el pedido
	pedido* pedido = malloc(sizeof(pedido));
	pedido->id_pedido = atoi(id_pedido);
	pedido->restaurante = restaurante_destino;
	pedido->destino_x = cliente_datos->pos_x;
	pedido->destino_y = cliente_datos->pos_y;

	if(strcmp(cliente_datos->nombre_restaurante,"RestoDefault")!= 0){
		pedido->estado_pedido = CONFIRMADO;
	}else{
		pedido->estado_pedido = FINALIZADO;
	}
	log_info(logger, "Pedido %d generado", pedido->id_pedido);


	asignarPedido(pedido);
	list_add(lista_pedidos,pedido);
	sem_post(&hay_alguien);

	return true;
}
void asignarPedido(pedido* pedido){
	int x_resto = pedido->restaurante->posicion_x;
	int y_resto = pedido->restaurante->posicion_y;
	repartidores* repartidorAsignado = repartidor_mas_cercano(x_resto,y_resto,cola_new);
	repartidorAsignado->pedido_asignado = pedido; //Guardo el pedido
	/*Como recién arranca, el destino del repartidor será el del restaurante*/
	repartidorAsignado->destino_x = x_resto;
	repartidorAsignado->destino_y = y_resto;

	reasignarRepartidor_ColaA_ColaB(repartidorAsignado, cola_new, cola_ready, m_cola_NEW, m_cola_READY);

	log_info(logger,"Repartidor %d toma el pedido %d",repartidorAsignado->id_repartidor,pedido->id_pedido);
}

void reasignarRepartidor_ColaA_ColaB(repartidores* repartidor, t_list* cola_A, t_list* cola_B, sem_t m_cola_A, sem_t m_cola_B){
    //Lo saco de new
	sem_wait(&m_cola_A);

	int id_repartidor = repartidor->id_repartidor;

	bool _es_el_repartidor(repartidores* repartidor_aux){
		return repartidor_aux->id_repartidor == id_repartidor;
	}

	list_remove_by_condition(cola_A, (void*)_es_el_repartidor);
    sem_post(&m_cola_A);

    //Lo pongo en ready
    sem_wait(&m_cola_B);
    list_add(cola_B, repartidor);
    sem_post(&m_cola_B);

}


void planificadorREADY(){
	while(1){
		sem_wait(&m_planif_ready);
		if(lasColasEstanVacias()){
			log_info(logger,"Estoy a espera de nuevos Pedidos");
			sem_wait(&hay_alguien);
		}
		log_info(logger,"estoy en planificadorReady");
		int g_multiproces = atoi(config_a->multiprocesamiento);
		if(list_size(cola_exec) < g_multiproces && list_size(cola_ready) != 0){
			repartidores* repartidor = planificarSegunAlgoritmo();
			reasignarRepartidor_ColaA_ColaB(repartidor, cola_ready, cola_exec, m_cola_READY, m_cola_EXEC);
		}
		sem_wait(&m_cola_READY);
		cola_ready = list_map(cola_ready, descanso);
		sem_post(&m_cola_READY);

		sem_post(&m_planif_exec);
	}
}
void planificadorEXEC(){
	while(1){
		sem_wait(&m_planif_exec);
		log_info(logger,"estoy en planificadorExec");
		sem_wait(&m_cola_EXEC);
		int l = list_size(cola_exec);
		sem_post(&m_cola_EXEC);
		for(int i = 0; i < l; i++){
			repartidores* repartidor_i = list_get(cola_exec, i);
			if(llego(repartidor_i) || seMovioLoSuficiente(repartidor_i)){
				repartidor_i->tiempo_en_movimiento = 0;
				reasignarRepartidor_ColaA_ColaB(repartidor_i, cola_exec, cola_blocked, m_cola_EXEC, m_cola_BLOCKED);
			}else{
				hacerUnMovimiento(repartidor_i);
			}
		}
		sem_post(&m_planif_blocked);
	}
}

void planificadorBLOCKED(){
	while(1){
		sem_wait(&m_planif_blocked); // inicializar m_blocked = 0
		log_info(logger,"estoy en planificadorBlocked");
		sem_post(&m_cola_BLOCKED);
		int l = list_size(cola_blocked);
		sem_wait(&m_cola_BLOCKED);

		for(int i = 0; i < l; i++){
			repartidores* repartidor_i = list_get(cola_blocked, i);

			if(descansoLoSuficiente(repartidor_i)){
				log_info(logger,"repartidor %d descanso lo suficiente",repartidor_i->id_repartidor);
				repartidor_i->tiempo_descansado = 0;
				reasignarRepartidor_ColaA_ColaB(repartidor_i, cola_blocked, cola_ready, m_cola_BLOCKED, m_cola_READY);
				log_info(logger,"repartidor %d fue movido a la cola de ready",repartidor_i->id_repartidor);
			}else if( repartidor_i->pedido_asignado->estado_pedido == FINALIZADO && llego(repartidor_i)){

				repartidor_i->tiempo_descansado = 0;

				if(estaCaminoAlrestaurante(repartidor_i)){
					log_info(logger,"repartidor %d llego al restaurante",repartidor_i->id_repartidor);
					repartidor_i->destino_x = repartidor_i->pedido_asignado->destino_x;
					repartidor_i->destino_y = repartidor_i->pedido_asignado->destino_y;
					reasignarRepartidor_ColaA_ColaB(repartidor_i, cola_blocked, cola_ready, m_cola_BLOCKED, m_cola_READY);
				}else{
					reasignarRepartidor_ColaA_ColaB(repartidor_i, cola_blocked, cola_exit, m_cola_BLOCKED, m_cola_EXIT);
					log_info(logger,"repartidor %d llego al cliente",repartidor_i->id_repartidor);
					sem_post(&pedidoEntregado);
				}
			}else{
				repartidor_i->tiempo_descansado++;
				log_info(logger,"repartidor %d aumenta su tiempo descanzado a %d",repartidor_i->id_repartidor, repartidor_i->tiempo_descansado);
			}
		}
		sleep(config_a->retardo);
		sem_post(&m_planif_ready);
	}
}


repartidores* planificarSegunAlgoritmo(){
	repartidores* repartidorElegido;
	if (algoritmoActual == FIFO){
		log_info(logger,"Se planifica por FIFO");
		repartidorElegido = elegirPorFIFO();
	} else if (algoritmoActual == SJF){
		log_info(logger,"Se planifica por SJF");
		repartidorElegido = elegirPorSJF();
	} else if (algoritmoActual == HRRN){
		log_info(logger,"Se planifica por HRRN");
		repartidorElegido = elegirPorHRRN();
	} else{
		repartidorElegido = NULL;
	}
	return repartidorElegido;
}
repartidores* elegirPorFIFO(){
    sem_wait(&m_cola_READY);
    repartidores* elElegido = list_get(cola_ready, 0);
    sem_post(&m_cola_READY);
    return elElegido;
}

repartidores* elegirPorSJF(){
    sem_wait(&m_cola_READY);
	repartidores* elElegido = repartidorConMenosMovimientosRestantes(cola_ready);
    sem_post(&m_cola_READY);
    return elElegido;
}

repartidores* elegirPorHRRN(){
    sem_wait(&m_cola_READY);
	repartidores* elElegido = repartidorConMayorRatioResponse(cola_ready);
    sem_post(&m_cola_READY);
    return elElegido;

}
repartidores* repartidorConMenosMovimientosRestantes(t_list* listaDeRepartidores){
	int cant_repartidores_posibles = list_size(listaDeRepartidores); //Para ir iterando

	repartidores* repartidor_elegido = list_get(listaDeRepartidores,0); //Asumo que es el 1°
	int rafaga_elegido = estimacionProximaRafaga(repartidor_elegido);

	repartidores* otro_repartidor; //Será el que voy comparando
	int rafaga_otro;

		for (int pos = 1; pos < cant_repartidores_posibles-1; pos++){
			otro_repartidor = list_get(listaDeRepartidores,pos);
			rafaga_otro = estimacionProximaRafaga(otro_repartidor);

			if (rafaga_otro < rafaga_elegido){
				repartidor_elegido = otro_repartidor; //Lo cambio
				rafaga_elegido = rafaga_otro; //Reasigno la distancia
			}
		}

		return repartidor_elegido;

}

repartidores* repartidorConMayorRatioResponse(t_list* cola){
	repartidores* elElegido = (repartidores*) list_fold(cola, list_get(cola, 0), (void*) repartidorConMenosRR);
	return elElegido;
}
bool lasColasEstanVacias(){

	sem_wait(&m_cola_READY);
	sem_wait(&m_cola_EXEC);
	sem_wait(&m_cola_BLOCKED);
	bool estado = (list_is_empty(cola_ready))&&(list_is_empty(cola_blocked))&&(list_is_empty(cola_exec));
	sem_post(&m_cola_READY);
	sem_post(&m_cola_EXEC);
	sem_post(&m_cola_BLOCKED);

	return estado;
}
void finalizarPedido(int id_pedido){
	pedido* pedidoFinalizado;

	bool _es_el_pedido(pedido* pedido_aux){
		return pedido_aux->id_pedido == id_pedido;
	}

	pedidoFinalizado = (pedido*)list_get(list_filter(lista_pedidos, (void*)_es_el_pedido),0);
	pedidoFinalizado->estado_pedido = FINALIZADO;
}
bool estaCaminoAlrestaurante(repartidores* repartidor){
	t_restaurantes* restaurante = repartidor->pedido_asignado->restaurante;
	return (repartidor->destino_x == restaurante->posicion_x) && (repartidor->destino_y == restaurante->posicion_y);
}
//************************Revisadas**************************//

void entregaPedidos(){
	while(1){
		sem_wait(&pedidoEntregado);
		log_info(logger,"entregar pedido al cliente");
	}
}



PLANIFICADOR_APP planificador_actual_APP(){
	char* algoritmoConfig = config_a->algoritmo;
	if (strcmp(algoritmoConfig,"FIFO")==0){
		log_info(logger,"Algoritmo de planificación de APP a largo plazo: FIFO\n");
		return FIFO;
	} else if (strcmp(algoritmoConfig,"SJF")==0){
		log_info(logger,"Algoritmo de planificación de APP a largo plazo: SJF\n");
		return SJF;
	} else if (strcmp(algoritmoConfig,"HRRN")==0){
		log_info(logger,"Algoritmo de planificación de APP a largo plazo: HRRN\n");
		return HRRN;
	} else{
		return ERROR;
	}
	/*Acá lo ideal es sacar la lógica del logger afuera, a donde sea que se usa la función*. El logger debería ser variable global?*/
}

/*función que solo toma el algoritmo, lo manda a ejecutar y devuelve el repartidor que le dio.
 * Esta sería la función a usar exactamente antes de switchear la ubicación del repartidor*/

/*Funciones para elegir repartidor en lista de Ready según cada algoritmo. Llaman a la lógica*/




/*Funciones que ejecutan la lógica de SJF y HRRN*/



int proximaRafaga(repartidores* repartidor){
	int rafagasParaLlegar = distanciaADestino(repartidor, repartidor->destino_x, repartidor->destino_y);
	return min(rafagasParaLlegar, repartidor->frecuencia_descanso);
}

float RR(repartidores* repartidor){
	int S = proximaRafaga(repartidor);
	int W = repartidor->tiempo_descansado;
	return 1 + S/W;
}

repartidores* repartidorConMenosRR(repartidores* r1, repartidores* r2){
	if(RR(r1)>RR(r2)){
		return r1;
	}else{
		return r2;
	}
}

/*Esta función va tomando la distancia pendiente de los repartidores y los compara para quedarse con el repartidor que menos trayecto pendiente le queda.
 *
 * IMPORTANTE: Acá faltaría incorporar lógica de estimación anterior para el SJF? Revisar
 * También revisar como obtener las coordenadas del destino actual del repartidor. Conviene ponerlo como atributo y se va asignando?*/

int estimacionProximaRafaga(repartidores* repartidor){
	int alpha = atoi(config_a->alpha);
	return (repartidor->estimacion_anterior * alpha + repartidor->tiempo_en_movimiento*(1-alpha));
}




void sacarRepartidoresDeReady(t_list* repartidoresASacar){
    sem_wait(&m_cola_READY);
    for (int pos = 0; pos < list_size(repartidoresASacar); pos++){
    	repartidores* actual = list_get(repartidoresASacar,pos);
    	list_remove(cola_ready,actual);
    }
    sem_post(&m_cola_READY);
}

void enviarRepartidoresABlocked(t_list* repartidoresAIngresar){
    sem_wait(&m_cola_BLOCKED);
    for (int pos = 0; pos < list_size(repartidoresAIngresar); pos++){
    	repartidores* actual = list_get(repartidoresAIngresar,pos);
    	list_add(cola_blocked,actual);
    }
    sem_post(&m_cola_BLOCKED);
}

void descanso(repartidores* repartidor){
	repartidor->tiempo_descansado ++;
}

int llego(repartidores* repartidor){
	return (repartidor->pos_x == repartidor->destino_x) && (repartidor->pos_y == repartidor->destino_y);
}

bool descansoLoSuficiente(repartidores* repartidor){
	log_info(logger,"descanso lo suficiente? tiempo descansado %d, tiempo descanso %d",repartidor->tiempo_descansado, repartidor->tiempo_descanso);
	return(repartidor->tiempo_descansado >= repartidor->tiempo_descanso);
}

int seMovioLoSuficiente(repartidores* repartidor){
	return(repartidor->tiempo_en_movimiento == repartidor->frecuencia_descanso);
}

/*
 *
repartidores* repartidorConMenosMovimientosRestantes(t_list* listaDeRepartidores){
	/*versión sin estimación
	int cant_repartidores_posibles = list_size(listaDeRepartidores); //Para ir iterando

	repartidores* repartidor_con_menos_pendiente = list_get(listaDeRepartidores,0); //Asumo que es el 1°
	int destino_x = repartidor_con_menos_pendiente->destino_x;
	int destino_y = repartidor_con_menos_pendiente->destino_y;
	//Esto se podría mejorar dentro de distanciaADestino
	int distancia_pendiente = distanciaADestino(repartidor_con_menos_pendiente,destino_x,destino_y);

	//Con estimación usaríamos esto y sacaríamos distancia a destino? Se aplicaría a los dos repartidores y vamos comparando
	//proxima_rafaga
	//= r


	repartidores* otro_repartidor; //Será el que voy comparando
	int distancia_otro;

	for (int pos = 1; pos < cant_repartidores_posibles-1; pos++){
		otro_repartidor = list_get(listaDeRepartidores,pos);
		//Acá deberíamos obtener las coordenadas del destino del repartidor
		int otro_x = otro_repartidor->destino_x;
		int otro_y = otro_repartidor->destino_y;
		distancia_otro = distanciaADestino(otro_repartidor,otro_x,otro_y);

		if (distancia_otro < distancia_pendiente){
			repartidor_con_menos_pendiente = otro_repartidor; //Lo cambio
			distancia_pendiente = distancia_otro; //Reasigno la distancia
		}
	}

	return repartidor_con_menos_pendiente;

	int cant_repartidores_posibles = list_size(listaDeRepartidores); //Para ir iterando

	repartidores* repartidor_elegido = list_get(listaDeRepartidores,0); //Asumo que es el 1°
	int rafaga_elegido = estimacionProximaRafaga(repartidor_elegido);

	repartidores* otro_repartidor; //Será el que voy comparando
	int rafaga_otro;

		for (int pos = 1; pos < cant_repartidores_posibles-1; pos++){
			otro_repartidor = list_get(listaDeRepartidores,pos);
			rafaga_otro = estimacionProximaRafaga(otro_repartidor);

			if (rafaga_otro < rafaga_elegido){
				repartidor_elegido = otro_repartidor; //Lo cambio
				rafaga_elegido = rafaga_otro; //Reasigno la distancia
			}
		}

		return repartidor_elegido;

}
 *
 */
