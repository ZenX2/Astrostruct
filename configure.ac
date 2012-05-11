#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([astrostruct], [1.0], [naelstrof@gmail.com])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_LANG([C++])

# Checks for libraries.
# mingw32 doesn't search libs very well, so if it fails to search we just check for existance and hope for the best
AC_SEARCH_LIBS([glBegin],[GL opengl32],[],[
	AC_CHECK_LIB([opengl32],[main],[LIBS="$LIBS -lopengl32"],[
		AC_MSG_ERROR(["Couldn't find opengl libraries!"])
	])
])
AC_SEARCH_LIBS([glewInit],[GLEW glew32],[],[
	AC_CHECK_LIB([glew32],[main],[LIBS="$LIBS -lglew32"],[
		AC_MSG_ERROR(["Couldn't find glew libraries!"])
	])
])
AC_SEARCH_LIBS([glfwInit],[glfw],[],[
	AC_CHECK_LIB([glfw],[main],[LIBS="$LIBS -lglfw"],[
		AC_MSG_ERROR(["Couldn't find glfw libraries!"])
	])
])
AC_SEARCH_LIBS([FT_Init_FreeType],[freetype],[],[
	AC_CHECK_LIB([freetype],[main],[LIBS="$LIBS -lfreetype"],[
		AC_MSG_ERROR(["Couldn't find freetype libraries!"])
	])
])
AC_SEARCH_LIBS([luaL_register],[lua],[],[
	AC_CHECK_LIB([lua],[main],[LIBS="$LIBS -llua"],[
		AC_MSG_ERROR(["Couldn't find lua libraries!"])
	])
])
AC_SEARCH_LIBS([SOIL_load_OGL_texture],[SOIL],[],[
	AC_CHECK_LIB([SOIL],[main],[LIBS="$LIBS -lSOIL"],[
		AC_MSG_ERROR(["Couldn't find soil libraries!"])
	])
])

# Checks for header files.
CXXFLAGS="$CXXFLAGS -I/usr/include/freetype2"
AC_CHECKING([for freetype headers])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <ft2build.h>
#include FT_FREETYPE_H
])],[AC_MSG_RESULT([found])], [AC_MSG_ERROR([Failed to find freetype headers!
Try adding CXXFLAGS="-I/usr/include/freetype2" to the command line!])])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([floor sqrt])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

echo \
"==========================
${PACKAGE_NAME} Version ${PACKAGE_VERSION}

Prefix: '${prefix}'
Compiler: '${CXX} ${CXXFLAGS} ${CPPFLAGS} ${LIBS}'

Now type 'make <target>' where the optional <target> is
	
	all			- build all binaries
	install			- install everything
=========================="
