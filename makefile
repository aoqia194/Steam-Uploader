# Compiler and flags
CXX = g++
CXXFLAGS = -g -I.
LDFLAGS = -L./SteamAPI/linux64 -lsteam_api -Wl,-rpath,'$$ORIGIN/SteamAPI/linux64' -pthread

# Source and output
SRC_DIR = ./src
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
TARGET = ./app

# Default build target
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Clean up compiled files
clean:
	rm -f $(TARGET)