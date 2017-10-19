#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include <atomic> // Para el índice
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

//**********************************************************************
// variables compartidas

const int num_items = 40 ,   // número de items
	       tam_vec   = 10 ;   // tamaño del buffer
unsigned  cont_prod[num_items] = {0}, // contadores de verificación: producidos
          cont_cons[num_items] = {0}; // contadores de verificación: consumidos
int buffer[tam_vec] = {0}; // Buffer donde ir guardando los datos
Semaphore accesoBuffer = 1; // Semáforo para acceder al buffer
int indicePila = 0; // Indice de la pila/buffer
Semaphore puedoEscribir = tam_vec; 	// Semáforo para ir escribiendo
Semaphore puedoLeer = 0; // Semáforo para ir leyendo


//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

//**********************************************************************
// funciones comunes a las dos soluciones (fifo y lifo)
//----------------------------------------------------------------------

// Modificado para que vaya del 1 al num_items
int producir_dato()
{
   static int contador = 1 ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "producido: " << contador << endl << flush ;

   cont_prod[contador-1] ++ ;
   return contador++ ;
}
//----------------------------------------------------------------------

// Modificado para que vaya del 1 al num_items
void consumir_dato( unsigned dato )
{
   assert( dato <= num_items );
   cont_cons[dato-1] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "                  consumido: " << dato << endl ;

}


//----------------------------------------------------------------------

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < num_items ; i++ )
   {  if ( cont_prod[i] != 1 )
      {  cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {  cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}

//----------------------------------------------------------------------

void funcion_hebra_productora(int n_hebra, int n_iteraciones) {
   for (unsigned i = 0; i < n_iteraciones; ++i) {
		  // Producimos el dato
      int dato = producir_dato();
			// Espera del semáforo de escritura
			sem_wait(puedoEscribir);
			// Escribimos en el buffer e incrementamos el índice
			sem_wait(accesoBuffer);
			buffer[indicePila++] = dato;
			int aux = indicePila - 1;
			sem_signal(accesoBuffer);
			// Mostramos por pantalla la escritura en buffer
			cout << "Productor: " << n_hebra << endl;
			cout << "Escribo en buffer[" << aux << "] = " << dato << endl;
			// Señal para leer del semáforo de lectura
			sem_signal(puedoLeer);
   }
	 // Por pantalla cuando ha acabado de producir
	 cout << "\nHebra productora: " << n_hebra << ", fin." << endl;

}

//----------------------------------------------------------------------

void funcion_hebra_consumidora(int n_hebra, int n_iteraciones) {
   for (unsigned i = 0; i < n_iteraciones; ++i) {
      int dato;
			// Espera al semáfoto de lectura
			sem_wait(puedoLeer);
			// Leemos el dato del buffer, decrementando antes el índice
			sem_wait(accesoBuffer);
			dato = buffer[--indicePila];
			int aux = indicePila;
			sem_signal(accesoBuffer);
			// Mostramos por pantalla la lectura
			cout << "                    Consumidor: " << n_hebra << endl;
			cout << "                    Leo del buffer[" << aux << "] = " << dato << endl;
			// Señal para escribir al semáforo de escritura
			sem_signal(puedoEscribir);
			// Consumimos el dato
      consumir_dato(dato);
    }
		// Por pantalla cuando ha acabado de consumir
		cout << "\nHebra consumidora: " << n_hebra << ", fin." << endl;
}

//----------------------------------------------------------------------

int main(int args, char* argv[]) {
	if (args != 3) {
		cout << "Error, parametros no válidos.";
		return 1;
	}

	int n_productores = atoi(argv[1]);
	int n_consumidores = atoi(argv[2]);

	thread hebrasProductoras[n_productores];
	thread hebrasConsumidoras[n_consumidores];

 	cout << "--------------------------------------------------------" << endl
      << "Problema de los productores-consumidores (solución LIFO)." << endl
      << "--------------------------------------------------------" << endl
      << flush ;

	for (int i = 0; i < n_productores - 1; ++i)
		hebrasProductoras[i] = thread(funcion_hebra_productora, i, (num_items/n_productores));
	hebrasProductoras[n_productores - 1] = thread(funcion_hebra_productora, n_productores - 1, ((num_items/n_productores) + (num_items % n_productores)));

	for (int i = 0; i < n_consumidores - 1; ++i)
		hebrasConsumidoras[i] = thread(funcion_hebra_consumidora, i, (num_items/n_consumidores));
	hebrasConsumidoras[n_consumidores - 1] = thread(funcion_hebra_consumidora, n_consumidores - 1,((num_items/n_consumidores) + (num_items % n_consumidores)));

	for (int i = 0; i < n_productores; ++i)
		hebrasProductoras[i].join();
	for (int i = 0; i < n_consumidores; ++i)
		hebrasConsumidoras[i].join();

	test_contadores();

	return 0;
}
