# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = "/Applications/CMake 2.8-9.app/Contents/bin/cmake"

# The command to remove a file.
RM = "/Applications/CMake 2.8-9.app/Contents/bin/cmake" -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = "/Applications/CMake 2.8-9.app/Contents/bin/ccmake"

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/zhaot/Work/neutube/neurolabi/c

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/zhaot/Work/neutube/neurolabi/c/cmake_build

# Include any dependencies generated for this target.
include CMakeFiles/testswc.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/testswc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testswc.dir/flags.make

CMakeFiles/testswc.dir/testswc.c.o: CMakeFiles/testswc.dir/flags.make
CMakeFiles/testswc.dir/testswc.c.o: ../testswc.c
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/zhaot/Work/neutube/neurolabi/c/cmake_build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/testswc.dir/testswc.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/testswc.dir/testswc.c.o   -c /Users/zhaot/Work/neutube/neurolabi/c/testswc.c

CMakeFiles/testswc.dir/testswc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/testswc.dir/testswc.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /Users/zhaot/Work/neutube/neurolabi/c/testswc.c > CMakeFiles/testswc.dir/testswc.c.i

CMakeFiles/testswc.dir/testswc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/testswc.dir/testswc.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /Users/zhaot/Work/neutube/neurolabi/c/testswc.c -o CMakeFiles/testswc.dir/testswc.c.s

CMakeFiles/testswc.dir/testswc.c.o.requires:
.PHONY : CMakeFiles/testswc.dir/testswc.c.o.requires

CMakeFiles/testswc.dir/testswc.c.o.provides: CMakeFiles/testswc.dir/testswc.c.o.requires
	$(MAKE) -f CMakeFiles/testswc.dir/build.make CMakeFiles/testswc.dir/testswc.c.o.provides.build
.PHONY : CMakeFiles/testswc.dir/testswc.c.o.provides

CMakeFiles/testswc.dir/testswc.c.o.provides.build: CMakeFiles/testswc.dir/testswc.c.o

# Object files for target testswc
testswc_OBJECTS = \
"CMakeFiles/testswc.dir/testswc.c.o"

# External object files for target testswc
testswc_EXTERNAL_OBJECTS =

testswc: CMakeFiles/testswc.dir/testswc.c.o
testswc: CMakeFiles/testswc.dir/build.make
testswc: libneurolabi.a
testswc: CMakeFiles/testswc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable testswc"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testswc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/testswc.dir/build: testswc
.PHONY : CMakeFiles/testswc.dir/build

CMakeFiles/testswc.dir/requires: CMakeFiles/testswc.dir/testswc.c.o.requires
.PHONY : CMakeFiles/testswc.dir/requires

CMakeFiles/testswc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testswc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testswc.dir/clean

CMakeFiles/testswc.dir/depend:
	cd /Users/zhaot/Work/neutube/neurolabi/c/cmake_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/zhaot/Work/neutube/neurolabi/c /Users/zhaot/Work/neutube/neurolabi/c /Users/zhaot/Work/neutube/neurolabi/c/cmake_build /Users/zhaot/Work/neutube/neurolabi/c/cmake_build /Users/zhaot/Work/neutube/neurolabi/c/cmake_build/CMakeFiles/testswc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testswc.dir/depend
