#build directory
BUILD_DIR := build

#directory for headers
HEADER_DIR := include

#directory for source files
SRC_DIR := src

#compiler
CC := g++

#compiler flags
CFLAGS := -Wall -Wextra

#list of objects
OBJ :=  $(BUILD_DIR)/main.o 

#target name
TARGET := mcassemble

#create build directory if not present
$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p ~/bin)

#objects are compiled from .c/c++ files of the same name
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -I$(HEADER_DIR) -c $< -o $@

#target is built from objects
~/bin/$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -I$(HEADER_DIR) -o ~/bin/$(TARGET) $(OBJ)

#remove build directory and executable
clean:	
	rm -rf $(BUILD_DIR)
	rm -rf ~/bin/$(TARGET)
