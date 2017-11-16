#include <iostream>
#include <iomanip>
#include <thread>
#include <random>
#include <mutex>
#include <thread>
#include "HoareMonitor.hpp"

using namespace std ;
using namespace HM ;

int* consumidos;
int* producidos;
mutex mtx;

template< int min, int max > int aleatorio() {
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

class MonitorSU : public HoareMonitor {
   private:
     int n_items;
     int items_producidos;
     int items_consumidos;
     int* buffer;
     int tam_buffer;
     int indice_prod;
     int indice_cons;
     CondVar colaProductores;
     CondVar colaConsumidores;

   public:
     MonitorSU(int tam_buffer, int n_items) {
        this->tam_buffer = tam_buffer;
        this->n_items = n_items;
        buffer = new int[tam_buffer];
        colaProductores = newCondVar();
        colaConsumidores = newCondVar();
        indice_prod = 0;
        indice_cons = 0;
        items_consumidos = 0;
        items_producidos = 0;
     }

     ~MonitorSU() {
       delete[] buffer;
     }

     bool deboExtraer() {
        ++items_consumidos;
        return items_consumidos <= n_items;
     }

     bool deboProducir() {
       ++items_producidos;
       return items_producidos <= n_items;
     }

     void insertarDato(int dato) {
       if ((indice_prod - indice_cons) == tam_buffer)
        colaProductores.wait();
       //cout << flush << "BP[" << indice_prod % tam_buffer << "] = " << dato << endl << flush;
       buffer[indice_prod % tam_buffer] = dato;
       ++indice_prod;
       colaConsumidores.signal();
     }

     int extraerDato() {
       if (indice_prod == indice_cons)
        colaConsumidores.wait();
       int dato = buffer[indice_cons % tam_buffer];
       //cout << flush << "                          BC[" << indice_cons % tam_buffer << "] = " << dato << endl << flush;
       ++indice_cons;
       colaProductores.signal();
       return dato;
     }
};

int producirDato(int i) {
  static int n_producidos = 0;
  this_thread::sleep_for(chrono::milliseconds(aleatorio<20,100>()));
  mtx.lock();
  cout << flush << "Hebra (" << i << ") produce: " << n_producidos << endl << flush;
  mtx.unlock();
  int dato = n_producidos;
  ++producidos[n_producidos];
  ++n_producidos;
  return dato;
}

void consumirDato(int dato, int i) {
  this_thread::sleep_for(chrono::milliseconds(aleatorio<20,100>()));
  mtx.lock();
  cout << flush << "                          Hebra (" << i << ") consume: " << dato << endl << flush;
  mtx.unlock();
  ++consumidos[dato];
}

void productor(MRef<MonitorSU> monitor, int id) {
  while (monitor->deboProducir()) {
    int dato = producirDato(id);
    monitor->insertarDato(dato);
  }
}

void consumidor(MRef<MonitorSU> monitor, int id) {
  while (monitor->deboExtraer()) {
    int dato = monitor->extraerDato();
    consumirDato(dato, id);
  }
}

// Parametros supuestos correctos
int main(int args, char* argv[]) {
  if (args != 5) {
    cout << "Uso: número items / número productores / número consumidores / tamaño buffer" << endl;
    return -1;
  }

  int num_items = atoi(argv[1]);
  int num_prods = atoi(argv[2]);
  int num_cons = atoi(argv[3]);
  int tam_buffer = atoi(argv[4]);

  producidos = new int[num_prods * num_items];
  consumidos = new int[num_prods * num_items];

  for (int i = 0; i < num_prods * num_items; ++i) {
    producidos[i] = 0;
    consumidos[i] = 0;
  }

  cout << "--------------------------------------------------------" << endl
      << "Problema de los productores-consumidores (solución FIFO)." << endl
      << "--------------------------------------------------------" << endl
      << flush ;

  MRef<MonitorSU> monitor = Create<MonitorSU>(tam_buffer, num_prods*num_items);

  thread hebrasProductoras[num_prods];
	thread hebrasConsumidoras[num_cons];

  for (int i = 0; i < num_prods; ++i)
    hebrasProductoras[i] = thread(productor, monitor, i);
  for (int i = 0; i < num_cons; ++i)
    hebrasConsumidoras[i] = thread(consumidor, monitor, i);

  for (int i = 0; i < num_prods; ++i)
    hebrasProductoras[i].join();
  for (int i = 0; i < num_cons; ++i)
    hebrasConsumidoras[i].join();

  bool ok = true ;
  cout << "Comprobando contadores ...." << endl;
  for (int i = 0 ; i < num_items ; i++) {
    if (producidos[i] != 1) {
      cout << "Error: valor " << i << " producido " << producidos[i] << " veces." << endl;
      ok = false ;
    }
    if (consumidos[i] != 1) {
      cout << "Error: valor " << i << " consumido " << consumidos[i] << " veces" << endl;
      ok = false;
    }
  }
  if (ok)
     cout << endl << flush << "Solución (aparentemente) correcta." << endl << flush;

  delete[] producidos;
  delete[] consumidos;
  return 0;
}
