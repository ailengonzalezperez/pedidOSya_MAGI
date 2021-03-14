#include <magi-library.h>
#include "estructuras_memoria.h"
#include "comanda.h"

/*FUNCIONES DE COMANDA PARA LOS OPCODE*/

bool guardarPedido(char* nombreRestaurante, int idPedido){
	log_info(logger, "Comienza guardarPedido para restaurante %s y pedido %d",nombreRestaurante,idPedido);
	restaurante* miRestaurante;
	if(existeRestaurante(nombreRestaurante)){
		miRestaurante = obtenerRestaurante(nombreRestaurante);
		agregarPedido(idPedido, miRestaurante);
	}else{
		generarRestaurante(nombreRestaurante);
		miRestaurante = obtenerRestaurante(nombreRestaurante);
		agregarPedido(idPedido, miRestaurante);
	}
	log_info(logger, "Concluye guardarPedido para restaurante %s y pedido %d",nombreRestaurante,idPedido);
	return true;
}

bool guardarPlato(char* nombreRestaurante, int idPedido, char* nombrePlato, int cantidad){
	if(existeRestaurante(nombreRestaurante)){
		log_info(logger, "Existe restaurante %s", nombreRestaurante);
		restaurante* miRestaurante = obtenerRestaurante(nombreRestaurante);
		if(existePedidoEnUnRestaurante(miRestaurante, idPedido)){
			log_info(logger, "Existe en %s el pedido %d", nombreRestaurante, idPedido);
			pedido* miPedido = obtenerUnPedido(miRestaurante, idPedido);
			if(existePlato(miPedido, nombrePlato)){
				log_info(logger, "Existe en %s en el pedido %d el plato %s", nombreRestaurante, idPedido, nombrePlato);
				plato* miPlato = obtenerPlato(miPedido, nombrePlato);
				actualizarCantidadTotalPlato(miPlato, cantidad);
			}else{
				log_info(logger, "No existe en %s en el pedido %d el plato %s", nombreRestaurante, idPedido, nombrePlato);
				int nuevaDireccionEnSwap = crearPaginaDePlatoEnSwap(nombrePlato);
				if (nuevaDireccionEnSwap == -1){
					log_info(logger,"No hay lugar en swap. No se crea el plato");
					return false;
				}
				//si me dijo que no hay lugar en swap -> cortar
				plato* miPlato = crearYAgregarPlatoAdministrativo(nombrePlato, miPedido,nuevaDireccionEnSwap);
				actualizarCantidadTotalPlato(miPlato, cantidad);
			}
		}else{
			log_info(logger, "No existe en %s el pedido %d, operacion no completada", nombreRestaurante, idPedido);
			return false;
		}
	}else{
		log_info(logger, "No existe restaurante %s, operacion no completada", nombreRestaurante);
		return false;
	}
	return true;
}

t_paquete* obtenerPedido(char* nombreRestaurante, int idPedido){
	t_paquete* resultado = crear_paquete_personalizado(OK);
	//t_list* platos = list_create();
	pedido* miPedido = NULL;
	if(existeRestaurante(nombreRestaurante)){
		log_info(logger, "Existe restaurante %s", nombreRestaurante);
		restaurante* miRestaurante = obtenerRestaurante(nombreRestaurante);

		if(existePedidoEnUnRestaurante(miRestaurante, idPedido)){
			log_info(logger, "Existe en %s el pedido %d", nombreRestaurante, idPedido);
			miPedido = obtenerUnPedido(miRestaurante, idPedido);

			obtenerPlatosDePedido(resultado, miPedido);

		}else{
			log_info(logger, "No existe el pedido %s", nombreRestaurante);
			resultado->codigo_operacion = FAIL;
		}
	}else{
		log_info(logger, "No existe el restaurante %s", nombreRestaurante);
		resultado->codigo_operacion = FAIL;
	}

	return resultado;
}
bool confirmarPedido(char* nombreRestaurante, int idPedido){

	if(existeRestaurante(nombreRestaurante)){
		log_info(logger, "Existe restaurante %s", nombreRestaurante);
		restaurante* miRestaurante = obtenerRestaurante(nombreRestaurante);
		if(existePedidoEnUnRestaurante(miRestaurante, idPedido)){
			log_info(logger, "Existe en %s el pedido %d", nombreRestaurante, idPedido);
			pedido* miPedido = obtenerUnPedido(miRestaurante, idPedido);
			if(miPedido->estado == EN_PROCESO){
				miPedido->estado = CONFIRMADO;
				return true;
			}else{
				log_info(logger, "El estado del pedido no es EN_PROCESO");
				return false;
			}

		}else{
			log_info(logger, "No existe en %s el pedido %d, operacion no completada", nombreRestaurante, idPedido);
			return false;
		}
	}else{
		log_info(logger, "No existe restaurante %s, operacion no completada", nombreRestaurante);
		return false;
	}
}

int platoListo(char* nombreRestaurante, int idPedido, char* platoNombre){

	if(existeRestaurante(nombreRestaurante)){
		log_info(logger, "Existe restaurante %s", nombreRestaurante);
		restaurante* miRestaurante = obtenerRestaurante(nombreRestaurante);
		if(existePedidoEnUnRestaurante(miRestaurante, idPedido)){
			log_info(logger, "Existe en %s el pedido %d", nombreRestaurante, idPedido);
			pedido* miPedido = obtenerUnPedido(miRestaurante, idPedido);
			if(miPedido->estado == CONFIRMADO){
				if(existePlato(miPedido, platoNombre)){
					log_info(logger, "Existe en %s en el pedido %d el plato %s", nombreRestaurante, idPedido, platoNombre);
					plato* miPlato = obtenerPlato(miPedido, platoNombre);
					actualizarCantidadLista(miPlato);
					actualizarEstadoPedido(miPedido);
					return true;
				}else{
					log_info(logger, "No existe en %s en el pedido %d el plato %s", nombreRestaurante, idPedido, platoNombre);
					return false;
				}
			}else{
				log_info(logger, "El estado del pedido no es CONFIRMADO");
				return false;
			}
		}else{
			log_info(logger, "No existe en %s el pedido %d, operacion no completada", nombreRestaurante, idPedido);
			return false;
		}
	}else{
		log_info(logger, "No existe restaurante %s, operacion no completada", nombreRestaurante);
		return false;
	}

}
int finalizarPedido(char* nombreRestaurante, int idPedido){

	if(existeRestaurante(nombreRestaurante)){
		log_info(logger, "Existe restaurante %s", nombreRestaurante);
		restaurante* miRestaurante = obtenerRestaurante(nombreRestaurante);

		if(existePedidoEnUnRestaurante(miRestaurante, idPedido)){
			log_info(logger, "Existe en %s el pedido %d", nombreRestaurante, idPedido);
			pedido* miPedido = obtenerUnPedido(miRestaurante, idPedido);
			borrarPaginasPedido(miPedido);
			borrarPedido(miRestaurante,idPedido);

			return true;
		}
		else{
			log_info(logger, "No existe en %s el pedido %d, operacion no completada", nombreRestaurante, idPedido);
			return false;
		}
	}else{
		log_info(logger, "No existe restaurante %s, operacion no completada", nombreRestaurante);
		return false;
	}
}


/*FUNCIONES AUXILIARES PARA QUE FUNCIONE LO OTRO :D */
void borrarPedido(restaurante* miRestaurante, int idPedido){

	bool esElPedido(pedido* miPedido){
		return miPedido->id_pedido == idPedido;
	}

	list_remove_by_condition(miRestaurante->lista_pedidos, (void*)esElPedido);
	log_info(logger,"Pedido %d eliminado de restaurante %s",idPedido,miRestaurante->nombre);
}


void borrarPaginasPedido(pedido* miPedido){
	t_list* lista_platos = miPedido->lista_platos;
	int n = list_size(lista_platos);

	for(int i = 0; i<n; i++){
		plato* actual = list_get(lista_platos, i);
		borrarPagina(actual);
		list_remove(lista_platos,i);
		log_info(logger,"Página del plato %d eliminada pedido %d",i,miPedido->id_pedido);
	}
}

void borrarPagina(plato* miPlato){

	if(miPlato->presencia){
		int indiceFrame = miPlato->frame;
		frame* miFrame = list_get(framesMemoriaPrincipal,indiceFrame);
		miFrame->libre = true;
	}

	int indiceSwap = miPlato->direccionEnSwap;
	bloqueEnSwap* miBloque = list_get(bloquesEnSwap,indiceSwap);
	miBloque->ocupado = false;

}

void actualizarCantidadLista(plato* miPlato){
	paginaPlato* miPagina;
	if (!miPlato->presencia){
			traerPaginaAMemoria(miPlato);
	}
	sem_wait(s_memoriaPrincipal);
	memcpy(&miPagina, &memoriaPrincipal[miPlato->frame*((int)sizeof(paginaPlato))], sizeof(paginaPlato));
	sem_post(s_memoriaPrincipal);
	log_info(logger,"Cantidad lista actual = %d",miPagina->cantidadLista);
	miPagina->cantidadLista++;
	log_info(logger,"Cantidad lista actualizada = %d",miPagina->cantidadLista);
}

void actualizarEstadoPedido(pedido* miPedido){
	t_list* lista_platos = miPedido->lista_platos;
	int count;
	int n = list_size(lista_platos);
	for(int i = 0; i<n; i++){
		plato* actual = list_get(lista_platos, i);
		paginaPlato* miPagina;

		if (!actual->presencia){
			traerPaginaAMemoria(actual);
		}
		sem_wait(s_memoriaPrincipal);
		memcpy(&miPagina, &memoriaPrincipal[actual->frame*((int)sizeof(paginaPlato))], sizeof(paginaPlato));
		sem_post(s_memoriaPrincipal);

		if(miPagina->cantidadLista == miPagina->totalRespectoPedido){
			count++;
		}
	}

	if(count == list_size(lista_platos)){
		miPedido->estado = FINALIZADO;
		log_info(logger, "El pedido %d esta finalizado", miPedido->id_pedido);
	}
	log_info(logger, "El pedido %d no esta finalizado, resta por terminar %d platos", miPedido->id_pedido, list_size(lista_platos)-count);
}

bool existePlato(pedido* miPedido, char* nombrePlato){
	t_list* lista_platos = miPedido->lista_platos;
	int n = list_size(lista_platos);
	for(int i = 0; i<n; i++){
		plato* actual = list_get(lista_platos, i);
		paginaPlato* miPagina;

		if (!actual->presencia){
			traerPaginaAMemoria(actual);
		}
		sem_wait(s_memoriaPrincipal);
		memcpy(&miPagina, &memoriaPrincipal[actual->frame*((int)sizeof(paginaPlato))], sizeof(paginaPlato));
		sem_post(s_memoriaPrincipal);

		if(strcmp(miPagina->nombrePlato,nombrePlato)==0){
			return true;
		}

	}
	return false;
}

plato* obtenerPlato(pedido* miPedido, char* nombrePlato){
	t_list* lista_platos = miPedido->lista_platos;
	plato* p;
	int n = list_size(lista_platos);
	for(int i = 0; i<n; i++){
		plato* actual = list_get(lista_platos, i);
		paginaPlato* miPagina;

		if (!actual->presencia){
			traerPaginaAMemoria(actual);
		}
		sem_wait(&s_memoriaPrincipal);
		memcpy(&miPagina, &memoriaPrincipal[actual->frame*((int)sizeof(paginaPlato))], sizeof(paginaPlato));
		sem_post(&s_memoriaPrincipal);

		if(strcmp(miPagina->nombrePlato,nombrePlato)==0){
			p = actual;
		}
	}
	return p;
}

bool existeRestaurante(char* nombreRestaurante){
	sem_wait(&s_listaRestaurante);
	int n = list_size(lista_restaurantes);
	for(int i = 0; i<n; i++){
		restaurante* r = list_get(lista_restaurantes, i);
		if(strcmp(r->nombre, nombreRestaurante) == 0){
			sem_post(&s_listaRestaurante);
			return true;
		}
	}
	sem_post(&s_listaRestaurante);
	return false;
}
void obtenerPlatosDePedido(t_paquete* paquete, pedido* pedido){
	t_list* platosDePedido = pedido->lista_platos;
	t_list* infoPedido = list_create();
	list_add(infoPedido, list_get(estados,pedido->estado));

	for (int i = 0; i< list_size(platosDePedido);i++){

		plato* actual = list_get(platosDePedido,i);

		if (!actual->presencia){
			log_info(logger, "El plato %d del pedido %d no esta presente en memoria, procedo a traerlo",i,pedido->id_pedido);
			traerPaginaAMemoria(actual);
		}

		paginaPlato* miPagina;
		sem_wait(s_memoriaPrincipal);
		memcpy(&miPagina, &memoriaPrincipal[actual->frame*((int)sizeof(paginaPlato))], sizeof(paginaPlato));
		sem_post(s_memoriaPrincipal);
		char* datosPedido = "[";
		string_append(&datosPedido,miPagina->nombrePlato);
		string_append(&datosPedido,",");
		string_append(&datosPedido,string_itoa(miPagina->totalRespectoPedido));
		string_append(&datosPedido,",");
		string_append(&datosPedido,string_itoa(miPagina->cantidadLista));
		string_append(&datosPedido,"]");

		list_add(infoPedido,datosPedido);

	}

	paquete->buffer = serializar_lista_nombres(infoPedido);
}

restaurante* obtenerRestaurante(char* nombreRestaurante){
	sem_wait(&s_listaRestaurante);
	int n = list_size(lista_restaurantes);
	restaurante* r;
	for(int i = 0; i<n; i++){
		restaurante* actual = (restaurante*) list_get(lista_restaurantes, i);
		if(strcmp(actual->nombre, nombreRestaurante) == 0){
			r = actual;
		}
	}
	sem_post(&s_listaRestaurante);
	return r;
}

void agregarPedido(int idPedido, restaurante* restaurante){
	pedido* nuevoPedido = generarNuevoPedido(idPedido,restaurante);
	list_add(restaurante->lista_pedidos, nuevoPedido);
	log_info(logger, "Se agregó el pedido N° %d a la lista del restaurante %s",idPedido,restaurante->nombre);
}

pedido* generarNuevoPedido(int idPedido, restaurante* restaurante){
	pedido* miPedido = malloc(sizeof(pedido));
	miPedido->id_pedido = idPedido;
	miPedido->resto_nombre = strdup(restaurante->nombre);
	miPedido->lista_platos = list_create();
	miPedido->estado = EN_PROCESO;
	log_info(logger, "Se generó el pedido N° %d",miPedido->id_pedido);
	return miPedido;
}

void generarRestaurante(char* nombreRestaurante){
	restaurante* r = malloc(sizeof(restaurante));
	r->lista_pedidos = list_create();
	r->nombre = strdup(nombreRestaurante);
	sem_wait(&s_listaRestaurante);
	list_add(lista_restaurantes,r);
	sem_post(&s_listaRestaurante);
	log_info(logger, "Se agregó %s a la lista_restaurantes",nombreRestaurante);
}

bool existePedidoEnUnRestaurante(restaurante* miRestaurante, int idPedido){
	//t_list* lista_pedidos = miRestaurante->lista_pedidos;
	t_list* listaDePedidos = miRestaurante->lista_pedidos;
	int n = list_size(listaDePedidos);
		for(int i = 0; i<n; i++){
			pedido* actual = list_get(miRestaurante->lista_pedidos, i);
			if(actual->id_pedido == idPedido){
				return true;
			}
		}
	return false;
}

pedido* obtenerUnPedido(restaurante* miRestaurante, int idPedido){
	t_list* lista_pedidos = miRestaurante->lista_pedidos;
	pedido* p;
	int n = list_size(lista_pedidos);
	for(int i = 0; i<n; i++){
		pedido* actual = list_get(lista_pedidos, i);
		if(actual->id_pedido == idPedido){
			p = actual;
		}
	}
	return p;
}

void actualizarCantidadTotalPlato(plato* miPlato, int cantidad){
	if(miPlato->presencia){
		int dirEnMem = miPlato->frame * 32;
		paginaPlato* miPaginaPlato = &memoriaPrincipal[dirEnMem];
		miPaginaPlato->totalRespectoPedido += (uint32_t)cantidad;
		sem_wait(&s_listaFrames);
		frame* frameActual = list_get(framesMemoriaPrincipal, miPlato->frame);
		frameActual->modificado = true;
		frameActual->usoReciente = true;
		cambiarInstanteDeUso(frameActual);
		sem_post(&s_listaFrames);
	}else{
		traerPaginaAMemoria(miPlato);
		actualizarCantidadTotalPlato(miPlato, cantidad);
	}

}

void traerPaginaAMemoria(plato* miPlato){
	int direccionSwap = miPlato->direccionEnSwap;
	frame* frameDestino;
	if(tengoUnFrameLibre()){
		frameDestino = primerFrameLibre();
	}else{
		frameDestino = elegirVictima();
	}
	memcpy(&memoriaPrincipal[frameDestino->nroFrame * 32], &memoriaSwap[direccionSwap * 32], sizeof(paginaPlato));
	frameDestino->modificado = false;
	frameDestino->usoReciente = true;
	cambiarInstanteDeUso(frameDestino);
	frameDestino->libre = false;
	miPlato->presencia = true;
	miPlato->frame = frameDestino->nroFrame;
}

bool tengoUnFrameLibre(){
	sem_wait(&s_listaFrames);
	for (int i = 0; list_size(framesMemoriaPrincipal); i++){
		frame* actual = list_get(framesMemoriaPrincipal,i);
		if (actual->libre){
			sem_post(&s_listaFrames);
			return true;
		}
	}
	sem_post(&s_listaFrames);
	return false;
}

frame* primerFrameLibre(){
	frame* libre;
	bool encontreLibre = false;
	int frameActual = 0;
	sem_wait(&s_listaFrames);
	while (frameActual < list_size(framesMemoriaPrincipal) && !encontreLibre){
		frame* actual = list_get(framesMemoriaPrincipal,frameActual);
		if (actual->libre){
			libre = actual;
			encontreLibre = true;
		}
		frameActual++;
	}
	sem_post(&s_listaFrames);
	return libre;
}

void cambiarInstanteDeUso(frame* miFrame){
	miFrame->ultimoUso = momentoDeUsoDePagina();
	log_info(logger,"El frame %d queda con momento de uso = %d",miFrame->nroFrame,miFrame->ultimoUso);
}

double momentoDeUsoDePagina() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

frame* elegirVictima(){
	frame* frameVictima;
	if (strcmp(config_cm->algoritmo,"LRU") == 0){
		frameVictima = aplicarLRU();
	} else{
		frameVictima = aplicarClockMejorado();
	}
	log_info(logger,"Víctima elegida = frame %d por algoritmo %s",frameVictima->nroFrame,config_cm->algoritmo);
	return frameVictima;
}

frame* aplicarLRU(){
	sem_wait(&s_listaFrames);
	frame* victima = (frame*) list_fold(framesMemoriaPrincipal, list_get(framesMemoriaPrincipal, 0), (void*) frameMenosReciente);
	sem_post(&s_listaFrames);
	return victima;
}

frame* frameMenosReciente(frame* frame1, frame* frame2){
	if (frame1->ultimoUso < frame2->ultimoUso){
		return frame1;
	} else{
		return frame2;
	}
}

void moverPuntero(){
	sem_wait(&s_punteroClock);
	log_info(logger,"El puntero arranca la búsqueda en %d",punteroClockEn);
	if (punteroClockEn == (list_size(framesMemoriaPrincipal)-1)){
		punteroClockEn = 0;
	} else{
		punteroClockEn++;
	}
	log_info(logger,"El puntero termina la búsqueda en %d",punteroClockEn);
	sem_post(&s_punteroClock);
}

frame* aplicarClockMejorado(){
	frame* actual;
	//Primera vuelta
	for(int i=0; i < list_size(framesMemoriaPrincipal); i++){
		actual = list_get(framesMemoriaPrincipal,punteroClockEn);
		moverPuntero();
		if (!actual->modificado && !actual->usoReciente){
			return actual;
		}
	}

	//Segunda vuelta

	for(int i=0; i < list_size(framesMemoriaPrincipal); i++){
		actual = list_get(framesMemoriaPrincipal,punteroClockEn);
		moverPuntero();
		if (!actual->usoReciente && actual->modificado){
			return actual;
		} else{
			actual->usoReciente = false;
		}
	}

	return aplicarClockMejorado();

}

int crearPaginaDePlatoEnSwap(char* nombrePlato){
	int inicioBloqueLibre = primerBloqueEnSwapLibre();
	if(inicioBloqueLibre != -1){
		paginaPlato* paginaAGuardar;
		strcpy (paginaAGuardar->nombrePlato, nombrePlato);
		paginaAGuardar->cantidadLista = 0;
		paginaAGuardar->totalRespectoPedido = 0;
		sem_wait(&s_swap);
		memcpy(&memoriaSwap[inicioBloqueLibre], &paginaAGuardar, sizeof(paginaPlato));
		sem_post(&s_swap);
		return inicioBloqueLibre;
	}
	return -1;
}

bool estaLibre(bloqueEnSwap* bloque){
	return !(bloque->ocupado);
}

int primerBloqueEnSwapLibre(){
	sem_wait(&s_listaBloquesSwap);
	if(list_find(bloquesEnSwap, (void*) estaLibre) != NULL){
		bloqueEnSwap* bloque = list_find(bloquesEnSwap, (void*) estaLibre);
		bloque->ocupado = true;
		sem_post(s_listaBloquesSwap);
		return bloque->posRespectoInicio;
	}
	sem_post(&s_listaBloquesSwap);
	return -1;
}


plato* crearYAgregarPlatoAdministrativo(char* nombrePlato, pedido* miPedido, int direccionSwap){
	plato* nuevoPlato = malloc(sizeof(plato));
	nuevoPlato->completo = false;
	nuevoPlato->direccionEnSwap = direccionSwap;
	nuevoPlato->frame = -1;
	nuevoPlato->presencia = false;
	list_add(miPedido->lista_platos,nuevoPlato);
	log_info(logger,"Creado el plato %s en la dirección de swap %d",nombrePlato,direccionSwap);
	return nuevoPlato;
}


void generarListaFrames(){
	int cantidadFrames = (atoi(config_cm->tamanio_memoria) / (int)sizeof(paginaPlato));
	log_info(logger,"Comienza generación de frames");
	for (int i=0; i<cantidadFrames;i++){
		frame* frameGenerado = generarFrame(i);
		list_add(framesMemoriaPrincipal,frameGenerado);
	}
	log_info(logger,"Finaliza generación de %d frames", cantidadFrames);
}

void generarListaBloquesEnSwap(){
	int cantidadBloques = (atoi(config_cm->tamanio_swap) / (int)sizeof(paginaPlato));
	log_info(logger,"Comienza el conteo de bloques de swap");
	for (int i=0; i<cantidadBloques;i++){
		bloqueEnSwap* bloqueEnSwapGenerado = generarBloqueEnSwap(i);
		list_add(bloquesEnSwap,bloqueEnSwapGenerado);
	}
	log_info(logger,"Finaliza generación de %d bloques de swap", cantidadBloques);
}

frame* generarFrame(int nroFrame){ //Función que se utiliza una sola vez: cuando arranca
	frame* frameGenerado = malloc(sizeof(frame));
	frameGenerado->nroFrame = nroFrame;
	frameGenerado->libre = true;
	frameGenerado->usoReciente = false;
	log_info(logger,"Frame %d generado con dirección logica %d",nroFrame,frameGenerado->nroFrame);
	return frameGenerado;

}

bloqueEnSwap* generarBloqueEnSwap(int i){
	bloqueEnSwap* bloqueSwap = malloc(sizeof(bloqueEnSwap));
	bloqueSwap->ocupado = false;
	bloqueSwap->posRespectoInicio = i;
	log_info(logger,"Bloque en swap generado con posición %d",i);
	return bloqueSwap;
}


void liberarFrames(){
	for (int i=0; i<list_size(framesMemoriaPrincipal); i++){
		frame* actual = list_get(framesMemoriaPrincipal,i);
		free(actual);
	}
	log_info(logger,"Frames MP liberados");
}

void liberarBloquesEnSwap(){
	for (int i=0;i<list_size(bloquesEnSwap);i++){
		frame* actual = list_get(bloquesEnSwap,i);
		free(actual);
	}
	log_info(logger,"Bloques swap liberados");
}

void liberarRestaurantes(){
	for (int i=0; i<list_size(lista_restaurantes); i++){
		restaurante* actual = list_get(lista_restaurantes,i);
		log_info(logger,"Liberar pedidos de %s",actual->nombre);
		liberarPlatos(actual->lista_pedidos);
		free(actual);
	}
	log_info(logger,"Restaurantes liberados");
}

void liberarPedidos(t_list* pedidos){
	for (int i=0; i<list_size(pedidos); i++){
		pedido* actual = list_get(pedidos,i);
		log_info(logger,"Liberar platos de pedido %d",actual->id_pedido);
		liberarPlatos(actual->lista_platos);
		free(actual);
	}
	log_info(logger,"Pedidos liberados");
}

void liberarPlatos(t_list* platos){
	for (int i=0; i<list_size(platos); i++){
		pedido* actual = list_get(platos,i);
		free(actual);
	}
	log_info(logger,"Platos liberados");
}


