CC=g++
SRC_DIR=src
BUILD_DIR=build
FLAGS=-std=c++0x -I$(SRC_DIR)

SOURCES = $(wildcard $(SRC_DIR)/*/*.cpp)

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
BUILD_SUBDIRS=$(sort $(dir $(OBJECTS)))

.PHONY: all clean directories 

all: chat_client chat_server

directories: 
	mkdir -p $(BUILD_SUBDIRS)

chat_server: $(OBJECTS) $(SRC_DIR)/chat_server.cpp
	$(CC) $(FLAGS) $^ -o $@
	
chat_client: $(OBJECTS) $(SRC_DIR)/chat_client.cpp
	$(CC) $(FLAGS) $^ -o $@

$(OBJECTS): $(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp | directories 
	$(CC) -c $(FLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) chat_client chat_server
