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

# Utility rule file for generate_tz_local_neuroseg_doubly_linked_list.h.

# Include the progress variables for this target.
include CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h.dir/progress.make

CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h: ../tz_local_neuroseg_doubly_linked_list.h

../tz_local_neuroseg_doubly_linked_list.h: ../tz_doubly_linked_list.h.t
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/zhaot/Work/neutube/neurolabi/c/cmake_build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating /Users/zhaot/Work/neutube/neurolabi/c/tz_local_neuroseg_doubly_linked_list.h"
	cd /Users/zhaot/Work/neutube/neurolabi/c && [ ! -f /Users/zhaot/Work/neutube/neurolabi/c/tz_local_neuroseg_doubly_linked_list.h ] || chmod u+w /Users/zhaot/Work/neutube/neurolabi/c/tz_local_neuroseg_doubly_linked_list.h
	cd /Users/zhaot/Work/neutube/neurolabi/c && perl ../shell/parsetmpl.pl /Users/zhaot/Work/neutube/neurolabi/c/tz_doubly_linked_list.h.t /Users/zhaot/Work/neutube/neurolabi/c/tz_local_neuroseg_doubly_linked_list.h Local_Neuroseg Local_Neuroseg_P compact_pointer
	cd /Users/zhaot/Work/neutube/neurolabi/c && chmod a-w /Users/zhaot/Work/neutube/neurolabi/c/tz_local_neuroseg_doubly_linked_list.h

generate_tz_local_neuroseg_doubly_linked_list.h: CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h
generate_tz_local_neuroseg_doubly_linked_list.h: ../tz_local_neuroseg_doubly_linked_list.h
generate_tz_local_neuroseg_doubly_linked_list.h: CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h.dir/build.make
.PHONY : generate_tz_local_neuroseg_doubly_linked_list.h

# Rule to build all files generated by this target.
CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h.dir/build: generate_tz_local_neuroseg_doubly_linked_list.h
.PHONY : CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h.dir/build

CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h.dir/cmake_clean.cmake
.PHONY : CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h.dir/clean

CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h.dir/depend:
	cd /Users/zhaot/Work/neutube/neurolabi/c/cmake_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/zhaot/Work/neutube/neurolabi/c /Users/zhaot/Work/neutube/neurolabi/c /Users/zhaot/Work/neutube/neurolabi/c/cmake_build /Users/zhaot/Work/neutube/neurolabi/c/cmake_build /Users/zhaot/Work/neutube/neurolabi/c/cmake_build/CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/generate_tz_local_neuroseg_doubly_linked_list.h.dir/depend

