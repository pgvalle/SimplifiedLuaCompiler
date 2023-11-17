# Chatgpt generated this file. I just tweaked it a little bit.

CC := c++
CFLAGS := -Wall -Werror -std=c++17
SRCDIR := source
BUILDDIR := build
EXECUTABLE := slc

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SOURCES))

# Generate the target executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile individual object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Remove object files and the executable
.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(EXECUTABLE)
