/*
 * funcionesviejas.c
 *
 *  Created on: 29 nov. 2020
 *      Author: utnso
 */
/*


void cargarDatosMemoriaPrincipalEnPaginaPlato(paginaPlato* miPaginaPlato, int ubicacion){
	memcpy(miPaginaPlato->totalRespectoPedido,memoriaPrincipal[ubicacion],sizeof(uint32_t));
	memcpy(miPaginaPlato->cantidadLista,memoriaPrincipal[ubicacion+4],sizeof(uint32_t));
	memcpy(miPaginaPlato->nombrePlato,memoriaPrincipal[ubicacion+8],24); //24 bytes tiene el nombre
}

void cargarDatosSwapEnPaginaPlato(paginaPlato* miPaginaPlato, int ubicacion){
	memcpy(&miPaginaPlato->totalRespectoPedido,memoriaSwap[ubicacion],sizeof(uint32_t));
	memcpy(&miPaginaPlato->cantidadLista,memoriaSwap[ubicacion+4],sizeof(uint32_t));
	memcpy(&miPaginaPlato->nombrePlato,memoriaSwap[ubicacion+8],24); //24 bytes tiene el nombre
}

void cargarDatosASwap(paginaPlato* miPaginaPlato, int ubicacion){
	memcpy(memoriaSwap[ubicacion],miPaginaPlato->totalRespectoPedido,sizeof(uint32_t));
	memcpy(memoriaSwap[ubicacion+4],miPaginaPlato->cantidadLista,sizeof(uint32_t));
	memcpy(memoriaSwap[ubicacion+8],miPaginaPlato->nombrePlato,24); //24 bytes tiene el nombre
}

void cargarDatosAMemoriaPrincipal(paginaPlato* miPaginaPlato, int ubicacion){
	memcpy(memoriaPrincipal[ubicacion],miPaginaPlato->totalRespectoPedido,sizeof(uint32_t));
	memcpy(memoriaPrincipal[ubicacion+4],miPaginaPlato->cantidadLista,sizeof(uint32_t));
	memcpy(memoriaPrincipal[ubicacion+8],miPaginaPlato->nombrePlato,24); //24 bytes tiene el nombre
}

void traerAMemoria(plato* plato){
	paginaPlato* miPagina;
	int ubicacion = plato->direccionEnSwap;
	sem_wait(s_swap);
	cargarDatosSwapEnPaginaPlato(miPagina,ubicacion);
	sem_post(s_swap);
	cargarAMemoria(miPagina);
	plato->presencia = true;
	cambiarInstanteDeUso(plato);
}

void cargarAMemoria(paginaPlato* miPagina){
	if(tengoUnFrameLibre()){
		int primerFrameLibre = buscarFrameLibre();
		cargarDatosAMemoriaPrincipal(miPagina,primerFrameLibre);
	}else{
		plato* platoVictima = elegirVictima();
		swappearVictimaConMiPagina(miPagina, platoVictima);
	}
}

void swappearVictimaConMiPagina(paginaPlato* miPagina, plato* platoVictima){
	if(platoVictima->modificado){
		cargarDatosASwap(miPagina, platoVictima->direccionEnSwap);
	}
	cargarDatosAMemoriaPrincipal(miPagina, platoVictima->direccionEnMemoriaPrincipal);
}



plato* aplicarLRU(){
	restaurante* primerRestaurante = list_get(lista_restaurantes,0);
	t_list* pedidosRestaurante = primerRestaurante->lista_pedidos;
	pedido* pedidoInicial = list_get(pedidosRestaurante,0);
	t_list* platos = pedidoInicial->lista_platos;

	plato* menosReciente = list_get(platos,0); //Tomo el primero
	double tiempoMasViejo = menosReciente->ultimoUso;

	restaurante* restauranteActual;
	pedido* pedidoActual;
	plato* platoActual;
	for (int i = 1; i < list_size(lista_restaurantes); i++){ //Recorro todos los restaurantes
		restauranteActual = list_get(lista_restaurantes,i);
		for (int j = 1; j < list_size(restauranteActual->lista_pedidos); j++){ //Recorro todos los pedidos
			pedidoActual = list_get(restauranteActual->lista_pedidos,j);
			for (int k = 1; k < list_size(pedidoActual->lista_platos); k++){ //Recorro todos los pLatos
				platoActual = list_get(pedidoActual->lista_platos,k);
				if (platoActual->ultimoUso < tiempoMasViejo){
					tiempoMasViejo = platoActual->ultimoUso;
					menosReciente = list_get(pedidoActual->lista_platos,k);
				}
			}
		}
	}
	return menosReciente;
}









int tamanioActualSwap(){
	int fileSize =0;
	if(swap) {
	  fseek(swap, 0 , SEEK_END);//Me muevo al final
	  fileSize = ftell(swap); // me fijo donde está
	  fseek(swap, 0 , SEEK_SET); // vuelvo al inicio
	}
	return fileSize;
}

int swapConEspacio(){
	return ((tamanioSwap - tamanioActualSwap()) > 32 ); //debo tener lugar para una página (si me quedan 2 bytesno sirve)
}

void generarListaFrames(){
	int cantidadFrames = floor(atoi(config_cm->tamanio_memoria) / 32);
	log_info(logger,"Comienza generación de frames");
	for (int i=0; i<cantidadFrames;i++){
		frame* frameGenerado = generarFrame(i);
		list_add(framesMemoriaPrincipal,frameGenerado);
	}
	log_info(logger,"Finaliza generación de frames");
}

frame* generarFrame(int nroFrame){ //Función que se utiliza una sola vez: cuando arranca
	frame* frameGenerado = malloc(sizeof(frame));
	frameGenerado->nroFrame = nroFrame;
	frameGenerado->libre = true;
	frameGenerado->uso = false;
	frameGenerado->direccion = &memoriaPrincipal[nroFrame*32];
	log_info(logger,"Frame %d generado con dirección %d",nroFrame,frameGenerado->direccion);
	return frameGenerado;

}

void liberarFrames(){
	for (int i=0;i<list_size(framesMemoriaPrincipal);i++){
		frame* actual = list_get(framesMemoriaPrincipal,i);
		free(actual);
	}
}



paginaPlato* paginaDePlato(plato* plato){
	paginaPlato* miPagina;
	if (plato->presencia){
		int ubicacion = plato->direccionEnMemoriaPrincipal;
		sem_wait(s_memoriaPrincipal);
		for(int i=0; i<4;i++){
			miPagina->totalRespectoPedido[i] = memoriaPrincipal[i+ubicacion];
		}
		for(int j=4; j<8; j++){
			miPagina->cantidadLista[j] = memoriaPrincipal[j+ubicacion];
		}
		for(int k=8; k<32; k++){
			miPagina->nombrePlato[k] = memoriaPrincipal[k+ubicacion];
		}

		cargarDatosEnPaginaPlato(miPagina,ubicacion);

		sem_post(s_memoriaPrincipal);
		cambiarInstanteDeUso(plato);
	}else{
		traerAMemoria(plato);
		paginaDePlato(plato); //Llamado recursivo ahora que ya la tengo en memoria
	}
	return miPagina;
}*/
/*
void cargarDatosMemoriaPrincipalEnPaginaPlato(paginaPlato* miPaginaPlato, int ubicacion){
	memcpy(miPaginaPlato->totalRespectoPedido,memoriaPrincipal[ubicacion],sizeof(uint32_t));
	memcpy(miPaginaPlato->cantidadLista,memoriaPrincipal[ubicacion+4],sizeof(uint32_t));
	memcpy(miPaginaPlato->nombrePlato,memoriaPrincipal[ubicacion+8],24); //24 bytes tiene el nombre
}

void cargarDatosSwapEnPaginaPlato(paginaPlato* miPaginaPlato, int ubicacion){
	memcpy(&miPaginaPlato->totalRespectoPedido,memoriaSwap[ubicacion],sizeof(uint32_t));
	memcpy(&miPaginaPlato->cantidadLista,memoriaSwap[ubicacion+4],sizeof(uint32_t));
	memcpy(&miPaginaPlato->nombrePlato,memoriaSwap[ubicacion+8],24); //24 bytes tiene el nombre
}

void cargarDatosASwap(paginaPlato* miPaginaPlato, int ubicacion){
	memcpy(memoriaSwap[ubicacion],miPaginaPlato->totalRespectoPedido,sizeof(uint32_t));
	memcpy(memoriaSwap[ubicacion+4],miPaginaPlato->cantidadLista,sizeof(uint32_t));
	memcpy(memoriaSwap[ubicacion+8],miPaginaPlato->nombrePlato,24); //24 bytes tiene el nombre
}

void cargarDatosAMemoriaPrincipal(paginaPlato* miPaginaPlato, int ubicacion){
	memcpy(memoriaPrincipal[ubicacion],miPaginaPlato->totalRespectoPedido,sizeof(uint32_t));
	memcpy(memoriaPrincipal[ubicacion+4],miPaginaPlato->cantidadLista,sizeof(uint32_t));
	memcpy(memoriaPrincipal[ubicacion+8],miPaginaPlato->nombrePlato,24); //24 bytes tiene el nombre
}

void traerAMemoria(plato* plato){
	paginaPlato* miPagina;
	int ubicacion = plato->direccionEnSwap;
	sem_wait(s_swap);
	cargarDatosSwapEnPaginaPlato(miPagina,ubicacion);
	sem_post(s_swap);
	cargarAMemoria(miPagina);
	plato->presencia = true;
	cambiarInstanteDeUso(plato);
}

void cargarAMemoria(paginaPlato* miPagina){
	if(tengoUnFrameLibre()){
		int primerFrameLibre = buscarFrameLibre();
		cargarDatosAMemoriaPrincipal(miPagina,primerFrameLibre);
	}else{
		plato* platoVictima = elegirVictima();
		swappearVictimaConMiPagina(miPagina, platoVictima);
	}
}

void swappearVictimaConMiPagina(paginaPlato* miPagina, plato* platoVictima){
	if(platoVictima->modificado){
		cargarDatosASwap(miPagina, platoVictima->direccionEnSwap);
	}
	cargarDatosAMemoriaPrincipal(miPagina, platoVictima->direccionEnMemoriaPrincipal);
}

plato* elegirVictima(){
	plato* platoVictima;
	if (strcmp(config_cm->algoritmo,"LRU") == 0){
		platoVictima = aplicarLRU();
	} else{
		platoVictima = aplicarClockMejorado();
	}
	return platoVictima;
}

plato* aplicarLRU(){
	restaurante* primerRestaurante = list_get(lista_restaurantes,0);
	t_list* pedidosRestaurante = primerRestaurante->lista_pedidos;
	pedido* pedidoInicial = list_get(pedidosRestaurante,0);
	t_list* platos = pedidoInicial->lista_platos;

	plato* menosReciente = list_get(platos,0); //Tomo el primero
	double tiempoMasViejo = menosReciente->ultimoUso;

	restaurante* restauranteActual;
	pedido* pedidoActual;
	plato* platoActual;
	for (int i = 1; i < list_size(lista_restaurantes); i++){ //Recorro todos los restaurantes
		restauranteActual = list_get(lista_restaurantes,i);
		for (int j = 1; j < list_size(restauranteActual->lista_pedidos); j++){ //Recorro todos los pedidos
			pedidoActual = list_get(restauranteActual->lista_pedidos,j);
			for (int k = 1; k < list_size(pedidoActual->lista_platos); k++){ //Recorro todos los pLatos
				platoActual = list_get(pedidoActual->lista_platos,k);
				if (platoActual->ultimoUso < tiempoMasViejo){
					tiempoMasViejo = platoActual->ultimoUso;
					menosReciente = list_get(pedidoActual->lista_platos,k);
				}
			}
		}
	}
	return menosReciente;
}




void generarRestaurante(char* nombreRestaurante){
	restaurante* r;
	r->lista_pedidos = list_create();
	r->nombre = strdup(nombreRestaurante);
	list_add(lista_restaurantes,r);
	log_info(logger, "Se agregó %s a la lista_restaurantes",nombreRestaurante);
}

void agregarPedido(int idPedido, restaurante* restaurante){
	pedido* nuevoPedido = generarNuevoPedido(idPedido,restaurante);
	list_add(restaurante->lista_pedidos, nuevoPedido);
	log_info(logger, "Se agregó el pedido N° %d a la lista del restaurante %s",idPedido,restaurante->nombre);
}

pedido* generarNuevoPedido(int idPedido, restaurante* restaurante){
	pedido* miPedido;
	miPedido->id_pedido = idPedido;
	miPedido->resto_nombre = strdup(restaurante->nombre);
	miPedido->lista_platos = list_create();
	log_info(logger, "Se generó el pedido N° %d",miPedido->id_pedido);
	return miPedido;
}

int tamanioActualSwap(){
	int fileSize =0;
	if(swap) {
	  fseek(swap, 0 , SEEK_END);//Me muevo al final
	  fileSize = ftell(swap); // me fijo donde está
	  fseek(swap, 0 , SEEK_SET); // vuelvo al inicio
	}
	return fileSize;
}

int swapConEspacio(){
	return ((tamanioSwap - tamanioActualSwap()) > 32 ); //debo tener lugar para una página (si me quedan 2 bytesno sirve)
}

void generarListaFrames(){
	int cantidadFrames = floor(atoi(config_cm->tamanio_memoria) / 32);
	log_info(logger,"Comienza generación de frames");
	for (int i=0; i<cantidadFrames;i++){
		frame* frameGenerado = generarFrame(i);
		list_add(framesMemoriaPrincipal,frameGenerado);
	}
	log_info(logger,"Finaliza generación de frames");
}

frame* generarFrame(int nroFrame){ //Función que se utiliza una sola vez: cuando arranca
	frame* frameGenerado = malloc(sizeof(frame));
	frameGenerado->nroFrame = nroFrame;
	frameGenerado->libre = true;
	frameGenerado->uso = false;
	frameGenerado->direccion = &memoriaPrincipal[nroFrame*32];
	log_info(logger,"Frame %d generado con dirección %d",nroFrame,frameGenerado->direccion);
	return frameGenerado;

}

void liberarFrames(){
	for (int i=0;i<list_size(framesMemoriaPrincipal);i++){
		frame* actual = list_get(framesMemoriaPrincipal,i);
		free(actual);
	}
}

bool tengoUnFrameLibre(){
	for (int i = 0; list_size(framesMemoriaPrincipal); i++){
		frame* actual = list_get(framesMemoriaPrincipal,i);
		if (actual->libre){
			return true;
		}
	}

	return false;
}

frame* primerFrameLibre(){
	frame* libre;
	bool encontreLibre = false;
	int frameActual = 0;
	while (frameActual < list_size(framesMemoriaPrincipal) && !encontreLibre){
		frame* actual = list_gest(framesMemoriaPrincipal,frameActual);
		if (actual->libre){
			libre = actual;
			encontreLibre = true;
		}
		frameActual++;
	}
	return libre;
}


}
return libre;
}
*/
