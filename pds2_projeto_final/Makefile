# Caminhos
CXX = g++
SRC_DIR = src
BIN_DIR = bin
INC_DIR = include
ALLEGRO_DIR = C:/allegro

# Fontes com caminho completo
SRCS = $(addprefix $(SRC_DIR)/, \
    main.cpp \
    Bird.cpp \
    GameObject.cpp \
    Scenario.cpp \
    GameEngine.cpp \
    Menu.cpp \
    GameOverScreen.cpp \
    Player.cpp \
    PlayerManager.cpp \
    Pipe.cpp \
    ScoreManager.cpp \
)
# Executável
TARGET = $(BIN_DIR)/flappy.exe

# Flags de compilação e linkagem
CXXFLAGS = -I$(INC_DIR) -I$(ALLEGRO_DIR)/include
LDFLAGS = $(ALLEGRO_DIR)/lib/liballegro_monolith.dll.a

# Regra principal
all: $(TARGET)

# Criar pasta bin se não existir
$(BIN_DIR):
	mkdir $(BIN_DIR)

# Compilar e linkar diretamente
$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@ $(LDFLAGS)

# Limpar executáveis
clean:
	del /Q $(BIN_DIR)\*.exe 2> NUL

# Rodar o executável
run: all
	$(TARGET)
