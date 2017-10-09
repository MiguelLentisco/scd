# ***** Variables *****
H_FILES = $(wildcard $(INCLUDE)/*.h)

# ****** Directorios ********
INCLUDE   = include
OBJ       = obj
SRC       = src
BIN       = bin

# ****** Opciones de compilación ********
AR        = ar
CXX       = g++
CXXFLAGS  = -std=c++11 -c -I./$(INCLUDE)
RM	  		= rm -f
THREAD 		= -lpthread

# ****** Compilación de módulos **********

.PHONY: all clean mrproper

all: $(BIN)/prodcons-fifo $(BIN)/prodcons-lifo

# --- Ejecutables ---

$(BIN)/prodcons-fifo: $(OBJ)/prodcons-fifo.o $(OBJ)/Semaphore.o
	$(CXX) -o $@ $^ $(THREAD)
$(BIN)/prodcons-lifo: $(OBJ)/prodcons-lifo.o $(OBJ)/Semaphore.o
	$(CXX) -o $@ $^ $(THREAD)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(OBJ)/prodcons-lifo.o: $(INCLUDE)/Semaphore.h

$(OBJ)/prodcons-fifo.o: $(INCLUDE)/Semaphore.h

# ************ Limpieza ************

clean:
	-@rm -f $(OBJ)/*
	@echo "Limpiando..."

mrproper: clean
	-@rm -f $(BIN)/*
	@echo "Limpieza completada."
