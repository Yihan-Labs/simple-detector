# Compiler and flags
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g -O3

# Root flags and libs
ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)

# Target executable name
TARGET = runOptimization

# Source files
SOURCES = EndcapConfiguration.C runOptimization.C PolygonUtils.C

# Object files
OBJECTS = $(SOURCES:.C=.o)

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -o $@ $^ $(ROOTLIBS)

# Generic rule for compiling .C to .o
%.o: %.C
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets
.PHONY: all clean
