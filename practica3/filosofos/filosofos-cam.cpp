#include <mpi.h>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <iostream>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

const int
   num_filosofos = 5 ,
   num_procesos  = 2*num_filosofos + 1,
   etiq_sentarse = 0,
   etiq_tenedor = 1,
   etiq_levantarse = 2,
   id_camarero = num_procesos - 1;

template< int min, int max > int aleatorio() {
  static default_random_engine generador((random_device())());
  static uniform_int_distribution<int> distribucion_uniforme(min, max);
  return distribucion_uniforme(generador);
}

// ---------------------------------------------------------------------

void funcion_filosofos(int id) {
  int id_ten_izq = (id+1)              % (num_procesos - 1), //id. tenedor izq.
      id_ten_der = (id+num_procesos-2) % (num_procesos - 1); //id. tenedor der.

  while (true) {
    cout << "Filósofo " << id << " solicita sentarse en la mesa." << endl;
    MPI_Ssend(&id, 1, MPI_INT, id_camarero, etiq_sentarse, MPI_COMM_WORLD);
    cout << "Filósofo " << id << " se ha sentado en la mesa." << endl;

    cout << "Filósofo " << id << " solicita ten. izq." << id_ten_izq << endl;
    MPI_Ssend(&id, 1, MPI_INT, id_ten_izq, etiq_tenedor, MPI_COMM_WORLD);
    cout << "Filósofo " << id << " solicita ten. der." << id_ten_der << endl;
    MPI_Ssend(&id, 1, MPI_INT, id_ten_der, etiq_tenedor, MPI_COMM_WORLD);

    cout << "Filósofo " << id << " comienza a comer." << endl;
    sleep_for(milliseconds(aleatorio<10,100>()));

    cout << "Filósofo " << id << " suelta ten. izq." << id_ten_izq << endl;
    MPI_Ssend(&id, 1, MPI_INT, id_ten_izq, etiq_tenedor, MPI_COMM_WORLD);
    cout<< "Filósofo " << id << " suelta ten. der." << id_ten_der << endl;
    MPI_Ssend(&id, 1, MPI_INT, id_ten_der, etiq_tenedor, MPI_COMM_WORLD);

    cout << "Filósofo " << id << " solicita levantarse de la mesa." << endl;
    MPI_Ssend(&id, 1, MPI_INT, id_camarero, etiq_levantarse, MPI_COMM_WORLD);
    cout << "Filósofo" << id << " se levanta de la mesa." << endl;

    cout << "Filosofo " << id << " comienza a pensar." << endl;
    sleep_for(milliseconds(aleatorio<10,100>()));
 }
}
// ---------------------------------------------------------------------

void funcion_tenedores(int id) {
  int id_filosofo;
  MPI_Status estado;

  while (true) {
     MPI_Recv(&id_filosofo, 1, MPI_INT, MPI_ANY_SOURCE, etiq_tenedor, MPI_COMM_WORLD, &estado);
     cout <<"Ten. " << id <<" ha sido cogido por filo. " << id_filosofo << endl;

     MPI_Recv(&id_filosofo, 1, MPI_INT, id_filosofo, etiq_tenedor, MPI_COMM_WORLD, &estado);
     cout <<"Ten. "<< id << " ha sido liberado por filo. " << id_filosofo << endl ;
  }
}
// ---------------------------------------------------------------------

void funcion_camarero() {
  int s = 0;
  MPI_Status estado;
  int etiqueta;
  int id;

  while (true) {
    if (s < num_filosofos - 1)
      etiqueta = MPI_ANY_TAG;
    else
      etiqueta = etiq_levantarse;

    MPI_Recv(&id, 1, MPI_INT, MPI_ANY_SOURCE, etiqueta, MPI_COMM_WORLD, &estado);

    switch (estado.MPI_TAG) {
      case etiq_levantarse:
        cout << "El camarero levanta de la mesa al filósofo " << id << endl;
        --s;
        break;
      case etiq_sentarse:
        cout << "El camarero sienta a la mesa al filósofo " << id << endl;
        ++s;
        break;
    }
  }
}

// ---------------------------------------------------------------------

int main( int argc, char** argv ) {
   int id_propio, num_procesos_actual;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &id_propio);
   MPI_Comm_size(MPI_COMM_WORLD, &num_procesos_actual);

   if (num_procesos == num_procesos_actual) {
      if (id_propio == id_camarero)
         funcion_camarero();
      else if (id_propio % 2 == 0)          // si es par
         funcion_filosofos(id_propio); //   es un filósofo
      else                               // si es impar
         funcion_tenedores(id_propio); //   es un tenedor
   } else {
      if (id_propio == 0)
        cout << "El número de procesos esperados es:    " << num_procesos << endl
             << "El número de procesos en ejecución es: " << num_procesos_actual << endl
             << "Programa abortado." << endl;
   }

   MPI_Finalize();
   return 0;
}

// ---------------------------------------------------------------------
