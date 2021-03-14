/*
 * planificacion_platos.c
 *
 *  Created on: 19 nov. 2020
 *      Author: utnso
 */

#include<stdio.h>
#include<time.h>
#include "planificacion_platos.h"
#include "restaurante.h"
#include "serializacion_restaurante.h"

PLANIFICADOR_RESTAURANTE planificador_actual_restaurante() {
	char* algoritmoConfig = config_get_string_value(config,
			"ALGORITMO_PLANIFICACION");
	if (strcmp(algoritmoConfig, "FIFO") == 0) {
		log_info(logger, "Algoritmo de planificación: FIFO");
		return FIFO; // @suppress("Symbol is not resolved")
	} else if (strcmp(algoritmoConfig, "RR") == 0) {
		log_info(logger, "Algoritmo de planificación: RR");
		quantum = config_get_int_value(config,"QUANTUM");
		log_info(logger,"Quantum: %d",quantum);
		return RR; // @suppress("Symbol is not resolved")
	} else {
		return ERROR; // @suppress("Symbol is not resolved")
	}
}

void* iniciar_planificador_platos() {
	while (1) {
		//Espero a que se agreguen nuevos platos y los paso de NEW a READY
		sem_wait(&nuevoPLATO);
		sem_wait(&mutexNEW);
		for (int i = 0; i < list_size(cola_new); i++) {
			pasar_a_ready((t_plato*) list_remove(cola_new, i));
		}
		sem_post(&mutexNEW);

		return NULL;
	}
}

void* pasar_a_ready(t_plato* plato) {

	bool cola_con_afinidad(cola_ready* cola){
		bool tiene_afinidad = false;
		char* afinidad_cola = cola->afinidad;
		if(strcmp(afinidad_cola,(char*)plato->afinidad)){
			tiene_afinidad = true;
		}
		return tiene_afinidad;
	}

	bool cola_sin_afinidad(cola_ready* cola){
		bool sin_afinidad = false;
		if(cola->afinidad == NULL){
			sin_afinidad = true;
		}
		return sin_afinidad;
	}

	void agregar_plato_a_cola(cola_ready* cola){
		list_add(cola->platos,plato);
		//Le aviso al cocinero que tiene un nuevo plato para cocinar
		sem_post(&(cola->platoNUEVO));
	}

	if (existen_colas_con_afinidad((char*)plato->afinidad)) {
		agregar_plato_a_cola((cola_ready*)list_find(colas_ready, (void*)cola_con_afinidad));

	} else {
		agregar_plato_a_cola((cola_ready*)list_find(colas_ready,(void*)cola_sin_afinidad));
	}
	return NULL;
}

void* iniciar_cocina(cola_ready* cocinero){
	while(1){
		//espero recibir un plato nuevo
		sem_wait(&cocinero->platoNUEVO);
		//en caso de que el cocinero no este preparando ningun plato (cpu libre),
		//prepara el nuevo plato, sino se queda esperando a que se libere la CPU.
		seleccionarPlatoParaEjecucion(cocinero);
	}
	return NULL;
}

void seleccionarPlatoParaEjecucion(cola_ready* cola) {
	if(!list_is_empty(cola->platos)){
	sem_wait(&cola->cpuLIBRE);
	sem_wait(&mutexREADY);
	sem_wait(&mutexEXEC);
	t_plato* plato_nuevo = list_get(cola->platos,0);
	agregarPlatoACola(cola_exec,(t_plato*) list_remove(cola->platos, 0));
	sem_post(&mutexREADY);
	sem_post(&mutexEXEC);
	uint32_t indiceExec = obtenerIndice(cola_exec,plato_nuevo);

	sem_wait(&mutexLOGGER);
	log_info(logger,"Comienza a procesarse plato: %s",(char*)plato_nuevo->afinidad);
	sem_post(&mutexLOGGER);

	procesarPlato((t_plato*)list_get(cola_exec,indiceExec), cola);
	}
	else{
		sem_wait(&mutexLOGGER);
		log_info(logger,"La cola de afinidad: %s no tiene platos para procesar",(char*)cola->afinidad);
		sem_post(&mutexLOGGER);
	}

}

	void procesarPlato(t_plato* plato, cola_ready* cocinero) {
		//Busco receta para el plato, y ejecuto cada una de sus acciones
		t_receta* receta_del_plato;
		for (int i = 0; i < list_size(recetas); i++) {
			t_receta* receta_actual = list_get(recetas, i);
			if (strcmp(plato->afinidad, receta_actual->nombre_plato) == 0) {
				receta_del_plato = receta_actual;
			}
		}
		plato->pasos = receta_del_plato->pasos;
		plato->ciclos_pendientes = receta_del_plato->ciclos;
		for (int i = 0; i < list_size(plato->pasos); i++) {
			//si no estan completados todos los ciclos de CPU para una accion,
			//manda a ejecutar esa accion con los ciclos pendientes
			if (strcmp((char*) list_get(plato->ciclos_pendientes, i),
					"0") != 0) {
				//ejecutar(plato,accion,cocinero);
				ejecutar(plato,i, cocinero);
			}
		}
		//verifico si se realizaron todos los pasos de la receta
		int pasos_completados = 0;
		for (int i = 0; i < list_size(plato->pasos); i++) {
			if (strcmp((char*) list_get(plato->ciclos_pendientes, i),
					"0") == 0){
				pasos_completados++;
			}
		}
		//al finalizar las operaciones sobre el plato (es decir, cuando todos los ciclos
		//pendientes esten en 0, pasa a la cola de exit
		if (pasos_completados == list_size(plato->pasos)) {
			sem_wait(&mutexEXEC);
			sem_wait(&mutexEXIT);
			uint32_t indice = obtenerIndice(cola_exec, plato);
			agregarPlatoACola(cola_exit,
					(t_plato*) list_remove(cola_exec, indice));
			sem_post(&mutexEXEC);
			sem_post(&mutexEXIT);
			//aviso que el plato esta listo
			plato->estado = FINALIZADO;
			sem_post(&platoLISTO);
			//Al finalizar la ejecucion del plato, replanifico:
			sem_post(&cocinero->cpuLIBRE);
			seleccionarPlatoParaEjecucion(cocinero);
		}
	}

void ejecutar(t_plato* plato, int numero_accion, cola_ready* cocinero) {
	char* _hornear = "hornear";
	char* _reposar = "reposar";
	if (strcmp((char*)list_get(plato->pasos,numero_accion),_hornear)==0) {
		sem_wait(&mutexLOGGER);
		log_info(logger,"Comienza a hornearse plato: %s",plato->afinidad);
		sem_post(&mutexLOGGER);

		hornear(plato, numero_accion,cocinero);
	} else if (strcmp((char*)list_get(plato->pasos,numero_accion),_reposar)==0) {
		sem_wait(&mutexLOGGER);
		log_info(logger,"Comienza a reposar plato: %s",plato->afinidad);
		sem_post(&mutexLOGGER);

		uint32_t indice;

		sem_wait(&mutexEXEC);
		sem_wait(&mutexBLOCK);
		indice = obtenerIndice(cola_exec,plato);
		agregarPlatoACola(cola_block,
				(t_plato*) list_remove(cola_exec, indice));
		sem_post(&mutexEXEC);
		sem_post(&mutexBLOCK);

		//libero la cpu para que el cocinero prepare otro plato
		sem_post(&(cocinero->cpuLIBRE));

		int ciclos = atoi((char*)list_get(plato->ciclos_pendientes,numero_accion));

		int reposo = sleep(ciclos);

		char* reemplazo = list_replace(plato->ciclos_pendientes,
							numero_accion,
							"0");

		sem_wait(&mutexBLOCK);
		indice = obtenerIndice(cola_block, plato);
		pasar_a_ready((t_plato*) list_remove(cola_block, indice));
		sem_post(&mutexBLOCK);

		sem_wait(&mutexLOGGER);
		log_info(logger,"Termina de reposar plato: %s",plato->afinidad);
		sem_post(&mutexLOGGER);

		//replanifico
		seleccionarPlatoParaEjecucion(cocinero);

	} else { //caso default
		sem_wait(&mutexLOGGER);
		log_info(logger,"Se comienza a %s plato: %s",(char*)list_get(plato->pasos,numero_accion),plato->afinidad);
		sem_wait(&mutexLOGGER);

		preparar(plato, numero_accion, cocinero);
	}

}

void hornear(t_plato* plato, int index_ciclos, cola_ready* cocinero){

	 if(algoritmo == FIFO){ // @suppress("Symbol is not resolved")
	 hornearConFIFO(plato, index_ciclos);
	 }
	 else if(algoritmo == RR){ // @suppress("Symbol is not resolved")
	 hornearConRR(plato, index_ciclos, cocinero);
	 }

}

void preparar(t_plato* plato, int index_ciclos, cola_ready* cocinero){

	 if(algoritmo == FIFO){ // @suppress("Symbol is not resolved")
	 prepararConFIFO(plato, index_ciclos);
	 }
	 else if(algoritmo == RR){ // @suppress("Symbol is not resolved")
	 prepararConRR(plato, index_ciclos, cocinero);
	 }

}

void hornearConFIFO(t_plato* plato, int index_ciclos) {
	int cont_ciclos = atoi((char*)list_get(plato->ciclos_pendientes,index_ciclos));
	int ciclos_faltantes = atoi((char*)list_get(plato->ciclos_pendientes,index_ciclos));
	if (cont_ciclos != 0) {
		sem_wait(&contHORNOS);// semaforo contador inicializado con la cantidad de hornos
		for (int i = 0; i < cont_ciclos; i++) {
			ciclos_faltantes--;
			char* reemplazo = list_replace(plato->ciclos_pendientes,
					index_ciclos,
					string_itoa(ciclos_faltantes));
			delay(1000); //delay(milisegundos);
		}
		sem_post(&contHORNOS);
	}
	sem_wait(&mutexLOGGER);
	log_info(logger,"Termina de hornearse plato: %s",plato->afinidad);
	sem_post(&mutexLOGGER);

}

void prepararConFIFO(t_plato* plato, int index_ciclos) {
	int cont_ciclos = atoi((char*)list_get(plato->ciclos_pendientes,index_ciclos));
	int ciclos_faltantes = atoi((char*)list_get(plato->ciclos_pendientes,index_ciclos));
	for (int i = 0; i < cont_ciclos; i++) {
		ciclos_faltantes--;
		char* reemplazo = list_replace(plato->ciclos_pendientes,
							index_ciclos,
							string_itoa(ciclos_faltantes));
		delay(1000);
	}
	sem_wait(&mutexLOGGER);
	log_info(logger,"Termina de %s plato: %s",(char*)list_get(plato->pasos,index_ciclos),plato->afinidad);
	sem_post(&mutexLOGGER);
}

void hornearConRR(t_plato* plato, int index_ciclos, cola_ready* cocinero) {
	int quantum_aux = cocinero->quantum_actual;
	int ciclos_faltantes = atoi((char*)list_get(plato->ciclos_pendientes,index_ciclos));
	if(quantum_aux==0){
		cocinero->quantum_actual = quantum;
		quantum_aux = cocinero->quantum_actual;
	}
	sem_wait(&contHORNOS);// hornos disponibles
	while (quantum_aux != 0) {
		if (ciclos_faltantes > 0) {
			quantum_aux--;
			cocinero->quantum_actual = quantum_aux;
			ciclos_faltantes--;
			char* reemplazo = list_replace(plato->ciclos_pendientes,
										index_ciclos,
										string_itoa(ciclos_faltantes));
		} else {
			sem_wait(&mutexLOGGER);
			log_info(logger,"Termina de hornearse plato: %s",plato->afinidad);
			sem_post(&mutexLOGGER);

			//libero el horno por fin de ejecucion
			sem_post(&contHORNOS);
			//si el plato ya uso todos los ciclos de CPU que necesitaba, sale de la funcion
			return;
		}
	}
	//libero el horno por fin de quantum
	sem_post(&contHORNOS);
	//replanifico por fin de quantum
	sem_wait(&mutexEXEC);
	sem_wait(&mutexREADY);
	uint32_t indice = obtenerIndice(cola_exec, plato);
	pasar_a_ready((t_plato*) list_remove(cola_exec, indice));
	sem_post(&mutexEXEC);
	sem_post(&mutexREADY);

	sem_post(&cocinero->cpuLIBRE);
	seleccionarPlatoParaEjecucion(cocinero);
}

void prepararConRR(t_plato* plato, int index_ciclos, cola_ready* cocinero) {
	int quantum_aux = cocinero->quantum_actual;
	int ciclos_faltantes = atoi((char*)list_get(plato->ciclos_pendientes,index_ciclos));
	if(quantum_aux==0){
			cocinero->quantum_actual = quantum;
			quantum_aux = cocinero->quantum_actual;
		}
	while (quantum_aux != 0) {
		if (ciclos_faltantes > 0) {
			quantum_aux--;
			cocinero->quantum_actual = quantum_aux;
			ciclos_faltantes--;
			char* reemplazo = list_replace(plato->ciclos_pendientes,
													index_ciclos,
													string_itoa(ciclos_faltantes));

		} else {
			sem_wait(&mutexLOGGER);
			log_info(logger,"Se termina de %s plato: %s",(char*)list_get(plato->pasos,index_ciclos),plato->afinidad);
			sem_post(&mutexLOGGER);

			//si el plato ya consumio todos sus ciclos de cpu, sale de la funcion
			return;
		}
	}
	//replanifico por fin de quantum
	sem_wait(&mutexEXEC);
	uint32_t indice = obtenerIndice(cola_exec, plato);
	pasar_a_ready((t_plato*) list_remove(cola_exec, indice));
	sem_post(&mutexEXEC);

	sem_post(&cocinero->cpuLIBRE);
	seleccionarPlatoParaEjecucion(cocinero);
}


bool existen_colas_con_afinidad(char* afinidad){
	bool hay_colas = false;
	for(int i = 0; i < list_size(colas_ready);i++){
		cola_ready* cola_actual = list_get(colas_ready,i);
		char* afinidad_actual = cola_actual->afinidad;
		if(strcmp(afinidad,afinidad_actual)==0){
			hay_colas = true;
		}
		}
	return hay_colas;
}


void agregarPlatoACola(t_list* cola_destino, t_plato* plato) {
	list_add(cola_destino, plato);
}

uint32_t obtenerIndice(t_list* cola, t_plato* plato) {
	for (uint32_t i = 0; i < list_size(cola); i++) {
		t_plato* plato_aux = (t_plato*) list_get(cola, i);
		if (strcmp(plato->afinidad, plato_aux->afinidad) == 0) {
			if (plato->id_pedido == plato_aux->id_pedido) {
				return i;
			}
		}
	}
	return -1;
}

void delay(int milliseconds){
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();

}

void* finalizadorPedidos(){
	while(1){
		t_plato* plato;
		sem_wait(&platoLISTO);
		if(!list_is_empty(cola_exit)){
		//obtengo el primer plato de la cola exit
		plato = list_get(cola_exit,0);
		//le aviso al sindicato que el plato esta listo
		plato_listo(string_itoa(plato->id_pedido), plato->afinidad);
	}
		//chequeo si el pedido al que pertenece el plato esta finalizado
		//si es asi le aviso a sindicato y app
		if(pedidoFinalizado(plato->id_pedido)){
			char* id_finalizado = string_itoa(plato->id_pedido);
			terminar_pedido(id_finalizado);
		}
		//saco al plato de la cola exit para no volver a avisar que el plato esta listo
		sem_wait(&mutexEXIT);
		list_remove(cola_exit,0);
		sem_post(&mutexEXIT);

	}
	return NULL;
}

bool pedidoFinalizado(int id){

	bool esElPedido(t_pedido* pedido){
		return pedido->id_pedido == id;
	}

	bool platoFinalizado(t_plato* plato){
		return plato->estado == FINALIZADO;
	}

	t_pedido* pedido_buscado = list_find(pedidos,(void*)esElPedido);

	return list_all_satisfy(pedido_buscado->platos,(void*)platoFinalizado);
}



