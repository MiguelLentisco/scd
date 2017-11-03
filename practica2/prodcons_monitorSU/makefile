

all: prodcons_su_lifo prodcons_su_fifo

prodcons_su_fifo: prodcons_su_fifo.cpp HoareMonitor.cpp HoareMonitor.hpp
	g++ -std=c++11 -I./ prodcons_su_fifo.cpp HoareMonitor.cpp -o prodcons_su_fifo -pthread

prodcons_su_lifo: prodcons_su_lifo.cpp HoareMonitor.cpp HoareMonitor.hpp
	g++ -std=c++11 -I./ prodcons_su_lifo.cpp HoareMonitor.cpp -o prodcons_su_lifo -pthread
