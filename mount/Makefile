LIBS := $(wildcard lib*.scad)
SOURCES := $(wildcard *.scad)
SOURCES := $(filter-out $(LIBS), $(SOURCES))
OUTPUT := $(patsubst %.scad, %.stl, $(SOURCES))

.PHONY: all
all: $(OUTPUT) 

%.stl: %.scad $(LIBS)
	openscad $< -o $@

.PHONY: clean
clean:
	-rm -f *.stl
	
