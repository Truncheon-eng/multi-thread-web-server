SRC_DIR := ./src
EXE_DIR := ./bin
OBJ_DIR := ./obj

PORT := 25012

EXE := $(EXE_DIR)/server.exe

SRC_CPP := $(wildcard $(SRC_DIR)/*.cpp)

OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_CPP))

CXX := g++
CXX_FLAGS := -lws2_32 -g -mconsole

.PHONY: all debug clean run

run: 
	$(EXE) $(PORT)

clean:
	@if exist $(EXE_DIR) (\
		@rmdir /s /q $(notdir $(EXE_DIR))\
	)
	@if exist $(OBJ_DIR) (\
		@rmdir /s /q $(notdir $(OBJ_DIR))\
	)

debug: $(EXE)
	@gdb -q $(EXE)

all: $(EXE)

$(EXE): $(OBJS) | $(EXE_DIR)
	$(CXX) $^ -o $@ $(CXX_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(CXX_FLAGS)

$(OBJ_DIR):
	mkdir $@

$(EXE_DIR):
	mkdir $@