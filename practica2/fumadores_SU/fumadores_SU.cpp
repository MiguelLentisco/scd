#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <mutex>
#include <thread>
#include "HoareMonitor.hpp"

using namespace std ;
using namespace HM ;

class Estanco : public HoareMonitor {
  private:
    CondVar colaEstanquero;
    CondVar colaFumadores[3];
    int ingrediente;

  public:
    Estanco() {
      colaEstanquero = newCondVar();
      for (int i = 0; i < 3; ++i)
        colaFumadores[i] = newCondVar();
      ingrediente = -1;
    }

    void obtenerIngrediente(int n) {
      if (ingrediente != n)
        colaFumadores[n].wait();
      ingrediente = -1;
      colaEstanquero.signal();
    }

    void ponerIngrediente(int n) {
      ingrediente = n;
      colaFumadores[n].signal();
    }

    void esperarRecogidaIngrediente() {
      if (ingrediente != -1)
        colaEstanquero.wait();
    }
};


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

int producirIngrediente() {
  // Ingrediente aleatorio producido
  int i = aleatorio<0,2>();
  // Calcular milisegundos aleatorios de duración de servir un ingredientePantalla
  chrono::milliseconds duracion_servir( aleatorio<200,400>() );
  // Informa que va a producir
  cout << "Estanquero: empieza a producir (" << duracion_servir.count() << " milisegundos)" << endl << flush;
  // Bloquea la hebra durante el tiempo de `duracion_servir` milisegundos
  this_thread::sleep_for(duracion_servir);
  // Informa de que ha terminado de servir
  cout << "Estanquero: termina de producir, comienza espera de consumo." << endl << flush;
  // Devuelve el tipo de ingrediente producido
  return i;
}

// Función que simula la acción de fumar, como un retardo aleatoria de la hebra
void fumar(int num_fumador) {
   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<500,1000>() );
   // informa de que comienza a fumar
    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl << flush;
   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );
   // informa de que ha terminado de fumar
    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl << flush;
}

// Función que ejecuta la hebra del fumador
void funcion_hebra_fumador(MRef<Estanco> estanco, int num_fumador) {
  // Bucle infinito de fumar
  while(true) {
    // Fumador se espera a que esté el ingrediente que necesita
    estanco->obtenerIngrediente(num_fumador);
    // Imprime por pantalla que obtiene el ingrediente del estanco
    cout << "\nSe retira el ingrediente: " << ingredientePantalla(num_fumador) << endl << flush;
    // Fuma
    fumar(num_fumador);
  }
}

// Función que ejecuta la hebra del estanquero
void funcion_hebra_estanquero(MRef<Estanco> estanco) {
  // Donde vamos a guardar el tipo de ingrediente códificado en intz
  int i;
  // Bucle infinito de producción
  while(true) {
    // Produzco el tipo de ingrediente
    i = producirIngrediente();
    // Espero a que no haya ingredientes para ponerlo
    estanco->ponerIngrediente(i);
    // Imprimo por pantalla que se pone el ingrediente
    cout << "\nSe pone el ingrediente: " << ingredientePantalla(i) << endl << flush;
    // Avisa a la hebra i, que tiene disponible el ingrediente i que necesita
    estanco->esperarRecogidaIngrediente();
  }
}

int main() {
  cout << "--------------------------------------------------------" << endl
       << "El estanquero y los tres fumadores" << endl
       << "--------------------------------------------------------" << endl
       << flush;

  auto estanco = Create<Estanco>();

  thread hebra_estanquera(funcion_hebra_estanquero, estanco),
         hebra_fumadora1(funcion_hebra_fumador, estanco, 0),
         hebra_fumadora2(funcion_hebra_fumador, estanco, 1),
         hebra_fumadora3(funcion_hebra_fumador, estanco, 2);

  // Realmente no es necesario esto ya que las hebras van a estar siempre
  // funcionando, pero lo pongo de todos modos.
  hebra_estanquera.join();
  hebra_fumadora1.join();
  hebra_fumadora2.join();
  hebra_fumadora3.join();
  return 0;
}
