#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <mutex>
#include <thread>
#include "HoareMonitor.hpp"

using namespace std ;
using namespace HM ;

mutex cerrojo;

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
    int n_sillas;
    int nBarberos;
    int posicion;

  public:
    Barberia(int nBarberos, int n_sillas) {
      posicion = -1;
      colaBarberos = newCondVar();
      colaClientes = newCondVar();
      this->nBarberos = nBarberos;
      clientesEsperando = 0;
      this->n_sillas = n_sillas;
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
        cerrojo.lock();
        cout << "Barbero " << id << " : no hay clientes, me duermo." << endl << flush;
        cerrojo.unlock();
        ++barberosDisponibles;
        colaBarberos.wait();
        cerrojo.lock();
        cout << "Barbero " << id << " : me despierta un cliente" << endl << flush;
        cerrojo.unlock();
        posicion = id;
        --barberosDisponibles;
      }
      else {
        cerrojo.lock();
        cout << "Barbero " << id << " : llamo a un cliente" << endl << flush;
        cerrojo.unlock();
        posicion = id;
        colaClientes.signal();
        --clientesEsperando;
      }
      cerrojo.lock();
      cout << "Barbero " << id << " : voy a cortar el pelo" << endl << flush;
      cerrojo.unlock();
    }

    void finCliente(int id) {
      cerrojo.lock();
      cout << "Barbero " << id << " : he cortado el pelo." << endl << flush;
      cerrojo.unlock();
      colaPuertas[id].signal();
      cerrojo.lock();
      cout << "Barbero " << id << " : miro en la sala de espera" << endl << flush;
      cerrojo.unlock();
    }

    void cortarPelo(int id) {
      cerrojo.lock();
      cout << "                                     Cliente " << id << " : entro en la barbería." << endl << flush;
      cerrojo.unlock();
      if (clientesEsperando < n_sillas) {
        if (barberosDisponibles != 0) {
          cerrojo.lock();
          cout << "                                     Cliente " << id << " : despierto al barbero." << endl << flush;
          cerrojo.unlock();
          colaBarberos.signal();
        }
        else {
          ++clientesEsperando;
          cerrojo.lock();
          cout << "                                     Cliente " << id << " : espero en la sala." << endl << flush;
          cerrojo.unlock();
          colaClientes.wait();
        }
        cerrojo.lock();
        cout << "                                     Cliente " << id << " : me llama el barbero." << endl << flush;
        cerrojo.unlock();
        colaPuertas[posicion].wait();
        cerrojo.lock();
        cout << "                                     Cliente " << id << " : me han cortado el pelo." << endl << flush;
        cerrojo.unlock();
      }
      else {
        cerrojo.lock();
        cout << "                                     Cliente " << id << " : peluquería llena, me voy muy enfadado." << endl << flush;
        cerrojo.unlock();
      }
    }
};

void esperarFueraBarberia(int id) {
  cerrojo.lock();
  cout << "                                     Cliente " << id << " : espero fuera de la barbería." << endl << flush;
  cerrojo.unlock();
  this_thread::sleep_for(chrono::milliseconds(aleatorio<800,2000>()));
  cerrojo.lock();
  cout << "                                     Cliente " << id << " : dejo de esperar fuera." << endl << flush;
  cerrojo.unlock();
}

void cortarPeloACliente(int id) {
  cerrojo.lock();
  cout << "Barbero " << id << " : cortando pelo a cliente." << endl << flush;
  cerrojo.unlock();
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
      << "El barbero y los clientes que van a cortarse el pelo." << endl
      << "--------------------------------------------------------" << endl
      << flush ;

  const int N_CLIENTES = 10;
  const int N_BARBEROS = 1;
  const int N_SILLAS = 5;
  auto barberia = Create<Barberia>(N_BARBEROS, N_SILLAS);
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
