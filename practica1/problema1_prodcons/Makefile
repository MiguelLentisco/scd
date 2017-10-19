# ****** Directorios ********
INCLUDE   = include
OBJ       = obj
SRC       = src
BIN       = bin
LATEX     = tex

# ****** Opciones de compilación ********
AR        = ar
CXX       = g++
CXXFLAGS  = -std=c++11 -c -I./$(INCLUDE)
THREAD    = -lpthread

# ****** Compilación de módulos **********
.PHONY: all clean mrproper crearpdf limpiarpdf

all: $(BIN)/prodcons-fifo $(BIN)/prodcons-lifo $(BIN)/prodcons-lifo-modificado $(BIN)/prodcons-fifo-modificado

# --- Ejecutables ---
$(BIN)/prodcons-fifo: $(OBJ)/prodcons-fifo.o $(OBJ)/Semaphore.o
	$(CXX) -o $@ $^ $(THREAD)
$(BIN)/prodcons-lifo: $(OBJ)/prodcons-lifo.o $(OBJ)/Semaphore.o
	$(CXX) -o $@ $^ $(THREAD)
$(BIN)/prodcons-lifo-modificado: $(OBJ)/prodcons-lifo-modificado.o $(OBJ)/Semaphore.o
	$(CXX) -o $@ $^ $(THREAD)
$(BIN)/prodcons-fifo-modificado: $(OBJ)/prodcons-fifo-modificado.o $(OBJ)/Semaphore.o
	$(CXX) -o $@ $^ $(THREAD)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(OBJ)/prodcons-lifo.o: $(INCLUDE)/Semaphore.h

$(OBJ)/prodcons-fifo.o: $(INCLUDE)/Semaphore.h

$(OBJ)/prodcons-lifo-modificado: $(INCLUDE)/Sempahore.

$(OBJ)/prodcons-fifo-modificado: $(INCLUDE)/Sempahore.h


# ************ PDf ************

crearpdf:
	@cd tex && make

limpiarpdf:
	@cd tex && make mrproper

# ************ Limpieza ************
clean:
	-@rm -f $(OBJ)/*
	@echo "Limpiando..."

mrproper: clean
	-@rm -f $(BIN)/*
	@echo "Limpieza completada."
