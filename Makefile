# Compilador
CXX = g++

# Diretórios
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = tests
ALLEGRO_DIR = C:/Allegro

# Flags de compilação
CXXFLAGS = -I. -I$(INC_DIR) -I$(ALLEGRO_DIR)/include -Wall -std=c++17

# Flags de link (jogo: -mwindows, testes: sem)
LDFLAGS_GAME = -L$(ALLEGRO_DIR)/lib -lallegro_monolith \
	-lopengl32 -ldinput8 -ldxguid -ldsound -lwinmm -lole32 -luuid -lcomdlg32 -lsetupapi -lgdi32 -luser32 -lkernel32 \
	-mwindows

LDFLAGS_TEST = -L$(ALLEGRO_DIR)/lib -lallegro_monolith \
	-lopengl32 -ldinput8 -ldxguid -ldsound -lwinmm -lole32 -luuid -lcomdlg32 -lsetupapi -lgdi32 -luser32 -lkernel32

# Ícone
ICON_RC = resources.rc
ICON_OBJ = $(OBJ_DIR)/icon.o

# Fontes do jogo
SRCS = main.cpp \
	Bird.cpp \
	Scenario.cpp \
	GameEngine.cpp \
	Menu.cpp \
	GameOverScreen.cpp \
	Player.cpp \
	PlayerManager.cpp \
	Pipe.cpp \
	ScoreManager.cpp \
	RankingScreen.cpp \
	ResolutionSelector.cpp \
	ConfigScreen.cpp

# Objetos do jogo
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Alvo principal
TARGET = $(BIN_DIR)/TravelingDragon.exe

# Objetos do jogo para testes (sem main.cpp)
FULL_SRCS = $(filter-out $(SRC_DIR)/main.cpp, $(addprefix $(SRC_DIR)/, $(SRCS)))
TEST_FULL_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(FULL_SRCS))

# Fontes de teste
TEST_SRCS = $(wildcard $(TEST_DIR)/test_*.cpp)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.test.o, $(TEST_SRCS))
TEST_BIN = $(BIN_DIR)/run_tests.exe

# Alvo padrão
all: $(TARGET)

# Linkagem do jogo
$(TARGET): $(OBJS) $(ICON_OBJ) | $(BIN_DIR)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJS) $(ICON_OBJ) -o $@ $(LDFLAGS_GAME)
	@echo "Build completo!"

# Compilar arquivos .cpp do jogo
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar recurso de ícone
$(ICON_OBJ): $(ICON_RC) | $(OBJ_DIR)
	@echo "Compiling icon resource..."
	windres $< -O coff -o $@

# Compilar testes
$(OBJ_DIR)/%.test.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling test $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linkar binário de testes
$(TEST_BIN): $(TEST_OBJS) $(TEST_FULL_OBJS) | $(BIN_DIR)
	@echo "Linking test executable with game objects..."
	$(CXX) $(TEST_OBJS) $(TEST_FULL_OBJS) -o $@ $(LDFLAGS_TEST)

# Rodar os testes com saída visível
test: $(TEST_BIN)
	@echo "Running tests..."
	$(TEST_BIN) --success --reporters=console --verbosity=high

# Criar diretórios
$(OBJ_DIR):
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)

# Limpar arquivos gerados
clean:
	@echo "Cleaning..."
	@if exist $(OBJ_DIR) del /Q $(OBJ_DIR)\*.o
	@if exist $(OBJ_DIR) del /Q $(OBJ_DIR)\*.test.o
	@if exist $(ICON_OBJ) del /Q $(ICON_OBJ)
	@if exist $(BIN_DIR)\*.exe del /Q $(BIN_DIR)\*.exe

# Executar o jogo
run: all
	@echo "Running $(TARGET)..."
	$(TARGET)
