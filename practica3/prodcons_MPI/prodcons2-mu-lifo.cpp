// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Práctica 3. Implementación de algoritmos distribuidos con MPI
//
// Archivo: prodcons2.cpp
// Implementación del problema del productor-consumidor con
// un proceso intermedio que gestiona un buffer finito y recibe peticiones
// en orden arbitrario
// (versión con un único productor y un único consumidor)
//
// Historial:
// Actualizado a C++11 en Septiembre de 2017
// -----------------------------------------------------------------------------

#include <iostream>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <mpi.h>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int
   // Múltimo de num_productores y num_consumidores
   num_items             = 20,
   tam_vector            = 10,
   num_productores       = 4,
   num_consumidores      = 5,
   etiq_producir         = 0,
   etiq_consumir         = 1;

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio() {
  static default_random_engine generador((random_device())());
  static uniform_int_distribution<int> distribucion_uniforme(min, max);
  return distribucion_uniforme(generador);
}
// ---------------------------------------------------------------------
// producir produce los numeros en secuencia (1,2,3,....)
// y lleva espera aleatorio
int producir(int id) {
   static int contador = 0;
   int num = ((num_items * id) / num_productores) + contador;
   sleep_for(milliseconds(aleatorio<10,100>()));
   cout << "Productor " << id << " ha producido valor: " << num << endl << flush;
   contador++;
   return num;
}
// ---------------------------------------------------------------------

void funcion_productor(int id) {
   unsigned int k = num_items / num_productores;
   for (unsigned int i = 0; i < k; ++i) {
      // producir valor
      int valor_prod = producir(id);
      // enviar valor
      cout << "Productor " << id << " va a enviar valor: " << valor_prod << endl << flush;
      MPI_Ssend( &valor_prod, 1, MPI_INT, num_productores, etiq_producir, MPI_COMM_WORLD );
   }
}
// ---------------------------------------------------------------------

void consumir(int id, int valor_cons) {
   // espera bloqueada
   sleep_for( milliseconds(aleatorio<110,200>()));
   cout << "Consumidor " << id << " ha consumido valor: " << valor_cons << endl << flush ;
}
// ---------------------------------------------------------------------

void funcion_consumidor(int id) {
   int         peticion,
               valor_rec = 1 ;
   MPI_Status  estado ;
   unsigned int k = num_items / num_consumidores;
   for (unsigned int i = 0; i < k; ++i ) {
      MPI_Ssend(&peticion,  1, MPI_INT, num_productores, etiq_consumir, MPI_COMM_WORLD);
      MPI_Recv(&valor_rec, 1, MPI_INT, num_productores, etiq_consumir, MPI_COMM_WORLD, &estado);
      cout << "Consumidor "<< id << " ha recibido valor: " << valor_rec << endl << flush ;
      consumir(id, valor_rec);
   }
}
// ---------------------------------------------------------------------

void funcion_buffer() {
   int        buffer[tam_vector],      // buffer con celdas ocupadas y vacías
              valor,                   // valor recibido o enviado
              primera_libre       = 0, // índice de primera celda libre
              primera_ocupada     = 0, // índice de primera celda ocupada
              num_celdas_ocupadas = 0, // número de celdas ocupadas
              etiqueta;
   MPI_Status estado ;                 // metadatos del mensaje recibido

   for (unsigned int i = 0; i < num_items * 2; ++i) {
      // 1. determinar si puede enviar solo prod., solo cons, o todos
      if (num_celdas_ocupadas == 0)
         etiqueta = etiq_producir;
      else if (num_celdas_ocupadas == tam_vector)
         etiqueta = etiq_consumir;
      else
         etiqueta = MPI_ANY_TAG;

      // 2. recibir un mensaje del emisor o emisores aceptables
      MPI_Recv(&valor, 1, MPI_INT, MPI_ANY_SOURCE, etiqueta, MPI_COMM_WORLD, &estado);

      // 3. procesar el mensaje recibido
      if (estado.MPI_SOURCE < num_productores) {
        buffer[primera_libre] = valor;
        primera_libre = (primera_libre+1) % tam_vector;
        num_celdas_ocupadas++;
        cout << "Buffer ha recibido valor: " << valor << endl;
      } else {
        valor = buffer[primera_ocupada];
        primera_ocupada = (primera_ocupada+1) % tam_vector;
        num_celdas_ocupadas--;
        cout << "Buffer va a enviar valor: " << valor << endl;
        MPI_Ssend(&valor, 1, MPI_INT, estado.MPI_SOURCE, etiq_consumir, MPI_COMM_WORLD);
      }
   }
}

// ---------------------------------------------------------------------

int main( int argc, char *argv[] ) {
   int id_propio, num_procesos_actual;
   int num_procesos_esperado = num_consumidores + num_productores + 1;

   MPI_Init( &argc, &argv );
   MPI_Comm_rank(MPI_COMM_WORLD, &id_propio);
   MPI_Comm_size(MPI_COMM_WORLD, &num_procesos_actual);

   if (num_procesos_esperado == num_procesos_actual &&
     num_items % num_productores == 0 && num_items % num_consumidores == 0) {
      if (id_propio < num_productores)
         funcion_productor(id_propio);
      else if (id_propio == num_productores)
         funcion_buffer();
      else
         funcion_consumidor(id_propio - (num_productores + 1));
   } else {
      if (id_propio == 0) {
        if (num_procesos_esperado != num_procesos_actual)
          cout << "El número de procesos esperados es:    " << num_procesos_esperado << endl
             << "El número de procesos en ejecución es: " << num_procesos_actual << endl;
        if (num_items % num_productores != 0)
          cout << "El número de items no es divisble entre el número de productores" << endl;
        if (num_items % num_consumidores != 0)
          cout << "El número de items no es divisble entre el número de consumidores" << endl;
      }
   }

   MPI_Finalize();
   return 0;
}
