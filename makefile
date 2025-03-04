#
# makefile for multirenamer.
#
# Copyright (C) 2024 Stefan Kleinschmidt (littlesmith23)
#


# If you build release binary, set y.
RELEASE = y
TARGET           = multirenamer
CXX_SRCS         = main.cpp multirenamer.cpp

ifeq ($(RELEASE),y)
CXXFLAGS          ?= -std=c++20 -Wall -O2 -I./include
else
CXXFLAGS          ?= -std=c++20 -Wall -O0 -g -DDEBUG=1 -I./include
endif

EXTRA_CXXFLAGS   =
EXTRA_LDFLAGS    =

# set cross compiler
LD               = $(CROSS)ld
GPP              = $(CROSS)g++
AR               = $(CROSS)ar
STRIP            = $(CROSS)strip
INSTALL         ?= install
RM              ?= rm -f

CXX_OBJS           = $(patsubst %.cpp,%.o,$(CXX_SRCS))

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

.PHONY: all clean install uninstall

all : $(TARGET)

$(TARGET): $(CXX_OBJS)
	$(GPP) $(LDFLAGS) -o $@ $(CXX_OBJS) $(STATIC_LIB) $(EXTRA_LDFLAGS)

%.o: %.c
	$(GPP) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(TARGET) *~

install:
	install $TARGET $(DESTDIR)($PREFIX)/bin/
	install -d /usr/share/doc/cscript
	install -m 644 README.md /usr/share/doc/multirenamer

uninstall:
	rm $(DESTDIR)($PREFIX)/bin/$TARGET
	rm -R /usr/share/doc/multirenamer
# end of file.
