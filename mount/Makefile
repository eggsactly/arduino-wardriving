# This Makefile uses openscad to convert .scad code to .stl objects for 3D 
# printing.
# This Makefile looks at all .scad files in its current directory and turns them
# into .stl objects, except for .scad files that start with `lib' because 
# the `lib' prefix is used to indicate .scad files that contain only modules.
# All files with the `lib' prefix are instead put in the sources list for all
# .scad files that do not start with `lib'.

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

