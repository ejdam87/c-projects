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
CMAKE_SOURCE_DIR = /mnt/c/Docs/Git/c-projects/structures

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Docs/Git/c-projects/structures/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/structures.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/structures.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/structures.dir/flags.make

CMakeFiles/structures.dir/main.c.o: CMakeFiles/structures.dir/flags.make
CMakeFiles/structures.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Docs/Git/c-projects/structures/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/structures.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/structures.dir/main.c.o   -c /mnt/c/Docs/Git/c-projects/structures/main.c

CMakeFiles/structures.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/structures.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Docs/Git/c-projects/structures/main.c > CMakeFiles/structures.dir/main.c.i

CMakeFiles/structures.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/structures.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Docs/Git/c-projects/structures/main.c -o CMakeFiles/structures.dir/main.c.s

CMakeFiles/structures.dir/structures.c.o: CMakeFiles/structures.dir/flags.make
CMakeFiles/structures.dir/structures.c.o: ../structures.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Docs/Git/c-projects/structures/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/structures.dir/structures.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/structures.dir/structures.c.o   -c /mnt/c/Docs/Git/c-projects/structures/structures.c

CMakeFiles/structures.dir/structures.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/structures.dir/structures.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Docs/Git/c-projects/structures/structures.c > CMakeFiles/structures.dir/structures.c.i

CMakeFiles/structures.dir/structures.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/structures.dir/structures.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Docs/Git/c-projects/structures/structures.c -o CMakeFiles/structures.dir/structures.c.s

# Object files for target structures
structures_OBJECTS = \
"CMakeFiles/structures.dir/main.c.o" \
"CMakeFiles/structures.dir/structures.c.o"

# External object files for target structures
structures_EXTERNAL_OBJECTS =

structures: CMakeFiles/structures.dir/main.c.o
structures: CMakeFiles/structures.dir/structures.c.o
structures: CMakeFiles/structures.dir/build.make
structures: CMakeFiles/structures.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Docs/Git/c-projects/structures/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable structures"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/structures.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/structures.dir/build: structures

.PHONY : CMakeFiles/structures.dir/build

CMakeFiles/structures.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/structures.dir/cmake_clean.cmake
.PHONY : CMakeFiles/structures.dir/clean

CMakeFiles/structures.dir/depend:
	cd /mnt/c/Docs/Git/c-projects/structures/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Docs/Git/c-projects/structures /mnt/c/Docs/Git/c-projects/structures /mnt/c/Docs/Git/c-projects/structures/cmake-build-debug /mnt/c/Docs/Git/c-projects/structures/cmake-build-debug /mnt/c/Docs/Git/c-projects/structures/cmake-build-debug/CMakeFiles/structures.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/structures.dir/depend
