/*
 * bitmap.h
 *
 *  Created on: 6 nov. 2020
 *      Author: utnso
 */

#ifndef BITMAP_H_
#define BITMAP_H_


#include "stdint.h"
#include "stdlib.h"
#include "sindicato.h"
#include <string.h>
#include <stdio.h>
#include <commons/bitarray.h>
#include <unistd.h>
#include<semaphore.h>

sem_t* s_bitmap;
sem_t* s_temporalFile;
t_bitarray* miBitmap;
char* contenidoBitmap;
char* direccionBitmap;
bool existeArchivo(char* path);


t_bitarray* crearBitmap(int cantBloques);

void setBit(t_bitarray* bitmap, int numeroBlock);

bool checkBit(t_bitarray* bitmap, int numeroBlock);

void clearBit(t_bitarray* bitmap, int numeroBlock);
bool existeArchivoBitmap();

#endif /* BITMAP_H_ */
