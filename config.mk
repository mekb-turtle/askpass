TARGET = askpass
VERSION = 1.0.0
URL =

EXTRA_SRC_FILES =
EXTRA_BINARY_FILES =
CFLAGS += -Wall `pkg-config --cflags gtk4`
LDLIBS += `pkg-config --libs gtk4` -lpcre2-8
