# CPP wrapper makefile
# Include this to link the C++ wrappers in your project against the library
# built by the top level Makefile. Add CPP_WRAPPER_LIB as a prerequisite of
# your build target, so the library is built (and the C sources generated) or
# brought up to date first, and link with CPP_WRAPPER_LDFLAGS after your own
# sources, e.g.
#
#	my_app: $(CPP_WRAPPER_LIB)
#		$(CXX) $(CXXFLAGS) my_app.cpp -o my_app $(CPP_WRAPPER_LDFLAGS)
#
# To link the shared library instead, use CPP_WRAPPER_SHARED_LIB and
# CPP_WRAPPER_SHARED_LDFLAGS. The program is given an rpath to the build
# directory, so it runs without setting LD_LIBRARY_PATH
#
# Options for the top level build, such as GEN_ARGS, can be set on the make
# command line and are passed through

# Set the root directory for the project
CPP_WRAPPER_ROOT_DIR ?= ..

# The library holding every module, built by the top level Makefile
CPP_WRAPPER_LIB = $(CPP_WRAPPER_ROOT_DIR)/build/libdsptoolbox.a

# The same modules as a shared library
CPP_WRAPPER_SHARED_LIB = $(CPP_WRAPPER_ROOT_DIR)/build/libdsptoolbox.so

# Set needed linker flags
CPP_WRAPPER_LDFLAGS += $(CPP_WRAPPER_LIB) -lm
CPP_WRAPPER_SHARED_LDFLAGS += $(CPP_WRAPPER_SHARED_LIB) -lm \
	-Wl,-rpath,$(abspath $(CPP_WRAPPER_ROOT_DIR)/build)

# Keep the including makefile's first target as its default goal, rather than
# the rule below
CPP_WRAPPER_SAVED_GOAL := $(.DEFAULT_GOAL)

# The top level make works out whether the library is up to date, and doesn't
# touch it if so, so targets depending on it only relink when it changes
$(CPP_WRAPPER_LIB): CPP_WRAPPER_FORCE
	$(MAKE) -C $(CPP_WRAPPER_ROOT_DIR) lib

$(CPP_WRAPPER_SHARED_LIB): CPP_WRAPPER_FORCE
	$(MAKE) -C $(CPP_WRAPPER_ROOT_DIR) shared

CPP_WRAPPER_FORCE:
.PHONY: CPP_WRAPPER_FORCE

.DEFAULT_GOAL := $(CPP_WRAPPER_SAVED_GOAL)
