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
srcdir			= .


# Build-related substitution variables
objects			= NGame.o NColor.o NEngine.o NInput.o NCamera.o NNode.o NScene.o NRender.o NShader.o NText.o NTexture.o NLua.o NConfig.o NGui.o
CXX				= g++
CXXFLAGS		= -g -O2 -I/usr/include/freetype2
LIBS			= -lSOIL -llua -lfreetype -lglfw -lGLEW -lGL 
DEFS			= -DHAVE_CONFIG_H

INSTALL			= /usr/bin/install -c
INSTALL_DATA	= ${INSTALL} -m 644

# Build targets
all: main

main: main.o $(objects)
	$(CXX) $(CXXFLAGS) $(DEFS) main.o $(objects) -o main $(LIBS)

clean:
	rm -f main *.o

#Autotools mumbo jumbo

Makefile: Makefile.in ../config.status
	cd .. && ./config.status $@

../config.status: ../configure
	cd .. && ./config.status --recheck

.PHONY: all clean install uninstall
