PARAMETER_FILE := parameters.scad
SOURCES := $(wildcard *.scad)
SOURCES := $(filter-out $(PARAMETER_FILE), $(SOURCES))
OUTPUT := $(patsubst %.scad, %.stl, $(SOURCES))

.PHONY: all
all: $(OUTPUT) 

%.stl: %.scad $(PARAMETER_FILE)
	openscad $< -o $@

.PHONY: clean
clean:
	-rm -f *.stl
	
