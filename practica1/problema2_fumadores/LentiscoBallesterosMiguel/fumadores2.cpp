#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <string>
#include <atomic>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

//-------------------------------------------------------------------------
// Variables compartidas

Semaphore no_ingredientes = 1;
Semaphore hay_ingrediente[4] = {Semaphore(0), Semaphore(0), Semaphore(0), Semaphore(0)};
atomic<int> cigarrosTotales;

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio() {
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

//-------------------------------------------------------------------------
// Función para transformar el índice en el ingrediente codificado en String
// Recordamos que 0 es tabaco, 1 es papel y 2 cerillas.

string ingredientePantalla(int i) {
  string decodificado;
  switch(i) {
    case 0:
      decodificado = "papel";
      break;
    case 1:
      decodificado = "cerillas";
      break;
    case 2:
      decodificado = "tabaco";
      break;
    case 3:
      decodificado = "boquillas";
      break;
    default:
      decodificado = "error";
      break;
  }
  return decodificado;
}

//--------------------------------------------------duracion_fumar-----------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

int producirIngrediente() {
  // Ingrediente aleatorio producido
  int i = aleatorio<0,3>();
  // Calcular milisegundos aleatorios de duración de servir un ingredientePantalla
  chrono::milliseconds duracion_servir( aleatorio<200,400>() );
  // Informa que va a producir
  cout << "Estanquero: empieza a producir (" << duracion_servir.count() << " milisegundos)" << endl;
  // Bloquea la hebra durante el tiempo de `duracion_servir` milisegundos
  this_thread::sleep_for(duracion_servir);
  // Informa de que ha terminado de servir
  cout << "Estanquero: termina de producir, comienza espera de consumo." << endl;
  // Devuelve el tipo de ingrediente producido
  return i;
}

//----------------------------------------------------------------------
// Función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero() {
  // Donde vamos a guardar el tipo de ingrediente códificado en intz
  int i;
  // Bucle infinito de producción
  while(true) {
    // Produzco el tipo de ingrediente
    i = producirIngrediente();
    // Espero a que no haya ingredientes para ponerlo
    sem_wait(no_ingredientes);
    // Imprimo por pantalla que se pone el ingrediente
    cout << "\nSe pone el ingrediente: " << ingredientePantalla(i) << endl;
    // Avisa a la hebra i, que tiene disponible el ingrediente i que necesita
    sem_signal(hay_ingrediente[i]);
  }
}

//-------------------------------------------------------------------------

// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

void fumar(int num_fumador) {
   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<500,1000>() );
   // informa de que comienza a fumar
    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;
   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );
   // informa de que ha terminado de fumar
    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador

void funcion_hebra_fumador(int num_fumador) {
  // Bucle infinito de fumar
  while(true) {
    // Fumador se espera a que esté el ingrediente que necesita
    sem_wait(hay_ingrediente[num_fumador]);

    // Imprime por pantalla que obtiene el ingrediente del estanco
    cout << "\nSe retira el ingrediente: " << ingredientePantalla(num_fumador) << endl;

    // Si es par fumo y aviso después al estanquero (es atomic)
    if (((cigarrosTotales % 2) == 0)) {
      // Fuma
      fumar(num_fumador);
      // Suma al contador
      cigarrosTotales++;
      // Avisa al estanquero que no hay ingredientes ya
      sem_signal(no_ingredientes);
    }
    // Si es impar, aviso al estanquero y fumo
    else  {
      // Aviso al estanquero que no hay ingredientes
      sem_signal(no_ingredientes);
      // Fumo
      fumar(num_fumador);
      // Sumo al contador;
      cigarrosTotales++;
    }
    // Muestra por pantalla los cigarros fumados por el momento
    cout << "\nCigarros fumados por el momento: " << cigarrosTotales << endl;
  }
}

//----------------------------------------------------------------------

int main() {
  cout << "--------------------------------------------------------" << endl
       << "El estanquero y los cuatro fumadores" << endl
       << "--------------------------------------------------------" << endl
       << flush;

  int n_fumadores = 4;
  cigarrosTotales = 0;
  thread hebras_fumadoras[n_fumadores];

  thread hebra_estanquera(funcion_hebra_estanquero);

  for (int i = 0; i < n_fumadores; ++i)
    hebras_fumadoras[i] = thread(funcion_hebra_fumador, i);

  // Realmente no es necesario esto ya que las hebras van a estar siempre
  // funcionando, pero lo pongo de todos modos.
  hebra_estanquera.join();
  for (int i = 0; i < n_fumadores; ++i)
    hebras_fumadoras[i].join();
  return 0;
}
