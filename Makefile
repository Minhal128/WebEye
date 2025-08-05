# WebEye Makefile
# Alternative build system for WebEye security scanner

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
DEBUGFLAGS = -std=c++17 -Wall -Wextra -g -O0 -DDEBUG
LDFLAGS = -lcurl -lpthread

# Directories
SRCDIR = src
BUILDDIR = build
BINDIR = $(BUILDDIR)/bin
OBJDIR = $(BUILDDIR)/obj

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Target executable
TARGET = $(BINDIR)/webeye

# Default target
all: $(TARGET)

# Create directories
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Build target
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build completed: $@"

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug build
debug: CXXFLAGS = $(DEBUGFLAGS)
debug: $(TARGET)

# Release build
release: CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -DNDEBUG
release: $(TARGET)

# Install target
install: $(TARGET)
	@echo "Installing WebEye..."
	sudo mkdir -p /usr/local/bin
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/webeye
	@echo "WebEye installed to /usr/local/bin/webeye"

# Uninstall target
uninstall:
	@echo "Uninstalling WebEye..."
	sudo rm -f /usr/local/bin/webeye
	@echo "WebEye uninstalled"

# Test target
test: $(TARGET)
	@echo "Running basic tests..."
	@if $(TARGET) --help >/dev/null 2>&1; then \
		echo "✓ Help command works"; \
	else \
		echo "✗ Help command failed"; \
		exit 1; \
	fi
	@echo "Basic tests passed"

# Clean target
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILDDIR)
	@echo "Build files cleaned"

# Clean all (including dependencies)
clean-all: clean
	@echo "Cleaning all files..."
	rm -f *.json
	rm -f *.log
	@echo "All files cleaned"

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@if ! command -v $(CXX) >/dev/null 2>&1; then \
		echo "✗ C++ compiler ($(CXX)) not found"; \
		exit 1; \
	fi
	@if ! pkg-config --exists libcurl; then \
		echo "✗ libcurl not found"; \
		echo "Install with: sudo apt-get install libcurl4-openssl-dev"; \
		exit 1; \
	fi
	@echo "✓ All dependencies found"

# Show help
help:
	@echo "WebEye Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          Build WebEye (default)"
	@echo "  debug        Build with debug symbols"
	@echo "  release      Build with optimizations"
	@echo "  install      Install to /usr/local/bin"
	@echo "  uninstall    Remove from /usr/local/bin"
	@echo "  test         Run basic tests"
	@echo "  clean        Remove build files"
	@echo "  clean-all    Remove all generated files"
	@echo "  check-deps   Check if dependencies are installed"
	@echo "  help         Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CXX          C++ compiler (default: g++)"
	@echo "  CXXFLAGS     Compiler flags"
	@echo "  LDFLAGS      Linker flags"
	@echo ""
	@echo "Examples:"
	@echo "  make                    # Build WebEye"
	@echo "  make debug              # Build with debug info"
	@echo "  make install            # Install to system"
	@echo "  make CXX=clang++        # Use Clang compiler"

# Phony targets
.PHONY: all debug release install uninstall test clean clean-all check-deps help

# Show target info
info:
	@echo "WebEye Build Information"
	@echo "========================"
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Target: $(TARGET)"
	@echo "Build dir: $(BUILDDIR)" 