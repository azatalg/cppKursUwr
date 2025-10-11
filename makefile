CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O3

TARGET = list
SRC = ./src/RunLists.cpp ./src/List1.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean

