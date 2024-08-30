# Compiler and flags
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g -O3
LDFLAGS = -lm -pthread

# Root flags and libs
ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)

# Target executable name
TARGET = runOptimization

# Source files
SOURCES = EndcapConfiguration.cpp runOptimization.cpp PolygonUtils.cpp
HEADERS = PolygonUtils.h EndcapConfiguration.h

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJECTS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -o $@ $^ $(ROOTLIBS) $(LDFLAGS)

# Generic rule for compiling .cpp to .o
%.o: %.C
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets
.PHONY: all clean
