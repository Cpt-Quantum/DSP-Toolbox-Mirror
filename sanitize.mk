# Sanitizer flags, included by the top level Makefile, the examples and
# cpp_bindings/cpp_wrapper.mk
#
# make SANITIZE=1 ...  Builds the library and examples with AddressSanitizer
#                      and UndefinedBehaviorSanitizer. The first error stops
#                      the program with a non-zero exit status
#
# SANITIZE set on the make command line is passed to the sub-makes, so the
# library and the examples are always built the same way

SANITIZE ?= 0

ifeq ($(SANITIZE),1)
SANITIZE_FLAGS = -fsanitize=address,undefined -fno-sanitize-recover=all \
	-fno-omit-frame-pointer -g
else
SANITIZE_FLAGS =
endif
