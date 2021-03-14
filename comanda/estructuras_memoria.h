/*
 * estructuras_memoria.h
 *
 *  Created on: 3 nov. 2020
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_MEMORIA_H_
#define ESTRUCTURAS_MEMORIA_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<sys/time.h>
#include<unistd.h>
#include <string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<stdint.h>
#include<readline/readline.h>
#include<magi-library.h>
#include<semaphore.h>
#include "comanda.h"
#include <math.h>

//Semáforos y variables globales
sem_t* s_memoriaPrincipal;
sem_t* s_swap;
sem_t* s_listaFrames;
sem_t* s_listaRestaurante;
sem_t* s_punteroClock;
sem_t* s_listaBloquesSwap;

t_list* estados;

int punteroClockEn;

//Estructuras de datos

typedef struct{
	char* nombre;
	t_list* lista_pedidos;
}restaurante;

typedef struct {
	char* resto_nombre;
	int id_pedido; //antes era char
	estado estado;
	t_list* lista_platos;
}pedido;

typedef struct {
	//char* plato_nombre; No tenemos que tener el nombre en la tabla de paginas
	bool presencia; //true - false
	bool completo; //arranca false. Se vuelve true cuando cantidadLista = totalRespectoPedido
	int direccionEnSwap;//la posicion se obtiene multiplicando por 32
	int frame;
}plato;

typedef struct{
	int nroFrame;
	bool libre;
	bool usoReciente;
	double ultimoUso;//con el contador de usos
	bool modificado;
} frame;

typedef struct {
	uint32_t totalRespectoPedido;
	uint32_t cantidadLista;
	char nombrePlato[24];
}paginaPlato;

typedef struct {
	 int posRespectoInicio;
	 bool ocupado;
}bloqueEnSwap;

paginaPlato* memoriaPrincipal;
paginaPlato* memoriaSwap;

/*FUNCIONES COMUNICACION COMANDA */
bool guardarPedido(char* nombreRestaurante, int idPedido);
bool guardarPlato(char* nombreRestaurante, int idPedido, char* nombrePlato, int cantidad);
bool confirmarPedido(char* nombreRestaurante, int idPedido);

t_paquete* obtenerPedido(char* nombreRestaurante, int idPedido);

int platoListo(char* nombreRestaurante, int idPedido, char* platoNombre);
int finalizarPedido(char* nombreRestaurante, int idPedido);
int crearPaginaDePlatoEnSwap(char* nombrePlato);
int primerBloqueEnSwapLibre();
double momentoDeUsoDePagina();

/*FUNCIONES AUXILIARES */
void borrarPedido(restaurante* miRestaurante, int idPedido);
void borrarPaginasPedido(pedido* miPedido);
void borrarPagina(plato* miPlato);

void actualizarCantidadLista(plato* miPlato);
void actualizarEstadoPedido(pedido* miPedido);
void actualizarCantidadTotalPlato(plato* miPlato, int cantidad);

void cambiarInstanteDeUso(frame* miFrame);
void moverPuntero();

void traerPaginaAMemoria(plato* miPlato);
void agregarPedido(int idPedido, restaurante* restaurante);
void generarRestaurante(char* nombreRestaurante);
void obtenerPlatosDePedido(t_paquete* paquete, pedido* pedido);

void generarListaFrames();
void generarListaBloquesEnSwap();
void liberarFrames();
void liberarBloquesEnSwap();
void liberarRestaurantes();
void liberarPedidos(t_list* pedidos);
void liberarPlatos(t_list* platos);

bool existePlato(pedido* miPedido, char* nombrePlato);
bool existeRestaurante(char* nombreRestaurante);
bool existePedidoEnUnRestaurante(restaurante* miRestaurante, int idPedido);
bool tengoUnFrameLibre();
bool estaLibre(bloqueEnSwap* bloque);

plato* obtenerPlato(pedido* miPedido, char* nombrePlato);
plato* crearYAgregarPlatoAdministrativo(char* nombrePlato, pedido* miPedido, int direccionSwap);

restaurante* obtenerRestaurante(char* nombreRestaurante);

pedido* generarNuevoPedido(int idPedido, restaurante* restaurante);
pedido* obtenerUnPedido(restaurante* miRestaurante, int idPedido);


frame* primerFrameLibre();
frame* elegirVictima();
frame* aplicarLRU();
frame* frameMenosReciente(frame* frame1, frame* frame2);
frame* aplicarClockMejorado();
frame* generarFrame(int nroFrame);

bloqueEnSwap* generarBloqueEnSwap(int i);

#endif /* ESTRUCTURAS_MEMORIA_H_ */
