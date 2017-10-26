#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <string>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

//-------------------------------------------------------------------------
// Variables compartidas

Semaphore no_ingredientes = 1;
Semaphore consultarApertura = 1;
Semaphore hay_ingrediente[3] = {Semaphore(0), Semaphore(0), Semaphore(0)};
bool estancoCerrado = false;

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
      decodificado = "tabaco";
      break;
    case 1:
      decodificado = "papel";
      break;
    case 2:
      decodificado = "cerillas";
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
  int i = aleatorio<0,2>();
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

void funcion_hebra_estanquero(int n_veces) {
  // Donde vamos a guardar el tipo de ingrediente códificado en intz
  int tipo;
  for (int i = 0; i < n_veces; ++i) {
    // Produzco el tipo de ingrediente
    tipo = producirIngrediente();
    // Espero a que no haya ingredientes para ponerlo
    sem_wait(no_ingredientes);
    // Imprimo por pantalla que se pone el ingrediente
    cout << "\nSe pone el ingrediente: " << ingredientePantalla(tipo) << endl;
    // Avisa a la hebra i, que tiene disponible el ingrediente i que necesita
    sem_signal(hay_ingrediente[tipo]);
  }
  sem_wait(consultarApertura);
  estancoCerrado = true;
  sem_signal(consultarApertura);

  sem_signal()
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
  bool aux;
  sem_wait(consultarApertura);
  aux = estancoCerrado;
  sem_signal(consultarApertura);

  while (!aux) {
    // Fumador se espera a que esté el ingrediente que necesita
    sem_wait(hay_ingrediente[num_fumador]);

    sem_wait(consultarApertura);
    aux = estancoCerrado;
    sem_signal(consultarApertura);

    if (!aux) {
      // Imprime por pantalla que obtiene el ingrediente del estanco
      cout << "\nSe retira el ingrediente: " << ingredientePantalla(num_fumador) << endl;
      // Avisa al estanquero que no hay ingredientes ya
      sem_signal(no_ingredientes);
      // Fuma
      fumar(num_fumador);

      sem_wait(consultarApertura);
      aux = estancoCerrado;
      sem_signal(consultarApertura);
    }
  }
}

//----------------------------------------------------------------------

int main(int args, char* argv[]) {
  cout << "--------------------------------------------------------" << endl
       << "El estanquero y los tres fumadores" << endl
       << "--------------------------------------------------------" << endl
       << flush;

  if (args != 2) {
    cerr << "Números de parámetros incorrecto. Incluir solo el nº de items.";
    return 1;
  }

  int n = atoi(argv[1]);

  thread hebra_estanquera(funcion_hebra_estanquero, n),
         hebra_fumadora1(funcion_hebra_fumador, 0),
         hebra_fumadora2(funcion_hebra_fumador, 1),
         hebra_fumadora3(funcion_hebra_fumador, 2);

  // Realmente no es necesario esto ya que las hebras van a estar siempre
  // funcionando, pero lo pongo de todos modos.
  hebra_estanquera.join();
  hebra_fumadora1.join();
  hebra_fumadora2.join();
  hebra_fumadora3.join();
  return 0;
}
