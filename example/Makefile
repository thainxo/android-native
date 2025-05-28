TARGET := test
ANDROID_ROOT := D:/programs/Android
INSTALL_DIR := /data/local/tmp

ANDROID_NDK_ROOT := $(ANDROID_ROOT)/sdk/ndk/26.1.10909125
ANDROID_NDK_HOST := windows-x86_64
NDK_PLATFORM_VER := 23
PREBUILD := $(ANDROID_NDK_ROOT)/toolchains/llvm/prebuilt/$(ANDROID_NDK_HOST)

BIN := $(PREBUILD)/bin
LIB := $(PREBUILD)/sysroot/usr/lib/arm-linux-androideabi/$(NDK_PLATFORM_VER)
INCLUDE := $(PREBUILD)/sysroot/usr/include

# Compiler
CXX := $(BIN)/armv7a-linux-androideabi$(NDK_PLATFORM_VER)-clang++
GDB_CLIENT := $(BIN)/arm-eabi-gdb

LIBCRT := $(LIB)/crtbegin_dynamic.o

LINKER := /system/bin/linker

DEBUG := -g

# Compiler flags
CFLAGS := $(DEBUG) -fno-short-enums -I$(INCLUDE)
CFLAGS += -Wl,-rpath-link=$(LIB),-dynamic-linker=$(LINKER) -L$(LIB)
CFLAGS += -nostdlib -lc

# Source files
SOURCES := \
	main.cpp \
	test.cpp

# Object files
OBJS = $(patsubst %.cpp, %.o, $(SOURCES))

# Default target
all: $(TARGET)

# Rule to link object files into the executable
$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBCRT)

# Rule to compile source files into object files
%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	@rm -f $(OBJS) $(APP)

# Phony targets
.PHONY: all clean