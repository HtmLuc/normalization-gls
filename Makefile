CFLAGS = -g -Wall -Wextra -pedantic -Iclass -Iheader
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst src/%.cpp,obj/%.o,$(SOURCES))
EXECUTABLE = bin/main
FILES = data/*.txt

ifeq ($(OS), Windows_NT)
CLEAR_TERMINAL = cls
DELETE = erase /Q
else
CLEAR_TERMINAL = clear
DELETE = rm -f
endif

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	g++ $(CFLAGS) $^ -o $@

obj/%.o: src/%.cpp
	g++ $(CFLAGS) -c $< -o $@

.PHONY: run
run: $(EXECUTABLE)
	./$(EXECUTABLE)

.PHONY: clean
clean:
	@$(DELETE) $(OBJECTS) $(EXECUTABLE) $(FILES)