# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lin/TinyJSON

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lin/TinyJSON/cmake-build-debug-wsl

# Include any dependencies generated for this target.
include example/CMakeFiles/parse.dir/depend.make

# Include the progress variables for this target.
include example/CMakeFiles/parse.dir/progress.make

# Include the compile flags for this target's objects.
include example/CMakeFiles/parse.dir/flags.make

example/CMakeFiles/parse.dir/parse.cpp.o: example/CMakeFiles/parse.dir/flags.make
example/CMakeFiles/parse.dir/parse.cpp.o: ../example/parse.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lin/TinyJSON/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/CMakeFiles/parse.dir/parse.cpp.o"
	cd /home/lin/TinyJSON/cmake-build-debug-wsl/example && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/parse.dir/parse.cpp.o -c /home/lin/TinyJSON/example/parse.cpp

example/CMakeFiles/parse.dir/parse.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parse.dir/parse.cpp.i"
	cd /home/lin/TinyJSON/cmake-build-debug-wsl/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lin/TinyJSON/example/parse.cpp > CMakeFiles/parse.dir/parse.cpp.i

example/CMakeFiles/parse.dir/parse.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parse.dir/parse.cpp.s"
	cd /home/lin/TinyJSON/cmake-build-debug-wsl/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lin/TinyJSON/example/parse.cpp -o CMakeFiles/parse.dir/parse.cpp.s

# Object files for target parse
parse_OBJECTS = \
"CMakeFiles/parse.dir/parse.cpp.o"

# External object files for target parse
parse_EXTERNAL_OBJECTS =

bin/parse: example/CMakeFiles/parse.dir/parse.cpp.o
bin/parse: example/CMakeFiles/parse.dir/build.make
bin/parse: lib/libTinyJSON.a
bin/parse: example/CMakeFiles/parse.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lin/TinyJSON/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/parse"
	cd /home/lin/TinyJSON/cmake-build-debug-wsl/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/parse.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/CMakeFiles/parse.dir/build: bin/parse

.PHONY : example/CMakeFiles/parse.dir/build

example/CMakeFiles/parse.dir/clean:
	cd /home/lin/TinyJSON/cmake-build-debug-wsl/example && $(CMAKE_COMMAND) -P CMakeFiles/parse.dir/cmake_clean.cmake
.PHONY : example/CMakeFiles/parse.dir/clean

example/CMakeFiles/parse.dir/depend:
	cd /home/lin/TinyJSON/cmake-build-debug-wsl && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lin/TinyJSON /home/lin/TinyJSON/example /home/lin/TinyJSON/cmake-build-debug-wsl /home/lin/TinyJSON/cmake-build-debug-wsl/example /home/lin/TinyJSON/cmake-build-debug-wsl/example/CMakeFiles/parse.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/CMakeFiles/parse.dir/depend

