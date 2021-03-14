#include"bitmap.h"
#include<string.h>
#include<commons/bitarray.h>
#include<magi-library.h>

/**
 * Construye y devuelve un nuevo bitmap, inicializado todo con 0s.
 * Los primeros 8 bytes representan el tamaño del bitmap.
 */

t_bitarray* crearBitmap(int cantBloques) {
	log_info(logger, "Se comienza a crear el Bitmap");

	//Recibimos la cantidad de bloques y lo dividimos por 8 para calcular su equivalente en bytes para la creacion del Bitmap.
	size_t size = (cantBloques/8);


	//Se elije utilizar la configuracion de most significant bit first
	bit_numbering_t mode = LSB_FIRST;

	//Aprovechamos la funcion de las commons para crear el bitarray en el archivo deseado, con el correspondiente tamaño.
	t_bitarray* punteroABitmap = bitarray_create_with_mode(contenidoBitmap, size, mode);

	if (existeArchivoBitmap()){
		actualizarContenidoBitmap();
	} else{
		FILE* archivoBitmap = fopen(direccionBitmap,"wb");
		fclose(archivoBitmap);
		for(int j=0; j<cantBloques; j++){
			clearBit(punteroABitmap, j);
		}
		actualizarArchivoBitmap();
	}

	log_info(logger, "Bitmap nuevo creado.");

	return punteroABitmap;
}

bool existeArchivoBitmap(){
    FILE* file = fopen(direccionBitmap, "rb");

    if (file != NULL){
        fclose(file);
        return true;
    }
    return false;
}


bool existeArchivo(char* path){
    FILE* file = fopen(path, "r");

    if (file != NULL){
        fclose(file);
        return true;
    }
    return false;
}

void setBit(t_bitarray* miBitmap, int numeroBlock) {

	//Pone el bit de la posicion numeroBlock en 1.
	sem_wait(&s_bitmap);
	actualizarContenidoBitmap();
	bitarray_set_bit(miBitmap, numeroBlock);
	actualizarArchivoBitmap();
	sem_post(&s_bitmap);
	log_info(logger, "El bit %d ahora está en 1 -> bloque ocupado", numeroBlock);

}


bool checkBit(t_bitarray* miBitmap, int numeroBlock) {

	//Queda esperando a que el bitmap pueda se abierto.
	sem_wait(&s_bitmap);
	actualizarContenidoBitmap();
	//Revisa el estado de un bit en particular.
	bool estado = bitarray_test_bit(miBitmap, numeroBlock);
	sem_post(&s_bitmap);
	log_info(logger, "El estado del bloque %d es %d.",numeroBlock, estado);

	return estado;
}

void clearBit(t_bitarray* miBitmap, int numeroBlock) {

	//pongo el bit seleccionado en 0
	sem_wait(&s_bitmap);
	actualizarContenidoBitmap();
	bitarray_clean_bit(miBitmap, numeroBlock);
	actualizarArchivoBitmap();
	sem_post(&s_bitmap);
	log_info(logger, "El bit %d ahora está en 0 -> bloque vacío", numeroBlock);


}

void actualizarArchivoBitmap(){
	FILE* archivoBitmap = fopen(direccionBitmap,"wb");
	fwrite((void*)contenidoBitmap, (size_t)cantBloques/8, 1 ,archivoBitmap);
	fclose(archivoBitmap);
	log_info(logger, "Archivo Bitmap actualizado.");
}

void actualizarContenidoBitmap(){
	FILE* archivoBitmap = fopen(direccionBitmap,"rb");
	fread((void*)contenidoBitmap, (size_t)cantBloques/8, 1 ,archivoBitmap);
	//log_info(logger,"contenidoBitmap %s",contenidoBitmap);
	fclose(archivoBitmap);

	log_info(logger, "Contenido Bitmap actualizado.");
}
