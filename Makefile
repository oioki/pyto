# Pyto — minimal Python interpreter (Linux + GCC)
# Build from project root: make
# Binary: build/pyto

SRCDIR   = pyto
BUILDDIR = build
TARGET   = $(BUILDDIR)/pyto

SOURCES  = $(SRCDIR)/input.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/operations.cpp \
           $(SRCDIR)/Pyto.cpp $(SRCDIR)/stdafx.cpp $(SRCDIR)/stream.cpp \
           $(SRCDIR)/symbol.cpp $(SRCDIR)/symtable.cpp $(SRCDIR)/syntax.cpp
OBJECTS  = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++98

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)
