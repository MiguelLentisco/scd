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

all: $(BIN)/fumadores-estanquero $(BIN)/fumadores-estanquero-modificado

# --- Ejecutables ---

$(BIN)/fumadores-estanquero-modificado: $(OBJ)/fumadores-estanquero-modificado.o $(OBJ)/Semaphore.o
	$(CXX) -o $@ $^ $(THREAD)

$(BIN)/fumadores-estanquero: $(OBJ)/fumadores-estanquero.o $(OBJ)/Semaphore.o
	$(CXX) -o $@ $^ $(THREAD)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(OBJ)/fumadores-estanquero.o: $(INCLUDE)/Semaphore.h

$(OBJ)/fumadores-estanquero-modificado.o: $(INCLUDE)/Semaphore.h

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
