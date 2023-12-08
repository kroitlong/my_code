# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/e/Linux_c++/c++/项目/db2023-1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/e/Linux_c++/c++/项目/db2023-1/build

# Include any dependencies generated for this target.
include src/parser/CMakeFiles/parser.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/parser/CMakeFiles/parser.dir/compiler_depend.make

# Include the progress variables for this target.
include src/parser/CMakeFiles/parser.dir/progress.make

# Include the compile flags for this target's objects.
include src/parser/CMakeFiles/parser.dir/flags.make

../src/parser/yacc.tab.cpp: ../src/parser/yacc.y
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/mnt/e/Linux_c++/c++/项目/db2023-1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "[BISON][yacc] Building parser with bison 3.8.2"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && /usr/bin/bison --defines=/mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/yacc.tab.h -o /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/yacc.tab.cpp /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/yacc.y

../src/parser/yacc.tab.h: ../src/parser/yacc.tab.cpp
	@$(CMAKE_COMMAND) -E touch_nocreate ../src/parser/yacc.tab.h

../src/parser/lex.yy.cpp: ../src/parser/lex.l
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/mnt/e/Linux_c++/c++/项目/db2023-1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "[FLEX][lex] Building scanner with flex 2.6.4"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser && /usr/bin/flex -o/mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/lex.yy.cpp lex.l

src/parser/CMakeFiles/parser.dir/yacc.tab.cpp.o: src/parser/CMakeFiles/parser.dir/flags.make
src/parser/CMakeFiles/parser.dir/yacc.tab.cpp.o: ../src/parser/yacc.tab.cpp
src/parser/CMakeFiles/parser.dir/yacc.tab.cpp.o: src/parser/CMakeFiles/parser.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/e/Linux_c++/c++/项目/db2023-1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/parser/CMakeFiles/parser.dir/yacc.tab.cpp.o"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/parser/CMakeFiles/parser.dir/yacc.tab.cpp.o -MF CMakeFiles/parser.dir/yacc.tab.cpp.o.d -o CMakeFiles/parser.dir/yacc.tab.cpp.o -c /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/yacc.tab.cpp

src/parser/CMakeFiles/parser.dir/yacc.tab.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parser.dir/yacc.tab.cpp.i"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/yacc.tab.cpp > CMakeFiles/parser.dir/yacc.tab.cpp.i

src/parser/CMakeFiles/parser.dir/yacc.tab.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parser.dir/yacc.tab.cpp.s"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/yacc.tab.cpp -o CMakeFiles/parser.dir/yacc.tab.cpp.s

src/parser/CMakeFiles/parser.dir/lex.yy.cpp.o: src/parser/CMakeFiles/parser.dir/flags.make
src/parser/CMakeFiles/parser.dir/lex.yy.cpp.o: ../src/parser/lex.yy.cpp
src/parser/CMakeFiles/parser.dir/lex.yy.cpp.o: ../src/parser/yacc.tab.h
src/parser/CMakeFiles/parser.dir/lex.yy.cpp.o: src/parser/CMakeFiles/parser.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/e/Linux_c++/c++/项目/db2023-1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/parser/CMakeFiles/parser.dir/lex.yy.cpp.o"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/parser/CMakeFiles/parser.dir/lex.yy.cpp.o -MF CMakeFiles/parser.dir/lex.yy.cpp.o.d -o CMakeFiles/parser.dir/lex.yy.cpp.o -c /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/lex.yy.cpp

src/parser/CMakeFiles/parser.dir/lex.yy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parser.dir/lex.yy.cpp.i"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/lex.yy.cpp > CMakeFiles/parser.dir/lex.yy.cpp.i

src/parser/CMakeFiles/parser.dir/lex.yy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parser.dir/lex.yy.cpp.s"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/lex.yy.cpp -o CMakeFiles/parser.dir/lex.yy.cpp.s

src/parser/CMakeFiles/parser.dir/ast.cpp.o: src/parser/CMakeFiles/parser.dir/flags.make
src/parser/CMakeFiles/parser.dir/ast.cpp.o: ../src/parser/ast.cpp
src/parser/CMakeFiles/parser.dir/ast.cpp.o: src/parser/CMakeFiles/parser.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/e/Linux_c++/c++/项目/db2023-1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/parser/CMakeFiles/parser.dir/ast.cpp.o"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/parser/CMakeFiles/parser.dir/ast.cpp.o -MF CMakeFiles/parser.dir/ast.cpp.o.d -o CMakeFiles/parser.dir/ast.cpp.o -c /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/ast.cpp

src/parser/CMakeFiles/parser.dir/ast.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parser.dir/ast.cpp.i"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/ast.cpp > CMakeFiles/parser.dir/ast.cpp.i

src/parser/CMakeFiles/parser.dir/ast.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parser.dir/ast.cpp.s"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser/ast.cpp -o CMakeFiles/parser.dir/ast.cpp.s

# Object files for target parser
parser_OBJECTS = \
"CMakeFiles/parser.dir/yacc.tab.cpp.o" \
"CMakeFiles/parser.dir/lex.yy.cpp.o" \
"CMakeFiles/parser.dir/ast.cpp.o"

# External object files for target parser
parser_EXTERNAL_OBJECTS =

lib/libparser.a: src/parser/CMakeFiles/parser.dir/yacc.tab.cpp.o
lib/libparser.a: src/parser/CMakeFiles/parser.dir/lex.yy.cpp.o
lib/libparser.a: src/parser/CMakeFiles/parser.dir/ast.cpp.o
lib/libparser.a: src/parser/CMakeFiles/parser.dir/build.make
lib/libparser.a: src/parser/CMakeFiles/parser.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/e/Linux_c++/c++/项目/db2023-1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX static library ../../lib/libparser.a"
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && $(CMAKE_COMMAND) -P CMakeFiles/parser.dir/cmake_clean_target.cmake
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/parser.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/parser/CMakeFiles/parser.dir/build: lib/libparser.a
.PHONY : src/parser/CMakeFiles/parser.dir/build

src/parser/CMakeFiles/parser.dir/clean:
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser && $(CMAKE_COMMAND) -P CMakeFiles/parser.dir/cmake_clean.cmake
.PHONY : src/parser/CMakeFiles/parser.dir/clean

src/parser/CMakeFiles/parser.dir/depend: ../src/parser/lex.yy.cpp
src/parser/CMakeFiles/parser.dir/depend: ../src/parser/yacc.tab.cpp
src/parser/CMakeFiles/parser.dir/depend: ../src/parser/yacc.tab.h
	cd /mnt/e/Linux_c++/c++/项目/db2023-1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/e/Linux_c++/c++/项目/db2023-1 /mnt/e/Linux_c++/c++/项目/db2023-1/src/parser /mnt/e/Linux_c++/c++/项目/db2023-1/build /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser /mnt/e/Linux_c++/c++/项目/db2023-1/build/src/parser/CMakeFiles/parser.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/parser/CMakeFiles/parser.dir/depend

