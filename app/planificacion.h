/*
 * planificacion.h
 *
 *  Created on: 24 oct. 2020
 *      Author: utnso
 */

#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_


pthread_t threadREADY;
pthread_t threadEXEC;
pthread_t threadBLOCK;
pthread_t threadEXIT;




PLANIFICADOR_APP algoritmoActual; /*Se debería asignar el valor actual en algún lado usando planificador_actual_APP*/
int gradoMultiprocesamiento; /*Se debería asignar el valor actual en algún lado leyendo config*/
/*Validar si hace falta pasar las colas por parámetro en todas las funciones. Se puede cambiar */

PLANIFICADOR_APP planificador_actual_APP();


repartidores* planificarSegunAlgoritmo();


/*Funciones para mover repartidor de una lista a otra*/
void reasignarRepartidor_ColaA_ColaB(repartidores* repartidor, t_list* cola_A, t_list* cola_B, sem_t m_cola_A, sem_t m_cola_B);


/*Funciones para elegir repartidor en lista de Ready según cada algoritmo. Llaman a la lógica*/

repartidores* elegirPorFIFO();
repartidores* elegirPorSJF();
repartidores* elegirPorHRRN();

/*Funciones que ejecutan la lógica de SJF y HRRN*/
repartidores* repartidorConMayorRatioResponse(t_list* cola);
repartidores* repartidorConMenosMovimientosRestantes(t_list* listaDeRepartidores);
repartidores* repartidorConMenosRR(repartidores* r1, repartidores* r2);

void sacarRepartidoresDeReady(t_list* repartidoresASacar);
void enviarRepartidoresABlocked(t_list* repartidoresAIngresar);

void planificadorEXEC();
void planificadorREADY();
void planificadorBLOCKED();
void entregaPedidos();

bool lasColasEstanVacias();
bool estaCaminoAlrestaurante(repartidores* repartidor);
void descanso(repartidores* repartidor);
void asignarPedido(pedido* pedido);

int estimacionProximaRafaga(repartidores* repartidor);
bool descansoLoSuficiente(repartidores* repartidor);

#endif /* PLANIFICACION_H_ */
