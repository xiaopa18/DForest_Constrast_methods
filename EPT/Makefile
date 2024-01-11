SOURCES=$(shell find . -name "*.cpp")
CXXFLAGS= -O3 -Wall -ffast-math -march=native
OBJECTS=$(SOURCES:%.cpp=%.o)
TARGET=main

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK.cpp) $^  $(LOADLIBES) $(LDLIBS) -o $@

.PHONY: clean
clean:
	rm -rf $(OBJECTS)
