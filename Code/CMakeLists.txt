# CMakeList.txt : CMake project for A12, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Corrosion")

# Add source to this project's executable.
add_executable (Corrosion
	"Compilers.h"  "Compilers.c"
	"Reader.h"     "Reader.c"     "MainReader.c"
	"Scanner.h"   "Scanner.c"    "MainScanner.c"
	"Parser.h"    "Parser.c"     "MainParser.c"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Corrosion PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
