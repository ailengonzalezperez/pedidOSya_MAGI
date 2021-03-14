/*
 * auxiliares_app.c
 *
 *  Created on: 17 oct. 2020
 *      Author: utnso
 */


#include "auxiliares_app.h"

repartidores* generarRepartidor(int id){ //Función que se utiliza una sola vez: cuando arranca
	char** ubicaciones = config_get_array_value(config, "REPARTIDORES");
	char** tiempo_descanso = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	char** frecuencia_descanso = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");
	repartidores* repartidor = malloc(sizeof(repartidores));
	char** a = string_split(ubicaciones[id],"|");
	repartidor->id_repartidor = id;
	repartidor->estimacion_anterior = atoi(config_a->estimacion); //inicial

	repartidor->tiempo_descanso = atoi(tiempo_descanso[id]);
	repartidor->frecuencia_descanso = atoi(frecuencia_descanso[id]);
	repartidor->pos_x = atoi(a[0]);
	repartidor->pos_y = atoi(a[1]);
	repartidor->tiempo_en_movimiento = 0;
	repartidor->tiempo_descansado = 0;
	string_iterate_lines(a, (void*)free);

	free(a);
	log_info(logger,"Repartidor %d generado",id);
	return repartidor;

}

t_list* generarRepartidores(){
    int k = cantidadRepartidores(config);
    t_list* repartidores = list_create();
    log_info(logger,"Comienza agregado de los %d repartidores a lista_repartidores",k);
    int n;
    for(n=0;n<k;n++){
        list_add(repartidores, generarRepartidor(n));
    }
    log_info(logger,"Finaliza agregado de los %d repartidores a lista_repartidores",k);
    return repartidores;
}

void handshakeComanda(void){
	crear_conexion(config_a->ip_comanda, config_a->puerto_comanda);
	log_info(logger, "El servidor de la comanda esta conectado");
}

void cargar_config(){
	config_a = malloc(sizeof(t_config_app));
	config_a->ip_comanda = config_get_string_value(config,"IP_COMANDA");//IP_COMANDA=127.0.0.1
	config_a->puerto_comanda = config_get_string_value(config,"PUERTO_COMANDA");//	PUERTO_COMANDA=5001
	config_a->puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");//PUERTO_ESCUCHA=5004
	config_a->ip_app = config_get_string_value(config,"IP_APP");//	IP_APP=127.0.0.1
	config_a->multiprocesamiento = config_get_string_value(config,"GRADO_DE_MULTIPROCESAMIENTO");//	GRADO_DE_MULTIPROCESAMIENTO=1
	config_a->algoritmo = config_get_string_value(config,"ALGORITMO_DE_PLANIFICACION");//ALGORITMO_DE_PLANIFICACION=FIFO
	config_a->alpha = config_get_string_value(config,"ALPHA");//ALPHA=0,5
	config_a->estimacion = config_get_string_value(config,"ESTIMACION_INICIAL");//	ESTIMACION_INICIAL=2
	config_a->repartidores;//REPARTIDORES=[1|1,5|2,1|4]
	config_a->Fdescanso;//FRECUENCIA_DE_DESCANSO=[8,5,10]
	config_a->Tdescanso;//TIEMPO_DE_DESCANSO=[2,1,4]
	config_a->log;//ARCHIVO_LOG=app.log
	config_a->platosDefault = config_get_string_value(config,"PLATOS_DEFAULT");
	config_a->pos_x_rest = config_get_string_value(config,"POSICION_REST_DEFAULT_X"); //POSICION_REST_DEFAULT_X=0
	config_a->pos_y_rest = config_get_string_value(config,"POSICION_REST_DEFAULT_Y"); //POSICION_REST_DEFAULT_Y=0
	config_a->retardo = config_get_int_value(config, "RETARDO_CICLO_CPU");
}


t_log* creacion_de_logger_app(t_config* config){
	char* nombre_archivo = config_get_string_value(config, "ARCHIVO_LOG");
	t_log* logger = iniciar_logger(nombre_archivo,"APP");

	if (logger == NULL){
		printf("No fue posible crear el archivo log.\n");
		exit(1);
	}

	log_info(logger,"Logger creado para app.\n"); //Registro creación de logger
	return logger;
}

t_config* creacion_de_config_app(char* archivo_config){
	t_config* config = leer_config(archivo_config);
	return config;
}


void iniciar_servicio_app(){

	iniciar_servidor(atender_request_app, config);

}



void terminar_app(){
	if(config != NULL){
		config_destroy(config);
		}
	free(config_a);
	log_info(logger,"Config destruido. Finaliza con destrucción de logger");
	if(logger != NULL){
		log_destroy(logger);
		}

	list_destroy(lista_restaurantes);
	list_destroy(lista_repartidores);
	list_destroy(cola_new);
	list_destroy(cola_ready);
	list_destroy(cola_blocked);
	list_destroy(cola_exec);
	list_destroy(cola_exit);
}
char* get_nombre_restaurante(t_restaurantes* restaurante){
	return restaurante->nombre;
}
bool esClienteNuevo(char* idCliente){
	bool _es_cliente(t_cliente* cliente){
			return strcmp(cliente->nombre, idCliente)==0;
	}
	t_list* lista = list_filter(lista_clientes, (void*)_es_cliente);

	return list_size(lista)==0;
}
t_cliente* datosCliente(char* idCliente){

	bool _es_cliente(t_cliente* cliente){
		return strcmp(cliente->nombre, idCliente)==0;
	}

	t_list* lista = list_filter(lista_clientes, (void*)_es_cliente);
	return (t_cliente*)list_get(lista,0);
}
void modificarSeleccionCliente(char* idCliente, char* nombreRestaurante){

	t_cliente* cliente_seleccion = datosCliente(idCliente);
	strcpy(cliente_seleccion->nombre_restaurante, nombreRestaurante);
}
t_restaurantes* datosRestaurante (char* nombre_restaurante){

	bool _es_restaurante(t_restaurantes* restaurante){
			return strcmp(restaurante->nombre, nombre_restaurante)==0;
	}

	t_list* lista = list_filter(lista_restaurantes, (void*)_es_restaurante);
	return (t_restaurantes*)list_get(lista,0);
}

int generarIdPedido() {
	int id_generado;
	sem_wait(&mutexIdPedido);
	aux_id_pedido++;
	id_generado = aux_id_pedido;
	sem_post(&mutexIdPedido);
	return id_generado;
}
//*******************Revisadas**********************/
int movimientosRestantes(repartidores r, int x_dest, int y_dest){
	return abs(x_dest - r.pos_x) + abs(y_dest - r.pos_y);
}

void hacerUnMovimiento(repartidores* r){
	int x_dest = r->destino_x;
	int y_dest = r->destino_y;
	int x_actual = r->pos_x;
	int y_actual = r->pos_y;
	if(r->pos_x != x_dest){
		if(r->pos_x < x_dest){
			r->pos_x++;
		}else if(r->pos_x > x_dest){
			r->pos_x--;
		}
	}else if(r->pos_y != y_dest){
		if(r->pos_y < y_dest){
			r->pos_y++;
		}else if(r->pos_y > y_dest){
			r->pos_y--;
		}
	}
	r->tiempo_en_movimiento++;
	log_info(logger,"Repartidor N° %d - Posicion Destino (%d,%d) Posicion Actual (%d,%d)",r->id_repartidor,x_dest,y_dest,x_actual,y_actual);
	log_info(logger,"Repartidor N° %d - Tiempo en movimiento: %d",r->id_repartidor, r->tiempo_en_movimiento);
}


int cantidadRepartidores(t_config*  config){
	char** repartidores = config_get_array_value(config, "REPARTIDORES");
	int i = 0;
	while(repartidores[i]!='\0'){
		i++;
	}
	log_info(logger,"La cantidad de repartidores es %d",i);
	return i;
}


int distanciaARestaurante(repartidores* repartidor, int x_resto, int y_resto){
	return ((((repartidor->pos_x-x_resto)^2) - ((repartidor->pos_y-y_resto)^2))^(1/2));
}

int distanciaADestino(repartidores* repartidor, int x, int y){
	return ((((repartidor->pos_x-x)^2) - ((repartidor->pos_y-y)^2))^(1/2));
}

/*repartidores masCercanoAlRestaurante(repartidores* repartidor1, repartidores* repartidor2, int x_resto, int y_resto){
	if (distanciaARestaurante(repartidor1, x_resto, y_resto) < distanciaARestaurante(repartidor2, x_resto, y_resto)){
		return repartidor1;
	}else{
		return repartidor2;
	}
}*/

repartidores* repartidor_mas_cercano(int x_resto, int y_resto, t_list* listaDeRepartidores){
	int cant_repartidores_posibles = list_size(listaDeRepartidores); //Para ir iterando

	repartidores* repartidor_mas_cercano = list_get(listaDeRepartidores,0); //Asumo que el 1° es el + cercano
	int distancia_mas_cercano = distanciaARestaurante(repartidor_mas_cercano,x_resto,y_resto);

	repartidores* otro_repartidor; //Será el que voy comparando
	int distancia_otro;

	for (int pos = 1; pos < cant_repartidores_posibles-1; pos++){
		otro_repartidor = list_get(listaDeRepartidores,pos);
		distancia_otro = distanciaARestaurante(otro_repartidor,x_resto,y_resto);

		if (distancia_otro < distancia_mas_cercano){
			repartidor_mas_cercano = otro_repartidor; //Lo cambio
			distancia_mas_cercano = distancia_otro; //Reasigno la distancia
		}
	}
	log_info(logger,"El repartidor más cercano es el N° %d",repartidor_mas_cercano->id_repartidor);
	return repartidor_mas_cercano;
}




/*
t_list* recibir_paquete_app(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);

	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);

		if(tamanio == 4){
			int* valor;
			memcpy(valor, buffer+desplazamiento, tamanio);
			desplazamiento+=tamanio;
			list_add(valores, valor);

		}else{
			char* valor = malloc(tamanio);
			memcpy(valor, buffer+desplazamiento, tamanio);
			desplazamiento+=tamanio;
			list_add(valores, valor);
		}
	}
	free(buffer);
	return valores;
	return NULL;
}*/




