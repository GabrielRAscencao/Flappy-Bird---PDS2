# Variáveis
CXX = g++
CXXFLAGS = -I C:\allegro\include
LDFLAGS = C:\allegro\lib\liballegro_monolith.dll.a

# Arquivos
SRC = src/main.cpp
OBJ = src/main.o
EXE = bin/programCpp.exe

# Regra padrão
all: $(EXE)

# Linkagem
$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compilação
$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpar arquivos
clean:
	del $(OBJ) $(EXE)
