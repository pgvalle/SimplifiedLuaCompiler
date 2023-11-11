# Chatgpt generated this file. I just tweaked it a little bit.

CC := g++
CFLAGS := -Wall -Werror
SRCDIR := source
BUILDDIR := build
EXECUTABLE := slc

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SOURCES))

# Generate the target executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile individual object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $@

# Remove object files and the executable
clean:
	rm -rf $(BUILDDIR) $(EXECUTABLE)
