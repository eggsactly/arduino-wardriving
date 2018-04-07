LIBS := $(wildcard lib*.scad)
SOURCES := $(filter-out $(LIBS), $(wildcard *.scad))
OUTPUT := $(patsubst %.scad, %.stl, $(SOURCES))

.PHONY: all
all: $(OUTPUT) 

%.stl: %.scad $(LIBS)
	openscad $< -o $@

.PHONY: clean
clean:
	-rm -f *.stl
	
