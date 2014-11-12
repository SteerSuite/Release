#
# Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
# See license.txt for complete license.
#

#
# To use this makefile:
#   1. Modify the project-dependent variables at the top of this file.
#   2. Use "gmake" to build the user-specified target.
#   3. To clean the directories and the target, use "gmake clean".
#
# For platform-dependent flags, you can make a script that initializes
# environment variables depending on the operating system, and use
# those environment variables in this makefile.
#
# Notes:
#   - This makefile is intended to use with GNU Make and gcc/g++.  It
#     might (or might not) work with other Make programs and other
#     compilers after small revisions.
#
#   - If GNU Make (gmake) is the default make for your system, you can
#     probably use "make" instead of "gmake".
#
#   - Two directories, one for .o object files, and one for .d
#     dependencies, are created.
#
#   - Rebuilds should be both robust and reasonably fast, even on very
#     large projects. Dependencies are created/updated automatically.
#



# TARGET_TYPE is the type of target to be built. Use exactly one of
# the following options: 
#
#  1. "executable"     - compiles and links a complete executable.
#  2. "staticLibrary"  - uses "ar" archival utility instead of "ld"
#  3. "sharedLibrary"  - uses the "-fPIC" and "-shared" flags.
#  4. "dynamicLibrary" - uses the "-fPIC" and "-shared" flags.
#
TARGET_TYPE := executable
#TARGET_TYPE := staticLibrary
#TARGET_TYPE := sharedLibrary
#TARGET_TYPE := dynamicLibrary


# The output binary you want to create.
TARGET_NAME := steertool

# The paths where source files are located.
SOURCE_DIRS := ../src/

ifeq ($(STEERSUITE_OSX),clang++)
$(info Using clang++ compiler and linker)
# The compiler you want to use.
CC := clang++

# The linker you want to use.
LD := clang++

else

# The compiler you want to use.
CC := g++

# The linker you want to use.
LD := g++
endif

# The archival utility you want to use (for creating static libraries)
AR := ar

# Any preprocessor definitions you want to give to the compiler.
# PREFIX EACH DEFINE WITH "-D".
DEFINES := $(BUILD_SCRIPT_DEFINES)

# The include paths that the compiler (and dependency generation) should search.
# PREFIX EACH INCLUDE DIRECTORY WITH "-I".
INCLUDES := $(STEERTOOL_INCLUDES)

# Flags passed to the compiler.
# -D defines and -I include options should go in DEFINES and INCLUDES, not here.
CFLAGS := $(BASIC_CFLAGS)

# Flags passed to the linker.
# **NOTE CAREFULLY** sometimes the ordering of linking multiple libraries is important.
LFLAGS := $(STEERTOOL_LFLAGS)


####################################################################
# Should not need to modify anything below this point in the file.
####################################################################

# The shell to use when executing commands
SHELL = /bin/bash

# The directory to put .d dependency files.
DEPDIR := deps

# The directory to put .o object files.
OBJDIR := objs

# All .c, .C, .cc, .cpp, and .cxx files from the SOURCE_DIRS. Each filename includes the relative path.
SOURCES := $(foreach DIR, $(SOURCE_DIRS), $(wildcard $(DIR)/*.c) $(wildcard $(DIR)/*.C) $(wildcard $(DIR)/*.cc) $(wildcard $(DIR)/*.cpp) $(wildcard $(DIR)/*.cxx))

# All the dependency files, from the corresponding source files, including the a new path that refers to the dependency directory.
DEPENDENCIES := $(foreach DEP, $(SOURCES), $(DEPDIR)/$(notdir $(basename $(DEP))).d)

# All the object files, from the corresponding source files, including a new path that refers to the object directory.
OBJECTS := $(foreach OBJ, $(SOURCES), $(OBJDIR)/$(notdir $(basename $(OBJ))).o)

# Cause an error if the TARGET_TYPE specified by the user is invalid.
ifneq ($(TARGET_TYPE),executable)
ifneq ($(TARGET_TYPE),staticLibrary)
ifneq ($(TARGET_TYPE),sharedLibrary)
ifneq ($(TARGET_TYPE),dynamicLibrary)
$(error Invalid TARGET_TYPE. Choices are: "executable", "staticLibrary", or "dynamicLibrary".)
endif
endif
endif
endif

# Tell the makefile to look for source files where the user specified them.
vpath %.c   $(SOURCE_DIRS)
vpath %.C   $(SOURCE_DIRS)
vpath %.cc  $(SOURCE_DIRS)
vpath %.cpp $(SOURCE_DIRS)
vpath %.cxx $(SOURCE_DIRS)



# The make rules

# "make" defaults to "make all" since "all" is the first specified target.
# "make all" builds the user-specified target.
all: $(TARGET_NAME)

# Tell the makefile that "all" and "clean" are not actually files.
.PHONY: all clean


#
# There are four versions of compiling/linking below, depending on TARGET_TYPE.
# To keep maximum flexibility, we intentionally did not simplify redundant parts
# of the build process across the four versions.
#
# Note that the $(CC) command produces the .o object file AND the .d dependency
# in one execution, which is a little faster than doing it in two steps.
#

##############################################
# In this case, the target is an executable
ifeq ($(TARGET_TYPE),executable)

$(TARGET_NAME): $(DEPDIR) $(OBJDIR) $(OBJECTS)
	@echo -n Linking executable...
	@$(LD) -o $@ $(OBJECTS) $(LFLAGS)
	@echo done.

$(OBJDIR)/%.o: %.c
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.C
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cc
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cpp
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cxx
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

endif
##############################################


##############################################
# In this case, the target is a static library.
ifeq ($(TARGET_TYPE),staticLibrary)

$(TARGET_NAME): $(DEPDIR) $(OBJDIR) $(OBJECTS)
	@echo -n Creating static library...
	@$(AR) rcs $@ $(OBJECTS)
	@echo done.

$(OBJDIR)/%.o: %.c
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.C
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cc
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cpp
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cxx
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

endif
##############################################


##############################################
# In this case, the target is a shared library.
ifeq ($(TARGET_TYPE),sharedLibrary)

$(TARGET_NAME): $(DEPDIR) $(OBJDIR) $(OBJECTS)
	@echo -n Creating shared library...
	@$(LD) -fPIC -shared -o $@ $(OBJECTS) $(LFLAGS)
	@echo done.

$(OBJDIR)/%.o: %.c
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -fPIC -shared -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.C
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -fPIC -shared -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cc
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -fPIC -shared -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cpp
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -fPIC -shared -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cxx
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -fPIC -shared -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

endif
##############################################


##############################################
# In this case, the target is a dynamic library.
ifeq ($(TARGET_TYPE),dynamicLibrary)

$(TARGET_NAME): $(DEPDIR) $(OBJDIR) $(OBJECTS)
	@echo -n Creating dynamic library...
	@$(LD) -fPIC -shared -o $@ $(OBJECTS) $(LFLAGS)
	@echo done.

$(OBJDIR)/%.o: %.c
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -fPIC -shared -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.C
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -fPIC -shared -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cc
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -fPIC -shared -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cpp
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -fPIC -shared -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

$(OBJDIR)/%.o: %.cxx
	@echo Compiling $<
	@$(CC) -MD -MF $(DEPDIR)/$(notdir $(@:.o=.d)) -fPIC -shared -c -o $@ $< $(CFLAGS) $(DEFINES) $(INCLUDES)

endif
##############################################


# The dependency directory should be created if it doesnt exist
$(DEPDIR):
	@mkdir -p $(DEPDIR)

# The object directory should be created if it doesnt exist
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Removes EVERYTHING in the object and dependency
# directories, even if the makefile did not create them.
clean:
	@echo -n Cleaning up... 
	@-rm -rf $(OBJDIR) $(DEPDIR) $(TARGET_NAME)
	@echo done.

# Don't include all the dependency files if we are just doing a "make clean".
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPENDENCIES)
endif

