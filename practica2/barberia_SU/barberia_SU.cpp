#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <mutex>
#include <thread>
#include "HoareMonitor.hpp"

using namespace std ;
using namespace HM ;

template< int min, int max > int aleatorio() {
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

class Barberia : public HoareMonitor {
  private:
    CondVar colaBarberos;
    CondVar colaClientes;
    CondVar* colaPuertas;
    int clientesEsperando;
    int barberosDisponibles;
    int maxClientes;
    int nBarberos;
    int posicion;


  public:
    Barberia(int nBarberos, int nMaxClientes) {
      posicion = -1;
      colaBarberos = newCondVar();
      colaClientes = newCondVar();
      this->nBarberos = nBarberos;
      clientesEsperando = 0;
      maxClientes = nMaxClientes;
      colaPuertas = new CondVar[nBarberos];
      for (int i = 0; i < nBarberos; ++i)
        colaPuertas[i] = newCondVar();
      barberosDisponibles = 0;
    }

    ~Barberia() {
      delete[] colaPuertas;
    }

    void siguienteCliente(int id) {
      if (clientesEsperando == 0) {
        cout << "Barbero " << id << " : no hay clientes, me duermo." << endl << flush;
        ++barberosDisponibles;
        colaBarberos.wait();
        cout << "Barbero " << id << " : me despierta un cliente" << endl << flush;
        posicion = id;
        --barberosDisponibles;
      }
      else {
        cout << "Barbero " << id << " : llamo a un cliente" << endl << flush;
        posicion = id;
        colaClientes.signal();
        --clientesEsperando;
      }
      cout << "Barbero " << id << " : voy a cortar el pelo" << endl << flush;
    }

    void finCliente(int id) {
      cout << "Barbero " << id << " : he cortado el pelo." << endl << flush;
      colaPuertas[id].signal();
      cout << "Barbero " << id << " : miro en la sala de espera" << endl << flush;
    }

    void cortarPelo(int id) {
      if (clientesEsperando < maxClientes) {
        if (barberosDisponibles != 0) {
          cout << "                                     Cliente " << id << " : despierto al barbero." << endl << flush;
          colaBarberos.signal();
        }
        else {
          ++clientesEsperando;
          cout << "                                     Cliente " << id << " : espero en la sala." << endl << flush;
          colaClientes.wait();
        }
        cout << "                                     Cliente " << id << " : me llama el barbero." << endl << flush;
        colaPuertas[posicion].wait();
        cout << "                                     Cliente " << id << " : me han cortado el pelo." << endl << flush;
      }
      else {
        cout << "                                     Cliente " << id << " : peluquería llena." << endl << flush;
      }
    }
};

void esperarFueraBarberia(int id) {
  cout << "                                     Cliente " << id << " : espero fuera de la barbería." << endl << flush;
  this_thread::sleep_for(chrono::milliseconds(aleatorio<200,1000>()));
  cout << "                                     Cliente " << id << " : entro en la barbería." << endl << flush;
}

void cortarPeloACliente(int id) {
  cout << "Barbero " << id << " : cortando pelo a cliente." << endl << flush;
  this_thread::sleep_for(chrono::milliseconds(aleatorio<200,1000>()));
}

void hebra_cliente(MRef<Barberia> barberia, int id) {
  while (true) {
    barberia->cortarPelo(id);
    esperarFueraBarberia(id);
  }
}

void hebra_barbero(MRef<Barberia> barberia, int id) {
  while(true) {
    barberia->siguienteCliente(id);
    cortarPeloACliente(id);
    barberia->finCliente(id);
  }
}

int main() {
  cout << "--------------------------------------------------------" << endl
      << "Los barberos y los clientes que van a cortarse el pelo." << endl
      << "--------------------------------------------------------" << endl
      << flush ;

  const int N_CLIENTES = 6;
  const int N_BARBEROS = 2;
  const int N_MAX_CLIENTES = 2;
  auto barberia = Create<Barberia>(N_BARBEROS, N_MAX_CLIENTES);
  thread clientes[N_CLIENTES];
  thread barberos[N_BARBEROS];

  for (int i = 0; i < N_CLIENTES; ++i)
    clientes[i] = thread(hebra_cliente, barberia, i);
  for (int i = 0; i < N_BARBEROS; ++i)
    barberos[i] = thread(hebra_barbero, barberia, i);
  for (int i = 0; i < N_CLIENTES; ++i)
    clientes[i].join();
  for (int i = 0; i < N_BARBEROS; ++i)
    barberos[i].join();
}
