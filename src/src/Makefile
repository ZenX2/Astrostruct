# src/Makefile.  Generated from Makefile.in by configure.

# Package-related substitution variables
package        	= astrostruct
version        	= 1.0
tarname        	= astrostruct
distdir        	= $(tarname)-$(version)

# Prefix-related substitution variables
prefix         	= /usr/local
exec_prefix    	= ${prefix}
bindir         	= ${exec_prefix}/bin

# VPATH-related substitution variables
srcdir			= ../../src
VPATH			= ../../src

# Build-related substitution variables
objects			= NGame.o NColor.o NEngine.o NInput.o NCamera.o NNode.o NScene.o NRender.o NShader.o
CXX				= g++
CXXFLAGS		= -g -O2
LIBS			= -lfreetype -lglfw -lGLEW -lGL 
DEFS			= -DHAVE_CONFIG_H

INSTALL			= /usr/bin/install -c
INSTALL_DATA	= ${INSTALL} -m 644

# Build targets
all: main

main: main.o $(objects)
	$(CXX) $(CXXFLAGS) $(DEFS) main.o $(objects) -o main $(LIBS)

clean:
	rm -f client server mapmaker *.o

#Autotools mumbo jumbo

Makefile: Makefile.in ../config.status
	cd .. && ./config.status $@

../config.status: ../configure
	cd .. && ./config.status --recheck

.PHONY: all clean install uninstall
